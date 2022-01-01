#!/bin/bash
mul=1
while getopts m:x: flag
do
case "${flag}" in
m) mode=${OPTARG};;
x) mul=${OPTARG};;
esac
done
if [ $mode = "0" ]
then
echo "Calculating Summarized Results"
for f in benchmarks/ascenttestbench/*.xml
do
ped="$(./Debug/bin/kiter -f "$f" -a SPeriodicScheduling | grep -P "(?:period\s+is\s+)\K\d+" -o)"
echo "period of $ped for $f"
echo "$f :" >> runres.txt
for a in ASAPScheduling So4Scheduling 
do
rr="$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a randomRouting -a AddVBuffers -a "$a" -p TDMA=benchmarks/fbf.txt -p PERIOD="$ped" -p MUL="$mul" | grep -P "(?:throughput\s+is\s+)\K\d+.\d+" -o)"
rxy="$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a xyRouting -a AddVBuffers -a "$a" -p TDMA=benchmarks/fbf.txt -p PERIOD="$ped" -p MUL="$mul" | grep -P "(?:throughput\s+is\s+)\K\d+.\d+" -o)"
b="$(./Debug/bin/kiter -f "$f" -a createNoC -a BufferlessNoCMapAndRoute -a AddVBuffers -a "$a" -p TDMA=benchmarks/fbf.txt -p PERIOD="$ped" -p MUL="$mul" | grep -P "(?:throughput\s+is\s+)\K\d+.\d+" -o)"
echo "$a threshold :  randomRoute = $rr xyRoute = $rxy bufferlessMR = $b"
echo "$a : $rr , $rxy, $b" >> runres.txt
done
done
fi
if [ $mode = "1" ] 
then
echo "WIP wait ah"
for f in benchmarks/ascenttestbench/*.xml
do
ped="$(./Debug/bin/kiter -f "$f" -a SPeriodicScheduling | grep -P "(?:period\s+is\s+)\K\d+" -o)"
for a in ASAPScheduling So4Scheduling
do 
echo "$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a randomRouting -a AddVBuffers -a "$a" -p TDMA=benchmark/fbf.txt -p PERIOD="$ped")" >> runres.txt
echo "$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a xyRouting -a AddVBuffers -a "$a" -p TDMA=benchmark/fbf.txt -p PERIOD="$ped")" >> runres.txt
echo "$(./Debug/bin/kiter -f "$f" -a createNoC -a BufferlessNoCMapAndRoute -a AddVBuffers -a "$a" -p TDMA=benchmark/fbf.txt -p PERIOD="$ped")" >> runres.txt
done
done
fi
if [ $mode = "2" ] 
then
echo "Store Entire Output Log"
for f in benchmarks/ascenttestbench/*.xml
do
ped="$(./Debug/bin/kiter -f "$f" -a SPeriodicScheduling | grep -P "(?:period\s+is\s+)\K\d+" -o)"
for a in ASAPScheduling So4Scheduling
do 
echo "$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a randomRouting -a AddVBuffers -a "$a" -p TDMA=benchmark/fbf.txt -p PERIOD="$ped")" >> runres.txt
echo "$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a xyRouting -a AddVBuffers -a "$a" -p TDMA=benchmark/fbf.txt -p PERIOD="$ped")" >> runres.txt
echo "$(./Debug/bin/kiter -f "$f" -a createNoC -a BufferlessNoCMapAndRoute -a AddVBuffers -a "$a" -p TDMA=benchmark/fbf.txt -p PERIOD="$ped")" >> runres.txt
done
done
fi
if [ $mode = "3" ] 
then
echo "Testing Validity of Schedule"
for f in benchmarks/ascenttestbench/*.xml
do
ped="$(./Debug/bin/kiter -f "$f" -a SPeriodicScheduling | grep -P "(?:period\s+is\s+)\K\d+" -o)"
for a in ASAPScheduling So4Scheduling
do 
rr="$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a randomRouting -a AddVBuffers -a "$a" -p TDMA=benchmarks/fbf.txt -p PERIOD="$ped" -p MUL="$mul" | grep -P "(?:Check:\s+)\K\d" -o)"
rxy="$(./Debug/bin/kiter -f "$f" -a createNoC -a randomMapping -a xyRouting -a AddVBuffers -a "$a" -p TDMA=benchmarks/fbf.txt -p PERIOD="$ped" -p MUL="$mul" | grep -P "(?:Check:\s+)\K\d" -o)"
b="$(./Debug/bin/kiter -f "$f" -a createNoC -a BufferlessNoCMapAndRoute -a AddVBuffers -a "$a" -p TDMA=benchmarks/fbf.txt -p PERIOD="$ped" -p MUL="$mul" | grep -P "(?:Check:\s+)\K\d" -o)"
echo "$a gives:  randomRoute = $rr xyRoute = $rxy bufferlessMR = $b"
done
done
fi

