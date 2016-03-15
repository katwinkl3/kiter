/*
 * nperiodic.cpp
 *
 *  Created on: 5 juil. 2013
 *      Author: toky
 */


#include <vector>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/nperiodic.h>
#include <algorithms/kperiodic.h>
#include <algorithms/repetition_vector.h>



void algorithms::print_Nperiodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {


	VERBOSE_INFO("NPeriodic EventGraph generation");

	// STEP 1 - Generate Event Graph
	models::EventGraph* eg = algorithms::generateNPeriodicEventGraph(dataflow);
	// STEP 2 - print it

	VERBOSE_INFO("NPeriodic EventGraph generation Done");

	std::cout << eg->printXML();


}

models::EventGraph* algorithms::generateNPeriodicEventGraph               (models::Dataflow *  const dataflow) {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	VERBOSE_DEBUG_ASSERT(dataflow->is_normalized() == false,"looser");

	models::EventGraph * eg = new models::EventGraph();

	/* generate nodes */
	{ForEachVertex(dataflow,pTask) {
		eg->addEventGroup(dataflow->getVertexId(pTask),1,dataflow->getNi(pTask));
	}}


	// DEFINITION DES REENTRANCES
	//******************************************************************

	{ForEachVertex(dataflow,pTask) {
		EXEC_COUNT rfactor = dataflow->getReentrancyFactor(pTask);
		VERBOSE_ASSERT(rfactor <=1 , "unsupported");

		if (rfactor == 1) {
				Vertex source= pTask;
				Vertex target = pTask;

				ARRAY_INDEX source_id = dataflow->getVertexId(source);
				ARRAY_INDEX target_id = dataflow->getVertexId(target);
				ARRAY_INDEX edge_id   = 0;

				EXEC_COUNT Ni = dataflow->getNi(source);
				EXEC_COUNT Nj = dataflow->getNi(target);

				TOKEN_UNIT wp  = 1;
				TOKEN_UNIT vp  = 1;
				TOKEN_UNIT mop = 1;

				TIME_UNIT  tj = dataflow->getVertexDuration(target);


				VERBOSE_DEBUG("Buffer " << edge_id << " from " << dataflow->getVertexName(source) << "to " << dataflow->getVertexName(target) << " : " ) ;

				VERBOSE_DEBUG("  wp  = " << wp  );
				VERBOSE_DEBUG("  vp  = " << vp  );
				VERBOSE_DEBUG("  mop = " << mop );



				for(unsigned long k = 1; k <= Ni ; k++) {

					long int tmpres = (long int) (std::floor( (double)((double) mop +(double)  wp * ((double) k-1.0)) / (double) vp)) + 1;
					long int bk =  (tmpres % (Nj));
					if (bk == 0) bk = Nj;
					long int ak = ((tmpres - bk) / Nj);

					VERBOSE_ASSERT(bk >= 1  , "bk out of bound");
					VERBOSE_ASSERT(bk <= (long int) Ni , "bk out of bound");
					VERBOSE_ASSERT_EQUALS(ak * (long int) Nj + bk , tmpres);


					models::EventGraphVertex source_event = eg->getEventGraphVertex(source_id,1,k);
					models::EventGraphVertex target_event = eg->getEventGraphVertex(target_id,1,bk);

					TIME_UNIT  d =   tj; // L(a) = l(ti)
					TIME_UNIT  w =   ak;

					eg->addEventConstraint(source_event ,target_event,w,d,edge_id);

				}
		}
	}}


	// DEFINITION DES CONTRAINTES DE PRECEDENCES
	//******************************************************************
	{ForEachChannel(dataflow,c) {

		Vertex source = dataflow->getEdgeSource(c);
		Vertex target = dataflow->getEdgeTarget(c);

		ARRAY_INDEX source_id = dataflow->getVertexId(source);
		ARRAY_INDEX target_id = dataflow->getVertexId(target);
		ARRAY_INDEX edge_id   = dataflow->getEdgeId(c);

		EXEC_COUNT Ni = dataflow->getNi(source);
		EXEC_COUNT Nj = dataflow->getNi(target);

		TOKEN_UNIT wp  = dataflow->getEdgeIn(c);
		TOKEN_UNIT vp  = dataflow->getEdgeOut(c);
		TOKEN_UNIT mop = dataflow->getPreload(c);

		//TIME_UNIT  ti = dataflow->getVertexDuration(source);
		TIME_UNIT  tj = dataflow->getVertexDuration(target);

		VERBOSE_DEBUG("Buffer " << edge_id << " from " << dataflow->getVertexName(source) << "to " << dataflow->getVertexName(target) << " : " ) ;

		VERBOSE_DEBUG("  wp  = " << wp  );
		VERBOSE_DEBUG("  vp  = " << vp  );
		VERBOSE_DEBUG("  mop = " << mop );


		if (wp <= vp ) {
			for(unsigned long k = 1; k <= Ni ; k++) {

					long int tmpres = (long int) (std::floor( (double)((double) mop +(double)  wp * ((double) k-1.0)) / (double) vp)) + 1;
					long int bk =  (tmpres % (Nj));
					if (bk == 0) bk = Nj;
					long int ak = ((tmpres - bk) / Nj);
					VERBOSE_ASSERT (bk >= 1,"Oups");
					VERBOSE_ASSERT_EQUALS(ak * (long int) Nj + bk , tmpres);


					models::EventGraphVertex source_event = eg->getEventGraphVertex(source_id,1,k);
					models::EventGraphVertex target_event = eg->getEventGraphVertex(target_id,1,bk);

					TIME_UNIT  d =   tj; // L(a) = l(ti)
					TIME_UNIT  w =   ak;

					eg->addEventConstraint(source_event ,target_event,w,d,edge_id);

			}
		} else {
			for(unsigned long k = 1; k <= Nj ; k++) {
				long int tmpres = (long int) std::ceil((double) (((double)  k * (double)  vp) - (double)  mop) / (double) wp );
				long int ck = 0;
				while ((tmpres - ck * (long int) Ni) <= 0) {
					ck--;
				}
				long int dk =  tmpres - ck * (long int)  Ni;

				VERBOSE_ASSERT_EQUALS(ck * (long int)Ni + dk , tmpres);


				models::EventGraphVertex source_event = eg->getEventGraphVertex(source_id,1,dk);
				models::EventGraphVertex target_event = eg->getEventGraphVertex(target_id,1,k);

				TIME_UNIT  d =   tj; // L(a) = l(ti)
				TIME_UNIT  w =   - ck;

				eg->addEventConstraint(source_event ,target_event,w,d,edge_id);

			}
		}

	}}



	return eg;
}


void algorithms::compute_NPeriodic_throughput  		   (models::Dataflow* const  dataflow, parameters_list_t){


		// STEP 0.1 - PRE
		VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
		VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

		//STEP 1 - Generate Event Graph
		VERBOSE_INFO("N-Periodic EventGraph generation");
		models::EventGraph* eg = generateNPeriodicEventGraph(dataflow);
		VERBOSE_INFO("NPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());
		//STEP 2 - resolve the MCRP on this Event Graph
		std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
		VERBOSE_DEBUG("Critical circuit is about " << howard_res.second.size() << " edges.");

		TIME_UNIT frequency = howard_res.first;

		if (VERBOSE_IS_DEBUG()) {
	    		std::cout << (eg->printDOT());
	    		VERBOSE_INFO( "Maximum throughput is " << frequency );
	    	} else {
	    		std::cout << "Maximum throughput is " << frequency  << std::endl;
	    	}

}


void algorithms::compute_NCleanPeriodic_throughput  		   (models::Dataflow* const  dataflow, parameters_list_t){


		// STEP 0.1 - PRE
		VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
		VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

		//STEP 1 - Generate Event Graph
		VERBOSE_INFO("N-Periodic EventGraph generation");
		models::EventGraph* eg = generateNPeriodicEventGraph(dataflow);
		VERBOSE_INFO("NPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());
		eg->FullConnectionned();
		VERBOSE_INFO("NPeriodic Reducted EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

		//STEP 2 - resolve the MCRP on this Event Graph
		std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
		VERBOSE_DEBUG("Critical circuit is about " << howard_res.second.size() << " edges.");

		TIME_UNIT frequency = howard_res.first;

		if (VERBOSE_IS_DEBUG()) {
	    		std::cout << (eg->printDOT());
	    		VERBOSE_INFO( "Maximum throughput is " << frequency );
	    	} else {
	    		std::cout << "Maximum throughput is " << frequency  << std::endl;
	    	}

}
