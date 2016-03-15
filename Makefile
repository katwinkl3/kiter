SHELL = /bin/bash

CPU_COUNT := $(shell cat /proc/cpuinfo |grep processor |wc -l)
SDF3_BINARY_ROOT := /home/toky/these/tools/sdf3/build/release/Linux/bin/
SDF3ANALYSIS_CSDF := timeout 60  ${SDF3_BINARY_ROOT}/sdf3analysis-csdf
SDF3ANALYSIS_SDF := timeout 60   ${SDF3_BINARY_ROOT}/sdf3analysis-sdf
SDF3_BENCHMARK := ./sdf3bench/

all: release_build debug_build
	@echo "###########"" ENTER IN all: build #####################"
clean:
	rm -Rf release debug

benchmark :  sdf.log  csdf.log csdf_sized.log

csdf.log: ./release/bin/kiter Makefile 
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  benchmark/*.xml ; do echo === $$f >> $@;echo === $$f  ; ./release/bin/kiter -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@ ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true ; done 
	for f in  benchmark/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@ ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true ; done 
	for f in  benchmark/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@ ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true ; done 
	for f in  benchmark/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@ ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true ; done 
	for f in  benchmark/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@ ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true ; done 

csdf_sized.log: ./release/bin/kiter Makefile
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  benchmark_sized/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@  ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true  ; done 
	for f in  benchmark_sized/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@  ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true  ; done 
	for f in  benchmark_sized/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@  ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true  ; done 
	for f in  benchmark_sized/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@  ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true  ; done 
	for f in  benchmark_sized/*.xml ; do echo === $$f >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  >> $@  ; ${SDF3ANALYSIS_CSDF}  --graph $$f  --algo throughput  >> $@ || true  ; done 

sdfg_throughput.zip :
	wget http://www.es.ele.tue.nl/sdf3/download/files/benchmarks/sdfg_throughput.zip

${SDF3_BENCHMARK} : sdfg_throughput.zip
	mkdir -p ${SDF3_BENCHMARK}
	cp sdfg_throughput.zip ${SDF3_BENCHMARK}/
	cd ${SDF3_BENCHMARK} ; unzip sdfg_throughput.zip 
	cd ${SDF3_BENCHMARK} ; unzip graphs/graphs1/graphs.zip; for f in *.xml ; do mv $$f one_$$f ; done
	cd ${SDF3_BENCHMARK} ; unzip graphs/graphs2/graphs.zip; for f in graph*.xml ; do mv $$f two_$$f ; done
	cd ${SDF3_BENCHMARK} ; unzip graphs/graphs3/graphs.zip; for f in graph*.xml ; do mv $$f three_$$f ; done
	cd ${SDF3_BENCHMARK} ; unzip graphs/graphs4/graphs.zip; for f in graph*.xml ; do mv $$f four_$$f ; done

sdf.log: ./release/bin/kiter Makefile ${SDF3_BENCHMARK}
	rm -f $@
	@echo "==============================================================================================="
	@echo "==============================================================================================="
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f  >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  -a deGrooteThroughput >> $@ ; ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ;  done
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f  >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  -a deGrooteThroughput >> $@ ; ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ;  done
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f  >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  -a deGrooteThroughput >> $@ ; ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ;  done
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f  >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  -a deGrooteThroughput >> $@ ; ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ;  done
	for f in  ${SDF3_BENCHMARK}/*.xml ; do echo === $$f  >> $@;echo === $$f ; ./release/bin/kiter -f $$f -a PeriodicThroughput  -a 1PeriodicThroughput -a KPeriodicThroughput -a KBisPeriodicThroughput -a KTerPeriodicThroughput  -a deGrooteThroughput >> $@ ; ${SDF3ANALYSIS_SDF}  --graph $$f  --algo throughput  >> $@ || true ;  done

release_build : release/Makefile
	@echo "###########"" ENTER IN realease_build : release/Makefile  #####################"
	@$(MAKE) -C release all


debug_build : debug/Makefile
	@echo "###########"" ENTER IN realease_build : release/Makefile  #####################"
	@$(MAKE) -C debug all 

release/Makefile: CMakeLists.txt
	@echo "###########"" ENTER IN build/Makefile: CMakeLists.txt #####################"
	@mkdir -p release
	@pushd release && cmake -D CMAKE_BUILD_TYPE=Release .. && popd

debug/Makefile: CMakeLists.txt
	@echo "###########"" ENTER IN build/Makefile: CMakeLists.txt #####################"
	@mkdir -p debug
	@pushd debug && cmake -D CMAKE_BUILD_TYPE=Debug .. && popd



.PHONY : release_build debug_build all clean tests test benchmark
