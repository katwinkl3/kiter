/*
 * so4_noc.cpp taken from "A Statically Scheduled Time-Division-Multiplexed
 *  Network-on-Chip for Real-Time Systems"
 *
 *  Created on: May 2021
 *      Author: katwinkl3
 */

#include <models/Dataflow.h>
#include <libxml/parser.h>
#include "actor.h"
#include "state.h"
#include "so4_noc.h"
#include <printers/stdout.h>
#include "../../models/Scheduling.h"
#include "../scc.h"

std::pair<TIME_UNIT, scheduling_t> algorithms::computeComponentSo4Schedule(models::Dataflow* const dataflow,
                                                 std::pair<ARRAY_INDEX, EXEC_COUNT> &minActorInfo, scheduling_t schedule, std::string filename) {
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
  StateList visitedStates;
  EXEC_COUNT minRepFactor = INT_MAX; // EXEC_COUNT is of type long int, and LONG_MAX has same max value
  ARRAY_INDEX minRepActorId;
  TOKEN_UNIT minRepActorExecCount = 0;
  TIME_UNIT timeStep;

  int periodic_state_idx;
  TIME_UNIT thr;
  bool end_check = false; // temp workaround before replacing with mathematical solution
  int actors_left = 0; // seems more efficient to use a counter check than to check through the array
  std::map<ARRAY_INDEX, TIME_UNIT> actors_check;
  {ForEachTask(dataflow, t){
    ++actors_left;
    actors_check[dataflow->getVertexId(t)] = -1;
  }}

   long slots; //TODO: take from input file instead
  slots = 2; //TODO: take from file 
  std::map<std::pair<ARRAY_INDEX, ARRAY_INDEX>, long> condition_param; //TODO: take from input file instead (rmb to convert from id), change input type accordingly
  std::map<ARRAY_INDEX, long> condition;
  condition_param.insert({{1,2},0});//4}); //id = v+1, i think??
  // condition.insert({{1,3},0});
  condition_param.insert({{1,4},1});//1});
  // condition.insert({{2,1},0});
  condition_param.insert({{2,3},0});//1});
  // condition.insert({{2,4},1});//4});
  condition_param.insert({{3,1},0});//4});
  // condition.insert({{3,2},0});//1});
  // condition.insert({{3,4},0});
  // condition.insert({{4,1},0});//1});
  // condition.insert({{4,2},0});
  condition_param.insert({{4,3},0});//4});

  xmlDocPtr doc =  xmlParseFile(filename.c_str());
  xmlNodePtr tdma_node;
  for (xmlNodePtr cur_node = doc->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (std::string((const char*)cur_node->name) == std::string("tdma")) tdma_node = cur_node;
		}
	}
  // xmlNodePtr tdma_node = xmlDocGetRootElement(doc)->children;
  const char* temp = (const char*) tdma_node->properties->children->content;
  char *stopstring; 
  slots = atoi(temp); //TODO: change to strtol 
  for (xmlNodePtr cur_node = tdma_node->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (std::string((const char*)cur_node->name) == std::string("rule")) {
        for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next){
          long src;
          long dest;
          long s;
          if (strcmp((const char*)cur_attr->name,"slot") == 0){
            temp = (const char*) cur_attr->children->content;
            s = strtol(temp, &stopstring, 10);
          }
          if (strcmp((const char*)cur_attr->name,"src") == 0){
            temp = (const char*) cur_attr->children->content;
            src = strtol(temp, &stopstring, 10);
          }
          if (strcmp((const char*)cur_attr->name,"dest") == 0){
            temp = (const char*) cur_attr->children->content;
            dest = strtol(temp, &stopstring, 10);
          }
          std::cout << 'slot'<< std::endl;;
        }
      }
		}
	}
  xmlFreeDoc(doc);
  

  {ForEachEdge(dataflow,e){
    condition.insert({dataflow->getEdgeId(e), condition_param[{dataflow->getVertexId(dataflow->getEdgeSource(e)), dataflow->getVertexId(dataflow->getEdgeTarget(e))}]});
  }}
  std::vector<ARRAY_INDEX> buffer;
  scheduling_t task_schedule; // {vertex_id : {time_unit: [time_unit]}}
  ARRAY_INDEX skip_edge = 0;
  ARRAY_INDEX skip_vertex = 0;
  TIME_UNIT skip_time = 0;
  TIME_UNIT curr_step = 0;
  std::map<ARRAY_INDEX, std::vector<std::vector<TIME_UNIT>>> starts; //{task idx : [[state 0 starts], [state 1 ...]]}
  std::map<ARRAY_INDEX, std::vector<TIME_UNIT>> state_start = {};  //{task idx : [ starts]}, 
  bool periodic_state = false;

  // initialise actors
  std::map<ARRAY_INDEX, Actor> actorMap;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
    }}
  State prevState(dataflow, actorMap);
  State currState(dataflow, actorMap);
  {ForEachTask(dataflow, t) {
      // track actor with lowest repetition factor (determines when states are stored)
      if (actorMap[dataflow->getVertexId(t)].getRepFactor() < minRepFactor) {
        minRepFactor = actorMap[dataflow->getVertexId(t)].getRepFactor();
        minRepActorId = actorMap[dataflow->getVertexId(t)].getId();
      }
    }}
 
  minActorInfo = std::make_pair(minRepActorId, minRepFactor);
  // Start ASAP execution loop
  while (true) {
    {ForEachEdge(dataflow, e) {
        prevState.setTokens(e, currState.getTokens(e));
      }}
    // end actor firing
    {ForEachTask(dataflow, t) {
        while (actorMap[dataflow->getVertexId(t)].isReadyToEndExec(currState)) {
          if (actorMap[dataflow->getVertexId(t)].getId() == minRepActorId) {
            minRepActorExecCount++;
            if (minRepActorExecCount == minRepFactor) {
              if (!end_check){
                if (!visitedStates.addState(currState)) {
                  VERBOSE_INFO("ending execution and computing throughput");
                  // compute throughput using recurrent state
                  thr = visitedStates.computeThroughput();
                  periodic_state_idx = visitedStates.computeIdx(currState); //idx for repeated state
                  end_check = true;
                }
              }
              if (!end_check){
                {ForEachTask(dataflow, task){ //start new state vector in starts
                  starts[dataflow->getVertexId(task)].push_back({});
                }}
              }
              currState.setTimeElapsed(0);
              minRepActorExecCount = 0;
            }
          }
          actorMap[dataflow->getVertexId(t)].execEnd(dataflow, currState);
          currState.updateState(dataflow, actorMap); // NOTE updating tokens/phase in state done separately from execEnd function, might be a cause for bugs
          std::cout <<"END FIRING\n"<< std::endl;
          std::cout << currState.print(dataflow) << std::endl;
        }
      }}
   
    // start actor firing
    skip_time = 0; //TODO: make it less ugly?
    skip_edge = 0;
    skip_vertex = 0;
    {ForEachTask(dataflow, t) {

      if (skip_time != 0 && dataflow->getVertexId(t) != skip_vertex){
        continue;
      }
        while (actorMap[dataflow->getVertexId(t)].isReadyForExec(currState)) {
          {ForInputEdges(dataflow, t, e){
            if (skip_time != 0 && dataflow->getVertexId(t) != skip_vertex){
              continue;
            }
            if (condition[dataflow->getEdgeId(e)] == (int) curr_step % slots){ // correct slot ? proceed : skip to time=slot
              actorMap[dataflow->getVertexId(t)].execStartWithMod(dataflow, currState, dataflow->getEdgeId(e));
              currState.updateState(dataflow, actorMap);
              std::cout <<"FIRING\n"<< std::endl;
              std::cout << currState.print(dataflow) << std::endl;
              if (end_check){
                if (actors_check[dataflow->getVertexId(t)] < 0){
                  actors_check[dataflow->getVertexId(t)] = curr_step;
                  starts[dataflow->getVertexId(t)].back().push_back(curr_step);
                  --actors_left;
                }
                if (actors_left == 0){
                  {ForEachTask(dataflow, task){ //Creates schedule after additional actor is fired after end of period
                    static_task_schedule_t initials; 
                    periodic_task_schedule_t periodics;
                    for(std::size_t i = 0; i < starts[dataflow->getVertexId(task)].size(); ++i){
                      if (i < periodic_state_idx){ //initials
                        initials.insert(initials.end(),starts[dataflow->getVertexId(task)][i].begin(),starts[dataflow->getVertexId(task)][i].end());
                        continue;
                      } 
                      if (i == starts[dataflow->getVertexId(task)].size() - 1){
                        periodics.second.insert(periodics.second.end(),starts[dataflow->getVertexId(task)][i].begin(),starts[dataflow->getVertexId(task)][i].end()-1);
                      } else { //periodic
                        periodics.second.insert(periodics.second.end(),starts[dataflow->getVertexId(task)][i].begin(),starts[dataflow->getVertexId(task)][i].end());
                      }
                    }
                    periodics.first = actors_check[dataflow->getVertexId(task)] - periodics.second[0]; 
                    task_schedule_t sched_struct = {initials,periodics};
                    schedule.set(dataflow->getVertexId(task), sched_struct);
                    std::cout << dataflow->getVertexName(task) << ": initial starts=" << commons::toString(initials) << ", periodic starts=" << commons::toString(periodics) << std::endl;
                  }}
                  return std::make_pair(thr, schedule);
                }

              } else {
                if (starts[dataflow->getVertexId(t)].size() == 0){
                  starts[dataflow->getVertexId(t)].push_back({curr_step}); //push into current state's task start list
                } else {
                  starts[dataflow->getVertexId(t)].back().push_back(curr_step);
                }
              }
              break; //finds first actor edge to execute then break the for loop for the while loop to continue
            } else {
              skip_time = condition[dataflow->getEdgeId(e)] + slots-((int) curr_step % slots); //dont execute until current actor fires
              skip_edge = dataflow->getEdgeId(e);
              skip_vertex = dataflow->getVertexId(t);
              break;
            }
          }}
          if (skip_time != 0){
            break;
          }
        }
        if (skip_time != 0){ //TODO: too ugly
            break;
        }
      }}
    // advance time and check for deadlocks
    if (skip_time != 0){
      timeStep = currState.advanceTimeWithMod(skip_time);
      curr_step+= timeStep;
    } else {
      timeStep = currState.advanceTimeWithMod(0);
      curr_step+= timeStep;
    }
    if (timeStep == LONG_MAX) { // NOTE should technically be LDBL_MAX cause TIME_UNIT is of type long double
      VERBOSE_INFO("Deadlock found!");
      return std::make_pair(0, schedule);
    }
  }
}

void algorithms::scheduling::So4Scheduling(models::Dataflow* const dataflow,
                                         parameters_list_t param_list) {
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
  std::map<int, std::vector<ARRAY_INDEX>> sccMap;
  std::vector<models::Dataflow*> sccDataflows;
  TIME_UNIT minThroughput = LONG_MAX; // NOTE should technically be LDBL_MAX cause TIME_UNIT is of type long double
  
  scheduling_t scheduling_result;
  int linesize = param_list.count("LINE")? commons::fromString<int>(param_list["LINE"]) : 80;

  // generate SCCs if any
  sccMap = computeSCCKosaraju(dataflow);
  if (sccMap.size() > 1) { // if the original graph isn't one SCC, need to break into SCC subgraphs
    sccDataflows = generateSCCs(dataflow, sccMap);
    for (auto g : sccDataflows) {
      if (g->getEdgesCount() > 0) {
        std::pair<ARRAY_INDEX, EXEC_COUNT> actorInfo;
        auto res_pair = computeComponentSo4Schedule(g, actorInfo, scheduling_result, param_list["TDMA"]);
        TIME_UNIT componentThroughput = res_pair.first;
        scheduling_result = res_pair.second;
        TIME_UNIT scaledThroughput = (componentThroughput * actorInfo.second) /
          dataflow->getNi(dataflow->getVertexById(actorInfo.first));
        if (scaledThroughput < minThroughput) {
          minThroughput = scaledThroughput;
        }
      } else if (g->getVerticesCount() == 1 && g->getEdgesCount() == 0) {
        /* NOTE this is a workaround from ignoring reentrancy edges --- if this
           condition is met, we assume that we have a single actor with re-entrancy */
        ARRAY_INDEX standaloneId = g->getVertexId(g->getFirstVertex());
        EXEC_COUNT standaloneRepFactor = g->getPhasesQuantity(g->getFirstVertex());
        Vertex vertexInDataflow = dataflow->getVertexById(standaloneId);
        /* repetition factor for standalone component will be equal to its phase count:
           this makes sense because, while it's producing and consuming 1 token in its
           re-entrant edge, it will need to execute its number of phases to arrive
           back at the same state */
        EXEC_COUNT repFactor = dataflow->getNi(vertexInDataflow);
        TIME_UNIT componentThroughput = (TIME_UNIT) (1 * standaloneRepFactor) /
          dataflow->getVertexTotalDuration(vertexInDataflow);
        TIME_UNIT scaledThroughput = (TIME_UNIT) componentThroughput / repFactor;
        if (scaledThroughput < minThroughput) {
          minThroughput = scaledThroughput;
        }
      }
    }
    
    TIME_UNIT omega = 1.0 / minThroughput ;
    models::Scheduling res = models::Scheduling(dataflow, omega, scheduling_result);

    std::cout << res.asASCII(linesize);
    std::cout << res.asText();
    std::cout << printers::Scheduling2Tikz(res);

    std::cout << "ASAP throughput is  " << minThroughput << std::endl;
    std::cout << "ASAP period is  " << omega << std::endl;

    return;
  }
  // if graph is strongly connected, just need to use computeComponentThroughput
  std::pair<ARRAY_INDEX, EXEC_COUNT> actorInfo; // look at note for computeComponentThroughput
  auto res_pair = computeComponentSo4Schedule(dataflow, actorInfo, scheduling_result, param_list["TDMA"]);
  minThroughput = res_pair.first;
  scheduling_result = res_pair.second;
  std::cout << "Throughput of graph: " << minThroughput << std::endl;
  TIME_UNIT omega = 1.0 / minThroughput ;
  models::Scheduling res = models::Scheduling(dataflow, omega, scheduling_result);
  std::cout << res.asASCII(linesize);
  std::cout << res.asText();

  std::cout << "ASAP throughput is  " << minThroughput << std::endl;
  std::cout << "ASAP period is  " << omega << std::endl;

  return;
}
