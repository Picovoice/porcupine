/*
  Copyright 2021 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

/**
 * Indexed DB configurations
 */
const DB_NAME = 'pv_db';
const STORE_NAME = 'pv_store';
const V = 1;
 
 /**
  * Storage Interface.
  */
interface PvStorage {
  setItem: (key: string, value: string) => void | Promise<void>;
  getItem: (key: string) => string | Promise<string>;
  removeItem: (key: string) => void | Promise<void>;
}
 
 /**
  * Opens indexedDB connection, handles version changes and gets the db instance.
  * 
  * @returns The instance of indexedDB connection.
  */
function getDB(): Promise<IDBDatabase> {
  return new Promise((resolve, reject) => {
    const request = self.indexedDB.open(DB_NAME, V);
    request.onerror = () => {
      reject(request.error);
    };
    request.onsuccess = () => {
      resolve(request.result);
    };
    request.onupgradeneeded = () => {
      request.result.createObjectStore(STORE_NAME);
    };
  });
}
 
 /**
  * Gets the storage to use. Either tries to use IndexedDB or localStorage.
  * 
  * @returns PvStorage instance to use as storage.
  */
export function getPvStorage(): PvStorage {
  if (self.indexedDB) {
    const requestHelper = (request: IDBRequest<IDBValidKey>): Promise<any> => {
      return new Promise((resolve, reject) => {
        request.onerror = () => {
          reject(request.error);
        };
        request.onsuccess = () => {
          resolve(request.result);
        };
      });
    }

    return {
      setItem: async (key: string, value: string) => {
        const db = await getDB();
        const request = db.transaction(STORE_NAME, 'readwrite').objectStore(STORE_NAME).put(value, key);
        await requestHelper(request);
        db.close();
      },
      getItem: async (key: string) => {
        const db = await getDB();
        const request = db.transaction(STORE_NAME, 'readonly').objectStore(STORE_NAME).get(key);
        const res = await requestHelper(request);
        db.close();
        return res;
      },
      removeItem: async (key: string) => {
        const db = await getDB();
        const request = db.transaction(STORE_NAME, 'readwrite').objectStore(STORE_NAME).delete(key);
        await requestHelper(request);
        db.close();
      }
    }
  } else if (self.localStorage) {
    return self.localStorage;
  }

  throw new Error("Cannot get a presistent storage object.");
}

/**
 * Convert a null terminated phrase stored inside an array buffer to a string
 *
 * @param arrayBuffer input array buffer
 * @param indexStart the index at which the phrase is stored
 * @return retrieved string
 */

 export function arrayBufferToStringAtIndex(
  arrayBuffer: Uint8Array,
  indexStart: number,
): string {
  let indexEnd = indexStart;
  while (arrayBuffer[indexEnd] !== 0) {
    indexEnd++;
  }
  const utf8decoder = new TextDecoder('utf-8'); 
  return utf8decoder.decode(arrayBuffer.subarray(indexStart, indexEnd));
}

/**
 * Decode a base64 string and stored it in a Uint8Array array
 *
 * @param base64String input base64 string
 * @return decoded array
 */

export function base64ToUint8Array(base64String: string): Uint8Array {
  const base64StringDecoded = atob(base64String);
  const binaryArray = new Uint8Array(base64StringDecoded.length);
  for (let i = 0; i < base64StringDecoded.length; i++) {
    binaryArray[i] = base64StringDecoded.charCodeAt(i);
  }
  return binaryArray;
}

/**
 * Encode an ArrayBuffer array to base64 string
 *
 * @param arrayBuffer input array
 * @param size size of the phrase to be encoded
 * @param index the index at which the phrase is stored
 * @return base64 string
 */

export function arrayBufferToBase64AtIndex(arrayBuffer: ArrayBuffer, size: number, index: number): string {
  let binary = '';
  for (let i = 0; i < size; i++) {
    // @ts-ignore
    binary += String.fromCharCode(arrayBuffer[index + i]);
  }
  return btoa(binary);
}

/**
 * Convert a string header to JS object
 *
 * @param stringHeader input string in json format
 * @return retrieved object
 */

// eslint-disable-next-line
export function stringHeaderToObject(stringHeader: string): object {
  const objectHeader = {};
  for (const property of stringHeader.split('\r\n')) {
    const keyValuePair = property.split(': ');
    if (keyValuePair[0] !== '') {
      // @ts-ignore
      objectHeader[keyValuePair[0]] = keyValuePair[1];
    }
  }
  return objectHeader;
}

/**
 * A wrapper to fetch that also supports timeout
 *
 * @param uri the URL of the resource
 * @param options other options related to fetch
 * @param time timeout value
 * @return received response
 */

export async function fetchWithTimeout(uri: string, options = {}, time = 5000): Promise<Response> {
  const controller = new AbortController();
  const config = { ...options, signal: controller.signal };
  const timeout = setTimeout(() => {
    controller.abort();
  }, time);
  const response = await fetch(uri, config);
  clearTimeout(timeout);
  return response;
}

/**
 * Checking whether the given AccessKey is valid
 *
 * @return true if the AccessKey is valid, false if not
 */

 export function isAccessKeyValid(accessKey: string): boolean {
  if (typeof accessKey !== 'string' || accessKey === undefined || accessKey === null) {
    return false;
  }
  const accessKeyCleaned = accessKey.trim();
  if (accessKeyCleaned === '') { return false; }
  try {
    return btoa(atob(accessKeyCleaned)) === accessKeyCleaned;
  } catch (err) {
    return false;
  }
}
