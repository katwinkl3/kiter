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

  // initialise actors
  std::map<ARRAY_INDEX, Actor> actorMap;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
      std::cout << std::endl;
    }}
  State prevState(dataflow, actorMap);
  State currState(dataflow, actorMap);
  std::cout << "Printing Previous State Status" << std::endl;
  prevState.print(dataflow);
  std::cout << "Printing Actor Statuses:" << std::endl;
  printStatus(dataflow);
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
    }}
  // testing functions
  printStatus(dataflow);
  {ForEachTask(dataflow, t) {
      while (actorMap[dataflow->getVertexId(t)].isReadyForExec(dataflow)) {
        actorMap[dataflow->getVertexId(t)].execStart(dataflow);
        currState.addExecution(t,
                               dataflow->getVertexDuration(t, actorMap[dataflow->getVertexId(t)].getPhase())); // use phase to mimic num execution
      }
    }}
  currState.updateState(dataflow, actorMap);
  std::cout << "Printing Current State Status" << std::endl;
  currState.print(dataflow);
  std::cout << "Printing Actor Statuses:" << std::endl;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
    }}
  printStatus(dataflow);
  // {ForEachTask(dataflow, t) {
  //     if (actorMap[dataflow->getVertexId(t)].isReadyForExec(dataflow)) {
  //       std::cout << "Actor " << dataflow->getVertexName(t) << " ready for execution" << std::endl;
  //       std::cout << "executing..." << std::endl;
  //       actorMap[dataflow->getVertexId(t)].execStart(dataflow);
  //       currState.addExecution(t,
  //                              dataflow->getVertexDuration(t, actorMap[dataflow->getVertexId(t)].getPhase())); // use phase to mimic num execution
  //       actorMap[dataflow->getVertexId(t)].execEnd(dataflow);
  //       printStatus(dataflow);
  //       actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
  //     } else {
  //       std::cout << "Actor " << dataflow->getVertexName(t) << " NOT ready for execution" << std::endl;
  //       printStatus(dataflow);
  //       actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
  //     }
  //     std::cout << std::endl;
  //   }}
  // currState.updateState(dataflow, actorMap);
  // currState.print(dataflow);
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
