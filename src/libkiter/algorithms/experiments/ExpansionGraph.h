/*
 * ExpansionGraph.h
 *
 *  Created on: Sep 26, 2019
 *      Author: toky
 */

#include <models/Dataflow.h> // Required because of Vertex and Edge ...
#include <algorithms/schedulings.h> // Required because of periodicity_vector_t ...

namespace models {
	class EventGraph;
}


class ExpansionGraph {

private:
	models::EventGraph* eg = nullptr;

private:
	ExpansionGraph() {};
	void generate_E1_Edges(models::Dataflow * const dataflow , EXEC_COUNT ki,  models::EventGraph* g , Vertex t  );
	void generate_E2_Edge (models::Dataflow * const dataflow , EXEC_COUNT Ki,  models::EventGraph* g , Vertex t  ) ;
	void generate_E3_Edges(models::Dataflow * const dataflow , const periodicity_vector_t& kValues,  models::EventGraph* g , Edge c) ;
	models::EventGraph* generate(models::Dataflow* dataflow,   const periodicity_vector_t& kvector) ;

public:
	ExpansionGraph (models::Dataflow* dataflow,   periodicity_vector_t& kvector ) {
		this->eg = this->generate(dataflow,kvector);
	}

	std::string toDOT ();


};
