/*
 * scc.h
 *
 *  Created on: 1 June 2021
 *      Author: jkmingwen
 */
#ifndef SCC_H_
#define SCC_H_

#include <models/Dataflow.h>

namespace models {
  class Dataflow;
}

namespace algorithms {
  void dfs(models::Dataflow* const dataflow);
  void computeDFSStack(models::Dataflow* const dataflow,
                       ARRAY_INDEX startId,
                       std::map<ARRAY_INDEX, bool> &visitedActors,
                       std::stack<ARRAY_INDEX> &dfsOrder);
}
#endif /* SCC_H_ */
