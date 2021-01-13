#!/bin/bash

set -x

#SDF3_VERSION=100927
SDF3_VERSION=140724

ORIGIN=$(pwd)
TARGET=$1

## CHECK TARGET FOLDER

if [ -d "${TARGET}" ]; then
    echo "Target '${TARGET}' exists and is a directory.";
else
    echo "Target '${TARGET}' does not exist. Please specify a valid directory.";
    exit 1;
fi

## DOWNLOAD SOURCE CODE

SDF3_ARCHIVE=sdf3-${SDF3_VERSION}.zip

if [ -e "${TARGET}/${SDF3_ARCHIVE}" ]; then
    echo "Archive already downloaded.";
else    
    wget --tries=1 http://www.es.ele.tue.nl/sdf3/download/files/releases/${SDF3_ARCHIVE} -O "${TARGET}/${SDF3_ARCHIVE}";
fi

## UNZIP AND COMPILE

SDF3_ROOT="${TARGET}/sdf3_${SDF3_VERSION}/"

if [ -e "${SDF3_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf" ]; then
    echo "Binary found, we assume it is done.";
else
    unzip -o "${TARGET}/${SDF3_ARCHIVE}" -d "${SDF3_ROOT}";
    pushd "${SDF3_ROOT}/sdf3/" && make && popd || exit 1
fi;

cd "${ORIGIN}" || exit 1

if [ -e "${SDF3_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf" ]; then
    echo "Script finished and binary is found.";
else
    echo "Error at the end of the script.";
    exit 1;
fi;
