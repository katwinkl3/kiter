/*
 * Scheduling.cpp
 *
 *  Created on: 25 June 2021
 *      Author: Katherine
 */

#include "Scheduling.h"
#include <models/Dataflow.h>
#include <printers/stdout.h>

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

		returnStream << printers::Scheduling2Tikz(*this);

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

bool models::Scheduling::check_valid_schedule (){

	// Step 0: Setting up symbolic execution

	const models::Dataflow* g = this->getDataflow();
	
	std::vector<EXEC_COUNT> buffer_load (g->getMaxEdgeId());
	{ForEachEdge(g,c) {
		buffer_load[g->getEdgeId(c)] = g->getPreload(c);
	}}

	EXEC_COUNT total = 0 ;
	std::vector<std::pair<EXEC_COUNT, EXEC_COUNT>>  task_execution (g->getMaxVertexId());
	// First element for initialization stage phase tracker, second element for Ni value
	{ForEachVertex(g,t) {
		EXEC_COUNT Ni =  g->getNi(t) ;
		task_execution[g->getVertexId(t)].first = 0;
		task_execution[g->getVertexId(t)].second = Ni;
		total += Ni ;
	}}


	scheduling_t s = this->getTaskSchedule(); // This should make a copy of schedule
	
	while(total > 0){

		// Finding next task in schedule (this feels like bad code)
		std::tuple<ARRAY_INDEX, task_schedule_t, TIME_UNIT> next_task;
		std::get<2>(next_task) = INT_MAX;
		for (auto val : s){

			if(!(val.second.initial_starts.empty())){

				if (val.second.initial_starts[0] < std::get<2>(next_task)){
					std::get<0>(next_task) = val.first;
					std::get<1>(next_task) = val.second;
					std::get<2>(next_task) = val.second.initial_starts[0];
				}

			} else {

				if (val.second.periodic_starts.second[0] < std::get<2>(next_task)){
					std::get<0>(next_task) = val.first;
					std::get<1>(next_task) = val.second;
					std::get<2>(next_task) = val.second.periodic_starts.second[0];
				}
			}
		}


		// Simulating task execution
		
		ARRAY_INDEX next_task_id = std::get<0>(next_task);
		Vertex next_task_v = g->getVertexById(next_task_id);
		// std::vector<TIME_UNIT> next_task_durations = g->getVertexPhaseDuration(next_task_v);
		EXEC_COUNT next_task_phase = task_execution[next_task_id].first;
		// TIME_UNIT next_task_phase_duration = next_task_durations[next_task_phase];
		// int execution_time = std::get<2>(next_task) + next_task_phase_duration;

		// note; Should not need to consider execution time, as simultaneous tasks are locally independent
		
		{ForInputEdges(g,next_task_id,inE)	{
			TOKEN_UNIT reqCount = (g->getEdgeOutVector(inE))[next_task_phase];
			TOKEN_UNIT inCount  = buffer_load[g->getEdgeId(inE)];
			buffer_load[g->getEdgeId(inE)] = inCount - reqCount;
			if (!(buffer_load[g->getEdgeId(inE)] >= 0)) {
				return false;
				// Should I check if execution time of input < execution of output + duration?
			}
		}}

		{ForOutputEdges(g,next_task_id,inE)	{
			TOKEN_UNIT reqCount = (g->getEdgeInVector(inE))[next_task_phase];
			TOKEN_UNIT inCount  = buffer_load[g->getEdgeId(inE)];
			buffer_load[g->getEdgeId(inE)] = inCount + reqCount;
		}}


		// Adjusting current phase iteration of executed task
		(task_execution[next_task_id]).first = (next_task_phase + 1) % g->getPhasesQuantity(next_task_v);

		// Popping execution from schedule 

		if (!(s[next_task_id].initial_starts.empty())){
			s[next_task_id].initial_starts.erase(s[next_task_id].initial_starts.begin());
		} else {
			// Appending periodic total
			total -= 1;
			EXEC_COUNT periodic_executions_left = (task_execution[next_task_id]).second;
			(task_execution[next_task_id]).second = periodic_executions_left - 1;

			// Adding next periodic execution and removing the one that was just executed 
			auto task_period = s[next_task_id].periodic_starts.first;
			s[next_task_id].periodic_starts.second.push_back(std::get<2>(next_task) + task_period);
			s[next_task_id].periodic_starts.second.erase(s[next_task_id].periodic_starts.second.begin());
		}
		

	}

}

