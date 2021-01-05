/*
 * state.h
 *
 *  Created on: 3 January 2021
 *      Author: jkmingwen
 */

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
  TIME_UNIT getRemExecTime(Vertex a) const; // returns amount of time left for execution
  TIME_UNIT getTimeElapsed() const; // returns total elapsed time in state
  void setPhase(Vertex a, PHASE_INDEX newPhase);
  void setTokens(Edge e, TOKEN_UNIT newTokens);
  void setRemExecTime(Vertex a, TIME_UNIT newTime);
  void setTimeElapsed(TIME_UNIT time);
  void updateState(models::Dataflow* const dataflow,
                   std::map<ARRAY_INDEX, Actor> actorMap); // updates state with current status of graph
  bool operator==(const State& s) const;
  
  /* bool addToVisitedStates(std::vector<State> &visitedStates); // checks list of visited states and adds to list if new state */
  
 private:
  std::map<Vertex, TIME_UNIT> executingActors; // track actor's currently executing and the time before completing execution
  std::map<Vertex, PHASE_INDEX> actorPhases; // track the current phases of each actor
  std::map<Edge, TOKEN_UNIT> currentTokens; // track number of tokens in each channel
  TIME_UNIT timeElapsed; // track time elapsed given state
  
};
