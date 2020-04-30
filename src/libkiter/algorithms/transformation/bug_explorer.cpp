/*
 * bug_explorer.cpp
 *
 *  Created on: Nov 22, 2019
 *      Author: toky
 */
#include <cstdio>
#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <algorithms/transformations.h>

#include <commons/commons.h>
#include <commons/SDF3Wrapper.h>


TIME_UNIT runKiterThroughput(models::Dataflow* const  dataflow) {
	models::Scheduling  sched = algorithms::scheduling::CSDF_KPeriodicScheduling(dataflow);
	return 1.0 / sched.getGraphPeriod();
}

bool compareKiterVsSDF3Results (models::Dataflow* const  dataflow, TIME_UNIT epsilon = std::numeric_limits<TIME_UNIT>::epsilon()) {
	  VERBOSE_INFO("runSDF3Throughput");
	  TIME_UNIT sdf3_res = commons::runSDF3Throughput(dataflow);
	  VERBOSE_INFO("runKiterThroughput");
	  TIME_UNIT kiter_res = runKiterThroughput(dataflow);
	  VERBOSE_INFO(std::scientific << std::setw( 11 ) << std::setprecision( 9 ) << " sdf3_res = " << sdf3_res << " kiter_res = " << kiter_res);
	  VERBOSE_INFO(std::scientific << std::setw( 11 ) << std::setprecision( 9 ) << " delta = " << std::abs(sdf3_res - kiter_res) << " epsilon = " << epsilon);
	  return not (std::abs(sdf3_res - kiter_res) > epsilon);
}

void algorithms::transformation::compare_sdf3_throughput (models::Dataflow* const  dataflow, parameters_list_t) {
	TIME_UNIT epsilon = std::numeric_limits<TIME_UNIT>::epsilon();
	VERBOSE_INFO("runSDF3Throughput");
	TIME_UNIT sdf3_res = commons::runSDF3Throughput(dataflow);
	VERBOSE_INFO("runKiterThroughput");
	TIME_UNIT kiter_res = runKiterThroughput(dataflow);
	VERBOSE_INFO(std::scientific << std::setw( 11 ) << std::setprecision( 9 ) << " sdf3_res = " << sdf3_res << " kiter_res = " << kiter_res);
	VERBOSE_INFO(std::scientific << std::setw( 11 ) << std::setprecision( 9 ) << " delta = " << std::abs(sdf3_res - kiter_res) << " epsilon = " << epsilon);
	bool result =  not (std::abs(sdf3_res - kiter_res) > epsilon);

	  std::cout << "Kiter: " << kiter_res << std::endl;
	  std::cout << "SDF3: " << sdf3_res << std::endl;
	  std::cout << "result: " << result << std::endl;

}

void algorithms::transformation::remove_task (models::Dataflow* const  dataflow, parameters_list_t params) {

	if (params.find("taskname") != params.end()) {
		VERBOSE_INFO("taskname = " << params["taskname"]);
		auto names = commons::split<std::string>(params["taskname"], ',');
		for (auto name : names) {
			auto t = dataflow->getVertexByName(name);
			VERBOSE_INFO("removeVertex " << dataflow->getVertexName(t));
			dataflow->removeVertex(t);
		}
	}

	if (params.find("taskid") != params.end()) {
		VERBOSE_INFO("taskid = " << params["taskid"]);
		auto ids = commons::split<std::string>(params["taskid"], ',');
		for (auto id : ids) {
			ARRAY_INDEX  task_id = commons::fromString<ARRAY_INDEX>(id);
			auto t = dataflow->getVertexById(task_id);
			VERBOSE_INFO("removeVertex " << dataflow->getVertexName(t));
			dataflow->removeVertex(t);
		}
	}

	VERBOSE_INFO("Removal done");
}
