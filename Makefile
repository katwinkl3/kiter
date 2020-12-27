
###
### Be aware of the timeout command in case of experiments !
###

SHELL = /bin/bash

SDF3_ARCHIVE := sdf3-140724.zip 

CPU_COUNT := $(shell cat /proc/cpuinfo |grep processor |wc -l)
SDF3_BENCHMARK := ./benchmarks/sdf3bench/
SDF3_MEM_BENCHMARK := ./benchmarks/sdf3mem/
SDF3_CS_BENCHMARK := ./benchmarks/sdf3cs/
SDF3_EXAMPLES := ./benchmarks/sdf3examples/
SDF3_ROOT := `pwd`/tools/sdf3/
SDF3_BINARY_ROOT := ${SDF3_ROOT}/sdf3/build/release/Linux/bin/
SDF3ANALYSIS_CSDF :=  timeout 180  ${SDF3_BINARY_ROOT}/sdf3analysis-csdf
SDF3ANALYSIS_SDF := timeout 180   ${SDF3_BINARY_ROOT}/sdf3analysis-sdf
KITER := timeout 180 ./Release/bin/kiter

SOURCES=$(shell find src tests)

all : release
	@echo "###########"" ENTER IN $@ : $^  #####################"

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
	@echo "make sdf3_build: Download SDF3 source and benchmarks and compile it."
	@echo "-------------------------------------------"

debug: ./Debug/bin/kiter
	@echo "###########"" ENTER IN $@ : $^  #####################"

release: ./Release/bin/kiter
	@echo "###########"" ENTER IN $@ : $^  #####################"

clean:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm -Rf Release Debug *.lp *.mps *.png *.dot *.pdf *.xml *.lp

benchmark :  sdf.log  csdf.log csdf_sized.log
	@echo "###########"" ENTER IN $@ : $^  #####################"

ubuntu_test:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	docker build -f tools/docker/DockerFile.u18 -t bbodin/kiter-u18 ./

test: ./Release/bin/kiter 
	@echo "###########"" ENTER IN $@ : $^  #####################"
	for f in  benchmarks/*.xml ; do echo === $$f ; ${KITER} -f $$f  -a 1PeriodicThroughput -a KPeriodicThroughput   ; done
	for f in  benchmarks/IB5CSDF/*.xml ; do echo === $$f ; ${KITER} -f $$f  -a 1PeriodicThroughput -a KPeriodicThroughput   ; done
	${KITER} -f benchmarks/sample.xml -a KPeriodicThroughput -pA=1 -pB=1 -pC=1
	${KITER} -f benchmarks/sample.xml -a KPeriodicThroughput -pA=2 -pB=1 -pC=2

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
	for f in  benchmarks/IB5CSDF/*.xml ; do echo === $$f >> $@;echo === $$f ; ${KITER} -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput   >> $@  ; if [ -x ${SDF3_BINARY_ROOT} ];then  ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true; fi  ; done 

sdfg_throughput.zip :
	@echo "###########"" ENTER IN $@ : $^  #####################"
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_throughput.zip

${SDF3_ARCHIVE}:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	wget http://www.es.ele.tue.nl/sdf3/download/files/releases/sdf3-140724.zip

sdfg_buffersizing.zip :
	@echo "###########"" ENTER IN $@ : $^  #####################"
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_buffersizing.zip

sdfg_designflow_case_study.zip:
	@echo "###########"" ENTER IN $@ : $^  #####################"
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_designflow_case_study.zip

sdf3_build : ${SDF3_BENCHMARK} ${SDF3_MEM_BENCHMARK}  ${SDF3_CS_BENCHMARK} ${SDF3_EXAMPLES} ${SDF3_BINARY_ROOT} 
	@echo "###########"" ENTER IN $@ : $^  #####################"



${SDF3_BINARY_ROOT} : ${SDF3_ARCHIVE}
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p ${SDF3_ROOT}
	cp ${SDF3_ARCHIVE} ${SDF3_ROOT}/
	cd ${SDF3_ROOT} && unzip -o ${SDF3_ARCHIVE}
	cd "${SDF3_ROOT}/sdf3/" && make ## TODO : This line of code is not working with recursive Makefiles

${SDF3_CS_BENCHMARK} : sdfg_designflow_case_study.zip
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<

${SDF3_MEM_BENCHMARK} : sdfg_buffersizing.zip
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<

${SDF3_EXAMPLES} :
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	for example in h263decoder h263encoder mp3decoder_granule_parallelism mp3decoder_block_parallelism mp3playback satellite samplerate modem  ; do \
	wget -nc "http://www.es.ele.tue.nl/sdf3/download/files/examples/$$example.xml" -O ${SDF3_EXAMPLES}/$$example.xml ; done

${SDF3_BENCHMARK} : sdfg_throughput.zip
	@echo "###########"" ENTER IN $@ : $^  #####################"
	mkdir -p $@
	cp $< $@/
	cd $@ && unzip $<
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs1/graphs.zip; for f in *.xml ; do mv $$f one_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs2/graphs.zip; for f in graph*.xml ; do mv $$f two_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs3/graphs.zip; for f in graph*.xml ; do mv $$f three_$$f ; done
	cd ${SDF3_BENCHMARK} && unzip graphs/graphs4/graphs.zip; for f in graph*.xml ; do mv $$f four_$$f ; done
	cd ${SDF3_BENCHMARK} && rm graphs scripts sdfg_throughput.zip -rf

sdf.log:  ./Release/bin/kiter Makefile 
	@echo "###########"" ENTER IN $@ : $^  #####################"
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f ;\
	echo === $$f === 1PeriodicThroughput  >> $@; ${KITER} -f $$f  -a 1PeriodicThroughput >> $@ ; \
	echo === $$f === KPeriodicThroughput  >> $@; ${KITER} -f $$f  -a KPeriodicThroughput >> $@ ; \
	echo === $$f === deGrooteThroughput  >> $@; ${KITER} -f $$f  -a deGrooteThroughput >> $@ ; \
	if [ -d ${SDF3_BINARY_ROOT} ];then ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ; fi ;\
	done

%/bin/kiter: ${SOURCES}  %/Makefile
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@$(MAKE) -C $* all

%/Makefile: ${SOURCES} CMakeLists.txt
	@echo "###########"" ENTER IN $@ : $^  #####################"
	@mkdir -p $*
	@pushd $* && cmake -D CMAKE_BUILD_TYPE=$* .. && popd

unit_test: ./Debug/bin/kiter
	@echo "###########"" ENTER IN $@ : $^  #####################"
	make -C Debug/ test



.PHONY :  all infos  debug release clean benchmark ubuntu_test test  unit_test
