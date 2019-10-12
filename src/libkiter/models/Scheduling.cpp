/*
 * Scheduling.cpp
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#include <models/Scheduling.h>
#include <models/Dataflow.h>

void models::Scheduling::verbose_print() {
	for (auto  key : this->_tasks_schedule) {
			auto task = key.first;
			auto task_vtx = _dataflow->getVertexById(key.first);
			VERBOSE_INFO ( "Task " <<  _dataflow->getVertexName(task_vtx)
					<<  " : duration=[ " << commons::toString(_dataflow->getVertexPhaseDuration(task_vtx)) <<  "]"
					<<     " period=" <<  _tasks_schedule[task].first << " HP=" << *HP << " Ni=" << _dataflow->getNi(task_vtx)
					<<     " starts=[ " << commons::toString(_tasks_schedule[task].second) << "]");

	}
}


