/*
 * Scheduling.h
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_

#include <models/Dataflow.h>

typedef std::map<ARRAY_INDEX,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> scheduling_t;

namespace models {

class Scheduling {
private:
	TIME_UNIT              _period ;
	scheduling_t           _tasks_schedule;
	const models::Dataflow*_dataflow;
public :
	Scheduling () : _period (0), _dataflow(nullptr)  {}
private :
	Scheduling (TIME_UNIT period, const models::Dataflow* dataflow) : _period (period), _dataflow(dataflow) {

	}

public :
	Scheduling (const models::Dataflow* dataflow, TIME_UNIT omega, scheduling_t tasks_schedule) : _period(omega), _tasks_schedule(tasks_schedule), _dataflow(dataflow) {

	}

	TIME_UNIT getGraphPeriod () {
		return this->_period;
	}
	scheduling_t& getTaskSchedule () {
		return this->_tasks_schedule;
	}

	void verbose_print ();

};
}

#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_ */
