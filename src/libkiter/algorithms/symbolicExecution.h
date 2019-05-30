/*
 * symbolicExecution.h
 *
 *  Created on: 28 march 2018
 *      Author: toky
 */

#ifndef SYMBOLIC_EXECUTION_H_
#define SYMBOLIC_EXECUTION_H_

namespace models  { class Dataflow; }

namespace algorithms {
    void symbolic_execution_with_packets(models::Dataflow* const  dataflow, parameters_list_t ) ;
    void symbolic_execution(models::Dataflow* const  dataflow, parameters_list_t ) ;
    void packet_list(models::Dataflow* const  dataflow, parameters_list_t ) ;
}

#endif /* SYMBOLIC_EXECUTION_H_ */
