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
   currentPhase(),
   consPhaseCount(),
   prodPhaseCount(),
   prodExecRate(),
   consExecRate() {}

Actor::Actor(models::Dataflow* const dataflow, Vertex a) {
  actor = a;
  numExecs = 0;
  std::cout << "initialising actor " << dataflow->getVertexName(actor) << std::endl;
  {ForInputEdges(dataflow, actor, e) {
      std::cout << "input port execution rates (phases = " << dataflow->getEdgeOutPhasesCount(e) <<"): ";
      currentPhase[e] = 0;
      consPhaseCount[e] = dataflow->getEdgeOutPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          std::cout << dataflow->getEdgeOutPhase(e, p) << " ";
          consExecRate[e][p] = dataflow->getEdgeOutPhase(e, p);
        }}
      std::cout << std::endl;
    }}
  {ForOutputEdges(dataflow, actor, e) {
      std::cout << "output port execution rates (phases = " << dataflow->getEdgeInPhasesCount(e) <<"): ";
      currentPhase[e] = 0;
      prodPhaseCount[e] = dataflow->getEdgeInPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          std::cout << dataflow->getEdgeInPhase(e, p) << "(p=" << p << ") ";
          prodExecRate[e][p] = dataflow->getEdgeInPhase(e, p);
        }}
      std::cout << std::endl;
    }}
}

void Actor::setPhase(Edge e, PHASE_INDEX p) {
  assert(p <= getPhaseCount(e));
  this->currentPhase[e] = p;
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

PHASE_INDEX Actor::getPhase(Edge e) {
  assert(this->currentPhase.find(e) != this->currentPhase.end());
  return this->currentPhase[e];
}

TOKEN_UNIT Actor::getExecRate(Edge e) {
  assert(this->prodExecRate.find(e) != this->prodExecRate.end() ||
         this->consExecRate.find(e) != this->consExecRate.end()); // given edge must be either input/output edge
  PHASE_INDEX curPhase = this->getPhase(e) + 1; // phase indexing starts from 1 but stored in Actor class as starting from 0
  if (this->prodExecRate.find(e) != this->prodExecRate.end()) {
    return this->prodExecRate[e][curPhase];
  } else if (this->consExecRate.find(e) != this->consExecRate.end()) {
    return this->consExecRate[e][curPhase];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

EXEC_COUNT Actor::getNumExecutions() {
  return this->numExecs;
}

// Advances phase for all ports of given channel
void Actor::advancePhase(models::Dataflow* const dataflow) {
  {ForInputEdges(dataflow, this->actor, e) {
      PHASE_INDEX nextPhase = (getPhase(e) + 1) % getPhaseCount(e);
      std::cout << "Current phase (input edge) is " << getPhase(e) << std::endl;
      this->getExecRate(e);
      std::cout << "Next phase (input edge) is " << nextPhase << std::endl;
      this->setPhase(e, nextPhase);
      this->getExecRate(e);
    }}
  {ForOutputEdges(dataflow, this->actor, e) {
      PHASE_INDEX nextPhase = (getPhase(e) + 1) % getPhaseCount(e);
      std::cout << "Current phase (output edge) is " << getPhase(e) << std::endl;
      this->getExecRate(e);
      std::cout << "Next phase (output edge) is " << nextPhase << std::endl;
      this->setPhase(e, nextPhase);
      this->getExecRate(e);
    }}
}

// Given current state of graph, returns whether actor is ready to execute or not
bool Actor::isReadyForExec(models::Dataflow* const dataflow) {
  // Execution conditions (for given phase):
  // (1) enough room in output channel, (2) enough tokens in input channel, (3) not currently executing
  bool isExecutable = true;
  {ForInputEdges(dataflow, this->actor, e) { // check if input channel has enough tokens for execution
      if (dataflow->getPreload(e) < this->getExecRate(e)) {
        isExecutable = false;
      }
    }}
  return isExecutable;
}

// Execute actor, consuming and producing tokens accordingly
void Actor::execute(models::Dataflow* const dataflow) {
  dataflow->reset_computation();
  {ForInputEdges(dataflow, this->actor, e) {
      dataflow->setPreload(e, dataflow->getPreload(e) - this->getExecRate(e));
    }}
  {ForOutputEdges(dataflow, this->actor, e) {
      dataflow->setPreload(e, dataflow->getPreload(e) + this->getExecRate(e));
    }}
  this->advancePhase(dataflow);
  this->numExecs++;
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
