/*
 * CSDF_NPeriodicScheduling.cpp
 *
 *  Created on: Oct 22, 2020
 *      Author: toky
 */


#include <algorithms/schedulings.h>
#include <models/EventGraph.h>
#include <printers/SDF3Wrapper.h>

#define VERBOSE_RPERIODIC_DEBUG(msg) {VERBOSE_CUSTOM_DEBUG("rperiodic",msg);}


void generateRPeriodicConstraint(const models::Dataflow * const dataflow , models::EventGraph* g , Edge c) {


	VERBOSE_RPERIODIC_DEBUG("Constraint for " << dataflow->getEdgeName(c) );

	ARRAY_INDEX id = dataflow->getEdgeId(c);

	Vertex source = dataflow->getEdgeSource(c);
	Vertex target = dataflow->getEdgeTarget(c);

	const ARRAY_INDEX source_id = dataflow->getVertexId(source);
	const ARRAY_INDEX target_id = dataflow->getVertexId(target);

	EXEC_COUNT source_phase_count = dataflow->getEdgeInPhasesCount(c);
	EXEC_COUNT target_phase_count = dataflow->getEdgeOutPhasesCount(c);


	EXEC_COUNT source_init_phase_count = dataflow->getEdgeInInitPhasesCount(c);
	EXEC_COUNT target_init_phase_count = dataflow->getEdgeOutInitPhasesCount(c);


	TOKEN_UNIT mop     =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));


	const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);

	TOKEN_UNIT normdapkm1 = 0;
	TOKEN_UNIT normdapk   = 0;

	bool doBufferLessEdges = dataflow->getEdgeType(c) == EDGE_TYPE::BUFFERLESS_EDGE;

	TIME_UNIT  maxd = 0;
	for (EXEC_COUNT pi = 1; pi <= source_phase_count ; pi++ ) {
		TIME_UNIT  maxd = std::max(maxd,dataflow->getVertexDuration(source, pi));
	}

	models::EventGraphVertex source_event = g->getEventGraphVertex(source_id,1);
	models::EventGraphVertex target_event = g->getEventGraphVertex(target_id,1);

	TIME_UNIT w = 0;
	TIME_UNIT d = maxd;

	g->addEventConstraint(source_event ,target_event,-w,d,id);

}

models::EventGraph* generateRPeriodicEventGraph(const models::Dataflow * const dataflow) {

	VERBOSE_ASSERT((dataflow->has_repetition_vector()),"inconsistent graph or repetition vector not computed");

	models::EventGraph * g = new models::EventGraph();


	/* generate nodes */
	{ForEachVertex(dataflow,t) {
		const ARRAY_INDEX tid = dataflow->getVertexId(t);

		EXEC_COUNT init_phase_count = dataflow->getInitPhasesQuantity(t);
		EXEC_COUNT periodic_phase_count = dataflow->getPhasesQuantity(t);

		g->addEvent(models::SchedulingEvent(tid,1));
	}}


	// DEFINITION DES CONTRAINTES DE PRECEDENCES
	//******************************************************************
	{ForEachChannel(dataflow,c) {
		generateRPeriodicConstraint(dataflow , g ,  c);
	}}



	return g;

}




void algorithms::scheduling::CSDF_RPeriodicScheduling (models::Dataflow*  dataflow, parameters_list_t ){

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT((dataflow->has_repetition_vector()),"inconsistent graph or repetition vector not computed");


    //STEP 1 - Build the EventGraph
    models::EventGraph* eg = generateRPeriodicEventGraph(dataflow);
    VERBOSE_INFO("EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
    TIME_UNIT res = howard_res.first;

    VERBOSE_INFO( "Maximum throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  res );
    VERBOSE_INFO( "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   );

    delete eg;

}
