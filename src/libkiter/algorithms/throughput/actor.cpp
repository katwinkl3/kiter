/*
 * actor.cpp
 *
 *  Created on: 22 December 2020
 *      Author: jkmingwen
 */

#include "actor.h"
#include <models/Dataflow.h>

Actor::Actor()
  :actor{},
   numExecs{0},
   phaseCount{0},
   consPhaseCount(),
   prodPhaseCount(),
   prodExecRate(),
   consExecRate() {}

Actor::Actor(models::Dataflow* const dataflow, Vertex a) {
  actor = a;
  numExecs = 0;
  phaseCount = dataflow->getPhasesQuantity(actor);
  std::cout << "initialising actor " << dataflow->getVertexName(actor) << std::endl;
  {ForInputEdges(dataflow, actor, e) {
      std::cout << "input port execution rates (phases = " << dataflow->getEdgeOutPhasesCount(e) <<"): ";
      consPhaseCount[e] = dataflow->getEdgeOutPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          std::cout << dataflow->getEdgeOutPhase(e, p) << " ";
          consExecRate[e][p] = dataflow->getEdgeOutPhase(e, p);
        }}
      std::cout << std::endl;
    }}
  {ForOutputEdges(dataflow, actor, e) {
      std::cout << "output port execution rates (phases = " << dataflow->getEdgeInPhasesCount(e) <<"): ";
      prodPhaseCount[e] = dataflow->getEdgeInPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          std::cout << dataflow->getEdgeInPhase(e, p) << " ";
          prodExecRate[e][p] = dataflow->getEdgeInPhase(e, p);
        }}
      std::cout << std::endl;
    }}
}

EXEC_COUNT Actor::getPhaseCount(Edge e) {
  assert(this->consPhaseCount.find(e) != this->consPhaseCount.end() ||
         this->prodPhaseCount.find(e) != this->prodPhaseCount.end()); // given edge must be either input/output edge
  if (this->consPhaseCount.find(e) != this->consPhaseCount.end()) {
    return this->consPhaseCount[e];
  } else if (this->prodPhaseCount.find(e) != this->prodPhaseCount.end()) {
    return this->prodPhaseCount[e];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

// Return phase of execution for given edge (allows for different number of phases for each port)
PHASE_INDEX Actor::getPhase(Edge e) {
  return (this->getNumExecutions() % getPhaseCount(e)) + 1;
}

// Return phase of execution for actor (assumes that all ports have equal number of phases)
PHASE_INDEX Actor::getPhase() {
  return (this->getNumExecutions() % this->phaseCount) + 1;
}

TOKEN_UNIT Actor::getExecRate(Edge e) {
  assert(this->prodExecRate.find(e) != this->prodExecRate.end() ||
         this->consExecRate.find(e) != this->consExecRate.end()); // given edge must be either input/output edge
  PHASE_INDEX curPhase = (this->getNumExecutions() % getPhaseCount(e)) + 1; // phase indexing starts from 1 but stored in Actor class as starting from 0
  if (this->prodExecRate.find(e) != this->prodExecRate.end()) {
    return this->prodExecRate[e][curPhase];
  } else if (this->consExecRate.find(e) != this->consExecRate.end()) {
    return this->consExecRate[e][curPhase];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

TOKEN_UNIT Actor::getExecRate(Edge e, PHASE_INDEX p) {
  assert(this->prodExecRate.find(e) != this->prodExecRate.end() ||
         this->consExecRate.find(e) != this->consExecRate.end()); // given edge must be either input/output edge
  if (this->prodExecRate.find(e) != this->prodExecRate.end()) {
    return this->prodExecRate[e][p];
  } else if (this->consExecRate.find(e) != this->consExecRate.end()) {
    return this->consExecRate[e][p];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

EXEC_COUNT Actor::getNumExecutions() {
  return this->numExecs;
}

// Given current state of graph, returns whether actor is ready to execute or not
bool Actor::isReadyForExec(State s) {
  // Execution conditions (for given phase):
  // (1) enough room in output channel, (2) enough tokens in input channel, (3) not currently executing
  bool isExecutable = true;
  for (auto const &e : this->consPhaseCount) {
    if (s.getTokens(e.first) < this->getExecRate(e.first)) {
      isExecutable = false;
    }
  }
  return isExecutable;
}

// Begin actor's execution, consuming tokens from input channels
void Actor::execStart(models::Dataflow* const dataflow) {
  dataflow->reset_computation();
  {ForInputEdges(dataflow, this->actor, e) {
      dataflow->setPreload(e, dataflow->getPreload(e) - this->getExecRate(e));
    }}
  this->numExecs++;
}

// End actor's execution, producing tokens into output channels
void Actor::execEnd(models::Dataflow* const dataflow) {
  dataflow->reset_computation();
  {ForOutputEdges(dataflow, this->actor, e) {
      dataflow->setPreload(e, dataflow->getPreload(e) + this->getExecRate(e));
    }}
}

void Actor::printStatus(models::Dataflow* const dataflow) {
  std::cout << "Actor " << dataflow->getVertexName(this->actor) << std::endl;
  std::cout << "\tNumber of executions: " << this->getNumExecutions() << std::endl;
  {ForInputEdges(dataflow, this->actor, e) {
      std::cout << "\tTokens consumed from Channel " << dataflow->getEdgeName(e)
                << ": " << this->getExecRate(e) << std::endl;
    }}
  {ForOutputEdges(dataflow, this->actor, e) {
      std::cout << "\tTokens produced into Channel " << dataflow->getEdgeName(e)
                << ": " << this->getExecRate(e) << std::endl;
    }}
}
