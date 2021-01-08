/*
 * state.cpp
 *
 *  Created on: 3 January 2021
 *      Author: jkmingwen
 */

#include "state.h"
#include <models/Dataflow.h>
#include <algorithm>

State::State()
  :executingActors(),
   actorPhases(),
   currentTokens(),
   actors(),
   timeElapsed{0} {}

// construct state using current graph and actor map information
State::State(models::Dataflow* const dataflow,
             std::map<ARRAY_INDEX, Actor> actorMap) {
  {ForEachEdge(dataflow, e) {
      currentTokens[e] = dataflow->getPreload(e);
    }}
  {ForEachTask(dataflow, t) {
      actorPhases[t] = actorMap[dataflow->getVertexId(t)].getPhase();
      actors.push_back(t);
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

TIME_UNIT State::advanceTime() {
  TIME_UNIT timeElapsed = LONG_MAX;
  for (auto &i : this->actors) {
    if (!this->executingActors[i].empty()) {
      timeElapsed = std::min(timeElapsed, this->executingActors[i].front());
    }
  }
  // check for cases where time shouldn't advance/deadlock
  if (timeElapsed == 0) {
    return timeElapsed; // there exists actors that need to end execution
  }
  if (timeElapsed == LONG_MAX) {
    return timeElapsed; // deadlock reached (no actors about to execute)
  }
  // advance time for all actors
  for (auto &it : this->executingActors) {
    this->advanceRemExecTime(it.first, timeElapsed);
  }
  this->timeElapsed += timeElapsed;

  return timeElapsed;
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

StateList::StateList() {

}

StateList::StateList(State s) {
  this->visitedStates.push_back(s);
}

bool StateList::addState(State s) {
  this->repeatedState = std::find(this->visitedStates.begin(),
                                  this->visitedStates.end(),
                                  s);
  if (this->repeatedState != this->visitedStates.end()) {
    return false;
  }
  this->visitedStates.push_back(s);
  return true;
}
