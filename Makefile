
###
### Be aware of the timeout command in case of experiments !
### We also assume that ./tools/install_sdf3.sh has been already executed
###



SHELL = /bin/bash

SDF3_BENCHMARK := ./benchmarks/sdf3bench/
SDF3_MEM_BENCHMARK := ./benchmarks/sdf3mem/
SDF3_CS_BENCHMARK := ./benchmarks/sdf3cs/
SDF3_EXAMPLES := ./benchmarks/sdf3examples/
ASCENT_TESTBENCH := ./benchmarks/ascenttestbench/
SDF3_ROOT := $(shell pwd)/tools/sdf3/
SDF3_BINARY_ROOT := ${SDF3_ROOT}/sdf3_140724/sdf3/build/release/Linux/bin/
SDF3ANALYSIS_CSDF :=  timeout 180  ${SDF3_BINARY_ROOT}/sdf3analysis-csdf
SDF3ANALYSIS_SDF := timeout 180   ${SDF3_BINARY_ROOT}/sdf3analysis-sdf
KITER := timeout 180 ./Release/bin/kiter

SOURCES=$(shell find src tests)

WGET=sleep 20 && wget --tries=1

ifeq (,"${TRAVIS}")
ifeq (,$(shell find "${SDF3_BINARY_ROOT}/sdf3analysis-csdf" 2> /dev/null))
$(error Could not find SDF3 binary required for Travis.)
endif
endif	


info :
	@echo "-------------------------------------------"
	@echo SDF3_ROOT is [${SDF3_ROOT}]
	@echo SDF3_BINARY_ROOT is [${SDF3_BINARY_ROOT}]
	@echo "-------------------------------------------"
	@echo "make release: Compile Kiter in release mode"
	@echo "make debug: Compile Kiter in debug mode (more verbose, slower)"
	@echo "make unit_test: Run Kiter unit tests"
	@echo "make ubuntu_test: Run kiter with Ubuntu Docker"
	@echo "make test: Run Kiter user-level test"
	@echo "make sdf3_benchmarks: Download SDF3 benchmarks."
	@echo "make sdf3: Download and Compile SDF3 (required for Travis CI)."	
	@echo "-------------------------------------------"

all : release debug
	@echo "###########"" ENTER IN $@ : $^  #####################"

sdf3: ${SDF3_BINARY_ROOT}


debug: ./Debug/bin/kiter
	@echo "###########"" ENTER IN $@ : $^  #####################"

release: ./Release/bin/kiter
	@echo "###########"" ENTER IN $@ : $^  #####################"

clean:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm -rf Release Debug *.lp *.mps *.png *.dot *.pdf *.xml *.lp ./tools/.ipynb_checkpoints

cleanall: clean
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm  -rf ./logs/ tools/sdf3*/ sdf.log benchmarks/sdf3*/ benchmarks/ascenttestbench/

benchmark :  sdf.log  csdf.log csdf_sized.log
	@echo "###########"" ENTER IN $@ : $^  #####################"

ubuntu_test:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	docker build -f tools/docker/DockerFile.u18 -t bbodin/kiter-u18 ./

test: ./Release/bin/kiter 
	@echo "###########"" ENTER IN $@ : $^  #####################"
#Bodin2013 and Bodin2016
	mkdir -p ./logs
	rm -rf ./logs/*
	for f in  benchmarks/*.xml ; do echo === $$f ; ${KITER} -f $$f  -a 1PeriodicThroughput -a KPeriodicThroughput >> ./logs/1KBenchmarks.log  ; done
	for f in  benchmarks/IB5CSDF/*.xml ; do echo === $$f ; ${KITER} -f $$f  -a 1PeriodicThroughput -a KPeriodicThroughput >> ./logs/1KBenchmarks.log  ; done
	${KITER} -f benchmarks/sample.xml -a KPeriodicThroughput -pA=1 -pB=1 -pC=1 | grep -q 23.000000
	${KITER} -f benchmarks/sample.xml -a KPeriodicThroughput -pA=2 -pB=1 -pC=2 | grep -q 23.000000

# test:  Verify Kperiodic
	./tools/verify_kperiodic.sh benchmarks/21.xml benchmarks/expansion_paper_norm_sdf.xml benchmarks/expansion_paper_sdf.xml benchmarks/faustTest.xml benchmarks/multrate.xml benchmarks/new_benchmark.xml benchmarks/sample.xml benchmarks/simpler_benchmark.xml benchmarks/tiny_r.xml benchmarks/tiny.xml > ./logs/verifykperiodic.log
# test:  Verify KDSE
	./tools/verify_dse.sh benchmarks/21.xml benchmarks/expansion_paper_norm_sdf.xml benchmarks/expansion_paper_sdf.xml benchmarks/faustTest.xml benchmarks/new_benchmark.xml benchmarks/sample.xml benchmarks/simpler_benchmark.xml benchmarks/tiny_r.xml benchmarks/tiny.xml > ./logs/verifydse.log

# test:  KDSE
	./tools/kdse_run_benchmarks.sh ./logs/ 30 > ./logs/kdse_run_benchmarks.log  # Limit to 30 sec for CI

csdf_benchmarks.log:  ./Release/bin/kiter Makefile 
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  benchmarks/*.xml ; do echo === $$f >> $@;echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput  >> $@ ; if [ -d ${SDF3_BINARY_ROOT} ];then  ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true; fi ; done 

csdf_IB5CSDF.log:  ./Release/bin/kiter Makefile
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  benchmarks/IB5CSDF/*.xml ; do echo === $$f >> $@;echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput   >> $@  ; if [ -d ${SDF3_BINARY_ROOT} ];then  ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true; fi  ; done 

${SDF3_ROOT}/sdfg_throughput.zip :
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p ${SDF3_ROOT}
	${WGET} "http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_throughput.zip" -O $@

${SDF3_ROOT}/sdfg_buffersizing.zip :
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p ${SDF3_ROOT}
	${WGET} "http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_buffersizing.zip" -O $@

${SDF3_ROOT}/sdfg_designflow_case_study.zip:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p ${SDF3_ROOT}
	${WGET} "http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_designflow_case_study.zip" -O $@

sdf3_benchmarks : ${SDF3_BENCHMARK} ${SDF3_MEM_BENCHMARK}  ${SDF3_CS_BENCHMARK} ${SDF3_EXAMPLES}
	@echo "###########"" ENTER IN $@ : $^  #####################"



${SDF3_BINARY_ROOT} : ./tools/install_sdf3.sh
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@echo "Please run manually '$< ${SDF3_ROOT}' before running the Makefile"
	exit 1

${SDF3_CS_BENCHMARK} : ${SDF3_ROOT}/sdfg_designflow_case_study.zip
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<
	sed -i.bak "s/xsi:noNamespaceSchemaLocation=\"http:..www.es.ele.tue.nl.sdf3.xsd.sdf3-sdf.xsd\"//g" $@/designflow_case_study/*.xml

${SDF3_MEM_BENCHMARK} : ${SDF3_ROOT}/sdfg_buffersizing.zip
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<
	sed -i.bak "s/xsi:noNamespaceSchemaLocation=\"http:..www.es.ele.tue.nl.sdf3.xsd.sdf3-sdf.xsd\"//g" $@/*.xml

${SDF3_EXAMPLES} :
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	for example in h263decoder h263encoder mp3decoder_granule_parallelism mp3decoder_block_parallelism mp3playback satellite samplerate modem  ; do \
	${WGET} "http://www.es.ele.tue.nl/sdf3/download/files/examples/$$example.xml" -O ${SDF3_EXAMPLES}/$$example.xml ; done
	sed -i.bak "s/xsi:noNamespaceSchemaLocation=\"http:..www.es.ele.tue.nl.sdf3.xsd.sdf3-sdf.xsd\"//g" $@/*.xml

${SDF3_BENCHMARK} : ${SDF3_ROOT}/sdfg_throughput.zip
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs1/graphs.zip; for f in *.xml ; do mv $$f one_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs2/graphs.zip; for f in graph*.xml ; do mv $$f two_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs3/graphs.zip; for f in graph*.xml ; do mv $$f three_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs4/graphs.zip; for f in graph*.xml ; do mv $$f four_$$f ; done
	cd ${SDF3_BENCHMARK} && rm -rf graphs scripts sdfg_throughput.zip

${ASCENT_TESTBENCH} : ${SDF3_BENCHMARK}
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $</one_* $@/
	for f in $@/*_buffer.xml; do mv -- "$$f" "$${f%_buffer.xml}.xml" ; done
	sed -i.bak "s/.*=\"_.*//" $@/*.xml
	rm $@/*.bak


sdf.log:  ./Release/bin/kiter Makefile 
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f ;\
	echo === $$f === 1PeriodicThroughput  >> $@; ${KITER} -f $$f  -a 1PeriodicThroughput >> $@ ; \
	echo === $$f === KPeriodicThroughput  >> $@; ${KITER} -f $$f  -a KPeriodicThroughput >> $@ ; \
	echo === $$f === deGrooteThroughput  >> $@; ${KITER} -f $$f  -a deGrooteThroughput >> $@ ; \
	if [ -d ${SDF3_BINARY_ROOT} ];then echo === $$f === SDF3  >> $@; ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ; fi ;\
	done

%/bin/kiter: ${SOURCES}  %/Makefile
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@$(MAKE) -C $* all

%/Makefile: ${SOURCES} CMakeLists.txt
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@mkdir -p $*
	@pushd $* && cmake -D CMAKE_BUILD_TYPE=$* .. && popd

unit_test: ./Debug/Makefile ./Debug/bin/kiter
	@echo "###########"" ENTER IN $@ : $^  #####################"
	make -C Debug/ test



.PHONY:  sdf3 all infos  debug release clean benchmark ubuntu_test test  unit_test
.PRECIOUS: %/Makefile tools/sdf3/%.zip
