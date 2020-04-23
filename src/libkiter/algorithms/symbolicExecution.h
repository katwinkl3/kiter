/*
 * symbolicExecution.h
 *
 *  Created on: 28 march 2018
 *      Author: toky
 */

#ifndef SYMBOLIC_EXECUTION_H_
#define SYMBOLIC_EXECUTION_H_

#include <commons/KiterRegistry.h>
namespace models  { class Dataflow; }

namespace algorithms {
    void symbolic_execution_with_packets(models::Dataflow* const  dataflow, parameters_list_t ) ;
    void symbolic_execution(models::Dataflow* const  dataflow, parameters_list_t ) ;
    void packet_list(models::Dataflow* const  dataflow, parameters_list_t ) ;
}

ADD_TRANSFORMATION(SymbolicExecutionWP,
		transformation_t({ "SymbolicExecutionWP" , "Execute task in ASAP fashion and print out the scheduling.", algorithms::symbolic_execution_with_packets}));



#endif /* SYMBOLIC_EXECUTION_H_ */
