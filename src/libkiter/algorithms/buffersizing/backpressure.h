/*
 * backpressure.h
 *
 *  Created on: 25 avr. 2013
 *      Author: toky
 */

#ifndef BACKPRESSURE_H_
#define BACKPRESSURE_H_

#include <commons/commons.h>
#include <commons/basic_types.h>

namespace models {
class Dataflow;
}
namespace algorithms {
    void compute_backpressure_memory_sizing (models::Dataflow* const  dataflow, parameters_list_t);
}


#endif /* BACKPRESSURE_H_ */
