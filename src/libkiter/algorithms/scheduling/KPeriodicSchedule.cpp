/*
 * KPeriodicSchedule.cpp
 *
 *  Created on: Sep 26, 2019
 *      Author: toky
 */


#include <algorithms/schedulings.h>
#include <algorithms/scheduling/ExpansionGraph.h>



void algorithms::scheduling::SDFKPeriodicScheduling (models::Dataflow* const  dataflow, parameters_list_t param_list) {

	// Step 1 - produce the K Vector

    std::map<Vertex,EXEC_COUNT> kvector;

    {ForEachVertex(dataflow,v) {
        kvector[v] = 1;
        if (param_list.count(dataflow->getVertexName(v)) == 1) {
            std::string str_value = param_list[dataflow->getVertexName(v)];
            kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
        }
    }}

	ExpansionGraph * eg = new ExpansionGraph (dataflow, &kvector);

	if (param_list.count("DOT") == 1) {
		 std::cout << eg->toDOT();
	}


}
