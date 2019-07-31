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

models::EventGraph* generateEventGraph				            (models::Dataflow * dataflow ) ;
void compute_1periodic_throughput     				            (models::Dataflow* const  dataflow, parameters_list_t);
void compute_csdf_1periodic_memory                              (models::Dataflow* const  dataflow, parameters_list_t);
TIME_UNIT optimal_1periodic_throughput                          (models::Dataflow* const  dataflow) ;

void print_periodic_eventgraph                                  (models::Dataflow* const  dataflow, parameters_list_t);
void print_1periodic_scheduling                                 (models::Dataflow* const  dataflow, parameters_list_t);

models::EventGraph* generateHarmonizedEventGraph                (models::Dataflow * dataflow ) ; // not found
TIME_UNIT optimal_harmonizedPeriodic_throughput                 (models::Dataflow* const  dataflow) ; // not found
void compute_HarmonizedPeriodic_throughput                      (models::Dataflow* const  dataflow, parameters_list_t); // not found

} // end of namespace algorithms

#endif /* THROUGHPUT_H_ */
