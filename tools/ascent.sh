#!/bin/sh

###
###  This script generates pictures for ASCENT paper.
###  Usage is : sh tools/ascent.sh with Debug/bin/kiter available.

OUTDIR=./ascent_pictures/
KITER=../Debug/bin/kiter
MOTIV=../benchmarks/NoCMotivation/sdf_mapping.xml

mkdir -p ./${OUTDIR}/
cd ./${OUTDIR}/

${KITER} -f ${MOTIV}  -a BufferlessNoCMapAndRoute -v10 -pcolor_routes=1 -a PrintMapping  > ./mapping.dot && dot ./mapping.dot -Kneato -Tpdf -o ./mapping.pdf
${KITER} -f ${MOTIV}  -a BufferlessNoCMapAndRoute -v10 -a BufferlessNoCScheduling

dot ./bufferless_noc_schedule_example_2_ModelNoCConflictFreeCommunication_noc.dot -Kneato -Tpdf -o ./modelnoc.pdf
dot ./bufferless_noc_schedule_example_6_end_noc.dot -Kneato -Tpdf -o ./final.pdf
