#!/bin/bash
FILE=Mi-RAM-HQ
make 
if test -f "../outputs/bin/Mi-RAM-HQ/Debug/$FILE"; then
    ../scripts/./ex.sh Mi-RAM-HQ
fi