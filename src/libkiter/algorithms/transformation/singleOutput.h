/*
 * singleOutput.h
 *
 *  Created on: 12 Aug 2021
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_SINGLEOUTPUT_H_
#define SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_SINGLEOUTPUT_H_

#include <string>
#include <vector>
#include <commons/commons.h>
#include <commons/KiterRegistry.h>



namespace models {
	class Dataflow;
	class Scheduling;
}



namespace algorithms {
namespace transformation {
void singleOutput    (models::Dataflow* const dataflow, parameters_list_t  parameters  ) ;
}
}


ADD_TRANSFORMATION(SingleOutput,
		transformation_t({ "SingleOutput" , "SingleOutput merging output fro ma HSDF task", algorithms::transformation::singleOutput}));

#endif /* SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_SINGLEOUTPUT_H_ */
