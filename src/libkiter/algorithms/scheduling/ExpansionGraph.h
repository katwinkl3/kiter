/*
 * ExpansionGraph.h
 *
 *  Created on: Sep 26, 2019
 *      Author: toky
 */

#include <models/EventGraph.h>
#include <models/Dataflow.h>
#include <algorithms/normalization.h>




class ExpansionGraph {

private:

	models::EventGraph* eg = nullptr;

	ExpansionGraph() {};


	void generate_E1_Edges(models::Dataflow * const dataflow , EXEC_COUNT ki,  models::EventGraph* g , Vertex t  ) {

	    const ARRAY_INDEX task_id = dataflow->getVertexId(t);

	    VERBOSE_DEBUG("generate E1 edges for task " <<  dataflow->getVertexId(t) << " with ki=" << ki);



	    for  (EXEC_COUNT source_ki = 1, target_ki = 2; target_ki <= ki; source_ki++, target_ki++) {

		    models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,source_ki);
		    models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,target_ki);


	        TIME_UNIT d =  0;
	        TIME_UNIT w =  0;

	        g->addEventConstraint(source_event ,target_event,w,d,0);

	    }

	}

	void generate_E2_Edge(models::Dataflow * const dataflow , EXEC_COUNT Ki,  models::EventGraph* g , Vertex t  ) {

	    const ARRAY_INDEX task_id = dataflow->getVertexId(t);
	    const TOKEN_UNIT Zi       = dataflow->getZi(t);

	    VERBOSE_DEBUG("generate E1 edges for task " <<  dataflow->getVertexId(t) << " with ki=" << Ki);


	    models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,Ki);
	    models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,1);


        TIME_UNIT d =  0;
        TIME_UNIT w =  Zi * Ki;

        g->addEventConstraint(source_event ,target_event,w,d,0);


	}

	void generate_E3_Edges(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,  models::EventGraph* g , Edge c) {


	    VERBOSE_DEBUG("Constraint for " << dataflow->getEdgeName(c) );

	    Vertex source = dataflow->getEdgeSource(c);
	    Vertex target = dataflow->getEdgeTarget(c);

	    const ARRAY_INDEX source_id = dataflow->getVertexId(source);
	    const ARRAY_INDEX target_id = dataflow->getVertexId(target);

	    const EXEC_COUNT  maxki        = kValues->at(source) ;
	    const EXEC_COUNT  maxkj        = kValues->at(target) ;


	    const TOKEN_UNIT Zi       = dataflow->getZi(source);
	    const TOKEN_UNIT Zj       = dataflow->getZi(target);

	    TOKEN_UNIT mop     =  commons::floor(dataflow->getNormMop(c), 1);

	    const TOKEN_UNIT  gcdz       = std::gcd(Zi,Zj);
	    const TOKEN_UNIT  gcdk       = std::gcd(maxki*Zi,maxkj*Zj);



        TIME_UNIT d =  dataflow->getVertexDuration(source);


	    for (EXEC_COUNT ai = 1; ai <= maxki ; ai++ ) {
		    for (EXEC_COUNT aj = 1; aj <= maxkj ; aj++ ) {

			    const TOKEN_UNIT  alphab     = ( Zi * ai  - Zj * aj) / gcdz ;


				const TOKEN_UNIT pibmax =  (TOKEN_UNIT)  std::floor(  (double) ( Zi - gcdz - mop -  alphab * gcdz) /  (double) gcdk);
				const TOKEN_UNIT pibmin =  (TOKEN_UNIT)   std::ceil( (double)  (std::max(Zi - Zj, (TOKEN_UNIT) 0) - mop - alphab * gcdz) /  (double) gcdk) ;

				if (pibmin <= pibmax) {

			        TIME_UNIT w =  - pibmax * gcdk;


		            models::EventGraphVertex source_event = g->getEventGraphVertex(source_id,ai);
		            models::EventGraphVertex target_event = g->getEventGraphVertex(target_id,aj);
			        g->addEventConstraint(source_event ,target_event,w,d,0);

			        VERBOSE_DEBUG("LINK FROM " << source_event << " to " << target_event);
			        VERBOSE_DEBUG("   alphab = " <<alphab);
			        VERBOSE_DEBUG("   pibmax = " <<pibmax);
			        VERBOSE_DEBUG("   pibmin = " <<pibmin);


				}

		    }
		}



	}


	models::EventGraph* generate(models::Dataflow* dataflow,   std::map<Vertex,EXEC_COUNT>* kvector) {


	    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

	    VERBOSE_ASSERT(algorithms::normalize(dataflow),"normalize failed");

	    models::EventGraph * g = new models::EventGraph();

	    VERBOSE_DEBUG_ASSERT(dataflow->is_normalized(),"The grah needs to be normalized");

	    /* generate nodes */
	    {ForEachVertex(dataflow,t) {
	        EXEC_COUNT phase_count = dataflow->getPhasesQuantity(t);
	        VERBOSE_ASSERT (phase_count == 1, "This is not a SDF");
	        for (EXEC_COUNT ki = 1; ki <= kvector->at(t) ; ki++ ) {
	        	g->addEvent(models::SchedulingEvent(dataflow->getVertexId(t),ki));
	        }
	    }}


	    // DEFINITION DES REENTRANCES
	    //******************************************************************

	    {ForEachVertex(dataflow,pTask) {
	        EXEC_COUNT ki = kvector->at(pTask);
	        this->generate_E1_Edges(dataflow,ki,g,pTask);
	        this->generate_E2_Edge(dataflow,ki,g,pTask);
	    }}


	    // DEFINITION DES CONTRAINTES DE PRECEDENCES
	    //******************************************************************
	    {ForEachChannel(dataflow,c) {
	    	this->generate_E3_Edges(dataflow , kvector,   g ,  c);
	    }}



	    return g;



	}

public:
	ExpansionGraph (models::Dataflow* dataflow,   std::map<Vertex,EXEC_COUNT>* kvector ) {

	    eg = this->generate(dataflow,kvector);

	}

	std::string toDOT () {
		return eg->printDOT();
	}


};
