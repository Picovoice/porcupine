## Alsa Demo

Set KEYWORD_PATH in the main.cpp file to the keyword file that is correct for your system type.  

Currently the KEYWORD_PATH is set to `pineapple_linux.ppn`. "pineapple" is the wakeword and "linux" is the OS system.  
Checkout the `resources/keyword_files` for a list of available pre-built wakewords to use.  
 
Requires `apt-get install libasound2-dev`  

### Compile

Replace ${SYSTEM} with the name of the operating system on your machine (e.g. linux, mac, windows, or raspberry-pi).  
Replace ${MACHINE} with the CPU architecture of current machine (x86_64 or i386)  
For Raspberry ARM  
- A, B, and Zero - ${MACHINE} = arm11  
- Pi2 - ${MACHINE} = cortex-a7  
- Pi3 & 3B+ - ${MACHINE} = cortex-a53  

Complile with g++  
Single Keyword Demo  
`g++ -O3 -o alsademo -I../../include -L../../lib/${SYSTEM}/$MACHINE -Wl,-rpath ../../lib/${SYSTEM}/$MACHINE main.cpp -lpv_porcupine -lasound`  

Multiple Keyword Demo  
`g++ -O3 -o alsamultipledemo -I../../include -L../../lib/${SYSTEM}/$MACHINE -Wl,-rpath ../../lib/${SYSTEM}/$MACHINE main_multiple_keywords.cpp -lpv_porcupine -lasound`  

### Run

`$> ./alsademo`  
This will connect to the alsa system "default" audio device, specified in either the `asoundrc` or `/etc/asound.conf` files.    
Use `arecord -l` to see a list of alsas devices on the system.  
