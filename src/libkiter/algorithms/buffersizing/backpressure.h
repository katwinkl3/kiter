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
#include <commons/KiterRegistry.h>

namespace models {
class Dataflow;
}
namespace algorithms {
    void compute_backpressure_memory_sizing (models::Dataflow* const  dataflow, parameters_list_t);
}
ADD_TRANSFORMATION(BackPressureSizing,
		transformation_t({ "BackPressureSizing" , "Buffer sizing method from Wiggers et al DAC 2007 paper.", algorithms::compute_backpressure_memory_sizing}));


#endif /* BACKPRESSURE_H_ */
