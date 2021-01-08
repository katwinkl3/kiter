/*
 * state.h
 *
 *  Created on: 3 January 2021
 *      Author: jkmingwen
 */

#ifndef STATE_H_
#define STATE_H_

#include "actor.h"
#include <models/Dataflow.h>

namespace models {
  class Dataflow;
}

class State {
 public:
  State();
  State(models::Dataflow* const dataflow,
        std::map<ARRAY_INDEX, Actor> actorMap);
  PHASE_INDEX getPhase(Vertex a) const; // returns current phase of actor
  TOKEN_UNIT getTokens(Edge e) const; // returns current tokens in edge
  std::list<TIME_UNIT> getRemExecTime(Vertex a) const; // returns amount of time left for execution
  TIME_UNIT getTimeElapsed() const; // returns total elapsed time in state
  void setPhase(Vertex a, PHASE_INDEX newPhase);
  void setTokens(Edge e, TOKEN_UNIT newTokens);
  void addRemExecTime(Vertex a, TIME_UNIT newTime);
  void advanceRemExecTime(Vertex a, TIME_UNIT timeStep);
  void setTimeElapsed(TIME_UNIT time);
  void updateState(models::Dataflow* const dataflow,
                   std::map<ARRAY_INDEX, Actor> actorMap); // updates state with current status of graph
  TIME_UNIT advanceTime();
  bool operator==(const State& s) const;

  /* bool addToVisitedStates(std::vector<State> &visitedStates); // checks list of visited states and adds to list if new state */

 private:
  std::map<Vertex, std::list<TIME_UNIT>> executingActors; // track actor's currently executing and the time before completing execution
  std::map<Vertex, PHASE_INDEX> actorPhases; // track the current phases of each actor
  std::map<Edge, TOKEN_UNIT> currentTokens; // track number of tokens in each channel
  std::vector<Vertex> actors; // store actors in state
  TIME_UNIT timeElapsed; // track time elapsed given state

};

class StateList {
public:
  StateList();
  StateList(State s);
  bool addState(State s);

private:
  std::list<State> visitedStates;
  std::list<State>::iterator repeatedState;
};
#endif /* STATE_H_ */
