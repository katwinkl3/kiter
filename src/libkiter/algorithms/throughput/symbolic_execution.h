/*
 * symbolic_execution.h
 *
 *  Created on: 21 December 2020
 *      Author: jkmingwen
 */

#ifndef SYMBOLIC_EXECUTION_H_
#define SYMBOLIC_EXECUTION_H_

#include <algorithms/schedulings.h>
#include <commons/KiterRegistry.h>

namespace models {
  class Dataflow;
}

namespace algorithms {
  void compute_asap_throughput(models::Dataflow* const dataflow,
                               parameters_list_t);
  std::string printStatus(models::Dataflow* const dataflow);
}

ADD_TRANSFORMATION(SymbExec,
		transformation_t({ "SymbExec" , "Execute graph using ASAP scheduling.", algorithms::compute_asap_throughput}));

#endif /* SYMBOLIC_EXECUTION_H_ */
