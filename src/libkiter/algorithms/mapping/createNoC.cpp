/*
	Create NoC
 */

#include <models/Dataflow.h>
#include <algorithms/mappings.h>

void algorithms::mapping::createNoC (models::Dataflow* const  dataflow, parameters_list_t params) {	

    VERBOSE_INFO("createNoC");
	int mesh_row = (int)ceil(sqrt((double) dataflow->getVerticesCount()));
	if(mesh_row <= 2)
		mesh_row = 2;
	NoC noc (mesh_row, mesh_row); 
	dataflow->setNoC(noc);

}

