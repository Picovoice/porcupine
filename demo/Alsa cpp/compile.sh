#!/bin/sh
g++ -o alsademo -I../../include -L../../lib/linux/x86_64 p.cpp -lpv_porcupine -lasound
