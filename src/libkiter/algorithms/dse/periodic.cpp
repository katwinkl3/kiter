/*
 * periodic.cpp
 *
 *  Created on: Apr 9, 2020
 *      Author: toky
 */

#include "periodic.h"
#include <map>
#include <chrono> // to take computation timings
#include <queue>
#include <commons/commons.h>
#include <algorithms/buffersizing.h>
#include <algorithms/buffersizing/periodic.h>
#include <algorithms/buffersizing/periodic_fixed.h> // include speriodic
#include <algorithms/throughput/kperiodic.h>


void algorithms::compute_csdf_dse_speriodic   (models::Dataflow* const  dataflow, parameters_list_t params) {

    bool      INTEGERSOLVING = commons::get_parameter<bool>(params, "ILP", false) ;

    const BufferSizingFun& sizing_fun = [INTEGERSOLVING](models::Dataflow* const d, TIME_UNIT p) -> BufferSizingResult {
		return algorithms::speriodic_memory_sizing_csdf( d,  p,  INTEGERSOLVING , false);
	};

    bool writeLogFiles = false;
   	 std::string logDirName = "./data/";
   	 // parse parameters for KDSE
   	 if (params.find("LOGDIR") != params.end()) { // log output of DSE (includes pareto points and all search points)
   		 writeLogFiles = true;
   		logDirName = params["LOGDIR"];
   	 } else {
   		 VERBOSE_WARNING("LOGDIR not specified");
   	 }

   	  std::ostream* dseout = &std::cout;

   	  std::ofstream dseLog;
   	  if (writeLogFiles) {
   		  dseLog.open(logDirName + dataflow->getGraphName() + "_dselog_speriodic.csv");
   		  dseout = &dseLog;
   	  }

      algorithms::compute_csdf_dse_from_function (dataflow,sizing_fun, *dseout);


	  if (writeLogFiles) {
		  dseLog.close();
	  }
}



void algorithms::compute_csdf_dse_periodic   (models::Dataflow* const  dataflow, parameters_list_t params) {

    bool      INTEGERSOLVING = commons::get_parameter<bool>(params, "ILP", false) ;

    const BufferSizingFun& sizing_fun = [INTEGERSOLVING](models::Dataflow* const d, TIME_UNIT p) -> BufferSizingResult {
		return algorithms::periodic_memory_sizing_csdf( d,  p,  INTEGERSOLVING , false);
	};


	 bool writeLogFiles = false;
	 std::string logDirName = "./data/";
	 // parse parameters for KDSE
	 if (params.find("LOGDIR") != params.end()) { // log output of DSE (includes pareto points and all search points)
		 writeLogFiles = true;
		 logDirName = params["LOGDIR"];
	 } else {
		 VERBOSE_WARNING("LOGDIR not specified");
	 }

	  std::ostream* dseout = &std::cout;

	  std::ofstream dseLog;
	  if (writeLogFiles) {
		  dseLog.open(logDirName + dataflow->getGraphName() + "_dselog_periodic.csv");
		  dseout = &dseLog;
	  }

	  algorithms::compute_csdf_dse_from_function (dataflow,sizing_fun, *dseout);

	  if (writeLogFiles) {
		  dseLog.close();
	  }
}
void algorithms::compute_csdf_dse_from_function   (models::Dataflow* const  dataflow, const BufferSizingFun& sizing_fun, std::ostream& dseLog) {

	VERBOSE_DEBUG("compute_csdf_dse_from_function");



    // STEP 1 -- 1 Periodic scheduling to get PERIOD
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_INFO("Please note you cna use the print parameter.");

    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}



    kperiodic_result_t result = KSchedule(dataflow,&kvector);
    TIME_UNIT FREQUENCY = result.throughput;
    TIME_UNIT MAX_GRANULARITY =FREQUENCY / 10000;
    //TIME_UNIT PERIOD = 1 / result.first;


    std::map<TIME_UNIT,TOKEN_UNIT> space;
    space[0] = 0;

    std::queue<std::pair<TIME_UNIT,TIME_UNIT>> range_to_explore;
    range_to_explore.push(std::pair<TIME_UNIT,TIME_UNIT>(0,FREQUENCY));

    std::chrono::duration<double, std::milli> cumulativeTime; // store timings

    dseLog << "storage distribution size,throughput,channel quantities,computation duration,cumulative duration"
              << std::endl; // initialise headers


    while (not range_to_explore.empty()) {

    	auto current_pair = range_to_explore.front();
    	range_to_explore.pop();

    	TIME_UNIT freq_left = current_pair.first;
    	TIME_UNIT freq_right = current_pair.second;


    	VERBOSE_DEBUG("  DSE freqs are " << freq_left << " - " << freq_right);

    	TIME_UNIT period_left = 1 / freq_left;
    	TIME_UNIT period_right =  1 / freq_right;

    	TIME_UNIT distance = freq_right - freq_left;
    	if (distance < MAX_GRANULARITY) {

        	VERBOSE_DEBUG(" SKIP ");
    		continue;
    	}
    	if (not space.count(freq_left)) {

        	VERBOSE_DEBUG(" DO freq_left ");
    	    auto startTime = std::chrono::steady_clock::now();
    		space[freq_left] = sizing_fun( dataflow,  period_left).total_size();
    	    auto endTime = std::chrono::steady_clock::now();
    	    std::chrono::duration<double, std::milli> execTime = endTime - startTime; // duration in ms
    	    cumulativeTime += execTime;

    	    dseLog << space[freq_left] << ","
        				<< freq_left << ","
    					<<"" << ","
    					<< execTime.count() << ","
    					<< cumulativeTime.count() << std::endl;

    	}
    	if (not space.count(freq_right)){

        	VERBOSE_DEBUG(" DO freq_right ");
    	    auto startTime = std::chrono::steady_clock::now();
    		space[freq_right] = sizing_fun( dataflow,  period_right).total_size();
    	    auto endTime = std::chrono::steady_clock::now();
    	    std::chrono::duration<double, std::milli> execTime = endTime - startTime; // duration in ms
    	    cumulativeTime += execTime;

    	    if (space[freq_right] > 0) {
    	    	dseLog << space[freq_right] << ","
            				<< freq_right << ","
        					<<"" << ","
        					<< execTime.count() << ","
        					<< cumulativeTime.count() << std::endl;
    	    }
    	}
    	TOKEN_UNIT size_left = space[freq_left];
    	TOKEN_UNIT size_right = space[freq_right];

    	//VERBOSE_INFO("With Period of " << period_left << " Size is " << size_left);
    	//VERBOSE_INFO("With Period of " << period_right << " Size is " << size_right);

    	if (size_left != size_right or freq_left == 0) {
        	VERBOSE_INFO(" (size_left != size_right)  Distance = " << distance);
    		range_to_explore.push(std::pair<TIME_UNIT,TIME_UNIT>(freq_left,freq_left + distance / 2.0));
    		range_to_explore.push(std::pair<TIME_UNIT,TIME_UNIT>(freq_left + distance / 2.0, freq_right));
    	}


    }

 	VERBOSE_DEBUG(" END OF LOOP ");

    std::map<TOKEN_UNIT,TIME_UNIT> pareto;
    for (auto item : space) {
    	TIME_UNIT F =  item.first ;
    	if (!F) continue;
    	TIME_UNIT P = 1 / F;
    	TOKEN_UNIT S = item.second;
    	if (not pareto.count(S)) {pareto[S] = P;}
    	if (pareto[S] > P) {pareto[S] = P;}

    }
    for (auto item : pareto) {
    	TIME_UNIT P =item.second;
    	TOKEN_UNIT S = item.first;
    	if (S > 0)
    	std::cout << P << "\t" << S << std::endl ;
    }


}
