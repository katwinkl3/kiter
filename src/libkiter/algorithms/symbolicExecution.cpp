/*
 * symbolicExecution.cpp
 *
 *  Created on: 28 March 2019
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/symbolicExecution.h>
#include <algorithms/repetition_vector.h>
#include <algorithms/nperiodic.h>


using namespace models;

void algorithms::packet_list(models::Dataflow* const  dataflow, parameters_list_t ) {

		// STEP 0.1 - PRE
		VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
		VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

		//STEP 1 - Generate Event Graph
		VERBOSE_INFO("N-Periodic EventGraph generation");
		models::EventGraph* eg = generateNPeriodicEventGraph(dataflow);
		eg->FullConnectionned();
		VERBOSE_INFO("NPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

		std::vector < models::EventGraphVertex > to_execute;
		std::map < models::EventGraphVertex , ARRAY_INDEX > ids;
		std::map < models::EventGraphEdge , TOKEN_UNIT > buffers;
		std::map < models::EventGraphEdge , ARRAY_INDEX > edge_ids;

		{ForEachEvent(eg, e){
			to_execute.push_back(e);
		}}
		{ForEachConstraint(eg, c){
			VERBOSE_INFO( "edge " << c << " has weight=" << eg->getWeight(c) << " and Duration=" << eg->getDuration(c));
			buffers[c] = eg->getWeight(c);
		}}
		std::cout << "#NUM_ROWS,NUM_COLS,NUM_BANKS" << std::endl;
		std::cout << "4,4,1" << std::endl;
		std::cout << "#SRC,DEST,DURATION,PACKET_ID,BANK_ID,DEP1,DEP2,..." << std::endl;
	    VERBOSE_INFO("Start of symbolic execution.");

	    ARRAY_INDEX packet_id = 0;
		while (to_execute.size()) {

			bool can_execute = false;

			VERBOSE_INFO ("Remains " << to_execute.size());
			// One more to execute
			for (auto it = to_execute.begin() ; it != to_execute.end() ; it ++) {
				auto e = *it;
				VERBOSE_INFO ("try " << e);
				can_execute = true;
				{ForEachInputs (eg, e , inc){
					if (buffers[inc] <= 0) {
						VERBOSE_INFO ("  Cannot " << e << " buffer " << inc);
						can_execute = false;
						break;
					}
				}}

				if (can_execute) {
					VERBOSE_INFO("Execute " << e);
					ids[e] = ids.size();

					std::vector <ARRAY_INDEX> deps ;
					{ForEachInputs (eg, e , inc){
						buffers[inc] -= 1;
						if (eg->getWeight(inc) == 0) {
							deps.push_back(edge_ids[inc]);
						}
					}}

					{ForEachOutputs (eg, e , outc){

						buffers[outc] += 1;
						edge_ids[outc] = packet_id++;
						ARRAY_INDEX src = eg->getTaskId(e) ;
						ARRAY_INDEX dst = eg->getConstraint(outc) ._t.getTaskId();
						TIME_UNIT duration = eg->getDuration(outc);
						ARRAY_INDEX pid = edge_ids[outc] ;
						ARRAY_INDEX bid = 0 ;

						std::cout << src % 16 << "," ;
						std::cout << dst % 16 << "," ;
						std::cout << duration << "," ;
						std::cout << pid << "," ;
						std::cout << bid  ;

						for (auto dep : deps) {
							std::cout << ","  << dep ;
						}
						std::cout << std::endl;
					}}
					to_execute.erase(it);

					break;
				}

			}

			if (!can_execute) FAILED ("Nothing executed, should never happen.");
		}

	    VERBOSE_INFO("End of symbolic execution.");

}

void algorithms::symbolic_execution(models::Dataflow* const  from, parameters_list_t ) {
	// Need RV.
	VERBOSE_ASSERT(computeRepetitionVector(from),"inconsistent graph");

	// Store task to execute by number of execution.
	EXEC_COUNT total = 0 ;
	std::vector < EXEC_COUNT >  remained_execution (from->getMaxVertexId());
	{ForEachVertex(from,t) {
		EXEC_COUNT Ni =  from->getNi(t) ;
		remained_execution[from->getVertexId(t)] = Ni ;
		total += Ni ;
	}}

	// Store buffer content.
	std::vector < EXEC_COUNT >  buffer_content (from->getMaxEdgeId());
	{ForEachChannel(from,c) {
		buffer_content[from->getEdgeId(c)] = from->getPreload(c);
	}}

    VERBOSE_INFO("Start of symbolic execution.");
	// Loop over to execute everyone. Unoptimized, could be much faster if tasks were stacked, and sum of dependences stored.
    // This does not model timings, useless for throughput
	while (total > 0) {
		{ForEachVertex(from,t) {
			ARRAY_INDEX vId = from->getVertexId(t);
			bool can_execute = false;
			if ( remained_execution [vId] > 0) {
				can_execute = true;
				//VERBOSE_DEBUG("Need to execute " << vId);
				{ForInputEdges(from,t,inE)	{
					TOKEN_UNIT inCount  = buffer_content[from->getEdgeId(inE)];
					TOKEN_UNIT reqCount = from->getEdgeOut(inE);
					if (!(inCount >= reqCount)) {
						can_execute = false;
						//VERBOSE_DEBUG("Cannot execute " << vId);
						break;
					}
				}}

				if (can_execute) {
					VERBOSE_DEBUG("Execute " << vId);
					remained_execution[from->getVertexId(t)] -= 1 ;
					total -= 1;

					{ForInputEdges(from,t,inE)	{
						buffer_content[from->getEdgeId(inE)] -= from->getEdgeOut(inE);
					}}

					{ForOutputEdges(from,t,outE)	{
						buffer_content[from->getEdgeId(outE)] += from->getEdgeIn(outE);
					}}

				}
			}

		}}
	}

    VERBOSE_INFO("End of symbolic execution.");


}











