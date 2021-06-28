/*
 * Scheduling.cpp
 *
 *  *  Created on: 25 June 2021
 *      Author: Katherine
 */

#include "Scheduling.h"
#include <models/Dataflow.h>

void models::Scheduling::verbose_print() {

	for (auto  key : this->_tasks_schedule) {
			auto task_vtx = _dataflow->getVertexById(key.first);
			key.second.periodic_starts.second.pop_back(); //TODO: resolve this later aiya

			std::cout << "Task " <<  _dataflow->getVertexName(task_vtx)
					<<  " : duration=[ " << commons::toString(_dataflow->getVertexPhaseDuration(task_vtx)) <<  "]"
					<<     " starts=[ " << commons::toString(key.second.initial_starts) << "]"
					<<     " period=" <<  key.second.periodic_starts.first 
					<<     " periods=[ " << commons::toString(key.second.periodic_starts.second) << "]"
					<< std::endl;

	}
}


std::string models::Scheduling::asText () {

		std::ostringstream returnStream;

		for (auto item : this->getTaskSchedule()) {
			ARRAY_INDEX tid = item.first;
			Vertex v = _dataflow->getVertexById(item.first);
			std::string  tname = _dataflow->getVertexName(v);
			TIME_UNIT period = item.second.periodic_starts.first;//item.second.first;
			std::vector<TIME_UNIT> &starts = item.second.initial_starts;//item.second.second;
			std::vector<TIME_UNIT> &pedstarts = item.second.periodic_starts.second;

			returnStream << std::setw(5) << tid << ") " << tname
					<< " | starts:" << commons::toString(starts)
					<< " | period:" << period
					<< " | periodic starts:" << commons::toString(pedstarts)
					<< " | durations:" << commons::toString(_dataflow->getVertexInitPhaseDuration(v))
			                           << ";[" << commons::toString(_dataflow->getVertexPhaseDuration(v)) << "]"
					<< std::endl;
		}


		return returnStream.str();

	}

std::string models::Scheduling::asASCIINewNew (int line_size) {


	std::ostringstream returnStream;
	int idx = 0;

	for (auto item : this->getTaskSchedule()) {
		ARRAY_INDEX tid = item.first;
		Vertex v = _dataflow->getVertexById(item.first);
		std::vector<TIME_UNIT> &starts = item.second.initial_starts;// initial starts
		std::vector<TIME_UNIT> &pedstarts = item.second.periodic_starts.second; //periodic starts
		TIME_UNIT period = item.second.periodic_starts.first;
		std::string start_str;
		std::string period_str;
		TIME_UNIT prev = -1;
		std::vector<TIME_UNIT> phases = _dataflow->getVertexPhaseDuration(v);
		int phase_count = phases.size();

		std::string line = "";
		int linesize = line_size;
		// add to string 1) initial starts 2) trailing spaces (before periodic starts)
		int exec_idx = 0;
		for (TIME_UNIT i: starts) {
			std::string add_space(i-prev-1, ' ');
			start_str += add_space;
			std::string exec(phases[exec_idx % phase_count]-1, '#');
			start_str += '@';
			start_str += exec;
			prev = i+exec.size();
			++exec_idx;
		}
		std::string leading_space(pedstarts[0]-prev-1, ' ');
		start_str += leading_space; 
		if (start_str.length() <= linesize) {
			line += start_str;
			linesize -= start_str.length();
		}
		else {
			line += (start_str.substr(0,linesize));
			linesize = 0;
		}

		// create period string without leading spaces so to use for both first and subsequent periods
		if (!pedstarts.empty()){
			prev = pedstarts[0]-1;
			for (TIME_UNIT i: pedstarts) {
				std::string add_space(i-prev-1, ' ');
				period_str += add_space;
				std::string exec(phases[exec_idx % phase_count]-1, '#');
				period_str += '@';
				period_str += exec;
				prev = i+exec.size();
				++exec_idx ;
			}
			// add trailing spaces after period starts (before next period)
			std::string trailing_space((period+pedstarts[0])-prev-1, ' ');
			period_str += trailing_space;
			// need to complete the period for all phases (by looking ahead)
			if(phase_count > pedstarts.size()){
				prev = pedstarts[0]-1;
				int phase_idx = 0;
				while (exec_idx % phase_count != 0){
					TIME_UNIT i = pedstarts[phase_idx % pedstarts.size()];
					std::string exec(phases[exec_idx % phase_count]-1, '#');
					period_str += '@';
					period_str += exec;
					if (i == pedstarts.back()){
						std::string trailing_space((period+pedstarts[0])-(i+exec.size())-1, ' ');
						period_str += trailing_space;
					} else {
						std::string add_space(pedstarts[phase_idx % pedstarts.size()]-i, ' ');
						period_str += add_space;
					}
					
					prev = i+exec.size();
					++phase_idx;
					++exec_idx;
				}
			}
			// continuously add period_str to line
			while (linesize > 0){
				if (linesize >= period_str.length()){
					line += period_str;
					linesize -= period_str.length();
				} else {
					line += (period_str.substr(0,linesize));
					linesize = 0;
				}
			}
		}
		
		linesize = line_size;
		++idx;

		returnStream << std::setw(5) << tid << " |" << line << std::endl;
	}

	return returnStream.str();
}


std::string models::Scheduling::asASCIINew (int line_size) {


	std::ostringstream returnStream;
	int idx = 0;

	for (auto item : this->getTaskSchedule()) {
		ARRAY_INDEX tid = item.first;
		Vertex v = _dataflow->getVertexById(item.first);
		std::vector<TIME_UNIT> &starts = item.second.initial_starts;// initial starts
		std::vector<TIME_UNIT> &pedstarts = item.second.periodic_starts.second; //periodic starts
		TIME_UNIT period = item.second.periodic_starts.first;
		std::string start_str;
		std::string period_str;
		TIME_UNIT prev = -1;

		std::string line = "";
		int linesize = line_size;
		// add to string 1) initial starts 2) trailing spaces (before periodic starts)
		for (TIME_UNIT i: starts) {
			std::string add_space(i-prev-1, ' ');
			start_str += add_space;
			start_str += "#";
			prev = i;
		}
		std::string leading_space(pedstarts[0]-prev-1, ' ');
		start_str += leading_space; 
		if (start_str.length() <= linesize) {
			line += start_str;
			linesize -= start_str.length();
		}
		else {
			line += (start_str.substr(0,linesize));
			linesize = 0;
		}

		// create period string without leading spaces so to use for both first and subsequent periods
		if (!pedstarts.empty()){
			prev = pedstarts[0]-1;
			for (TIME_UNIT i: pedstarts) {
				std::string add_space(i-prev-1, ' ');
				period_str += add_space;
				period_str += "#";
				prev = i;
			}
			// add trailing spaces after period starts (before next period)
			std::string trailing_space((period+pedstarts[0])-prev-1, ' ');
			period_str += trailing_space;
			// continuously add period_str to line
			while (linesize > 0){
				if (linesize >= period_str.length()){
					line += period_str;
					linesize -= period_str.length();
				} else {
					line += (period_str.substr(0,linesize));
					linesize = 0;
				}
			}
		}
		
		linesize = line_size;
		++idx;

		returnStream << std::setw(5) << tid << " |" << line << std::endl;
	}

	return returnStream.str();
}

std::string models::Scheduling::asASCII (int line_size) {

	return asASCIINewNew(line_size);

	std::ostringstream returnStream;

	 for (auto item : this->getTaskSchedule()) {
	 	ARRAY_INDEX tid = item.first;
	 	Vertex v = _dataflow->getVertexById(item.first);
	 	std::string  tname = _dataflow->getVertexName(v);
	 	TIME_UNIT period = item.second.periodic_starts.first;//item.second.first;
	 	std::vector<TIME_UNIT> &starts = item.second.initial_starts;//item.second.second;
	 	std::string line = "";
	 	ARRAY_INDEX max_iter = line_size / period;

	 	VERBOSE_INFO("tid=" << tid);

	 	for (TIME_UNIT t = 0 ; t < line_size ; t ++) {
	 		bool execute = false;
	 		for (ARRAY_INDEX iteration = 0 ; iteration <= max_iter ; iteration ++) {
	 			for (auto sidx = 0 ; sidx < starts.size() ; sidx++) {
	 				TIME_UNIT s = starts[sidx] + iteration * period;
	 				EXEC_COUNT exec_count = sidx + iteration * starts.size();
	 				TIME_UNIT d = _dataflow->getVertexDuration(v, 1 + (exec_count % _dataflow->getPhasesQuantity(v))); // TODO : unsupported init phases
	 				//TIME_UNIT normalize = (time > s) ? (time - s) - ((time - s) / period) : (time - s);
	 				//bool execute_here = ((0  <= normalize ) and (normalize < duration ));
	 				bool case1 = ((t   <= s) and (s   <   t+1 ));
	 				bool case2 =  (((s+d - 0.01) >  t) and (s+d + 0 <=  t+1 ));
	 				bool case3 = ((s   <  t) and (t+1 <   s+d - 0 ));
	 				bool execute_here = case1 or case2 or case3;

	 				execute = execute or execute_here;
	 			}
	 		}
	 		line += execute ? "#" : " ";
	 	}
	 	returnStream << std::setw(5) << tid << " |" << line << std::endl;
	 }


	return returnStream.str();

}

