// /*
//  * symbolic_execution.cpp
//  *
//  *  Created on: 15 December 2020
//  *      Author: jkmingwen
//  */

// #include <map>
// #include <vector>
// #include <printers/stdout.h>
// #include <commons/verbose.h>
// #include <commons/commons.h>
// #include <models/Dataflow.h>
// #include "symbolic_execution.h"
// #include "actor.h"
// #include "state.h"

// // TODO: Functions to add:
// // (0) Actor class
// // - Attributes: current phase, number of phases, execution rates (indexed by phase), status (currently firing)
// // - Methods: advance phase, execute
// // (1) Print status

// void algorithms::compute_asap_throughput(models::Dataflow* const dataflow,
//                                          parameters_list_t param_list) {
//   VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
//   VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
//   StateList visitedStates;
//   EXEC_COUNT minRepFactor = INT_MAX; // EXEC_COUNT is of type long int, and LONG_MAX has same max value
//   ARRAY_INDEX minRepActorId;
//   TOKEN_UNIT minRepActorExecCount = 0;
//   TIME_UNIT timeStep;

//   // initialise actors
//   std::map<ARRAY_INDEX, Actor> actorMap;
//   {ForEachTask(dataflow, t) {
//       actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
//       VERBOSE_INFO("\n");
//     }}
//   State prevState(dataflow, actorMap);
//   State currState(dataflow, actorMap);
//   VERBOSE_INFO("Printing initial state status");
//   VERBOSE_INFO(prevState.print(dataflow));
//   VERBOSE_INFO("Printing actor statuses:");
//   VERBOSE_INFO(printStatus(dataflow));
//   {ForEachTask(dataflow, t) {
//       VERBOSE_INFO(actorMap[dataflow->getVertexId(t)].printStatus(dataflow));
//       // track actor with lowest repetition factor (determines when states are stored)
//       if (actorMap[dataflow->getVertexId(t)].getRepVec() < minRepFactor) {
//         minRepFactor = actorMap[dataflow->getVertexId(t)].getRepVec();
//         minRepActorId = actorMap[dataflow->getVertexId(t)].getId();
//       }
//     }}
//   VERBOSE_INFO("Actor with ID " << minRepActorId
//                << " is actor with lowest repetition factor ("
//                << minRepFactor << ")");
//   // Start ASAP execution loop
//   while (true) {
//     {ForEachEdge(dataflow, e) {
//         prevState.setTokens(e, currState.getTokens(e));
//       }}
//     // end actor firing
//     {ForEachTask(dataflow, t) {
//         while (actorMap[dataflow->getVertexId(t)].isReadyToEndExec(currState)) {
//           if (actorMap[dataflow->getVertexId(t)].getId() == minRepActorId) {
//             minRepActorExecCount++;
//             if (minRepActorExecCount == minRepFactor) {
//               // TODO add to state list and check for revisited state
//               VERBOSE_INFO("Adding the following state to list of visited states:");
//               VERBOSE_INFO(currState.print(dataflow));
//               if (!visitedStates.addState(currState)) {
//                 VERBOSE_INFO("ending execution and computing throughput");
//                 // should now compute throughput using recurrent state
//                 TIME_UNIT thr = visitedStates.computeThroughput();
//                 std::cout << "throughput computed is: " << thr << std::endl;
//                 return;
//               }
//               currState.setTimeElapsed(0);
//               minRepActorExecCount = 0;
//             }
//           }
//           actorMap[dataflow->getVertexId(t)].execEnd(dataflow, currState);
//           currState.updateState(dataflow, actorMap); // NOTE updating tokens/phase in state done separately from execEnd function, might be a cause for bugs
//         }
//       }}
//     VERBOSE_INFO("Printing current state status after ending firing");
//     VERBOSE_INFO(currState.print(dataflow));
//     VERBOSE_INFO("Printing Actor Statuses:");
//     {ForEachTask(dataflow, t) {
//         VERBOSE_INFO(actorMap[dataflow->getVertexId(t)].printStatus(dataflow));
//       }}
//     VERBOSE_INFO(printStatus(dataflow));
//     // start actor firing
//     {ForEachTask(dataflow, t) {
//         while (actorMap[dataflow->getVertexId(t)].isReadyForExec(currState)) {
//           actorMap[dataflow->getVertexId(t)].execStart(dataflow, currState);
//           currState.updateState(dataflow, actorMap);
//         }
//       }}
//     VERBOSE_INFO("Printing Current State Status");
//     VERBOSE_INFO(currState.print(dataflow));
//     VERBOSE_INFO("Printing Actor Statuses:");
//     {ForEachTask(dataflow, t) {
//         VERBOSE_INFO(actorMap[dataflow->getVertexId(t)].printStatus(dataflow));
//       }}
//     VERBOSE_INFO(printStatus(dataflow));
//     // advance time and check for deadlocks
//     timeStep = currState.advanceTime();
//     if (timeStep == LONG_MAX) { // NOTE should technically be LDBL_MAX cause TIME_UNIT is of type long double
//       VERBOSE_INFO("Deadlock found!");
//       return; // should be returning 0
//     }
//   }
// }

// // prints current status of dataflow graph
// std::string algorithms::printStatus(models::Dataflow* const dataflow) {
//   std::stringstream outputStream;

//   outputStream << "\nToken counts:" << std::endl;
//   {ForEachEdge(dataflow, e) {
//       outputStream << "\tChannel " << dataflow->getEdgeName(e) << " ("
//                    << dataflow->getVertexName(dataflow->getEdgeSource(e))
//                    << "->"
//                    << dataflow->getVertexName(dataflow->getEdgeTarget(e))
//                    << "): " << dataflow->getPreload(e) << std::endl;
//     }}
//   return outputStream.str();
// }