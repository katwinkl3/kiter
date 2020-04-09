#!/bin/sh

GFOLDER=~/Dropbox/SoftwareControlledNoC/rtas-2020/sdfgaphs/graphs/

for f in ${GFOLDER}/one_* ; do
    echo ./Debug/bin/kiter -f ${f} -a SoftwareControlledNoCBufferless -v5
done
