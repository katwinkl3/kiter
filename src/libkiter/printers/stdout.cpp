/*
 * stdout.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <algorithms/repetition_vector.h>
#include <printers/stdout.h>


void printers::printGraph    (models::Dataflow* const  dataflow, parameters_list_t ) {

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	          std::cout << "Graph name             = " <<  dataflow->getName() << std::endl;
			  std::cout << "Task count             = " <<  dataflow->getVerticesCount() << std::endl;
	          std::cout << "Channels total         = " <<  dataflow->getEdgesCount() << std::endl;
	          EXEC_COUNT total = 0;

	          {ForEachTask(dataflow,t) {
	              total += dataflow->getNi(t) ;

	                    }}
              std::cout << "Complexity         = " <<  total << std::endl;

}


void printers::printInfos    (models::Dataflow* const  dataflow, parameters_list_t ) {

                VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

              std::cout << "Graph name             = " <<  dataflow->getName() << std::endl;
              std::cout << "Task count             = " <<  dataflow->getVerticesCount() << std::endl;
              std::cout << "Channels total         = " <<  dataflow->getEdgesCount() << std::endl;

              {ForEachTask(dataflow,t) {
                  std::cout << " - " <<  dataflow->getVertexName(t) << " N=" << dataflow->getNi(t) << std::endl;

              }}


}


