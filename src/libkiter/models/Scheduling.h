/*
 * Scheduling.h
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_

#include <models/Dataflow.h>
#include <set>
#include <map>
#include <vector>

typedef std::map<ARRAY_INDEX,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> scheduling_t;
typedef std::set<Edge>  critical_circuit_t;

namespace models {

class Scheduling {
private:
	TIME_UNIT              _period ;
	const models::Dataflow*_dataflow;
	scheduling_t           _tasks_schedule;
	critical_circuit_t     _critical_edges;
public :
	Scheduling () : _period (0), _dataflow(nullptr)  {}
private :
	Scheduling (TIME_UNIT period, const models::Dataflow* dataflow) : _period (period), _dataflow(dataflow) {

	}

public :
	Scheduling (const models::Dataflow* dataflow, TIME_UNIT omega, scheduling_t tasks_schedule, critical_circuit_t critical_edges) : _period(omega), _dataflow(dataflow), _tasks_schedule(tasks_schedule), _critical_edges (critical_edges) {

	}

	TIME_UNIT getGraphPeriod () const {
			return this->_period;
	}

	TIME_UNIT getGraphThroughput () const {
			return 1.0 / this->_period;
	}
	const scheduling_t& getTaskSchedule () const{
		return this->_tasks_schedule;
	}

	const critical_circuit_t& getCriticalEdges() const {
		return this->_critical_edges;
	}

	void verbose_print ();

};
}

#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_ */
