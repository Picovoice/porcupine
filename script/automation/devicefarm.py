import argparse
import datetime
import requests
import time
import random
import string
import os

import boto3


def upload_device_farm(
        client,
        run_uuid,
        project_arn,
        filepath,
        df_type,
        mime='application/octet-stream'):
    response = client.create_upload(projectArn=project_arn,
        name = f"{run_uuid}_{os.path.basename(filepath)}",
        type=df_type,
        contentType=mime)
    upload_arn = response['upload']['arn']
    upload_url = response['upload']['url']

    with open(filepath, 'rb') as file_stream:
        print(f"Uploading {filepath} to Device Farm as {response['upload']['name']}... ", end='')
        put_req = requests.put(upload_url, data=file_stream, headers={"content-type": mime})
        print(" done!")
        if not put_req.ok:
            raise Exception("Failed to upload: \n" + put_req.reason)

    started = datetime.datetime.now()
    while True:
        print(f"Upload of `{filepath}` in state `{response['upload']['status']}`; total time {str(datetime.datetime.now() - started)}")
        if response['upload']['status'] == 'FAILED':
            raise Exception("The upload failed processing: \n" + (response['upload']['message'] if 'message' in response['upload'] else response['upload']['metadata']))
        if response['upload']['status'] == 'SUCCEEDED':
            break
        time.sleep(5)
        response = client.get_upload(arn=upload_arn)

    return upload_arn


def schedule_run(
        client,
        run_uuid,
        project_arn,
        app_arn,
        test_arn,
        test_spec_arn,
        test_df_type,
        device_pool_arn):
    response = client.schedule_run(
        projectArn = project_arn,
        appArn = app_arn,
        devicePoolArn = device_pool_arn,
        name=run_uuid,
        test = {
            "type": test_df_type,
            "testSpecArn": test_spec_arn,
            "testPackageArn": test_arn
            }
        )
    run_arn = response['run']['arn']
    return run_arn


def wait_for_run(
        client,
        run_uuid,
        run_arn):
    start_time = datetime.datetime.now()
    try:
        while True:
            response = client.get_run(arn=run_arn)
            state = response['run']['status']
            if state == 'COMPLETED' or state == 'ERRORED':
                break
            else:
                print(f"Run `{run_uuid}` in state `{state}`; total time {str(datetime.datetime.now() - start_time)}")
                time.sleep(10)
    except Exception as e:
        client.stop_run(arn=run_arn)
        print(e)
        exit(1)

    print(f"Run `{run_uuid}` FINISHED in state `{state}`; total time {str(datetime.datetime.now() - start_time)}")


def main(args: argparse.Namespace) -> None:
    run_uuid = f"{args.run_name}-{datetime.date.today().isoformat()}-{''.join(random.sample(string.ascii_letters,8))}"
    client = boto3.client(
        "devicefarm",
        aws_access_key_id=args.aws_id,
        aws_secret_access_key=args.aws_secret,
        region_name="us-west-2")

    print(f">> Starting device farm run: {run_uuid}")

    params = {}
    if (args.type == 'android'):
        params['app_type'] = "ANDROID_APP"
        params['test_package_type'] = "INSTRUMENTATION_TEST_PACKAGE"
        params['test_type'] = "INSTRUMENTATION"
    elif (args.type == 'ios'):
        params['app_type'] = "IOS_APP"
        params['test_package_type'] = "XCTEST_UI_TEST_PACKAGE"
        params['test_type'] = "XCTEST_UI"
    else:
        print("Invalid device type")
        exit(1)

    app_arn = upload_device_farm(
        client,
        run_uuid,
        args.project_arn,
        args.app_path,
        params['app_type'])
    print(f">> app_arn: {app_arn}")
    test_arn = upload_device_farm(
        client,
        run_uuid,
        args.project_arn,
        args.test_path,
        params['test_package_type'])
    print(f">> test_arn: {test_arn}")

    run_arn = schedule_run(
        client,
        run_uuid,
        args.project_arn,
        app_arn,
        test_arn,
        args.test_spec_arn,
        params['test_type'],
        args.device_pool_arn)
    print(f">> run_arn: {run_arn}")
    wait_for_run(client, run_uuid, run_arn)

    jobs_response = client.list_jobs(arn=run_arn)
    for job in jobs_response['jobs']:
        job_name = job['name']
        result = job['result']  # PENDING, PASSED, WARNED, FAILED, SKIPPED, ERRORED, STOPPED
        if result not in ['PASSED']:
            print(f">> Run `{run_uuid}` ({run_arn}) job `{job_name}` failed!")
            print(f">> See AWS console for details.")
            exit(1)
        else:
            print(f">> Run `{run_uuid}` ({run_arn}) job `{job_name}` passed!")


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--type', choices=['android', 'ios'], required=True)

    parser.add_argument('--run_name', required=True)
    parser.add_argument('--app_path', required=True)
    parser.add_argument('--test_path', required=True)

    parser.add_argument('--aws_id', required=True)
    parser.add_argument('--aws_secret', required=True)

    parser.add_argument('--project_arn', required=True)
    parser.add_argument('--test_spec_arn', required=True)
    parser.add_argument('--device_pool_arn', required=True)

    args = parser.parse_args()

    main(args)
