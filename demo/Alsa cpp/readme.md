## Alsa Demo

Inside the main.cpp file change the KEYWORD_PATH to the keyword you want 
Currently the wakeword is set to "PINEAPPLE" for testing under linux  

Requires `apt-get install libasound2-dev`  

Use `./compile.sh` to compile and build the app `alsademo`  

To run..
`$> ./alsademo`  
this is run and connect to the system "delete" audio device

`$>./alsademo hw:0` 
Option to add arg of the audio device listed from `arecord -l` to use that device 


 

