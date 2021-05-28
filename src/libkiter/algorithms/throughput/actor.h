/*
 * actor.h
 *
 *  Created on: 22 December 2020
 *      Author: jkmingwen
 */
#ifndef ACTOR_H_
#define ACTOR_H_

#include "state.h"
#include <models/Dataflow.h>

namespace models {
  class Dataflow;
}

class State;

class Actor {
 public:
  Actor();
  Actor(models::Dataflow* const dataflow, Vertex a); // actor must be associated to a graph

  EXEC_COUNT getPhaseCount(Edge e);
  PHASE_INDEX getPhase(Edge e); // returns current phase using edge (allows for different number of phases for each port)
  PHASE_INDEX getPhase();
  EXEC_COUNT getRepVec();
  ARRAY_INDEX getId();
  void setId(ARRAY_INDEX newId);
  TOKEN_UNIT getExecRate(Edge e);
  TOKEN_UNIT getExecRate(Edge e, PHASE_INDEX p);
  EXEC_COUNT getNumExecutions();
  bool isReadyForExec(State s);
  bool isReadyToEndExec(State s);
  void execStart(models::Dataflow* const dataflow, State &s);
  void execEnd(models::Dataflow* const dataflow, State &s);
  void printStatus(models::Dataflow* const dataflow);
 private:
  Vertex actor;
  EXEC_COUNT numExecs; // track number of executions
  PHASE_INDEX phaseCount; // number of phases of execution
  EXEC_COUNT repVector;
  ARRAY_INDEX id;
  std::map<Edge, EXEC_COUNT> consPhaseCount; // number of consumption phases of execution
  std::map<Edge, EXEC_COUNT> prodPhaseCount; // number of production phases of execution
  std::map<Edge, std::map<PHASE_INDEX, TOKEN_UNIT>> prodExecRate;
  std::map<Edge, std::map<PHASE_INDEX, TOKEN_UNIT>> consExecRate;
};
#endif /* ACTOR_H_ */
