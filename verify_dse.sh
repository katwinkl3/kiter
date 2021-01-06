#!/usr/bin/bash

SDF3_BINARY_ROOT=./sdf3/sdf3/build/release/Linux/bin/
SDF3ANALYSIS_CSDF="timeout 180 ${SDF3_BINARY_ROOT}/sdf3analysis-csdf"
SDF3ANALYSIS_SDF="timeout 180 ${SDF3_BINARY_ROOT}/sdf3analysis-sdf"
KITER="timeout 180 ./Release/bin/kiter"
LOG_DIR=./data/dse_verification/
SDF3_LOG=${LOG_DIR}/sdf3_pp_logs
KITER_LOG=${LOG_DIR}/kiter_pp_logs
BENCH_DIR=./benchmarks/sdf3mem/ # set to directory containing benchmarks

# make directories to store logs for verification
mkdir -p ${LOG_DIR} ${SDF3_LOG} ${KITER_LOG}

# assumes that the benchmarks are all SDFs (rather than CSDFs) - use $SDF3ANALYSIS_CSDF for CSDF benchmarks
for BENCHMARK in ${BENCH_DIR}/*.xml; do
    GRAPH=${BENCHMARK##*/}
    GRAPH_NAME=${GRAPH%.xml}
    echo "running DSEs on ${GRAPH_NAME}...";
    # generate respective pareto logs
    echo ${KITER} -f ${BENCH_DIR}/${GRAPH_NAME}.xml -a KPeriodicThroughputwithDSE -p LOG=t;
    ${KITER} -f ${BENCH_DIR}/${GRAPH_NAME}.xml -a KPeriodicThroughputwithDSE -p LOG=t;
    mv ./data/pp_logs/${GRAPH_NAME}_pp_kiter.csv ${KITER_LOG}
    ${SDF3ANALYSIS_SDF} --graph ${BENCH_DIR}/${GRAPH_NAME}.xml --algo buffersize > ${LOG_DIR}/${GRAPH_NAME}_pp_sdf3.xml;
    # convert SDF3 DSE output to CSV (from XML):
    python xml_to_csv.py "${BENCH_DIR}/${GRAPH_NAME}.xml" "${LOG_DIR}/${GRAPH_NAME}_pp_sdf3.xml" "${SDF3_LOG}";
    echo "DSE complete"
done

# compare logs
TOTAL_TESTS=0
PASSED=0
for BENCHMARK in ${BENCH_DIR}/*.xml; do
    GRAPH=${BENCHMARK##*/}
    GRAPH_NAME=${GRAPH%.xml}
    SDF3_RES="${SDF3_LOG}/${GRAPH_NAME}_pp_sdf3.csv"
    KITER_RES="${KITER_LOG}/${GRAPH_NAME}_pp_kiter.csv"

    if [ -f ${SDF3_RES} ] && [ -f ${KITER_RES} ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        printf "Checking ${GRAPH_NAME}..."
        if diff -q <(sort $KITER_RES) <(sort $SDF3_RES); then # sort results to avoid mismatches due to ordering
            PASSED=$((PASSED + 1))
            echo "results match!"
        else
            echo "      Run 'diff -y <(sort $KITER_RES) <(sort $SDF3_RES)' to see differences"
            # diff -y <(sort $KITER_RES) <(sort $SDF3_RES)
        fi
    fi
done

if [ $PASSED -eq $TOTAL_TESTS ]; then
    echo "All tests passed; DSEs produce identical results!"
else
    echo "$PASSED/$TOTAL_TESTS matching DSE results"
fi
