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

typedef std::pair<TIME_UNIT,std::vector<TIME_UNIT>> periodic_task_schedule_t;
typedef std::map<ARRAY_INDEX,periodic_task_schedule_t> scheduling_t;
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


	std::string asText () {

		std::ostringstream returnStream;

		for (auto item : this->getTaskSchedule()) {
			ARRAY_INDEX tid = item.first;
			Vertex v = _dataflow->getVertexById(item.first);
			std::string  tname = _dataflow->getVertexName(v);
			TIME_UNIT period = item.second.first;
			std::vector<TIME_UNIT> &starts = item.second.second;
			returnStream << std::setw(5) << tid << ") " << tname
					<< " | starts:" << commons::toString(starts)
					<< " | period:" << period
					<< " | durations:" << commons::toString(_dataflow->getVertexInitPhaseDuration(v))
			                           << ";[" << commons::toString(_dataflow->getVertexPhaseDuration(v)) << "]"
					<< std::endl;
		}


		return returnStream.str();

	}
	std::string asASCII (int line_size) {


		std::ostringstream returnStream;

		for (auto item : this->getTaskSchedule()) {
			ARRAY_INDEX tid = item.first;
			Vertex v = _dataflow->getVertexById(item.first);
			std::string  tname = _dataflow->getVertexName(v);
			TIME_UNIT period = item.second.first;
			std::vector<TIME_UNIT> &starts = item.second.second;
			std::string line = "";
			ARRAY_INDEX max_iter = line_size / period;
			for (TIME_UNIT time = 0 ; time < line_size ; time ++) {
				bool execute = false;
				for (ARRAY_INDEX iteration = 0 ; iteration <= max_iter ; iteration ++) {
					for (ARRAY_INDEX sidx = 0 ; sidx < starts.size() ; sidx++) {
						TIME_UNIT s = starts[sidx] + iteration * period;
						EXEC_COUNT exec_count = sidx + iteration * starts.size();
						TIME_UNIT duration = _dataflow->getVertexDuration(v, 1 + (exec_count % _dataflow->getPhasesQuantity(v))); // TODO : unsupported init phases
						TIME_UNIT normalize = (time > s) ? (time - s) - ((time - s) / period) : (time - s);
						bool execute_here = ((0  <= normalize ) and (normalize < duration ));
						execute = execute or execute_here;
					}
				}
				line += execute ? "#" : " ";
			}
			returnStream << std::setw(5) << tid << " | " << line << std::endl;
		}


		return returnStream.str();

	}
};
}

#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_ */
