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
std::list<TIME_UNIT> State::getRemExecTime(Vertex a) const {
  return executingActors.at(a);
}

// returns total elapsed time in state
TIME_UNIT State::getTimeElapsed() const {
  return timeElapsed;
}

// set phase of actor
void State::setPhase(Vertex a, PHASE_INDEX newPhase) {
  this->actorPhases[a] = newPhase;
}

void State::setTokens(Edge e, TOKEN_UNIT newTokens) {
  this->currentTokens[e] = newTokens;
}

void State::addRemExecTime(Vertex a, TIME_UNIT newTime) {
  this->executingActors[a].push_back(newTime);
}

// reduce execution times for given actor by the given time step
void State::advanceRemExecTime(Vertex a, TIME_UNIT timeStep) {
  for (auto &it : this->executingActors[a]) {
    it = it - timeStep;
  }
}

void State::setTimeElapsed(TIME_UNIT time) {
  this->timeElapsed = time;
}

void State::updateState(models::Dataflow* const dataflow,
                        std::map<ARRAY_INDEX, Actor> actorMap) {
  {ForEachTask(dataflow, t) {
      setPhase(t, actorMap[dataflow->getVertexId(t)].getPhase());
    }}
  {ForEachEdge(dataflow, e) {
      setTokens(e, dataflow->getPreload(e));
    }}
}

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
