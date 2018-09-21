#!/bin/bash

echo "Building Porcupine Alsa Demo.."

SYSTEM=`uname`
MACHINE="`uname -m`"
LIB="NONE"


if [ $SYSTEM = "Darwin" ]
then
    SYSTEM="mac"
    LIB="-L../../lib/mac/$MACHINE"
fi

if [ $SYSTEM = "Windows" ]
then
    SYSTEM="windows"
    LIB="-L../../lib/windows/$MACHINE"
fi

if [ $SYSTEM = "Linux" ]
then
    if [[ $MACHINE == arm* ]];
    then
        SYSTEM="raspberry-pi"
        LIB="-L../../lib/raspberry-pi/arm11"
        MACHINE="arm11"
    else
        SYSTEM="linux"
        LIB="-L../../lib/linux/$MACHINE"
    fi
fi


if [ $LIB != "NONE" ]
then
    g++ -o alsademo -I../../include $LIB main.cpp -lpv_porcupine -lasound
    export LD_LIBRARY_PATH=../../lib/$SYSTEM/$MACHINE
    echo "Build Complete"
    echo "Use ./alsademo to begin"
    echo "read the README.MD for any extra information"

else
    echo "Porcupine is not supported on $SYSTEM/$MACHINE yet!"
fi
