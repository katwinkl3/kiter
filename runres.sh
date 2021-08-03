#!/bin/bash
echo "start"
for f in benchmarks/ascenttestbench/*.xml
do
echo "$f random map random route so4" >> runres.txt
./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a randomRouting -a So4Scheduling -p TDMA="benchmarks/fbf.txt" &>> runres.txt
echo "$f random map xy route so4" >> runres.txt
./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a xyRouting -a So4Scheduling -p TDMA="benchmarks/fbf.txt" &>> runres.txt
done

