/*
# 
# Project: Porcupine Alsa
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
*/


#include <iostream>
#include <signal.h>
#include <alsa/asoundlib.h>
#include <pv_porcupine.h>


using namespace std;

// Change these to match your environment!
const char MODEL_PATH[] = "../../lib/common/porcupine_params.pv";
const char KEYWORD_PATH[] = "../../resources/keyword_files/pineapple_linux.ppn";
float sensitivity = 0.5f;

static pv_porcupine_object_t *porcupineObject;

const char snd_device[] = "default";

snd_pcm_t* capture_handle;
snd_pcm_hw_params_t* hw_params;
snd_pcm_info_t* s_info;
unsigned int srate = 16000;
unsigned int nchan = 1;

volatile sig_atomic_t stop;

#define ERROR 1;

void catchcancel(int signum) {
    stop = 1;
}

int init_soundcard()
{
	//connect to and set parameters for the sound device
	//returns any error setting or connecting

	int err = 0;

	if ((err = snd_pcm_open(&capture_handle, snd_device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
	{
		cout << "cannot open audio device " << snd_device << " (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		cout << "cannot allocate hardware parameter structure (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
	{
		cout << "cannot initialize hardware parameter structure (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		cout << "cannot set access type (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params,SND_PCM_FORMAT_S16_LE)) < 0)
	{
		cout << "cannot set sample format (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,&srate, 0)) < 0)
	{
		cout << "cannot set sample rate (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, nchan))< 0)
	{
		cout << "cannot set channel count (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0)
	{
		cout << "cannot set parameters (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_prepare(capture_handle)) < 0)
	{
		cout << "cannot prepare audio interface for use (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	if ((err = snd_pcm_start(capture_handle)) < 0)
	{
		cout << "cannot start soundcard (" << snd_strerror(err) << ", " << err << ")" << endl;
		return ERROR;
	}

	snd_pcm_hw_params_free(hw_params);

	return 0;
}

int start_porcupine()
{
	//connect to porcupine
	pv_status_t status = pv_porcupine_init(MODEL_PATH, KEYWORD_PATH, sensitivity, &porcupineObject);
	if (status == PV_STATUS_SUCCESS) {
		cout << "Porcupine initialized" << endl;
	} else {
		cout << "Failed to initialize Porcupine" << endl;
		return ERROR;
	}
	return 0;
}

int listen()
{
	//listen on the mic for the wake word

	int err = 0;
	const int buffer_size = pv_porcupine_frame_length();
	short wav_data[buffer_size];
	bool detected;

	signal(SIGINT, catchcancel);

	while (!stop)
	{
		err = snd_pcm_readi(capture_handle, wav_data, buffer_size) != buffer_size;
		if (err < 0) {
			cout << "read from audio interface failed (" << snd_strerror(err) << ", " << err << ")" << endl;
			if (err == -32) // Broken pipe
			{
				if ((err = snd_pcm_prepare(capture_handle)) < 0)
				{
					cout << "cannot prepare audio interface for use (" << snd_strerror(err) << ", " << err << ")" << endl;
					return ERROR;
				}
			}
			else
			{
				return ERROR;
			}
		} else {
			pv_porcupine_process(porcupineObject, wav_data, &detected);

			if (detected) {
				// Detected keyword. Do something!
				cout << "Detected keyword!" << endl;
			}
		}
	}
	return 0;
}

int close_connections()
{
	//disconnect from porcupine
	pv_porcupine_delete(porcupineObject);
	
	//disconnect from soundcard
	return snd_pcm_close(capture_handle);
}

int main(int argc, char *argv[])
{
	cout << "Open default soundcard" << endl;
	init_soundcard();

	cout << "Init Porcupine" << endl;
	start_porcupine();

	if (porcupineObject != NULL) {
		cout << "Listen..." << endl;
		listen();
	}

	cout << "\nClosed connections" << endl;
	close_connections();

	return 0;
}