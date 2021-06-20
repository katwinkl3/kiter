/*
 * Scheduling.cpp
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#include <models/SchedulingMod.h>
#include <models/Dataflow.h>

void models::SchedulingMod::verbose_print() {
	for (auto  key : this->_tasks_schedule) {
			auto task = key.first;
			auto task_vtx = _dataflow->getVertexById(key.first);
			// VERBOSE_INFO ( "Task " <<  _dataflow->getVertexName(task_vtx)
			// 		<<  " : duration=[ " << commons::toString(_dataflow->getVertexPhaseDuration(task_vtx)) <<  "]"
			// 		<<     " period=" <<  key.second.periodic_starts.first << " Ni=" << _dataflow->getNi(task_vtx)
			// 		<<     " starts=[ " << commons::toString(key.second.initial_starts) << "]"
			// 		);
			std::cout << "Task " <<  _dataflow->getVertexName(task_vtx)
					<<  " : duration=[ " << commons::toString(_dataflow->getVertexPhaseDuration(task_vtx)) <<  "]"
					<<     " starts=[ " << commons::toString(key.second.initial_starts) << "]"
					<<     " period=" <<  key.second.periodic_starts.first 
					<<     " periods=[ " << commons::toString(key.second.periodic_starts.second) << "]"
					<< std::endl;

	}
}
