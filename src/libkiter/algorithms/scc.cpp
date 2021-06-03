/*
 * scc.cpp
 *
 *  Created on: 1 June 2021
 *      Author: jkmingwen
 */
#include <map>
#include <vector>
#include <stack>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include "scc.h"

void algorithms::computeSCCKosaraju(models::Dataflow* const dataflow) {
  std::map<ARRAY_INDEX, bool> visitedActors;
  ARRAY_INDEX startId;
  std::stack<ARRAY_INDEX> dfsOrder;
  std::vector<ARRAY_INDEX> sccActors;
  std::map<int, std::vector<ARRAY_INDEX>> sccMap;
  int sccCount = 1;

  {ForEachTask(dataflow, t) {
      visitedActors[dataflow->getVertexId(t)] = false;
    }}
  for (auto const& it : visitedActors) {
    if (!visitedActors[it.first]) {
      computeDFSStack(dataflow, it.first, visitedActors, dfsOrder);
    }
  }

  // reset visited actor list
  for (auto const& it : visitedActors) {
    visitedActors[it.first] = false;
  }
  // perform transposed version of dfs according to order stack
  while (!dfsOrder.empty()) {
    ARRAY_INDEX firstId = dfsOrder.top();
    dfsOrder.pop();
    if (!visitedActors[firstId]) { // only perform DFS on actors in stack that haven't been visited
      dfsTranspose(dataflow, firstId, visitedActors, sccActors);
      // add to SCC map and reset for next component
      sccMap[sccCount] = sccActors;
      sccActors.clear();
      sccCount++;
    }
  }
  // test output
  for (auto const& component : sccMap) {
    std::cout << "Printing strongly connected component " << component.first << std::endl;
    for (auto id : component.second) {
      std::cout << id << " ";
    }
    std::cout << std::endl;
  }
}

// Performs a DFS on dataflow graph, tracks order of actors finished --- helper function for Kosaraju's algorithm
void algorithms::computeDFSStack(models::Dataflow* const dataflow,
                                 ARRAY_INDEX startId,
                                 std::map<ARRAY_INDEX, bool> &visitedActors,
                                 std::stack<ARRAY_INDEX> &dfsOrder) {
  Vertex startActor = dataflow->getVertexById(startId);
  if (visitedActors[startId]) {
    return;
  }
  visitedActors[startId] = true;
  {ForOutputEdges(dataflow, startActor, e) {
      ARRAY_INDEX nextId = dataflow->getVertexId(dataflow->getEdgeTarget(e));
      computeDFSStack(dataflow, nextId, visitedActors, dfsOrder);
    }}
  dfsOrder.push(startId);
}

// Computes the DFS of the transpose of the given graph, storing actors that belong to the same SCC in a vector
void algorithms::dfsTranspose(models::Dataflow* const dataflow, ARRAY_INDEX startId,
                  std::map<ARRAY_INDEX, bool> &visitedActors,
                  std::vector<ARRAY_INDEX> &sccActors) {
  Vertex startActor = dataflow->getVertexById(startId);
  if (visitedActors[startId]) {
    return;
  }
  visitedActors[startId] = true;
  sccActors.push_back(startId);
  {ForInputEdges(dataflow, startActor, e) {
      ARRAY_INDEX nextId = dataflow->getVertexId(dataflow->getEdgeSource(e));
      dfsTranspose(dataflow, nextId, visitedActors, sccActors);
    }}
}
