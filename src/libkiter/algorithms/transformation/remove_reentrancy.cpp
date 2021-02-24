/*
 * remove_reentrancy.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: toky
 */


#include "remove_reentrancy.h"
#include <models/Dataflow.h>

void algorithms::transformation::remove_reentrancy (models::Dataflow* const  dataflow, parameters_list_t params) {

	if (params.find("taskname") != params.end()) {
		auto names = commons::split<std::string>(params["taskname"], ',');
		for (auto name : names) {
			auto t = dataflow->getVertexByName(name);
			VERBOSE_INFO("Remove Reentracy for " << dataflow->getVertexName(t));
			dataflow->setReentrancyFactor(t, 0);
		}
	}  else if (params.find("taskid") != params.end()) {
		auto ids = commons::split<std::string>(params["taskid"], ',');
		for (auto id : ids) {
			ARRAY_INDEX  task_id = commons::fromString<ARRAY_INDEX>(id);
			auto t = dataflow->getVertexById(task_id);
			VERBOSE_INFO("Remove Reentracy for " << dataflow->getVertexName(t));
			dataflow->setReentrancyFactor(t, 0);
		}
	} else {
		VERBOSE_INFO("Remove Reentracy for every nodes");
		for (auto t : dataflow->vertices()) {
			dataflow->setReentrancyFactor(t, 0);
		}
	}
}

