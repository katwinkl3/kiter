#!/usr/bin/bash

TARGET_TXT_DIR=~/yalenus/research/spectrum/softwaredefinednoc/newdatasets/
TARGET_PNG_DIR=~/Dropbox/SoftwareControlledNoC/sdfgaphs/

for graph in h263decoder mp3decoder samplerate modem satellite; do
    ./debug/bin/kiter  -a SymbolicExecutionWP -v 10 -ppsize=4 -f sdf3bench/one_${graph}_buffer.xml > ${graph}.txt
    ./debug/bin/kiter  -a PrintGraph                          -f sdf3bench/one_${graph}_buffer.xml > ${graph}.dot
    dot ${graph}.dot -Tpng -o ${graph}.png
    ## cp ${graph}.txt $TARGET_TXT_DIR/${graph}.txt
    ## cp ${graph}.png $TARGET_PNG_DIR/${graph}.png
done

