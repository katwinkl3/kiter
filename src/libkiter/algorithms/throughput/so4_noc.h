/*
 * so4_noc.h taken from "A Statically Scheduled Time-Division-Multiplexed
 *  Network-on-Chip for Real-Time Systems"
 *
 *  Created on: May 2021
 *      Author: katwinkl3
 */

#ifndef SYMBOLIC_EXECUTION_H_
#define SYMBOLIC_EXECUTION_H_

#include <algorithms/schedulings.h>
#include <commons/KiterRegistry.h>

namespace models {
  class Dataflow;
}

namespace algorithms {
  // void so4_noc(models::Dataflow* const dataflow, parameters_list_t);
  std::pair<TIME_UNIT, scheduling_t> computeComponentSo4Schedule(models::Dataflow* const dataflow,
                                       std::pair<ARRAY_INDEX, EXEC_COUNT> &minActorInfo, scheduling_t schedule, std::string filename);
  std::vector<models::Dataflow*> generateSCCs(models::Dataflow* const dataflow,
                                              std::map<int, std::vector<ARRAY_INDEX>> sccMap);
}

// ADD_TRANSFORMATION(so4Noc,
// 		transformation_t({ "so4Noc" , "Execute using .", algorithms::so4_noc}));

#endif /* SYMBOLIC_EXECUTION_H_ */