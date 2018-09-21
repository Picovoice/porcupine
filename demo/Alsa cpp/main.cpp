/*
 * 
 * Project: Porcupine Alsa
 * Created Date: Tuesday, September 18th 2018, 7:41:55 pm
 * Author: Greg
 * -----
 * Last Modified: Tue Sep 18 2018
 * Modified By: Greg
 * -----
 * Copyright (c) 2018 Greg
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */


#include <iostream>
#include <alsa/asoundlib.h>
#include "../../include/pv_porcupine.h"


using namespace std;

#define NO_ERROR	0
#define OPEN_ERROR	1
#define MALLOC_ERROR	2
#define ANY_ERROR	3
#define ACCESS_ERROR	4
#define FORMAT_ERROR	5
#define RATE_ERROR	6
#define CHANNELS_ERROR	7
#define PARAMS_ERROR	8
#define PREPARE_ERROR	9
#define SNDREAD_ERROR	10
#define START_ERROR	11
#define START_PORCUPINE_ERROR	12

#define MAX_BUF_SIZE	512
#define MAX_SAMPLES	256000

// Change these to match your environment!
static char MODEL_PATH[] = "../../lib/common/porcupine_params.pv";
static char KEYWORD_PATH[] = "../../resources/keyword_files/pineapple_linux.ppn";

static pv_porcupine_object_t *porcupineObject;

static char snd_device[] = "default";
snd_pcm_t* capture_handle;
snd_pcm_hw_params_t* hw_params;
snd_pcm_info_t* s_info;
unsigned int srate = 16000;
unsigned int nchan = 1;


int init_soundcard(const char *devicename)
{
    //connect to and set parameters for the sound device
    //returns any error setting or connecting

	int err = 0;

    if (devicename)
        strcpy(snd_device, devicename);

	if ((err = snd_pcm_open(&capture_handle, snd_device,SND_PCM_STREAM_CAPTURE, 0)) < 0)
	{
		fprintf(stderr, "cannot open audio device %s (%s, %d)\n",snd_device, snd_strerror(err), err);
		return OPEN_ERROR;
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		fprintf(stderr,"cannot allocate hardware parameter structure (%s, %d)\n",snd_strerror(err), err);
		return MALLOC_ERROR;
	}

	if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
	{
		fprintf(stderr,"cannot initialize hardware parameter structure (%s, %d)\n",snd_strerror(err), err);
		return ANY_ERROR;
	}

	if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "cannot set access type (%s, %d)\n",snd_strerror(err), err);
		return ACCESS_ERROR;
	}

	if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params,SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "cannot set sample format (%s, %d)\n",snd_strerror(err), err);
		return FORMAT_ERROR;
	}

	if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,&srate, 0)) < 0)
	{
		fprintf(stderr, "cannot set sample rate (%s, %d)\n",snd_strerror(err), err);
		return RATE_ERROR;
	}

	if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, nchan))< 0)
	{
		fprintf(stderr, "cannot set channel count (%s, %d)\n",snd_strerror(err), err);
		return CHANNELS_ERROR;
	}

	if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0)
	{
		fprintf(stderr, "cannot set parameters (%s, %d)\n",snd_strerror(err), err);
		return PARAMS_ERROR;
	}

	if ((err = snd_pcm_prepare(capture_handle)) < 0)
	{
		fprintf(stderr,"cannot prepare audio interface for use (%s, %d)\n",snd_strerror(err), err);
		return PREPARE_ERROR;
	}
	if ((err = snd_pcm_start(capture_handle)) < 0)
	{
		fprintf(stderr,"cannot start soundcard (%s, %d)\n",snd_strerror(err), err);
		return START_ERROR;
	}

	snd_pcm_hw_params_free(hw_params);

	return NO_ERROR;
}

int close_soundcard()
{
    //disconnect for soundcard
	return snd_pcm_close(capture_handle);
}

int start_porcupine()
{
    //connect to porcupine
    pv_status_t status = pv_porcupine_init(MODEL_PATH, KEYWORD_PATH, 0.5, &porcupineObject);
    if (status == PV_STATUS_SUCCESS) {
        fprintf (stdout, "Porcupine initialized\n");
    } else {
        fprintf (stderr, "Failed to initialize Porcupine\n");
        return START_PORCUPINE_ERROR;
    }
    return NO_ERROR;
}

int listen()
{
    //listen on the mic for the wake word

	int err = 0;
    short wav_data[MAX_BUF_SIZE];
    bool detected;

	while (1)
	{
		err = snd_pcm_readi(capture_handle, wav_data, MAX_BUF_SIZE) != MAX_BUF_SIZE;
		if (err < 0) {
			fprintf(stderr, "read from audio interface failed (%s, %d)\n", snd_strerror(err), err);
			if (err == -32) // Broken pipe
			{
				if ((err = snd_pcm_prepare(capture_handle)) < 0)
				{
					fprintf(stderr, "cannot prepare audio interface for use (%s, %d)\n", snd_strerror(err), err);
					return PREPARE_ERROR;
				}
			}
			else
			{
				return SNDREAD_ERROR;
			}
		} else {
            pv_status_t status = pv_porcupine_process(porcupineObject, wav_data, &detected);

            if (detected) {
                // Detected keyword. Do something!
                fprintf(stdout,"Detected keyword!\n");
            }
        }
	}
	return NO_ERROR;
}

int main(int argc, char *argv[])
{
	cout << "Open default soundcard" << endl;
    init_soundcard(argv[1]);

    cout << "Init Porcupine" << endl;
    start_porcupine();

    if (porcupineObject != NULL) {
        cout << "Listen..." << endl;
        listen();
    }

    cout << "Close default soundcard" << endl;
    close_soundcard();
	
	return 0;
}
