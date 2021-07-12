/*
 * actor.cpp
 *
 *  Created on: 22 December 2020
 *      Author: jkmingwen
 *  Modified: June 2021 by katwinkl3
 */

#include "actor.h"
#include <models/Dataflow.h>

Actor::Actor()
  :actor{},
   numExecs{0}, // NOTE this might not be necessary as we only need to track numExecs for actor with lowest repetition factor
   phaseCount{0},
   repFactor{0},
   id{},
   isExecuting{false},
   consPhaseCount(),
   prodPhaseCount(),
   prodExecRate(),
   consExecRate() {}

Actor::Actor(models::Dataflow* const dataflow, Vertex a) {
  actor = a;
  numExecs = 0;
  phaseCount = dataflow->getPhasesQuantity(actor);
  repFactor = dataflow->getNi(actor);
  id = dataflow->getVertexId(actor);
  isExecuting = false;
  VERBOSE_INFO("initialising actor " << dataflow->getVertexName(actor));
  {ForInputEdges(dataflow, actor, e) {
      VERBOSE_INFO("input port execution rates (phases = "
                   << dataflow->getEdgeOutPhasesCount(e) <<"): ");
      consPhaseCount[e] = dataflow->getEdgeOutPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          VERBOSE_INFO(dataflow->getEdgeOutPhase(e, p) << " ");
          consExecRate[e][p] = dataflow->getEdgeOutPhase(e, p);
        }}
      VERBOSE_INFO("\n");
    }}
  {ForOutputEdges(dataflow, actor, e) {
      VERBOSE_INFO("output port execution rates (phases = "
                   << dataflow->getEdgeInPhasesCount(e) <<"): ");
      prodPhaseCount[e] = dataflow->getEdgeInPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          VERBOSE_INFO(dataflow->getEdgeInPhase(e, p) << " ");
          prodExecRate[e][p] = dataflow->getEdgeInPhase(e, p);
        }}
      VERBOSE_INFO("\n");
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

EXEC_COUNT Actor::getRepFactor() {
  return this->repFactor;
}

ARRAY_INDEX Actor::getId() {
  return this->id;
}

void Actor::setId(ARRAY_INDEX newId) {
  this->id = newId;
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
    if (s.getTokens(e.first) < this->getExecRate(e.first) || this->isExecuting) {
      isExecutable = false;
    }
  }
  return isExecutable;
}

// Outputs condition for whether actor can 0: execute 1: wait for another slot 2: do nothing
// int Actor::isReadyForExecWithMod(State s) {
//   bool lack_tokens = false;
//   for (auto const &e : this->consPhaseCount) {
//     if (this->isExecuting) {
//       return 2;
//     }
//     if (s.getTokens(e.first) < this->getExecRate(e.first)){
//       lack_tokens = true;
//     }
//   }
//   return (int) lack_tokens;
// }

// Given current state of graph, returns whether actor is ready to end execution or not
bool Actor::isReadyToEndExec(State s) {
  bool isEndable = true;
  if (s.getExecQueue()[this->actor].empty() ||
      s.getExecQueue()[this->actor].front().first != 0) {
    isEndable = false;
  }
  return isEndable;
}

// Begin actor's execution, consuming tokens from input channels
void Actor::execStart(models::Dataflow* const dataflow, State &s) {
  dataflow->reset_computation();
  {ForInputEdges(dataflow, this->actor, e) {
      dataflow->setPreload(e, dataflow->getPreload(e) - this->getExecRate(e));
    }}
  std::pair<TIME_UNIT, PHASE_INDEX> newExec(dataflow->getVertexDuration(this->actor,
                                                                        this->getPhase()),
                                            this->getPhase());
  s.addExecution(this->actor, newExec);
  this->numExecs++;
  this->isExecuting = true;
}

// Execute and store record for tokens to be produced 
// void Actor::execStartWithMod(models::Dataflow* const dataflow, State &s, std::map<TIME_UNIT, std::map<ARRAY_INDEX, std::pair<long, bool>>> *buffer, 
// TIME_UNIT step, long slots, std::map<ARRAY_INDEX, long> condition) {
//   dataflow->reset_computation();
//   std::pair<TIME_UNIT, PHASE_INDEX> newExec(dataflow->getVertexDuration(this->actor,
//                                                                       this->getPhase()),
//                                           this->getPhase());
//   {ForInputEdges(dataflow, this->actor, e) {
//     dataflow->setPreload(e, dataflow->getPreload(e) - this->getExecRate(e));
//     TIME_UNIT end_t = step+newExec.first; //without considering respective slot
//     TIME_UNIT new_t;
//     long end_t_mod = (int) end_t % slots;
//     long correct_slot = condition[dataflow->getEdgeId(e)];
//     if (end_t_mod <= correct_slot){
//       new_t = end_t + (correct_slot - end_t_mod);
//     } else{
//       new_t = end_t + slots - (end_t_mod - correct_slot);
//     }
//     if (!(*buffer)[new_t].count(dataflow->getEdgeId(e))){ // prepare event where tokens=0 (to be filled in later) and actor execution=false
//       (*buffer)[new_t][dataflow->getEdgeId(e)] = {0, false};
//     }
//   }}
//   s.addExecution(this->actor, newExec);
//   this->numExecs++;
//   this->isExecuting = true;
// }

// Only stores record for to-be executed actors at their respective slots
// void Actor::execNextStartWithMod(models::Dataflow* const dataflow, State &s, std::map<TIME_UNIT, std::map<ARRAY_INDEX, std::pair<long, bool>>> *buffer, 
// TIME_UNIT step, long slots, std::map<ARRAY_INDEX, long> condition) {
//   std::pair<TIME_UNIT, PHASE_INDEX> newExec(dataflow->getVertexDuration(this->actor,
//                                                                       this->getPhase()),
//                                           this->getPhase());
//   {ForInputEdges(dataflow, this->actor, e) {
//     TIME_UNIT t_mod = (int) step % slots;
//     long correct_slot = condition[dataflow->getEdgeId(e)];
//     TIME_UNIT next_t;
//     if (t_mod <= correct_slot){
//        next_t = step + (correct_slot - t_mod);
//     } else{
//       next_t = step + slots - (t_mod - correct_slot);
//     }
//     (*buffer)[next_t][dataflow->getEdgeId(e)].first += 0; // do not affect the token val that will be released at next_t
//     (*buffer)[next_t][dataflow->getEdgeId(e)].second = true; // add execution event at slot
//   s.addExecution(this->actor, newExec);
//   this->numExecs++;
//   this->isExecuting = true;
//   }}
// }

// End actor's execution, producing tokens into output channels
void Actor::execEnd(models::Dataflow* const dataflow, State &s) {
  PHASE_INDEX currentPhase;
  dataflow->reset_computation();
  {ForOutputEdges(dataflow, this->actor, e) {
      currentPhase = s.getRemExecTime(this->actor).front().second;
      VERBOSE_INFO("ending execution for phase "
                << currentPhase << ", producing "
                << this->getExecRate(e, currentPhase) << " tokens");
      dataflow->setPreload(e, dataflow->getPreload(e) + this->getExecRate(e, currentPhase));
    }}
  s.removeFrontExec(this->actor);
  this->isExecuting = false;
}

// End actor's execution without producing tokens
void Actor::execEndWithMod(models::Dataflow* const dataflow, State &s, std::deque<std::pair<TIME_UNIT, std::pair<ARRAY_INDEX, long>>> *buffer,
TIME_UNIT step, long slots, std::map<ARRAY_INDEX, long> condition) {
  PHASE_INDEX currentPhase;
  dataflow->reset_computation();
  {ForOutputEdges(dataflow, this->actor, e) {
      currentPhase = s.getRemExecTime(this->actor).front().second;
      TIME_UNIT end_time;
      long end_t_mod = (int) step % slots;
      long correct_slot = condition[dataflow->getEdgeId(e)];
      if (end_t_mod <= correct_slot){
        end_time = step + (correct_slot - end_t_mod);
      } else{
        end_time = step + slots - (end_t_mod - correct_slot);
      }
      // (*buffer)[step][dataflow->getEdgeId(e)].first += this->getExecRate(e, currentPhase);
      (*buffer).push_back({end_time, {dataflow->getEdgeId(e), this->getExecRate(e, currentPhase)}});
      /// buffer[dtep].puhback(pair<>(edgeid,execrate))
    }}
  s.removeFrontExec(this->actor);
  this->isExecuting = false;
}

std::string Actor::printStatus(models::Dataflow* const dataflow) {
  std::stringstream outputStream;

  outputStream << "\nActor " << dataflow->getVertexName(this->actor)
               << " (ID: " << this->getId() << ")" << std::endl;
  outputStream << "\tNumber of executions: " << this->getNumExecutions()
               << std::endl;
  {ForInputEdges(dataflow, this->actor, e) {
      outputStream << "\tTokens consumed from Channel "
                   << dataflow->getEdgeName(e) << ": " << this->getExecRate(e)
                   << std::endl;
    }}
  {ForOutputEdges(dataflow, this->actor, e) {
      outputStream << "\tTokens produced into Channel "
                   << dataflow->getEdgeName(e) << ": " << this->getExecRate(e)
                   << std::endl;
    }}
  return outputStream.str();
}