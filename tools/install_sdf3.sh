#!/bin/bash

set -x


if [ "$#" -ne 1 ]; then
    echo -ne "Error\nUsage: $0 TARGET\n"
    exit 1
fi

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
    
if md5sum --status -c "${TARGET}/${SDF3_ARCHIVE}.md5"; then
    echo "Archive already downloaded.";
else
    rm -f ${TARGET}/${SDF3_ARCHIVE}
    wget --tries=1 http://www.es.ele.tue.nl/sdf3/download/files/releases/${SDF3_ARCHIVE} -O "${TARGET}/${SDF3_ARCHIVE}";
fi

## UNZIP AND COMPILE

SDF3_ROOT="${TARGET}/sdf3_${SDF3_VERSION}/"

if [ -e "${SDF3_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf" ]; then
    echo "Binary found, we assume it is done.";
else
    unzip -o "${TARGET}/${SDF3_ARCHIVE}" -d "${SDF3_ROOT}";
    if [[ "$OSTYPE" == "darwin"* ]]; then
	sed -i.bak "s/-fno-tree-fre//g" ${SDF3_ROOT}/sdf3/etc/Makefile.inc
    fi;
    pushd "${SDF3_ROOT}/sdf3/" && make && popd || exit 1
fi;

cd "${ORIGIN}" || exit 1

if [ -e "${SDF3_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf" ]; then
    echo "Script finished and binary is found.";
else
    echo "Error, sdf3analysis-csdf not found.";
    exit 1;
fi;


## UNZIP, PATCH AND COMPILE THE CUSTOM VERSION

SDF3_CUSTOM_ROOT="${TARGET}/sdf3_custom/"

if [ -e "${SDF3_CUSTOM_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf" ]; then
    echo "Binary found, we assume it is done.";
else
    unzip -o "${TARGET}/${SDF3_ARCHIVE}" -d "${SDF3_CUSTOM_ROOT}";
    pushd "${SDF3_CUSTOM_ROOT}/" && patch --verbose -p1 < ../../sdf3.patch && popd || exit 1
    if [[ "$OSTYPE" == "darwin"* ]]; then
	sed -i.bak "s/-fno-tree-fre//g" ${SDF3_CUSTOM_ROOT}/sdf3/etc/Makefile.inc
    fi;
    pushd "${SDF3_CUSTOM_ROOT}/sdf3/" && make && popd || exit 1
fi;

cd "${ORIGIN}" || exit 1

if [ -e "${SDF3_CUSTOM_ROOT}/sdf3/build/release/Linux/bin/sdf3analysis-csdf" ]; then
    echo "Script finished and binary is found.";
else
    echo "Error at the end of the script.";
    exit 1;
fi;
