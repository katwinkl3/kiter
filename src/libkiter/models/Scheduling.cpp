/*
 * Scheduling.cpp
 *
 *  Created on: 25 June 2021
 *      Author: Katherine
 */

#include "Scheduling.h"
#include <models/Dataflow.h>

void models::Scheduling::verbose_print() {

	for (auto  key : this->_tasks_schedule) {
			auto task_vtx = _dataflow->getVertexById(key.first);

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


struct task_track {
	ARRAY_INDEX id;
	TIME_UNIT exec_time;
};

struct task_catalog {
	EXEC_COUNT cur_phase;
	EXEC_COUNT task_Ni;
	std::vector<TIME_UNIT> phase_durations;
	std::vector<TIME_UNIT> schedule;
};

bool models::Scheduling::check_valid_schedule (){

	// Step 0: Setting up symbolic execution

	const models::Dataflow* g = this->getDataflow();
	scheduling_t s = this->getTaskSchedule();
	
	std::vector<EXEC_COUNT> buffer_load (g->getMaxEdgeId());

	{ForEachEdge(g,c) {
		buffer_load[g->getEdgeId(c)] = g->getPreload(c);
		// ###
		VERBOSE_INFO("EDGE ID: " << g->getEdgeId(c) << " | EDGE INIT " << buffer_load[g->getEdgeId(c)]);
		// ###
	}}


	std::map<ARRAY_INDEX, task_catalog> task_log;

	EXEC_COUNT total = 0 ;
	{ForEachVertex(g,t) {
		ARRAY_INDEX id = g->getVertexId(t); 
		EXEC_COUNT Ni =  g->getNi(t);
		task_catalog t;
		t.cur_phase = 0;
		t.task_Ni = Ni;
		t.phase_durations = g->getVertexPhaseDuration((g->getVertexById(id)));
		total += Ni;
		task_log[id] = t;
	}}

	// Creating one centralized schedule vector for each task

	for (std::pair<const ARRAY_INDEX, task_schedule_t> task : s){

		ARRAY_INDEX task_id = task.first;

		task_log[task_id].schedule = task.second.initial_starts;
		total += task.second.initial_starts.size(); 

		for (int i = 0; i <= task_log[task_id].task_Ni; i++){
			TIME_UNIT task_period = s[task_id].periodic_starts.first;
			TIME_UNIT out_exec_time = s[task_id].periodic_starts.second[0];
			task_log[task_id].schedule.push_back(out_exec_time);

			s[task_id].periodic_starts.second.push_back(out_exec_time + task_period);
			s[task_id].periodic_starts.second.erase(s[task_id].periodic_starts.second.begin());
		}
	}

	// ###
	for(auto task : task_log){ VERBOSE_INFO("TASK ID: " << task.first << " | Ni: " << task.second.task_Ni << " | PhaseDur: " << commons::toString(task.second.phase_durations) << " | Schedule: " << commons::toString(task.second.schedule)); }
	// ###

	
	// Step 1: Symbolic execution

	task_track next_task;

	// Tracking most recent next tasks of each task
	std::vector<TIME_UNIT> task_history (g->getMaxEdgeId()); 
	
	// Initializing next task
	for (std::pair<const ARRAY_INDEX, task_catalog> task : task_log){
		if (task.second.schedule[0] == 0){
			next_task.id = task.first;
			next_task.exec_time = 0;
		}
	}

	// ###
	VERBOSE_INFO("INIT TASK ID: " << next_task.id << " | EXEC TIME: " << next_task.exec_time);
	// ###


	while(total > task_log.size()){ // not sure about setting task_log size here

		// Finding all tasks that completed execution before next_task is consumed

		std::vector<ARRAY_INDEX> out_tasks;

		for (std::pair<const ARRAY_INDEX, task_catalog> task : task_log){
			
			ARRAY_INDEX id = task.first;
			TIME_UNIT phase_time = (task_log[id].phase_durations)[task_log[id].cur_phase];
			TIME_UNIT finish_time = phase_time + task_log[id].schedule[0];

			if (finish_time <= next_task.exec_time){
				out_tasks.push_back(id);

				// ###
				VERBOSE_INFO("OUT TASK ID: " << id << " | FINISH TIME: " << finish_time << " | PHASE:" << task_log[id].cur_phase);
				// ###
			} 
		}

		// ###
		VERBOSE_INFO("OUT TASKS: " << commons::toString(out_tasks));
		// ###

		// Executing symbolic executon

		for (ARRAY_INDEX out_id : out_tasks){
			Vertex out_v = g->getVertexById(out_id);
			{ForOutputEdges(g,out_v,inE){
				TOKEN_UNIT reqCount = (g->getEdgeInVector(inE))[task_log[out_id].cur_phase];
				TOKEN_UNIT inCount  = buffer_load[g->getEdgeId(inE)];
				buffer_load[g->getEdgeId(inE)] = inCount + reqCount;
			}}

		}

		Vertex in_v = g->getVertexById(next_task.id);
		{ForInputEdges(g,in_v,inE)	{
			TOKEN_UNIT reqCount = (g->getEdgeOutVector(inE))[task_log[next_task.id].cur_phase];
			TOKEN_UNIT inCount  = buffer_load[g->getEdgeId(inE)];
			buffer_load[g->getEdgeId(inE)] = inCount - reqCount;

			// ###
			VERBOSE_INFO("BUFFER " << g->getEdgeId(inE) << " has " << inCount - reqCount << " TOKENS");
			// ###

			if (!(buffer_load[g->getEdgeId(inE)] >= 0)) {
				return false;
			}
		}}

		for (ARRAY_INDEX out_id : out_tasks){
			task_log[out_id].cur_phase = (task_log[out_id].cur_phase + 1) % task_log[out_id].phase_durations.size();
			task_log[out_id].schedule.erase(task_log[out_id].schedule.begin());
		}

		// ###
		VERBOSE_INFO("TASK HISTORY: " << commons::toString(task_history));
		// ###
		

		// ###
		VERBOSE_INFO("######## LOOP ITERATIONS LEFT: " << total << " ###########");
		// ###

		// Finding next task in schedule
		task_history[next_task.id] = next_task.exec_time;

		std::map<TIME_UNIT, ARRAY_INDEX> potential_next_tasks;

		for (std::pair<const ARRAY_INDEX, task_catalog> task : task_log){
		if (!(task.second.schedule.empty())){

			std::size_t i = 0;
			while(task.second.schedule[i] < next_task.exec_time || task.second.schedule[i] <= task_history[task.first]){
				if (i + 1 == task.second.schedule.size()){ break; }
				i++;			
			}

			if (task.second.schedule[i] > task_history[task.first]){
			potential_next_tasks[task.second.schedule[i]] = task.first;
			}
		}}

		next_task.id = (potential_next_tasks.begin())->second;
		next_task.exec_time = (potential_next_tasks.begin())->first;

		// ###
		for(auto task : task_log){ VERBOSE_INFO("TASK ID: " << task.first <<" | SCHEDULE " << commons::toString(task.second.schedule)); }
		VERBOSE_INFO("BUFFER LOAD: " << commons::toString(buffer_load));
		VERBOSE_INFO("NEXT TASK ID: " << next_task.id << " | EXEC TIME: " << next_task.exec_time);
		// ### 

		total--;

	}	

	return true;

}




	