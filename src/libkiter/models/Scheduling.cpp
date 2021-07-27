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


struct task_track {
	ARRAY_INDEX id;
	task_schedule_t schedule;
	TIME_UNIT exec_time;
};

struct task_execution {
	EXEC_COUNT cur_phase;
	EXEC_COUNT task_Ni;
	std::vector<TIME_UNIT> phase_durations;
};

bool models::Scheduling::check_valid_schedule (){

	// Step 0: Setting up symbolic execution

	const models::Dataflow* g = this->getDataflow();
	
	std::vector<EXEC_COUNT> buffer_load (g->getMaxEdgeId());
	{ForEachEdge(g,c) {
		buffer_load[g->getEdgeId(c)] = g->getPreload(c);
	}}

	EXEC_COUNT total = 0 ;
	std::vector<task_execution>  task_log (g->getMaxVertexId());
	{ForEachVertex(g,t) {
		ARRAY_INDEX id = g->getVertexId(t); 
		EXEC_COUNT Ni =  g->getNi(t) ;
		task_log[id].cur_phase = 0;
		task_log[id].task_Ni = Ni;
		task_log[id].phase_durations = g->getVertexPhaseDuration((g->getVertexById(id)));
		total += Ni;
	}}

	// Step 1: Symbolic execution

	scheduling_t s = this->getTaskSchedule();

	// defining and initializing the first "next_task"
	task_track next_task;
	auto first_task = s.begin();
	next_task.id = first_task->first;
	next_task.schedule = first_task->second;
	next_task.exec_time = first_task->second.initial_starts[0];



	int iter = 0;
	TIME_UNIT max_time = std::numeric_limits<TIME_UNIT>::max();
	
	while(total > 0){

		//
		VERBOSE_INFO("######## LOOP ITERATION " << iter << " ###########")
		iter++;
		//

		// Finding next task in schedule
		
		TIME_UNIT* exec_time =& max_time;

		ARRAY_INDEX* task_id;
		task_schedule_t* task_schedule;

		for (auto val : s){

			if(!(val.second.initial_starts.empty())){

				// if same task, need to find next phase to execute
				EXEC_COUNT i = 0;
				if (val.first == next_task.id){
					while (val.second.initial_starts[i] <= next_task.exec_time){
						i++;
					}
				} else {
					while (val.second.initial_starts[i] < next_task.exec_time){
						i++;
					}
				}

				if (val.second.initial_starts[i] < *exec_time){
					ARRAY_INDEX task_id_tmp = val.first;
					task_id =& task_id_tmp;

					task_schedule_t task_schedule_tmp = val.second;
					task_schedule =& val.second;
					
					TIME_UNIT exec_time_tmp = val.second.initial_starts[i];
					exec_time =& exec_time_tmp;
				}

			} else {

				EXEC_COUNT i = 0;
				if (val.first == next_task.id){
					while (val.second.periodic_starts.second[i] <= next_task.exec_time){
						i++;
					} 
				} else {
					while (val.second.periodic_starts.second[i] < next_task.exec_time){
						i++;
					}
				}

				if (val.second.periodic_starts.second[i] < *exec_time){
					ARRAY_INDEX task_id_tmp = val.first;
					task_id =& task_id_tmp;

					task_schedule_t task_schedule_tmp = val.second;
					task_schedule =& val.second;

					TIME_UNIT exec_time_tmp = val.second.periodic_starts.second[i];
					exec_time =& exec_time_tmp;
				}
			}
		}

		next_task.exec_time = *exec_time;
		next_task.id = *task_id;
		next_task.schedule = *task_schedule;

		//
		VERBOSE_INFO("Next Task ID: " << next_task.id << " | Next Task Phase: " << task_log[next_task.id].cur_phase);
		VERBOSE_INFO("Next task EXEC TIME: " << next_task.exec_time);
		//

		// Finding all tasks that completed execution before next_task consumes

		std::vector<ARRAY_INDEX> out_tasks;

		for (auto val : s){

			ARRAY_INDEX id = val.first;
			TIME_UNIT phase_time = (task_log[id].phase_durations)[task_log[id].cur_phase];
			TIME_UNIT* task_finish_time;

			if (val.second.initial_starts.empty()){
				TIME_UNIT q = val.second.initial_starts[0] + phase_time;
				task_finish_time =& q;
			} else {
				TIME_UNIT p = val.second.periodic_starts.second[0] + phase_time;
				task_finish_time =& p;
			}

			if (*task_finish_time <= next_task.exec_time){ // getting unused variable task_finish_time warning?
					//
					VERBOSE_INFO("Task " << id << " completes execution at time " << task_finish_time )
					//
					out_tasks.push_back(id);
				}

		}

		// Simulating task execution
		
		for (auto out_id : out_tasks){

			{ForOutputEdges(g,out_id,inE){
				TOKEN_UNIT reqCount = (g->getEdgeInVector(inE))[task_log[out_id].cur_phase];
				TOKEN_UNIT inCount  = buffer_load[g->getEdgeId(inE)];
				buffer_load[g->getEdgeId(inE)] = inCount + reqCount;
			}}

		}

		{ForInputEdges(g,next_task.id,inE)	{
			TOKEN_UNIT reqCount = (g->getEdgeOutVector(inE))[task_log[next_task.id].cur_phase];
			TOKEN_UNIT inCount  = buffer_load[g->getEdgeId(inE)];
			buffer_load[g->getEdgeId(inE)] = inCount - reqCount;
			//
			VERBOSE_INFO("Buffer " << g->getEdgeId(inE) << " has " << inCount - reqCount << "tokens");
			//
			if (!(buffer_load[g->getEdgeId(inE)] >= 0)) {
				return false;
			}
		}}

		// Clearing executed tasks from schedule

		for (auto out_id : out_tasks){
			
			// Adjusting current phase iteration of executed task
			task_log[out_id].cur_phase = (task_log[out_id].cur_phase + 1) % task_log[out_id].phase_durations.size();

			// Popping execution from schedule 

			if (!(s[out_id].initial_starts.empty())){
				s[out_id].initial_starts.erase(s[out_id].initial_starts.begin());
			} else {
				// Appending periodic total
				total -= 1;
				EXEC_COUNT periodic_executions_left = task_log[out_id].task_Ni;
				task_log[out_id].task_Ni = periodic_executions_left - 1;

				// Adding next periodic execution and removing the one that was just executed 
				TIME_UNIT task_period = s[out_id].periodic_starts.first;
				TIME_UNIT out_exec_time = s[out_id].periodic_starts.second[0];
				s[out_id].periodic_starts.second.push_back(out_exec_time + task_period);
				s[out_id].periodic_starts.second.erase(s[next_task.id].periodic_starts.second.begin());
			}

		}
	
	}

	return true;

}

