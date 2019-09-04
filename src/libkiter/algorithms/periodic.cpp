/*
 * throughput.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <cmath>
#include <vector>
#include <commons/verbose.h>
#include <commons/glpsol.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/periodic.h>
#include <algorithms/repetition_vector.h>

void algorithms::print_1periodic_scheduling    (models::Dataflow* const  dataflow, parameters_list_t) {

    TIME_UNIT SCHEDULING_SIZE = 26;
    TIME_UNIT res = optimal_1periodic_throughput(dataflow);
    models::EventGraph* eg =  algorithms::generateEventGraph( dataflow  );
    eg->computeStartingTime (res);
    TIME_UNIT omega = 1 / res ;
    std::cout << "\\begin{scheduling}{" << dataflow->getVerticesCount() <<  "}{" << SCHEDULING_SIZE <<  "}{3.2}{5}" << std::endl;

    {ForEachVertex(dataflow,v) {
        std::cout << "\\taskname{"  << dataflow->getVertexId(v) <<  "}{"  << dataflow->getVertexName(v) <<  "}" << std::endl;
    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        EXEC_COUNT tp = se.getTaskPhase();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
        std::cout << "\\addexecution[premier]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{"  << duration <<  "}{"  << start <<  "}" << std::endl;
    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        EXEC_COUNT tp = se.getTaskPhase();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
        TIME_UNIT period = dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
        TIME_UNIT iteration = 0;
        while (start + period * (iteration + 1) + duration <= SCHEDULING_SIZE) {
            iteration += 1;
            //std::cout << "\\addexecution[suivant]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{"  << duration <<  "}{"  << start + period * iteration <<  "}" << std::endl;
        }
        if (iteration > 0) {
            std::cout << "\\addperiodictask[suivant]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{"  << duration <<  "}{"  << start + period <<  "}{"  << period  <<  "}{"  << iteration - 1 <<   "}" << std::endl;
        }
    }}

    std::cout << "\\end{scheduling}"  << std::endl;

}

void algorithms::compute_1periodic_throughput    (models::Dataflow* const  dataflow, parameters_list_t) {

	TIME_UNIT res = optimal_1periodic_throughput(dataflow);
	std::cout << "Maximum throughput is " << res   << std::endl;
	std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

	//models::EventGraph* eg =  algorithms::generateEventGraph( dataflow  );
	//eg->computeStartingTime (res);

}


models::EventGraph* algorithms::generateEventGraph(models::Dataflow * dataflow  ) {

	models::EventGraph * g = new models::EventGraph();

	std::vector< std::vector <models::EventGraphVertex> > task2vertex;

	/* generate nodes */
	{ForEachVertex(dataflow,t) {
		task2vertex.push_back(std::vector<models::EventGraphVertex>());
		EXEC_COUNT phase_count = dataflow->getPhasesQuantity(t);
		//VERBOSE_DEBUG("generate " << phase_count << " node for task " << dataflow->getVertexId(t));
		for (EXEC_COUNT i = 1; i <= phase_count ; i++ ) {
			models::EventGraphVertex newevent = g->addEvent(models::SchedulingEvent(dataflow->getVertexId(t),i,1));
			task2vertex[t.v].push_back(newevent);
		}
	}}


	///* reentrancy */
	{ForEachVertex(dataflow,t) {
		Vertex source = t;
		Vertex target = t;
		const EXEC_COUNT max_k  =  dataflow->getPhasesQuantity(t);

		const TIME_UNIT timefactor = (dataflow->getReentrancyFactor(t) <= 0)?0:1;

		// constraintes k --> k + 1

		for(EXEC_COUNT k = 1; k < max_k ; k++) {
			EXEC_COUNT kp = k + 1;
			const TIME_UNIT       d    = dataflow->getVertexDuration(t,k) * timefactor;
			const TIME_UNIT       w    = 0;
			models::EventGraphVertex source_event = task2vertex[source.v][(unsigned int) k-1];
			models::EventGraphVertex target_event = task2vertex[target.v][(unsigned int) kp-1];

			g->addEventConstraint(source_event ,target_event,-w,d,0);

		}

		if (dataflow->getReentrancyFactor(t) > 0) // consider only a factor 1
		{
			// constraintes last_k --> 1
			const TIME_UNIT       d    = dataflow->getVertexDuration(t,max_k) * timefactor;
			const TIME_UNIT       w    =   - (TIME_UNIT) max_k / (  (TIME_UNIT) dataflow->getNi(source) );
			models::EventGraphVertex source_event = task2vertex[source.v][(unsigned int) max_k-1];
			models::EventGraphVertex target_event = task2vertex[target.v][(unsigned int) 0];
			VERBOSE_DEBUG("add task constraint :" << dataflow->getVertexName(t) << " : " <<   target_event  << " --> " <<  source_event << " : " <<  " (- " << max_k << " / " << dataflow->getNi(source)  << ") ," << d );
			g->addEventConstraint(source_event ,target_event,-w,d,0);
		}

	}}

	/* generate buffer precedence relation */
	{ForEachEdge(dataflow,c) {
	    VERBOSE_DEBUG("Constraint for " << dataflow->getEdgeName(c) );
		ARRAY_INDEX id = dataflow->getEdgeId(c);
		Vertex source = dataflow->getEdgeSource(c);
		Vertex target = dataflow->getEdgeTarget(c);
		EXEC_COUNT source_phase_count = dataflow->getEdgeInPhasesCount(c);
		EXEC_COUNT target_phase_count = dataflow->getEdgeOutPhasesCount(c);

		TOKEN_UNIT mop     =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

		const TOKEN_UNIT  Wc        =  dataflow->getEdgeIn(c);
		const TOKEN_UNIT  Rc        =  dataflow->getEdgeOut(c);

        const TOKEN_UNIT  gcdz       = boost::math::gcd((Wc),(Rc));
        const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);

		TOKEN_UNIT normdapkm1 = 0;
		TOKEN_UNIT normdapk   = 0;

		//VERBOSE_DEBUG("gcdz=" << gcdz);
		//VERBOSE_DEBUG("mop=" << mop);
		//VERBOSE_DEBUG("Wc=" << Wc << " Rc=" << Rc);
		for (EXEC_COUNT i = 1; i <= source_phase_count ; i++ ) {
			TIME_UNIT  d =  dataflow->getVertexDuration(source,i); // L(a) = l(ti)
			TOKEN_UNIT normdamkp = 0;
			const TOKEN_UNIT wak        = dataflow->getEdgeInPhase(c,i)   ;
			normdapk += wak;
			models::EventGraphVertex source_event = task2vertex[source.v][(unsigned int) i-1];
			for (EXEC_COUNT j = 1; j <= target_phase_count ; j++ ) {
				const TOKEN_UNIT vakp       = dataflow->getEdgeOutPhase(c,j) ;
				normdamkp += vakp;
				models::EventGraphVertex target_event = task2vertex[target.v][(unsigned int) j-1];

#ifdef OLDVERSION
				const TOKEN_UNIT  alphamin = (TOKEN_UNIT)  commons::ceil(std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp,gcdz);
				const TOKEN_UNIT alphamax  =  commons::floor( - mop - normdapkm1 + normdamkp - stepa ,gcdz);
#else

				const TOKEN_UNIT lpimax =    ( - mop - normdapkm1 + normdamkp - stepa) ;
				const TOKEN_UNIT rpimax =     lpimax % gcdz;
				const TOKEN_UNIT alphamax =   (rpimax >= 0) ? ( lpimax - rpimax ) : ( lpimax - rpimax - gcdz );


				const TOKEN_UNIT lpimin =    (std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp) ;

				if (lpimin > alphamax ) continue; // ca ne fera qu'empirer

				const TOKEN_UNIT rpimin =     lpimin % gcdz;
				const TOKEN_UNIT alphamin =   (rpimin <= 0) ? ( lpimin - rpimin ) : ( lpimin - rpimin + gcdz );

#endif

				//TIME_UNIT w = (TIME_UNIT) alphamax /  (TIME_UNIT) Wc ;
				VERBOSE_DEBUG("  ki=" << i<<" kj=" << j << " (" <<  source_event  << "," << target_event  << ")");
				VERBOSE_DEBUG("  alphamin=" << alphamin <<"   alphamax=" << alphamax );
				if (alphamin <= alphamax) {

					TIME_UNIT w = ((TIME_UNIT) alphamax * (TIME_UNIT) dataflow->getPhasesQuantity(source) ) / ( (TIME_UNIT) Wc * (TIME_UNIT) dataflow->getNi(source) );
					VERBOSE_DEBUG("   w = (" << alphamax << " * " << dataflow->getPhasesQuantity(source) << ") / (" << Wc << " * " << dataflow->getNi(source) << ")");
					VERBOSE_DEBUG("   d = (" << d << ")");
					g->addEventConstraint(source_event ,target_event,-w,d,id);
				} else {
				}

			}

			normdapkm1 = normdapk;
		}

	}}


	return g;

}



void algorithms::print_periodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {
	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	// STEP 0 - CSDF Graph should be normalized
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");
	//algorithms::normalize(dataflow);

	// STEP 1 - Generate Event Graph
	models::EventGraph* eg = algorithms::generateEventGraph(dataflow);
	// STEP 2 - print it


	std::cout << eg->printXML();


}

TIME_UNIT algorithms::optimal_1periodic_throughput    (models::Dataflow* const  dataflow) {
	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	// STEP 0 - Compute repetition vector
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

	//STEP 1 - Generate Event Graph
	models::EventGraph* eg = generateEventGraph(dataflow);

	//STEP 2 - resolve the MCRP on this Event Graph
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

	//STEP 3 - print result
	VERBOSE_INFO(" -> Critical circuit size is " << howard_res.second.size()  << " nodes.");
	for (std::vector<models::EventGraphEdge>::iterator itr = howard_res.second.begin(); itr != howard_res.second.end(); ++itr)
	{
		VERBOSE_DEBUG(" -> "<< "(" <<  eg->getConstraint(*itr)._id << ":" <<   eg->getConstraint(*itr)._w << "," << eg->getConstraint(*itr)._d << ") " );

	}

	delete eg;

	return howard_res.first;
}




void algorithms::compute_csdf_1periodic_memory   (models::Dataflow* const  dataflow, parameters_list_t params) {


	commons::ValueKind CONTINUE_OR_INTEGER = commons::KIND_CONTINUE;
    VERBOSE_INFO("Please note you can specify the INTEGERSOLVING and ILPGENERATIONONLY parameters.");

	if (params.find("INTEGERSOLVING")!= params.end() ) CONTINUE_OR_INTEGER = commons::KIND_INTEGER;

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	TIME_UNIT FREQUENCY = 1.0 / dataflow->getPeriod();

	// STEP 0 - CSDF Graph should be normalized
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	// STEP 1 - Compute normalized period
	TIME_UNIT PERIOD = dataflow->getPeriod()  ;

	//##################################################################
	// Linear program generation
	//##################################################################
	const std::string problemName =  "PeriodicSizing_" + dataflow->getName() + "_" + commons::toString(FREQUENCY) + "_" + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "INT" : "");
	commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);


	// Starting times
	//******************************************************************
	{ForEachVertex(dataflow,pVertex) {
		std::string name = dataflow->getVertexName(pVertex);
		for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(pVertex) ; a++) {
			g.addColumn("s_" + commons::toString<EXEC_COUNT>(a) + "_" + name,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
		}
	}}






	// Constraints
	//******************************************************************

	{ForEachEdge(dataflow,c) {
		const Vertex source   = dataflow->getEdgeSource(c);
		const Vertex target   = dataflow->getEdgeTarget(c);

		const std::string  buffername= dataflow->getEdgeName(c);
		const std::string  feedbackbuffername= "feedback_" + dataflow->getEdgeName(c);
		const std::string  feedback_mo_name   = "Mop_" + feedbackbuffername;
		const std::string  mo_name   = "Mop_" + buffername;
		const std::string  sourceStr = dataflow->getVertexName(source);
		const std::string  targetStr = dataflow->getVertexName(target);

		const TIME_UNIT  mu_i        = (TIME_UNIT) (PERIOD * dataflow->getPhasesQuantity(source)) / (TIME_UNIT) ( dataflow->getNi(source));
		const TIME_UNIT  mu_j        = (TIME_UNIT) (PERIOD * dataflow->getPhasesQuantity(target)) / (TIME_UNIT) ( dataflow->getNi(target));

		const TOKEN_UNIT  Zi        = dataflow->getEdgeIn(c);
		const TOKEN_UNIT  Zj        = dataflow->getEdgeOut(c);

		const TOKEN_UNIT  mop      =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

		const TOKEN_UNIT  gcdz      = boost::math::gcd((Zi),(Zj));

		VERBOSE_DEBUG("Mu_i = " << mu_i);
		VERBOSE_DEBUG("Mu_j = " << mu_j);

		TOKEN_UNIT wai    = 0;  /* wai data write at start ai  */
		TOKEN_UNIT cwai   = 0;  /* cwai cumul data write at start ai   */
		TOKEN_UNIT cwaim1 = 0;  /* cwai cumul data write at start ai-1 */

		TOKEN_UNIT raj    = 0;   /* raj data read at start aj  */
		TOKEN_UNIT craj   = 0;   /* craj cumul data read at start aj   */
		TOKEN_UNIT crajm1 = 0;   /* craj cumul data read at start aj-1 */

		// Feedback Buffer marking (in objectif, its Theta pondered)
		g.addColumn("loopbackfix",CONTINUE_OR_INTEGER,commons::bound_s(commons::FIX_BOUND,dataflow->getVerticesCount()),2);
		g.addColumn(mo_name,CONTINUE_OR_INTEGER,commons::bound_s(commons::FIX_BOUND,dataflow->getPreload(c)),(double) dataflow->getTokenSize(c));
		g.addColumn(feedback_mo_name,CONTINUE_OR_INTEGER,commons::bound_s(commons::LOW_BOUND,0),(double) dataflow->getTokenSize(c));

		// init value for ai = 0
		wai    = 0;
		cwai   = 0;
		cwaim1 = 0;

		for(EXEC_COUNT ai = 1; ai <= dataflow->getPhasesQuantity(source) ; ai++) {

			const TIME_UNIT       ltai    = dataflow->getVertexDuration(source,ai);

			// update wai and cwai (new execution of ti)
			wai = dataflow->getEdgeInPhase(c,ai);
			cwai += wai;

			// init value for aj = 0
			raj    = 0;
			craj   = 0;
			crajm1 = 0;

			for(EXEC_COUNT  aj = 1; aj <= dataflow->getPhasesQuantity(target) ; aj++) {

				const TIME_UNIT       ltaj    = dataflow->getVertexDuration(target,aj);

				// update raj and craj (new execution of tj)
				raj = dataflow->getEdgeOutPhase(c,aj);
				craj += raj;

				// *** Normal Buffer constraint computation
				const TOKEN_UNIT  Ha        =   std::max((TOKEN_UNIT)0, wai - raj);
				const TOKEN_UNIT  alphamin  =   commons::ceil(Ha + craj - cwai - mop,gcdz);
				const TOKEN_UNIT  alphamax  =   commons::floor(  craj - cwaim1 - mop - 1 ,gcdz);


				if (alphamin <= alphamax) { // check if contraint exist
					const std::string pred_row_name = "precedence_" + buffername + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj);
					TIME_UNIT coef = ltai + (TIME_UNIT) alphamax * (TIME_UNIT) ( mu_i /  (TIME_UNIT) Zi);
					VERBOSE_DEBUG("LP : s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " >= " << ltai << " + " << (TIME_UNIT) alphamax << " * (" << mu_i << "/" << Zi <<  ")");
					VERBOSE_DEBUG("     s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " >= " << coef);

					g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) coef));
					if ( (ai != aj) || (source != target)) {
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(aj) + "_"+ targetStr    ,  1        );
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(ai) + "_"+ sourceStr    , -1        );
					}
				}

				// *** Feedback Buffer constraint computation
				if ( std::min (wai,raj) > 0) { // best know constraint test for the moment (should be improve)
					const std::string pred_row_name = "precedence_" + feedbackbuffername + "_" +
							commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(ai);
					const std::string local_mo_name = feedback_mo_name  + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj) ;

					//TOKEN_UNIT      quot = commons::floor(- crajm1 + cwai  - gcdz,gcdz);
					//TOKEN_UNIT      rem =  TOKEN_UNIT - quot;
					//TIME_UNIT       coef     =  ltaj  + (TIME_UNIT) ( mu_j /  (TIME_UNIT) Zj) * quot;
					TIME_UNIT       coef     =  ltaj ;
					//VERBOSE_DEBUG(" add feedback coef, " << ltaj << " + " << (TIME_UNIT)  (quot - gcdz) << " * " << mu_j << "=" << coef);

					g.addColumn(local_mo_name,CONTINUE_OR_INTEGER,commons::bound_s(),0);
					// main constraint
					g.addRow(pred_row_name  ,commons::bound_s(commons::LOW_BOUND,coef));
					g.addCoef(pred_row_name ,local_mo_name                     , -   (double) ( (double) mu_j /  (double) Zj) * (double) gcdz );

					if ( (ai != aj) || (source != target)) {
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(ai) + "_"+ sourceStr    ,  1        );
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(aj) + "_"+ targetStr    , -1        );
					}
					VERBOSE_DEBUG("LP : s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " >= " << ltaj << " + f * " << gcdz <<  " *(" << mu_j << "/" << Zj <<  ")");
					VERBOSE_DEBUG("     s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - f * " << (TIME_UNIT) ( mu_j /  (TIME_UNIT) Zj) * gcdz  << " >= " << coef);


					//local mop bound
					// -Mo -WaPred + Ra - 1 - gcdz + 1 \leq local_mo * gcdz  \leq -Mo -WaPred + Ra - 1
					// Low bound :
					//   -Mo -WaPred + Ra - 1 - gcdz + 1 \leq local_mo
					//   -Mo -gcdz * local_mo  \leq WaPred - Ra + gcdz
					//   Ra - WaPred - gcdz - gcdz * local_mo  \leq + Mo
					//   Mo \geq  Ra - WaPred - gcdz - gcdz * local_mo
					g.addRow (local_mo_name + "_L",commons::bound_s(commons::LOW_BOUND, - crajm1 + cwai  - gcdz ));
					g.addCoef(local_mo_name + "_L",local_mo_name        , (double) gcdz   );
					g.addCoef(local_mo_name + "_L",feedback_mo_name     , 1       );


					// resume
					//
					// -local_mo * gcdz - Mo \leq  WaPred - Ra + gcdz
					//  local_mo * gcdz + Mo \leq -WaPred + Ra - 1

				} // end of  if ( std::min (wai,raj) > 0)

				crajm1 = craj;
			}


			cwaim1 = cwai;
		}

	}}




	// LoopBack
	//******************************************************************

	{ForEachVertex(dataflow,t) {
		const TIME_UNIT timefactor    = (dataflow->getReentrancyFactor(t) <= 0)?0:1;


		const std::string  name   = dataflow->getVertexName(t);
		const EXEC_COUNT max_k    = dataflow->getPhasesQuantity(t) ;
		const TIME_UNIT  mu_i     = (TIME_UNIT)  (PERIOD * dataflow->getPhasesQuantity(t)) / (TIME_UNIT) (dataflow->getNi(t));
		VERBOSE_DEBUG( "Mu_" << name << " = " << mu_i  << "   lti = " << dataflow->getVertexPhaseDuration(t) << "  Ni = " << dataflow->getNi(t) << "   Phi=" << dataflow->getPhasesQuantity(t));
		// constraintes k --> k + 1

		for(EXEC_COUNT k = 1; k < max_k ; k++) {
			EXEC_COUNT kp = k + 1;
			const std::string pred_row_name = "reentrancy_" + name + "_" + commons::toString<EXEC_COUNT>(k) + "_" + commons::toString<EXEC_COUNT>(kp);
			const TIME_UNIT       ltk    = dataflow->getVertexDuration(t,k) * timefactor;

			g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND,(double)ltk));
			g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(kp) + "_"+ name    ,  1      );
			g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(k) + "_"+ name    , -1       );
		}

		// constraintes last_k --> 1
		const std::string pred_row_name = "reentrancy_" + name + "_phi_1";
		const TIME_UNIT       ltk    = dataflow->getVertexDuration(t,max_k) * timefactor;
		const TIME_UNIT coef = ltk -  mu_i;

		VERBOSE_DEBUG("LoopBack for "<< name << " is " << ltk  <<  "-" << mu_i << " = " << coef);

		g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND,(double)coef));

		if (max_k > 1){
			g.addCoef(pred_row_name ,"s_1_"+ name    ,  1      );
			g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(max_k) + "_"+ name    , -1       );
		}


	}}

	//##################################################################
	// SOLVE LP
	//##################################################################

	// commons::GLPParameters ilp_params = commons::getDefaultParams();

	// ilp_params.general_doScale = true;
	// ilp_params.linear_doAdvBasis = true;
	// ilp_params.linear_method = commons::DUAL_LINEAR_METHOD;
	//
	// bool sol = g.solve(ilp_params);
	if (params.find("ILPGENERATIONONLY")!= params.end() )  {
		g.writeMPSProblem();
		return;
	}
	bool sol = g.solveWith();

	VERBOSE_INFO("[CSDF K-periodic]  Solved");

	//##################################################################
	// GATHERING RESULTS
	//##################################################################

	// BUFFER SIZES
	//******************************************************************
	if (sol) {
		DATA_UNIT total_buffer_size = 0;
		// ** Good value retreiving method **
		ARRAY_INDEX  edge_indice = 0; // FIXME
		{ForEachEdge(dataflow,c) {
			edge_indice++;
			const Vertex source   = dataflow->getEdgeSource(c);
			const Vertex target   = dataflow->getEdgeTarget(c);

			const std::string  buffername= dataflow->getEdgeName(c);
			const std::string  feedbackbuffername= "feedback_" + dataflow->getEdgeName(c);
			const std::string  feedback_mo_name   = "Mop_" + feedbackbuffername;
			const std::string  sourceStr = dataflow->getVertexName(source);
			const std::string  targetStr = dataflow->getVertexName(target);

			const TOKEN_UNIT  Zi        = dataflow->getEdgeIn(c);
			const TOKEN_UNIT  Zj        = dataflow->getEdgeOut(c);

			const TOKEN_UNIT  gcdz      = boost::math::gcd((Zi),(Zj));


			TOKEN_UNIT wai    = 0;  /* wai data write at start ai  */
			TOKEN_UNIT cwai   = 0;  /* cwai cumul data write at start ai   */

			TOKEN_UNIT raj    = 0;   /* raj data read at start aj  */
			TOKEN_UNIT craj   = 0;   /* craj cumul data read at start aj   */
			TOKEN_UNIT crajm1 = 0;   /* craj cumul data read at start aj-1 */

			// init value for ai = 0
			wai    = 0;
			cwai   = 0;
			TOKEN_UNIT feedbackmopmax =  commons::ceil(g.getValue(feedback_mo_name),1);
			if (CONTINUE_OR_INTEGER == commons::KIND_INTEGER) feedbackmopmax = (TOKEN_UNIT) round(g.getIntegerValue(feedback_mo_name));
			VERBOSE_DEBUG(dataflow->getEdgeName(c) << " : Starting with " << g.getValue(feedback_mo_name) << " => " << feedbackmopmax );
			for(EXEC_COUNT ai = 1; ai <= dataflow->getPhasesQuantity(source) ; ai++) {

				// update wai and cwai (new execution of ti)
				wai = dataflow->getEdgeInPhase(c,ai);
				cwai += wai;

				// init value for aj = 0
				raj    = 0;
				craj   = 0;
				crajm1 = 0;

				for(EXEC_COUNT  aj = 1; aj <= dataflow->getPhasesQuantity(target) ; aj++) {

					// update raj and craj (new execution of tj)
					raj = dataflow->getEdgeOutPhase(c,aj);
					craj += raj;

					//   -Mo -WaPred + Ra - 1 - gcdz + 1 \leq local_mo
					//   -Mo -gcdz * local_mo  \leq WaPred - Ra + gcdz
					//   -Mo \leq WaPred - Ra + gcdz  + gcdz * local_mo
					//   Mo   >= -  WaPred + Ra - gcdz - gcdz * local_mo


					const std::string pred_row_name = "precedence_" + feedbackbuffername + "_" + commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(ai);
					const std::string local_coef = feedback_mo_name  + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj) ;

					if (!g.haveValue(local_coef)) {
						if (std::min(wai,raj) > 0 ) VERBOSE_FAILURE();
						continue; // skip null constraint
					}

					TOKEN_UNIT from_lp = commons::floor(g.getValue(local_coef),1);
					if (CONTINUE_OR_INTEGER == commons::KIND_INTEGER)
						from_lp =  (TOKEN_UNIT) round(g.getIntegerValue(local_coef));

					// *** Feedback Buffer constraint computation
					TOKEN_UNIT bound =  - from_lp * gcdz - crajm1 + cwai - gcdz;
					if (bound > feedbackmopmax) {
						VERBOSE_DEBUG("   -> new fmax => " << g.getValue(local_coef) << "=> " << from_lp);
						VERBOSE_DEBUG("   -> new bound = " <<  - from_lp <<  "*"  << gcdz << "-" << crajm1 << "+" <<cwai << "-" << gcdz<< "=" << bound );
						feedbackmopmax = bound;
					}
					crajm1 = craj;
				}
			}

			TOKEN_UNIT buffersize =  feedbackmopmax  + dataflow->getPreload(c);
			VERBOSE_INFO(dataflow->getEdgeName(c) << " :" << g.getValue(feedback_mo_name) << " + " << dataflow->getPreload(c) << " -> " << feedbackmopmax << " + " << dataflow->getPreload(c)<< " = " << buffersize  );
			//TOKEN_UNIT from_integer_mop          = commons::floor(g.getIntegerValue(feedback_mo_name),1);

			total_buffer_size += buffersize * dataflow->getTokenSize(c);
		}}

		VERBOSE_INFO("Loopback buffers : " << dataflow->getVerticesCount());
		std::cout << "Total buffer size : " << total_buffer_size
				<< " + 2 * " << dataflow->getVerticesCount() << " = "
				<< total_buffer_size + 2 * dataflow->getVerticesCount() << std::endl ;
	} else {
		VERBOSE_ERROR("No feasible solution");
	}
	return;


}
