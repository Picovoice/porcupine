#!/bin/sh

echo "Building Porcupine Alsa Demo.."

lowercase(){
    echo "$1" | sed "y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/"
}

SYSTEM=`lowercase \`uname\``
MACHINE="`uname -m`"

g++ -o alsademo -I../../include -L../../lib/$SYSTEM/$MACHINE main.cpp -lpv_porcupine -lasound
export LD_LIBRARY_PATH=../../lib/$SYSTEM/$MACHINE

echo "Build Complete"
echo "Use ./alsademo to begin"
echo "read the README.MD for any extra information"
