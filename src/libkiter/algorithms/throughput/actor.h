/*
 * actor.h
 *
 *  Created on: 22 December 2020
 *      Author: jkmingwen
 */

#include <models/Dataflow.h>

namespace models {
  class Dataflow;
}

class Actor {
 public:
  Actor();
  Actor(models::Dataflow* const dataflow, Vertex a); // actor must be associated to a graph

  void setPhase(Edge e, PHASE_INDEX p);
  EXEC_COUNT getPhaseCount(Edge e);
  PHASE_INDEX getPhase(Edge e); // returns current phase
  TOKEN_UNIT getExecRate(Edge e);
  EXEC_COUNT getNumExecutions();
  void advancePhase(models::Dataflow* const dataflow);
  bool isReadyForExec(models::Dataflow* const dataflow);
  void execute(models::Dataflow* const dataflow);

 private:
  Vertex actor;
  EXEC_COUNT numExecs; // track number of executions
  std::map<Edge, PHASE_INDEX> currentPhase;
  std::map<Edge, EXEC_COUNT> consPhaseCount; // number of consumption phases of execution
  std::map<Edge, EXEC_COUNT> prodPhaseCount; // number of production phases of execution
  std::map<Edge, std::map<PHASE_INDEX, TOKEN_UNIT>> prodExecRate;
  std::map<Edge, std::map<PHASE_INDEX, TOKEN_UNIT>> consExecRate;
};
