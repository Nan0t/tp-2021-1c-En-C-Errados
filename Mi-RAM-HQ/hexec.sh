#!/bin/bash
FILE=Mi-RAM-HQ
make $FILE
if test -f "./$FILE"; then
    valgrind --tool=helgrind ./$FILE
fi