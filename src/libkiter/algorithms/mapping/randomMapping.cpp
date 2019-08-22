/*
 * randomMapping.cpp
 *
 *  Created on: Aug 21, 2019
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <algorithms/mappings.h>

void algorithms::mapping::randomMapping (models::Dataflow* const  dataflow, parameters_list_t params) {

	for (auto t : dataflow->vertices()) {
		if (params.find(dataflow->getVertexName(t)) != params.end()) {
			dataflow->setMapping(t, commons::fromString<node_id_t> (params[dataflow->getVertexName(t)]));
		} else {
			node_id_t random_id = std::rand() % dataflow->getNoC()->size();
			dataflow->setMapping(t, random_id);
		}
	}

}

