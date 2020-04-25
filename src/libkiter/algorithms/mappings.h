/*
 * mappings.h
 *
 *  Created on: Aug 21, 2019
 *      Author: toky
 */

#ifndef _MAPPINGS_H_
#define _MAPPINGS_H_

#include <commons/commons.h>
#include <commons/KiterRegistry.h>

namespace models {
	class Dataflow;
}
namespace algorithms {

	namespace mapping {
		void randomMapping (models::Dataflow* const  dataflow, parameters_list_t params);
		void moduloMapping (models::Dataflow* const  dataflow, parameters_list_t params);
		void BufferlessNoCMapAndRoute (models::Dataflow* const  dataflow, parameters_list_t params);
	}

	void ModelNoCConflictFreeCommunication(models::Dataflow* const  dataflow, parameters_list_t   param_list = parameters_list_t());

}
// Helpers to map vertex to cores, need a way to also consider routers
ADD_TRANSFORMATION(randomMapping,
		transformation_t({ "randomMapping" , "This command will associate a mapping to each task of the graph. Task unspecified as parameters will be randomly allocated to a core.", algorithms::mapping::randomMapping} )
);
ADD_TRANSFORMATION(moduloMapping,
		transformation_t({ "moduloMapping" , "This command will associate a mapping to each task of the graph. Task unspecified as parameters will be randomly allocated to a core.", algorithms::mapping::moduloMapping} )
);
ADD_TRANSFORMATION(BufferlessNoCMapAndRoute,
		transformation_t({ "BufferlessNoCMapAndRoute" , "Mapping and Routing combined.", algorithms::mapping::BufferlessNoCMapAndRoute} )
);
ADD_TRANSFORMATION(ModelNoCConflictFreeCommunication,
		transformation_t({ "ModelNoCConflictFreeCommunication" , "Given a graph with Mapping, can add fictive task to model network", algorithms::ModelNoCConflictFreeCommunication} )
);


#endif
