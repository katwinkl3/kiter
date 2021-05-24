/*
 * expansion_experiment.h
 *
 *  Created on: Feb 7, 2021
 *      Author: toky
 */

#include <commons/KiterRegistry.h>

namespace models {
	class Dataflow;
}

namespace algorithms {

void generate_expansion       (models::Dataflow* const  dataflow, parameters_list_t);


}


ADD_TRANSFORMATION(PartialExpansion,
		transformation_t({ "PartialExpansion" , "Generate the Partial Expansion Graph from WIP.", algorithms::generate_expansion}));
