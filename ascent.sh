#!/bin/sh

mkdir -p ascent_pictures
rm ascent_pictures/* -rf

./Debug/bin/kiter -f motivation/sdf_mapping.xml  -a BufferlessNoCMapAndRoute -v10 -pcolor_routes=1 -a PrintMapping  > mapping.dot && dot mapping.dot -Kneato -Tpdf -o ascent_pictures/mapping.pdf

./Debug/bin/kiter -f motivation/sdf_mapping.xml  -a BufferlessNoCMapAndRoute -v10 -a BufferlessNoCScheduling

dot bufferless_noc_schedule_example_2_ModelNoCConflictFreeCommunication_noc.dot -Kneato -Tpdf -o ascent_pictures/modelnoc.pdf

dot bufferless_noc_schedule_example_6_end_noc.dot -Kneato -Tpdf -o ascent_pictures/final.pdf
