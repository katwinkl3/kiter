
###
### Be aware of the timeout command in case of experiments !
###

SHELL = /bin/bash
BUILDS = Debug/bin/kiter

CPU_COUNT := $(shell cat /proc/cpuinfo |grep processor |wc -l)
SDF3_BENCHMARK := ./sdf3bench/
SDF3_MEM_BENCHMARK := ./sdf3mem/
SDF3_CS_BENCHMARK := ./sdf3cs/

SDF3_ROOT := ./sdf3/
SDF3_BINARY_ROOT := ${SDF3_ROOT}/sdf3/build/release/Linux/bin/
SDF3ANALYSIS_CSDF :=  timeout 180  ${SDF3_BINARY_ROOT}/sdf3analysis-csdf
SDF3ANALYSIS_SDF := timeout 180   ${SDF3_BINARY_ROOT}/sdf3analysis-sdf
KITER := timeout 180 ./Release/bin/kiter

SOURCES=$(shell find src tests)

info :
	@echo "----------------------------------------"
	@echo SDF3_ROOT is [${SDF3_ROOT}]
	@echo SDF3_BINARY_ROOT is [${SDF3_BINARY_ROOT}]
	@echo "----------------------------------------"
	@echo "make build"
	@echo "make travis_test"
	@echo "make sdf3_build"
	@echo "----------------------------------------"

all : build
	@echo "###########"" ENTER IN $@ : $^  #####################"

build: ${BUILDS}
	@echo "###########"" ENTER IN $@ : $^  #####################"
clean:
	rm -Rf Release Debug *.lp *.mps 

benchmark :  sdf.log  csdf.log csdf_sized.log

travis_test: ${SDF3_BENCHMARK}
	for f in  benchmark/*.xml ; do echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput   ; done
	for f in  benchmark_sized/*.xml ; do echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput ;  done
	${KITER} -f benchmark/sample.xml -a PrintKPeriodicThroughput -pA=1 -pB=1 -pC=1
	${KITER} -f benchmark/sample.xml -a PrintKPeriodicThroughput -pA=2 -pB=1 -pC=2

csdf.log:  ./Release/bin/kiter Makefile 
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  benchmark/*.xml ; do echo === $$f >> $@;echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput  >> $@ ; if [ -d ${SDF3_BINARY_ROOT} ];then  ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true; fi ; done 

csdf_sized.log:  ./Release/bin/kiter Makefile
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  benchmark_sized/*.xml ; do echo === $$f >> $@;echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput   >> $@  ; if [ -x ${SDF3_BINARY_ROOT} ];then  ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true; fi  ; done 

sdfg_throughput.zip :
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_throughput.zip

sdf3-140724.zip :
	wget http://www.es.ele.tue.nl/sdf3/download/files/releases/sdf3-140724.zip

sdfg_buffersizing.zip :
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_buffersizing.zip

sdfg_designflow_case_study.zip:
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_designflow_case_study.zip

sdf3_build : ${SDF3_BINARY_ROOT} ${SDF3_BENCHMARK} ${SDF3_MEM_BENCHMARK}  ${SDF3_CS_BENCHMARK}

${SDF3_BINARY_ROOT} : sdf3-140724.zip 
	mkdir -p ${SDF3_ROOT}
	cp sdf3-140724.zip ${SDF3_ROOT}
	cd ${SDF3_ROOT} && unzip -o sdf3-140724.zip
	cd ${SDF3_ROOT}/sdf3/ && make

${SDF3_CS_BENCHMARK} : sdfg_designflow_case_study.zip
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<

${SDF3_MEM_BENCHMARK} : sdfg_buffersizing.zip
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<

${SDF3_BENCHMARK} : sdfg_throughput.zip
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs1/graphs.zip; for f in *.xml ; do mv $$f one_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs2/graphs.zip; for f in graph*.xml ; do mv $$f two_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs3/graphs.zip; for f in graph*.xml ; do mv $$f three_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs4/graphs.zip; for f in graph*.xml ; do mv $$f four_$$f ; done
	cd ${SDF3_BENCHMARK} && rm graphs scripts sdfg_throughput.zip -rf

sdf.log:  ./Release/bin/kiter Makefile 
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f  >> $@;echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput  -a deGrooteThroughput >> $@ ; if [ -d ${SDF3_BINARY_ROOT} ];then ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ; fi ;  done

%/bin/kiter: ${SOURCES}  %/Makefile
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@$(MAKE) -C $* all

%/Makefile: ${SOURCES} CMakeLists.txt
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@mkdir -p $*
	@pushd $* && cmake -D CMAKE_BUILD_TYPE=$* .. && popd

test: Debug/bin/kiter Debug/Makefile
	make -C Debug/ test

various:
	./Release/bin/kiter -f ./benchmark/21.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/BlackScholes.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/Echo.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/expansion_paper_norm_sdf.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/expansion_paper_sdf.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/H264.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/JPEG2000.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/new_benchmark.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/Pdectect.xml -a KPeriodicThroughput -v 5
	./Release/bin/kiter -f ./benchmark/sample.xml -a KPeriodicThroughput -v 5

.PHONY :  all clean tests various test benchmark
