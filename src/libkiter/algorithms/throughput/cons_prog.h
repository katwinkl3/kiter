/*
 * Constraint Programming With MiniZinc
 *
 *  Created on: Dec 2021
 *      Author: katwinkl3
 */

#ifndef CONSTRAINT_PROG_H_
#define CONSTRAINT_PROG_H_

#include <algorithms/schedulings.h>
#include <commons/KiterRegistry.h>
#include <commons/commons.h>

namespace models {
  class Dataflow;
}

namespace algorithms {
  std::string printFile(models::Dataflow* const dataflow);
}

#endif /* CONSTRAINT_PROG_H_ */
