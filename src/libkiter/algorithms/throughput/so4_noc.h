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
  void so4_noc(models::Dataflow* const dataflow, parameters_list_t);
}

ADD_TRANSFORMATION(so4Noc,
		transformation_t({ "so4Noc" , "Execute using .", algorithms::so4_noc}));

#endif /* SYMBOLIC_EXECUTION_H_ */