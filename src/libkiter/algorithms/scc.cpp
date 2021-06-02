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

void algorithms::dfs(models::Dataflow* const dataflow) {
  std::map<ARRAY_INDEX, bool> visitedActors;
  ARRAY_INDEX startId;
  std::stack<ARRAY_INDEX> dfsOrder;
  std::cout << "DFS start:" << std::endl;
  {ForEachTask(dataflow, t) {
      std::cout << "\tID of Actor " << dataflow->getVertexName(t) << ": "
                << dataflow->getVertexId(t) << std::endl;
      visitedActors[dataflow->getVertexId(t)] = false;
    }}
  startId = visitedActors.begin()->first;
  computeDFSStack(dataflow, startId, visitedActors, dfsOrder);
  std::cout << "Printing stack:" << std::endl;
  while (!dfsOrder.empty()) {
    std::cout << dfsOrder.top();
    dfsOrder.pop();
  }
  std::cout << std::endl;
}

// Performs a DFS on dataflow graph, tracks order of actors finished --- helper function for Kosaraju's algorithm
void algorithms::computeDFSStack(models::Dataflow* const dataflow,
                                 ARRAY_INDEX startId,
                                 std::map<ARRAY_INDEX, bool> &visitedActors,
                                 std::stack<ARRAY_INDEX> &dfsOrder) {
  Vertex startActor = dataflow->getVertexById(startId);
  if (visitedActors[startId]) {
    std::cout << "Actor " << dataflow->getVertexName(startActor)
              << " already visited --- backtracking..."<< std::endl;
    return;
  }
  std::cout << "Visiting Actor " << dataflow->getVertexName(startActor) << std::endl;
  visitedActors[startId] = true;
  {ForOutputEdges(dataflow, startActor, e) {
      ARRAY_INDEX nextId = dataflow->getVertexId(dataflow->getEdgeTarget(e));
      computeDFSStack(dataflow, nextId, visitedActors, dfsOrder);
    }}
  // std::cout << "Actor " << dataflow->getVertexName(startActor) << " finished" << std::endl;
  dfsOrder.push(startId);
}
