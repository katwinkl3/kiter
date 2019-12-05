/*
 * throughput.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef THROUGHPUT_H_
#define THROUGHPUT_H_

namespace models {
	class Dataflow;
	class EventGraph;
}

namespace algorithms {

void compute_csdf_1periodic_memory                              (models::Dataflow* const  dataflow, parameters_list_t);
} // end of namespace algorithms

#endif /* THROUGHPUT_H_ */
