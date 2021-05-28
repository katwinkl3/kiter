/*
 * symbolic_execution.cpp
 *
 *  Created on: 15 December 2020
 *      Author: jkmingwen
 */

#include <map>
#include <vector>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include "symbolic_execution.h"
#include "actor.h"
#include "state.h"

// TODO: Functions to add:
// (0) Actor class
// - Attributes: current phase, number of phases, execution rates (indexed by phase), status (currently firing)
// - Methods: advance phase, execute
// (1) Print status

void algorithms::compute_asap_throughput(models::Dataflow* const dataflow,
                                         parameters_list_t param_list) {
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
  StateList visitedStates;
  EXEC_COUNT minRepFactor = INT_MAX; // EXEC_COUNT is of type long int, and LONG_MAX has same max value
  ARRAY_INDEX minRepActorId;
  TOKEN_UNIT minRepActorExecCount = 0;
  TIME_UNIT timeStep;

  // initialise actors
  std::map<ARRAY_INDEX, Actor> actorMap;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
      std::cout << std::endl;
    }}
  State prevState(dataflow, actorMap);
  State currState(dataflow, actorMap);
  std::cout << "Printing initial state status" << std::endl;
  prevState.print(dataflow);
  std::cout << "Printing actor statuses:" << std::endl;
  printStatus(dataflow);
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
      // track actor with lowest repetition factor (determines when states are stored)
      if (actorMap[dataflow->getVertexId(t)].getRepVec() < minRepFactor) {
        minRepFactor = actorMap[dataflow->getVertexId(t)].getRepVec();
        minRepActorId = actorMap[dataflow->getVertexId(t)].getId();
      }
    }}
  std::cout << "Actor with ID " << minRepActorId
            << " is actor with lowest repetition factor ("
            << minRepFactor << ")" << std::endl;

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
              // TODO add to state list and check for revisited state
                std::cout << "Adding the following state to list of visited states:" << std::endl;
                currState.print(dataflow);
              if (!visitedStates.addState(currState)) {
                std::cout << "ending execution and computing throughput" << std::endl;
                // should now compute throughput using recurrent state
                std::cout<< "throughput computed is: "
                         << visitedStates.computeThroughput() << std::endl;
                return;
              }
              currState.setTimeElapsed(0);
              minRepActorExecCount = 0;
            }
          }
          actorMap[dataflow->getVertexId(t)].execEnd(dataflow, currState);
          currState.updateState(dataflow, actorMap); // NOTE updating tokens/phase in state done separately from execEnd function, might be a cause for bugs
        }
      }}
    std::cout << "Printing current state status after ending firing" << std::endl;
    currState.print(dataflow);
    std::cout << "Printing Actor Statuses:" << std::endl;
    {ForEachTask(dataflow, t) {
        actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
      }}
    printStatus(dataflow);
    // start actor firing
    printStatus(dataflow);
    {ForEachTask(dataflow, t) {
        while (actorMap[dataflow->getVertexId(t)].isReadyForExec(currState)) {
          actorMap[dataflow->getVertexId(t)].execStart(dataflow, currState);
          currState.updateState(dataflow, actorMap);
        }
      }}
    std::cout << "Printing Current State Status" << std::endl;
    currState.print(dataflow);
    std::cout << "Printing Actor Statuses:" << std::endl;
    {ForEachTask(dataflow, t) {
        actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
      }}
    printStatus(dataflow);
    // advance time and check for deadlocks
    timeStep = currState.advanceTime();
    if (timeStep == LONG_MAX) { // NOTE should technically be LDBL_MAX cause TIME_UNIT is of type long double
      std::cout << "Deadlock found!" << std::endl;
      return; // should be returning 0
    }
  }
}

// prints current status of dataflow graph
void algorithms::printStatus(models::Dataflow* const dataflow) {
  std::cout << "Token counts:" << std::endl;
  {ForEachEdge(dataflow, e) {
      std::cout << "\tChannel " << dataflow->getEdgeName(e) << " ("
                << dataflow->getVertexName(dataflow->getEdgeSource(e))
                << "->"
                << dataflow->getVertexName(dataflow->getEdgeTarget(e))
                << "): " << dataflow->getPreload(e) << std::endl;
    }}
}
