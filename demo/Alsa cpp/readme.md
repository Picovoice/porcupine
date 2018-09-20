## Alsa Demo

Inside the main.cpp file change the KEYWORD_PATH to the keyword you want
Inside the compile.sh change the 

Use `./compile.sh` to compile and build the app `alsademo`  

Use `arecord -l` to get a list of audio recording devices  

To run..
`$> ./alsademo`  
this is run and connect to the system "delete" audio device

`$>./alsademo hw:0` 
add arg of the audio device listed from "arecord -l" to use that device 


 

