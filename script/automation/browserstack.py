import argparse
import requests
import time

APP_URI = 'https://api-cloud.browserstack.com/app-automate/{}/v2/app'
TEST_URI = 'https://api-cloud.browserstack.com/app-automate/{}/v2/test-suite'
BUILD_URI = 'https://api-cloud.browserstack.com/app-automate/{}/v2/build'
STATUS_URI = 'https://api-cloud.browserstack.com/app-automate/{}/v2/builds/{}'

devices_dict = {
    'android-min-max': [
        'Samsung Galaxy S8-7.0',
        'Samsung Galaxy M52-11.0',
        'Google Pixel 9-15.0'
    ],
    'android-perf': [
        'Google Pixel 6 Pro-15.0'
    ],
    'ios-min-max': [
        'iPhone SE 2022-15',
        'iPhone 14 Plus-16',
        'iPhone 14-18'
    ],
    'ios-perf': [
        'iPhone 13-18',
    ]
}


def main(args: argparse.Namespace) -> None:
    app_files = {
        'file': open(args.app_path, 'rb')
    }

    app_response = requests.post(
        APP_URI.format(args.type),
        files=app_files,
        auth=(args.username, args.access_key)
    )
    app_response_json = app_response.json()

    if not app_response.ok:
        print('App Upload Failed', app_response_json)
        exit(1)

    test_files = {
        'file': open(args.test_path, 'rb')
    }
    test_response = requests.post(
        TEST_URI.format(args.type),
        files=test_files,
        auth=(args.username, args.access_key)
    )
    test_response_json = test_response.json()

    if not test_response.ok:
        print('Test Upload Failed', test_response_json)
        exit(1)

    build_headers = {
        'Content-Type': 'application/json'
    }
    build_data = {
        'app': app_response_json['app_url'],
        'testSuite': test_response_json['test_suite_url'],
        'project': args.project_name,
        'devices': devices_dict[args.devices],
        'deviceLogs': True
    }

    while True:
        build_response = requests.post(
            BUILD_URI.format(args.type),
            headers=build_headers,
            json=build_data,
            auth=(args.username, args.access_key)
        )
        if (build_response is not None and 'message' in build_response.json() and '[BROWSERSTACK_ALL_PARALLELS_IN_USE]'
                in build_response.json()['message']):
            print('Parallel threads limit reached. Waiting...', flush=True)
            time.sleep(60)
        else:
            break

    if build_response is None:
        print('Build Failed')
        exit(1)

    build_response_json = build_response.json()

    if not build_response.ok:
        print('Build Failed', build_response.json())
        exit(1)

    if build_response_json['message'] != 'Success':
        print('Build Unsuccessful')
        exit(1)

    print(
        'View build results at https://app-automate.browserstack.com/dashboard/v2/builds/{}'
        .format(build_response_json['build_id']))

    while True:
        time.sleep(60)
        status_response = requests.get(
            STATUS_URI.format(args.type, build_response_json['build_id']),
            auth=(args.username, args.access_key)
        )
        status_response_json = status_response.json()
        status = status_response_json['status']

        if not status_response.ok:
            print('Status Request Failed', status_response_json)
            exit(1)

        if status != 'queued' and status != 'running':
            break

    print('Status:', status)
    if status != 'passed':
        exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--type', choices=['espresso', 'xcuitest'], required=True)
    parser.add_argument('--username', required=True)
    parser.add_argument('--access_key', required=True)

    parser.add_argument('--project_name', required=True)
    parser.add_argument('--devices', choices=devices_dict.keys(), required=True)
    parser.add_argument('--app_path', required=True)
    parser.add_argument('--test_path', required=True)
    args = parser.parse_args()

    main(args)
