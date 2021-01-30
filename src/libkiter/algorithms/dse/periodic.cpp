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
#include <algorithms/buffersizing/periodic.h>
#include <algorithms/throughput/kperiodic.h>

void algorithms::compute_csdf_dse_periodic   (models::Dataflow* const  dataflow, parameters_list_t params) {


	 bool writeLogFiles = false;
	 std::string dirName = "./data/";
	 // parse parameters for KDSE
	 if (params.find("LOGDIR") != params.end()) { // log output of DSE (includes pareto points and all search points)
		 writeLogFiles = true;
		 dirName = params["LOGDIR"];
	 }
	  std::string logDirName = dirName + "/dse_logs/";

    VERBOSE_INFO("Please note you can specify the INTEGERSOLVING.");
    bool INTEGERSOLVING = params.find("INTEGERSOLVING")!= params.end();


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
    std::ofstream dseLog;
     if (writeLogFiles) {
       dseLog.open(logDirName + dataflow->getGraphName() + "_dselog_periodic.csv");
       dseLog << "storage distribution size,throughput,channel quantities,computation duration,cumulative duration"
              << std::endl; // initialise headers
     }


    while (not range_to_explore.empty()) {

    	auto current_pair = range_to_explore.front();
    	range_to_explore.pop();

    	TIME_UNIT freq_left = current_pair.first;
    	TIME_UNIT freq_right = current_pair.second;

    	TIME_UNIT period_left = 1 / freq_left;
    	TIME_UNIT period_right =  1 / freq_right;

    	TIME_UNIT distance = freq_right - freq_left;
    	if (distance < MAX_GRANULARITY) continue;
    	if (not space.count(freq_left)) {
    	    auto startTime = std::chrono::steady_clock::now();
    		space[freq_left] = algorithms::periodic_memory_sizing_csdf( dataflow,  period_left,  INTEGERSOLVING , false);
    	    auto endTime = std::chrono::steady_clock::now();
    	    std::chrono::duration<double, std::milli> execTime = endTime - startTime; // duration in ms
    	    cumulativeTime += execTime;

    	    if (writeLogFiles and space[freq_left] > 0) {
        	    dseLog << space[freq_left] << ","
        				<< freq_left << ","
    					<<"" << ","
    					<< execTime.count() << ","
    					<< cumulativeTime.count() << std::endl;
    	    }
    	}
    	if (not space.count(freq_right)){

    	    auto startTime = std::chrono::steady_clock::now();
    		space[freq_right] = algorithms::periodic_memory_sizing_csdf( dataflow,  period_right,  INTEGERSOLVING , false);
    	    auto endTime = std::chrono::steady_clock::now();
    	    std::chrono::duration<double, std::milli> execTime = endTime - startTime; // duration in ms
    	    cumulativeTime += execTime;

    	    if (writeLogFiles and space[freq_right] > 0) {
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

    	if (size_left != size_right) {
        	VERBOSE_INFO("   Distance = " << distance);
    		range_to_explore.push(std::pair<TIME_UNIT,TIME_UNIT>(freq_left,freq_left + distance / 2.0));
    		range_to_explore.push(std::pair<TIME_UNIT,TIME_UNIT>(freq_left + distance / 2.0, freq_right));
    	}


    }

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

    if (writeLogFiles) {
       dseLog.close();
    }
}
