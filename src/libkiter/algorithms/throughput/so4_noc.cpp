/*
 * so4_noc.cpp taken from "A Statically Scheduled Time-Division-Multiplexed
 *  Network-on-Chip for Real-Time Systems"
 *
 *  Created on: May 2021
 *      Author: katwinkl3
 */

#include <models/Dataflow.h>
#include "actor.h"
#include "state.h"
#include "so4_noc.h"
#include <models/SchedulingMod.h>

std::pair<TIME_UNIT, static_task_schedule_t> get_starts(std::vector<std::vector<TIME_UNIT>> periods){
  std::vector<TIME_UNIT> res;
  int period = 0;
  for(std::size_t i = 0; i < periods.size(); ++i) {
    res.push_back(periods[i][0]);
    if (i == periods.size() - 1){
      period = periods[i][1] - periods[0][0];
    }
  }
  TIME_UNIT ped = period;
  std::cout <<     " PERIOD=[ " << commons::toString(res) << "], " << "length = " << commons::toString(period) << std::endl;
  return {ped, res};
}

void algorithms::so4_noc(models::Dataflow* const dataflow,   parameters_list_t param_list) {
  
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

  StateList visitedStates;
  EXEC_COUNT minRepFactor = INT_MAX; // EXEC_COUNT is of type long int, and LONG_MAX has same max value
  ARRAY_INDEX minRepActorId;
  TOKEN_UNIT minRepActorExecCount = 0;
  TIME_UNIT timeStep;

  long slots; //TODO: take from input file instead
  slots = 1; //hardcode
  std::map<std::pair<ARRAY_INDEX, ARRAY_INDEX>, long> condition; //TODO: take from input file instead; rmb to convert from id
  condition.insert({{1,2},0});//4}); //id = v+1, i think??
  // condition.insert({{1,3},0});
  condition.insert({{1,4},0});//1});
  // condition.insert({{2,1},0});
  condition.insert({{2,3},0});//1});
  // condition.insert({{2,4},1});//4});
  condition.insert({{3,1},0});//4});
  // condition.insert({{3,2},0});//1});
  // condition.insert({{3,4},0});
  // condition.insert({{4,1},0});//1});
  // condition.insert({{4,2},0});
  condition.insert({{4,3},0});//4});

  TIME_UNIT curr_step = 0;  
  std::map<ARRAY_INDEX, std::vector<TIME_UNIT>> statics; //initial phase
  std::map<ARRAY_INDEX, std::vector<std::vector<TIME_UNIT>>> periodics; //periodic phase
  bool periodic_state = false;
  
  // initialise actors
  std::map<ARRAY_INDEX, Actor> actorMap;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
      VERBOSE_INFO("\n");
    }}

  State prevState(dataflow, actorMap);
  State currState(dataflow, actorMap);

  scheduling_t_mod task_schedule; // {vertex_id : {time_unit: [time_unit]}}
 
  {ForEachTask(dataflow, t) {
      if (actorMap[dataflow->getVertexId(t)].getRepFactor() < minRepFactor) {
        minRepFactor = actorMap[dataflow->getVertexId(t)].getRepFactor();
        minRepActorId = actorMap[dataflow->getVertexId(t)].getId();
      }
    }}
 
  // Start ASAP execution loop
  while (true) {
    {ForEachEdge(dataflow, e) {
        prevState.setTokens(e, currState.getTokens(e));
      }}
    // end actor firing
    {ForEachTask(dataflow, t) {
        while (actorMap[dataflow->getVertexId(t)].isReadyToEndExecWithMod(currState, dataflow, condition, (int) curr_step % slots)) {
          if (actorMap[dataflow->getVertexId(t)].getId() == minRepActorId) {
            minRepActorExecCount++;
            if (minRepActorExecCount == minRepFactor) {
              // end initial
              if (!periodic_state){periodic_state = true;}
              if (!visitedStates.addState(currState)) {
                VERBOSE_INFO("ending execution and computing throughput");
                TIME_UNIT thr = visitedStates.computeThroughput();
                std::cout << "throughput computed is: " << thr << std::endl;

                {ForEachTask(dataflow, t2){
                  task_schedule_t sched_struct = {statics[dataflow->getVertexId(t2)], get_starts(periodics[dataflow->getVertexId(t2)])};
                  task_schedule.insert(std::make_pair(dataflow->getVertexId(t2), sched_struct));
                }}
                models::SchedulingMod res = models::SchedulingMod(dataflow, 1/thr, task_schedule);
                res.verbose_print();
                return;
              }
              currState.setTimeElapsed(0);
              minRepActorExecCount = 0;
            }
          }
          actorMap[dataflow->getVertexId(t)].execEndWithMod(dataflow, currState, condition, (int) curr_step % slots);
          currState.updateState(dataflow, actorMap); // NOTE updating tokens/phase in state done separately from execEnd function, might be a cause for bugs
        }
      }}
   
    
    // start actor firing
    {ForEachTask(dataflow, t) {
        while (actorMap[dataflow->getVertexId(t)].isReadyForExecWithMod(dataflow, currState, condition, (int) curr_step % slots)) { //too heavy; modify datatype
          actorMap[dataflow->getVertexId(t)].execStartWithMod(dataflow, currState, condition, (int) curr_step % slots); 
        // while (actorMap[dataflow->getVertexId(t)].isReadyForExec(currState)) { //too heavy; modify datatype
        //   actorMap[dataflow->getVertexId(t)].execStart(dataflow, currState); 
          currState.updateState(dataflow, actorMap);
          if (periodic_state){
            periodics[dataflow->getVertexId(t)].push_back({curr_step});
          } else {
            statics[dataflow->getVertexId(t)].push_back(currState.getTimeElapsed());
          }
        }
      }}
    
    curr_step++;
    timeStep = currState.advanceTimeWithMod();
    if (timeStep == LONG_MAX) { //TODO: get a better fail safe this wont work now eheh
      VERBOSE_INFO("Deadlock found!");
      return; // should be returning 0
    }
  }
}