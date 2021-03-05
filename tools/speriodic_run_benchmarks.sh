#!/bin/bash
# This script runs the benchmarks used in the paper on K-periodic driven DSE.

set -x


MAX_TIME=2m

KITER="./Release/bin/kiter_gurobi"
BENCH_DIR="benchmarks/"


if [ "$#" -le 0 ]; then
    echo -ne "Error\nUsage: $0 LOGDIR [MAX_TIME]\n"
    exit 1
fi

if [ "$#" -ge 2 ]; then
    MAX_TIME=$2
fi

if [ ! -x ${KITER} ]; then
    echo -ne "Error Kiter is missing: ${KITER} Not found.\n"
    exit 1
fi



COMMAND_PREFIX="timeout --foreground  ${MAX_TIME}"
LOGDIR="$1/dse_logs/"
THLOG="$1/speriodic_benchmark_throughput.log"
mkdir -p "${LOGDIR}"



########## THROUGHPUT NUMBERS OUTPUT AS CSV #################

echo "Filename;tasks;buffers;sumqt;sumphit;sumnt;sumki;SPeriodic;1Periodic;KPeriodic" > "${THLOG}"
for f in $(find "${BENCH_DIR}/IB5CSDF" "${BENCH_DIR}/AGB5CSDF/" -type f); do
    #echo "Run ${f}"
    THS=$(${COMMAND_PREFIX} ${KITER} -f "$f" -a SPeriodicThroughput | grep throughput | sed "s/.* \([^ ]\)/\1/g")
    TH1=$(${COMMAND_PREFIX} ${KITER} -f "$f" -a 1PeriodicThroughput | grep throughput | sed "s/.* \([^ ]\)/\1/g")

    KPERIODICLOG=$(${COMMAND_PREFIX} ${KITER} -f "$f" -a KPeriodicThroughput)
    
    THK=$(echo "${KPERIODICLOG}" | grep -i Throughput | sed "s/.* \([^ ]\)/\1/g")
    ski=$(echo "${KPERIODICLOG}" | grep -i "Scheduling Size" | sed "s/.* \([^ ]\)/\1/g")

    
    tcount=$(./Release/bin/kiter -f "$f" -a PrintInfos| grep  "Task count" | sed "s/.* \([^ ]\)/\1/g")
    bcount=$(./Release/bin/kiter -f "$f" -a PrintInfos| grep "Channel count" | sed "s/.* \([^ ]\)/\1/g")

    
    sqt=$(./Release/bin/kiter -f "$f" -a PrintInfos| grep "(qt)" | sed "s/.* \([^ ]\)/\1/g")
    sphit=$(./Release/bin/kiter -f "$f" -a PrintInfos| grep "(phit)" | sed "s/.* \([^ ]\)/\1/g")
    snt=$(./Release/bin/kiter -f "$f" -a PrintInfos| grep "Nt=" | sed "s/.* \([^ ]\)/\1/g")

    echo "${f};${tcount};${bcount};${sqt};${sphit};${snt};${ski};${THS};${TH1};${THK}" >> "${THLOG}"
done




########## BUFFER SIZING NUMBERS OUTPU IN LOGDIR AS CSV #################


for filename in $(find "${BENCH_DIR}/IB5CSDF" "${BENCH_DIR}/AGB5CSDF/" -type f| grep -v sized); do
    echo "Run ${filename}"
    ${COMMAND_PREFIX} ${KITER} -f "${filename}" -a PeriodicDSE -p LOGDIR="${LOGDIR}"  > /dev/null 2> /dev/null
    ${COMMAND_PREFIX} ${KITER} -f "${filename}" -a SPeriodicDSE -p LOGDIR="${LOGDIR}"  > /dev/null 2> /dev/null
done


########## PRODUCE TABLES AND FIGURES #####################################


./tools/dsereader.py --logdir "${LOGDIR}" level_2_bench5 level_3_bench11 level_3_bench18 level_5_bench2 level_6_bench8 --opareto randombench_pareto.png 
./tools/dsereader.py --logdir "${LOGDIR}" Black-scholes echo MotionJPEG2000_CODEC_cad_V3 ViolaJones_Methode1 encoderH264  --opareto kiterbench_pareto.png

./tools/generate_speriodic_table.py "${THLOG}" > speriodic_throughput.tex
