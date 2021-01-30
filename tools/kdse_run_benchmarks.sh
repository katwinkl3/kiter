#!/usr/bin/bash


if [ "$#" -ne 1 ]; then
    echo -ne "Error\nUsage: $0 LOGDIR\n"
    exit 1
fi

LOGDIR=$1
MAX_TIME=1m

KITER="./Release/bin/kiter"
SDF3="./tools/sdf3/sdf3_custom/sdf3/build/release/Linux/bin/sdf3analysis-sdf"
CSDF3="./tools/sdf3/sdf3_custom/sdf3/build/release/Linux/bin/sdf3analysis-csdf"

COMMAND_PREFIX="timeout --foreground  ${MAX_TIME}"

# This script will run the benchmarks used in the paper on K-periodic driven DSE.
# Note that, by default, the logs will be written to ./data/{dse_logs, pp_logs}
# while KDSE has the -p LOGDIR=directory flag to specify a different log directory,
# this has not yet been implemented in SDF3 (not even in the patch) --- I will add
# it in the future
SDF3_BENCH_DIR="benchmarks/sdf3mem/"
KITER_BENCH_DIR="benchmarks/IB5CSDF/"
# split according to estimated computation times (roughly from shortest to longest)
# "shorter" meaning that I would expect both KDSE and SDF3 to complete their DSE
# "short" SDF3 benchmarks
for graph in bipartite samplerate modem satellite fig8 h263decoder; do
    echo "Run ${graph}"
    ${COMMAND_PREFIX} ${KITER} -f ${SDF3_BENCH_DIR}${graph}.xml -a KPeriodicThroughputwithDSE -p LOGDIR=${LOGDIR}  > /dev/null 2> /dev/null
    SDF3LOGDIR=${LOGDIR} ${COMMAND_PREFIX} ${SDF3} --graph ${SDF3_BENCH_DIR}${graph}.xml --algo buffersize  > /dev/null 2> /dev/null
done

for graph in BlackScholes Echo PDectect H264 JPEG2000; do
    echo "Run ${graph}"
    ${COMMAND_PREFIX} ${KITER} -f ${KITER_BENCH_DIR}${graph}.xml -a KPeriodicThroughputwithDSE -p LOGDIR=${LOGDIR} > /dev/null 2> /dev/null
    SDF3LOGDIR=${LOGDIR} ${COMMAND_PREFIX} ${CSDF3} --graph ${KITER_BENCH_DIR}${graph}.xml --algo buffersize > /dev/null 2> /dev/null
done


./tools/dsereader.py --logdir ./logs/dse_logs/ bipartite samplerate modem satellite fig8 h263dec --outputfile sdf3bench.png
./tools/dsereader.py --logdir ./logs/dse_logs/ Black-scholes echo MotionJPEG2000_CODEC_cad_V3 ViolaJones_Methode1 encoderH264 --outputfile kiterbench.png
