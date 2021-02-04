#!/bin/bash

#set -x

KITER_ROOT_DIR="./"

XML2CSV="${KITER_ROOT_DIR}/tools/xml_to_csv.py"
SDF3_BINARY_ROOT="${KITER_ROOT_DIR}/tools/sdf3/sdf3_140724/sdf3/build/release/Linux/bin/"
SDF3ANALYSIS_CSDF="timeout --foreground 180 ${SDF3_BINARY_ROOT}/sdf3analysis-csdf"
SDF3ANALYSIS_SDF="timeout --foreground 180 ${SDF3_BINARY_ROOT}/sdf3analysis-sdf"
KITER="timeout --foreground 180 ${KITER_ROOT_DIR}/Release/bin/kiter"


LOG_DIR=${KITER_ROOT_DIR}/logs/dse_verification/
SDF3_LOG=${LOG_DIR}/sdf3_pp_logs
KITER_LOG=${LOG_DIR}/kiter_pp_logs


# make directories to store logs for verification
mkdir -p ${LOG_DIR} ${SDF3_LOG} ${KITER_LOG}

#BENCH_DIR=${KITER_ROOT_DIR}/benchmarks/sdf3mem/ # set to directory containing benchmarks
#BENCHMARK_LIST=$(find "${BENCH_DIR}"/*.xml)

echo "Arguments: $*"
BENCHMARK_ARRAY=( "$@" )








# compare logs
TOTAL_TESTS=0
PASSED=0

# assumes that the benchmarks are all SDFs (rather than CSDFs) - use $SDF3ANALYSIS_CSDF for CSDF benchmarks
for BENCHMARK in "${BENCHMARK_ARRAY[@]}"; do
    GRAPH=${BENCHMARK##*/}
    GRAPH_NAME=${GRAPH%.xml}
    echo "running DSEs on ${GRAPH_NAME}...";
    # generate respective pareto logs
    echo "${KITER}" -f "${BENCHMARK}" -a KPeriodicThroughputwithDSE -p LOGDIR=${LOG_DIR};
    ${KITER} -f "${BENCHMARK}" -a KPeriodicThroughputwithDSE -p LOGDIR=${LOG_DIR} > /dev/null;
    mv "${LOG_DIR}"/pp_logs/*.csv "${KITER_LOG}/${GRAPH_NAME}"_pp_kiter.csv
    if head -n 3 "${BENCHMARK}" | grep csdf > /dev/null; then
	echo "${SDF3ANALYSIS_CSDF} --graph \"${BENCHMARK}\" --algo buffersize"
	${SDF3ANALYSIS_CSDF} --graph "${BENCHMARK}" --algo buffersize > tmp.xml;
	grep -v "analysis time" tmp.xml >  "${LOG_DIR}/${GRAPH_NAME}"_pp_sdf3.xml
	rm tmp.xml
    else
	echo "${SDF3ANALYSIS_SDF} --graph \"${BENCHMARK}\" --algo buffersize"
	${SDF3ANALYSIS_SDF} --graph "${BENCHMARK}" --algo buffersize > "${LOG_DIR}/${GRAPH_NAME}"_pp_sdf3.xml;
    fi
    # convert SDF3 DSE output to CSV (from XML):
    ${XML2CSV} "${BENCHMARK}" "${LOG_DIR}/${GRAPH_NAME}_pp_sdf3.xml" "${SDF3_LOG}";
    echo "DSE complete"

    
    SDF3_RES="${SDF3_LOG}/${GRAPH_NAME}_pp_sdf3.csv"
    KITER_RES="${KITER_LOG}/${GRAPH_NAME}_pp_kiter.csv"

    
    if [ -f "${SDF3_RES}" ] && [ -f "${KITER_RES}" ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        printf "Checking %s" "${GRAPH_NAME}..."
        if diff -q <(sort "$KITER_RES") <(sort "$SDF3_RES") > /dev/null; then # sort results to avoid mismatches due to ordering
            PASSED=$((PASSED + 1))
            echo "results match!"
        else
            echo "failed! Run 'diff -y <(sort $KITER_RES) <(sort $SDF3_RES)' to see differences"
            # diff -y <(sort $KITER_RES) <(sort $SDF3_RES)
        fi
    else
	echo "***** One of them failed."
    fi
done

if [ $PASSED -eq $TOTAL_TESTS ]; then
    echo "All tests passed; DSEs produce identical results!"
else
    echo "$PASSED/$TOTAL_TESTS matching DSE results"
fi
