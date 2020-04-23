/*
 * throughput.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef THROUGHPUT_H_
#define THROUGHPUT_H_

#include <commons/KiterRegistry.h>

namespace models {
	class Dataflow;
	class EventGraph;
}

namespace algorithms {

TOKEN_UNIT periodic_memory_sizing_csdf   (models::Dataflow* const  dataflow, TIME_UNIT PERIOD, bool INTEGERSOLVING, bool ILPGENERATIONONLY) ;
void compute_csdf_1periodic_memory       (models::Dataflow* const  dataflow, parameters_list_t);

} // end of namespace algorithms


ADD_TRANSFORMATION(PeriodicSizing,
		transformation_t({ "PeriodicSizing" , "Minimal Buffer size estimation by periodic scheduling method.", algorithms::compute_csdf_1periodic_memory}));

#endif /* THROUGHPUT_H_ */
