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

// returns list of amount of time left for executions for actor
std::list<std::pair<TIME_UNIT, PHASE_INDEX>> State::getRemExecTime(Vertex a) const {
  return executingActors.at(a);
}

// returns entire execution queue
std::map<Vertex, std::list<std::pair<TIME_UNIT, PHASE_INDEX>>> State::getExecQueue() {
  return this->executingActors;
}

// remove first execution of given actor from its execution queue
void State::removeFrontExec(Vertex a) {
  this->executingActors[a].pop_front();
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

void State::addExecution(Vertex a, std::pair<TIME_UNIT, PHASE_INDEX> newExec) {
  VERBOSE_INFO("Adding new execution of time " << newExec.first
               << " and phase " << newExec.second);
  this->executingActors[a].push_back(newExec);
}

// reduce execution times for given actor by the given time step
void State::advanceRemExecTime(Vertex a, TIME_UNIT timeStep) {
  for (auto &it : this->executingActors[a]) {
    it.first = it.first - timeStep;
  }
}

void State::setTimeElapsed(TIME_UNIT time) {
  this->timeElapsed = time;
}

// update phase count for each actor and token counts per channel
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
      timeElapsed = std::min(timeElapsed, this->executingActors[i].front().first);
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
  VERBOSE_INFO("Time advanced by: " << timeElapsed);
  VERBOSE_INFO("Total time elapsed: " << this->getTimeElapsed());
  return timeElapsed;
}

TIME_UNIT State::advanceTimeWithMod(int time) {
  if (time > 0){
    this->timeElapsed += time;
    return time;
  }

  TIME_UNIT timeElapsed = LONG_MAX;
  for (auto &i : this->actors) {
    if (!this->executingActors[i].empty()) {
      timeElapsed = std::min(timeElapsed, this->executingActors[i].front().first);
    }
  }
  // check for cases where time shouldn't advance/deadlock
  if (timeElapsed == 0) {
    return timeElapsed; // there exists actors that need to end execution
  }
  if (timeElapsed == LONG_MAX) {
    return 0; // simply have faith that the periodic code did not break amen
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

std::string State::print(models::Dataflow* const dataflow) {
  std::stringstream outputStream;
  outputStream << "\nPrinting state status:" << std::endl;
  outputStream << "\tActor phases:" << std::endl;
  for (auto const &it : this->actorPhases) {
    outputStream << "\t\tPhase of Actor " << dataflow->getVertexName(it.first)
                 << ": " << this->getPhase(it.first) << std::endl;
  }
  outputStream << "\tChannel token counts:" << std::endl;
  for (auto const &it : this->currentTokens) {
    outputStream << "\t\tChannel " << dataflow->getEdgeName(it.first) << ": "
                 << this->getTokens(it.first) << std::endl;
  }
  outputStream << "\tPrinting execution queues:" << std::endl;
  for (auto const &it : this->executingActors) {
    outputStream << "\t\tExecution queue for Actor "
                 << dataflow->getVertexName(it.first) << ": " << std::endl;
    for (auto const &q : it.second) {
      outputStream << "\t\t  ";
      outputStream << q.first << " ";
    }
    outputStream << std::endl;
  }
  outputStream << "Total time elapsed: " << this->getTimeElapsed() << std::endl;

  return outputStream.str();
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

std::list<State>::iterator StateList::getRepeatedState() {
  return this->repeatedState;
}

// Computes throughput given a revisited state
// Note that a repeated state needs to have been identified before
// this function can be called
TIME_UNIT StateList::computeThroughput() {
  EXEC_COUNT number_iterations = 0;
  TIME_UNIT total_time = 0;
  // TODO add assert warning for instances where this function is
  // called before a repeated state has been identified
  for (std::list<State>::iterator i = this->getRepeatedState();
       i != this->visitedStates.end(); i++) {
    State &state = *i;
    number_iterations++;
    total_time += state.getTimeElapsed();
  }
  VERBOSE_INFO("\nTotal time: " << total_time << "\nNum executions: "
               << number_iterations);
  return (TIME_UNIT) (number_iterations/total_time);
}

// Get index of state
int StateList::computeIdx(State s){
  auto i = std::find(this->visitedStates.begin(), this->visitedStates.end(), s);
  if (i != this->visitedStates.end()) {
    return std::distance(this->visitedStates.begin(), i);
  } else {
    return -1;
  }
}