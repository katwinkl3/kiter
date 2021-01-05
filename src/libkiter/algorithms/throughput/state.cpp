/*
 * state.cpp
 *
 *  Created on: 3 January 2021
 *      Author: jkmingwen
 */

#include "state.h"
#include <models/Dataflow.h>

State::State()
  :executingActors(),
   actorPhases(),
   currentTokens(),
   timeElapsed{0} {}

// construct state using current graph and actor map information
State::State(models::Dataflow* const dataflow,
             std::map<ARRAY_INDEX, Actor> actorMap) {
  {ForEachEdge(dataflow, e) {
      currentTokens[e] = dataflow->getPreload(e);
    }}
  {ForEachTask(dataflow, t) {
      actorPhases[t] = actorMap[dataflow->getVertexId(t)].getPhase();
    }}
  timeElapsed = 0;
}

// returns current phase of actor
PHASE_INDEX State::getPhase(Vertex a) const {
  return actorPhases.at(a);
}

// returns current tokens in edge
TOKEN_UNIT State::getTokens(Edge e) const {
  return currentTokens.at(e);
}

// returns amount of time left for execution
TIME_UNIT State::getRemExecTime(Vertex a) const {
  return executingActors.at(a);
}

// returns total elapsed time in state
TIME_UNIT State::getTimeElapsed() const {
  return timeElapsed;
}

// void State::updateState(models::Dataflow* const dataflow,
//                         std::map<ARRAY_INDEX, Actor> actors) {
  
// }

bool State::operator==(const State& s) const {
  if (this->getTimeElapsed() != s.getTimeElapsed()) {
    return false;
  }
  for (auto it = this->actorPhases.begin(); it != this->actorPhases.end(); it++) {
    if (this->getPhase(it->first) != s.getPhase(it->first)) {
      return false;
    }
  }
  for (auto it = this->currentTokens.begin(); it != this->currentTokens.end(); it++) {
    if (this->getTokens(it->first) != s.getTokens(it->first)) {
      return false;
    }
  }
  for (auto it = this->executingActors.begin(); it != this->executingActors.end(); it++) {
    if (this->getRemExecTime(it->first) != s.getRemExecTime(it->first)) {
      return false;
    }
  }
  
  return true;
}
