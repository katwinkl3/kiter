#!/bin/sh

###
###  This script generates pictures for ASCENT paper.
###  Usage is : sh tools/ascent.sh with Debug/bin/kiter available.

OUTDIR=./ascent_pictures/
KITER=../Debug/bin/kiter
MOTIV=../benchmarks/NoCMotivation/sdf_mapping.xml

mkdir -p ./${OUTDIR}/
cd ./${OUTDIR}/ || exit

${KITER} -f ${MOTIV}  -a BufferlessNoCMapAndRoute -v10 -pcolor_routes=1 -a PrintMapping  > ./mapping.dot
${KITER} -f ${MOTIV}  -a BufferlessNoCMapAndRoute -v10 -a BufferlessNoCScheduling

cp bufferless_noc_schedule_example_2_ModelNoCConflictFreeCommunication_noc.dot modelnoc.dot
cp bufferless_noc_schedule_example_6_end_noc.dot final.dot


dot ./mapping.dot -Kneato -Tpdf -o ./mapping.pdf
dot modelnoc.dot -Kneato -Tpdf -o ./modelnoc.pdf
dot final.dot -Kneato -Tpdf -o ./final.pdf
