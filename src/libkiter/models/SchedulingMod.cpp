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
			key.second.periodic_starts.second.pop_back(); //TODO: resolve this later aiya
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


std::string models::SchedulingMod::asText () {

		std::ostringstream returnStream;

		for (auto item : this->getTaskSchedule()) {
			ARRAY_INDEX tid = item.first;
			Vertex v = _dataflow->getVertexById(item.first);
			std::string  tname = _dataflow->getVertexName(v);
			TIME_UNIT period = item.second.periodic_starts.first;//item.second.first;
			std::vector<TIME_UNIT> &starts = item.second.initial_starts;//item.second.second;
			std::vector<TIME_UNIT> &pedstarts = item.second.periodic_starts.second;
			TIME_UNIT next = pedstarts.back();
			pedstarts.pop_back(); //TODO: resolve this!!
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


std::string models::SchedulingMod::asASCII (int line_size) {


	std::ostringstream returnStream;

	for (auto item : this->getTaskSchedule()) {
		ARRAY_INDEX tid = item.first;
		Vertex v = _dataflow->getVertexById(item.first);
		TIME_UNIT period = item.second.periodic_starts.first;// period
		std::vector<TIME_UNIT> &starts = item.second.initial_starts;// initial starts
		std::vector<TIME_UNIT> &pedstarts = item.second.periodic_starts.second; //periodic starts
		TIME_UNIT next = pedstarts.back();
		std::string start_str;
		std::string period_str;
		TIME_UNIT prev = -1;
		int idx = 0;
		std::string line = "";
		int linesize = line_size;

		// add for initial starts, and include sta (before periodic starts)
		for (TIME_UNIT i: starts) {
			std::string add_space(i-prev-1, ' ');
			start_str += add_space;
			start_str += "#";
			prev = i;
		}
		std::string leading_space(pedstarts[0]-prev-1, ' ');
		start_str += leading_space; 
		prev = pedstarts[0]-1;

		// add to line while within limit
		if (start_str.length() <= linesize) {
			line += start_str;
			linesize -= start_str.length();
		}
		else {
			line += (start_str.substr(0,linesize));
			linesize = 0;
		}

		// create period string without leading spaces so to use for both first and subsequent periods
		for (int i = 0; i < pedstarts.size()-1; ++i) {
			std::string add_space(pedstarts[i]-prev-1, ' ');
			period_str += add_space;
			period_str += "#";
			prev = pedstarts[i];
		}
		// add trailing spaces after period starts (before next period)
		std::string trailing_space(next-prev-1, ' ');
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

		linesize = line_size;

		returnStream << std::setw(5) << tid << " |" << line << std::endl;
	}

	// for (auto item : this->getTaskSchedule()) {
	// 	ARRAY_INDEX tid = item.first;
	// 	Vertex v = _dataflow->getVertexById(item.first);
	// 	std::string  tname = _dataflow->getVertexName(v);
	// 	TIME_UNIT period = item.second.periodic_starts.first;//item.second.first;
	// 	std::vector<TIME_UNIT> &starts = item.second.initial_starts;//item.second.second;
	// 	std::string line = "";
	// 	ARRAY_INDEX max_iter = line_size / period;

	// 	VERBOSE_INFO("tid=" << tid);

	// 	for (TIME_UNIT t = 0 ; t < line_size ; t ++) {
	// 		bool execute = false;
	// 		for (ARRAY_INDEX iteration = 0 ; iteration <= max_iter ; iteration ++) {
	// 			for (auto sidx = 0 ; sidx < starts.size() ; sidx++) {
	// 				TIME_UNIT s = starts[sidx] + iteration * period;
	// 				EXEC_COUNT exec_count = sidx + iteration * starts.size();
	// 				TIME_UNIT d = _dataflow->getVertexDuration(v, 1 + (exec_count % _dataflow->getPhasesQuantity(v))); // TODO : unsupported init phases
	// 				//TIME_UNIT normalize = (time > s) ? (time - s) - ((time - s) / period) : (time - s);
	// 				//bool execute_here = ((0  <= normalize ) and (normalize < duration ));
	// 				bool case1 = ((t   <= s) and (s   <   t+1 ));
	// 				bool case2 =  (((s+d - 0.01) >  t) and (s+d + 0 <=  t+1 ));
	// 				bool case3 = ((s   <  t) and (t+1 <   s+d - 0 ));
	// 				bool execute_here = case1 or case2 or case3;

	// 				execute = execute or execute_here;
	// 			}
	// 		}
	// 		line += execute ? "#" : " ";
	// 	}
	// 	returnStream << std::setw(5) << tid << " |" << line << std::endl;
	// }


	return returnStream.str();

}