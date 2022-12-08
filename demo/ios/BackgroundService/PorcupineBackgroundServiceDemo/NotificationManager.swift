//
//  Copyright 2021 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import UIKit
import UserNotifications

class NotificationManager: NSObject, UNUserNotificationCenterDelegate {
    public static let shared = NotificationManager()

    private override init() {
        super.init()

        UNUserNotificationCenter.current().delegate = self
    }

    public func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        didReceive response: UNNotificationResponse,
        withCompletionHandler completionHandler: @escaping () -> Void
    ) {
        completionHandler()
    }

    public func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        willPresent notification: UNNotification,
        withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void
    ) {
        completionHandler([.badge, .alert, .sound])
    }

    public func requestNotificationAuthorization() {
        UNUserNotificationCenter.current().requestAuthorization(
            options: [.badge, .alert, .sound],
            completionHandler: { (_, error) in
                if let error = error {
                    print("Error: ", error)
                }
            })
    }

    public func sendNotification() {
        let content = UNMutableNotificationContent()
        content.title = "Porcupine"
        content.body = "Wake word detected"
        content.sound = UNNotificationSound.default

        let request = UNNotificationRequest(identifier: UUID().uuidString, content: content, trigger: nil)

        UNUserNotificationCenter.current().add(request) {(error) in
            if let error = error {
                print("Notification Error: ", error)
            }
        }
    }

}
