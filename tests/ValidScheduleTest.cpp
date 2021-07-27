/*
 *
 *  Created on: Jul 23, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <models/Scheduling.h>
#include "helpers/random_generator.h"
#include <algorithms/throughput/csdf_strictly_periodic.h>
#include <algorithms/schedulings.h>


#include <map> 

BOOST_FIXTURE_TEST_SUITE( valid_schedule_test , WITH_VERBOSE)

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

BOOST_AUTO_TEST_CASE( small_graph_test_valid_schedule)
{
    models::Dataflow* gg = generate_random_graph(5, 6, 3, 5, 3);

    models::Scheduling sched = algorithms::scheduling::CSDF_SPeriodicScheduling(gg);
    sched.verbose_print();
    
    // Step 0: Setting up symbolic execution

	const models::Dataflow* g = sched.getDataflow();
	
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

    VERBOSE_INFO("STEP 0 COMPLETE")

	// Step 1: Symbolic execution

	scheduling_t s = sched.getTaskSchedule();

    VERBOSE_INFO(s[0].initial_starts.size())

	// defining and initializing the first "next_task"
	task_track next_task;
	auto first_task = s.begin();
	next_task.id = first_task->first;
	next_task.schedule = first_task->second;
	next_task.exec_time = first_task->second.initial_starts[0];

    VERBOSE_INFO(next_task.id << " *** " << next_task.exec_time)



	int iter = 0;
	TIME_UNIT max_time = std::numeric_limits<TIME_UNIT>::max();
    
    // sched.check_valid_schedule();


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
				// return false;
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


    delete gg;
}


BOOST_AUTO_TEST_SUITE_END()




