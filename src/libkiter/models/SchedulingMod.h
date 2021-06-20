/*
 * Scheduling.h
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULINGMOD_SCHEDULINGMOD_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULINGMOD_SCHEDULINGMOD_H_

#include <models/Dataflow.h>
#include <set>
#include <map>
#include <vector>

typedef std::vector<TIME_UNIT> static_task_schedule_t;
typedef std::pair<TIME_UNIT, static_task_schedule_t> periodic_task_schedule_t;
struct task_schedule_t {
	static_task_schedule_t initial_starts;
	periodic_task_schedule_t periodic_starts;
};
typedef std::map<ARRAY_INDEX, task_schedule_t> scheduling_t_mod;
typedef std::set<Edge>  critical_circuit_t;

namespace models {

class SchedulingMod {
private:
	TIME_UNIT              _period ;
	const models::Dataflow*_dataflow;
	scheduling_t_mod           _tasks_schedule;
	// critical_circuit_t     _critical_edges;
public :
	SchedulingMod () : _period (0), _dataflow(nullptr)  {}
private :
	SchedulingMod (TIME_UNIT period, const models::Dataflow* dataflow) : _period (period), _dataflow(dataflow) {

	}

public :
	SchedulingMod (const models::Dataflow* dataflow, TIME_UNIT omega, scheduling_t_mod tasks_schedule 
	// ,critical_circuit_t critical_edges
	) : _period(omega), _dataflow(dataflow), _tasks_schedule(tasks_schedule)
	// , _critical_edges (critical_edges) 
	{

	}

	TIME_UNIT getGraphPeriod () const {
			return this->_period;
	}

	TIME_UNIT getGraphThroughput () const {
			return 1.0 / this->_period;
	}
	const scheduling_t_mod& getTaskSchedule () const{
		return this->_tasks_schedule;
	}

	// const critical_circuit_t& getCriticalEdges() const {
	// 	return this->_critical_edges;
	// }

	void verbose_print ();

};
}

#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULINGMOD_SCHEDULINGMOD_H_ */