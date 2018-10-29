Go Demo
=======

Uses Go bindings from [github.com/charithe/porcupine-go](https://github.com/charithe/porcupine-go/)

This demo application was tested on 64-bit linux using Go 1.11 with module support.

The `keyword` argument to the application should consist of three components separated by the colon (`:`) character:

`keyword_name` **:** `path_to_keyword_data_file` **:** `sensitivity_value_between_0_and_1`


Single Keyword Detection
------------------------

The following example detects the word "pineapple" from the default audio input device.

```shell
gst-launch-1.0 -v alsasrc ! audioconvert ! audioresample ! audio/x-raw,channels=1,rate=16000,format=S16LE ! filesink location=/dev/stdout | \
    go run main.go \
    -model_path=../../lib/common/porcupine_params.pv \
    -keyword=pineapple:../../resources/keyword_files/pineapple_linux.ppn:0.5

```

Multiple Keyword Detection
---------------------------

The following example detects "pineapple", "blueberry" and "grapefruit" from the default audio input device.

```shell
gst-launch-1.0 -v alsasrc ! audioconvert ! audioresample ! audio/x-raw,channels=1,rate=16000,format=S16LE ! filesink location=/dev/stdout | \
    go run main.go \
    -model_path=../../lib/common/porcupine_params.pv \
    -keyword=pineapple:../../resources/keyword_files/pineapple_linux.ppn:0.5 \
    -keyword=blueberry:../../resources/keyword_files/blueberry_linux.ppn:0.5 \
    -keyword=grapefruit:../../resources/keyword_files/grapefruit_linux.ppn:0.5 
```

