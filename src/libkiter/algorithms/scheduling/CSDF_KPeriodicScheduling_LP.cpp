/*
 * CSDF_KPeriodicScheduling.cpp
 *
 *  Created on: Oct 4, 2019
 *      Author: toky
 */


#include <algorithms/repetition_vector.h>
#include <algorithms/schedulings.h>

#include <algorithms/kperiodic.h>
#include <models/EventGraph.h>
#include <lp/glpsol.h>


static inline const std::string OMEGA_COL_STR ()  {
	return "OMEGA" ;
}

static inline const std::string START_COL_STR (const std::string name, const EXEC_COUNT a, const EXEC_COUNT k)  {
	return "s_" + name + "_" + commons::toString<EXEC_COUNT>(a)+ "_" + commons::toString<EXEC_COUNT>(k) + "_" ;
}

static inline const std::string PRED_ROW_STR (const std::string buffername, const EXEC_COUNT ai, const EXEC_COUNT ki, const EXEC_COUNT aj, const EXEC_COUNT kj)  {
	return "precedence_" + buffername + "___" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(ki)+ "___" + commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(kj);
}

static inline const std::string BL_PRED_ROW_STR (const std::string buffername, const EXEC_COUNT ai, const EXEC_COUNT ki, const EXEC_COUNT aj, const EXEC_COUNT kj)  {
	return "bufferless_precedence_" + buffername + "___" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(ki)+ "___" + commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(kj);
}

bool add_precedences_constraints (commons::GLPSol &g, commons::idx_t OMEGA_COL, const models::Dataflow* const dataflow, const periodicity_vector_t& kvector, const Edge c) {

	const std::string  buffername = dataflow->getEdgeName(c);

	const Vertex source   = dataflow->getEdgeSource(c);
	const Vertex target   = dataflow->getEdgeTarget(c);

	const std::string  sourceStr = dataflow->getVertexName(source);
	const std::string  targetStr = dataflow->getVertexName(target);

	const EXEC_COUNT  periodicity_t =  kvector.at(source);
	const EXEC_COUNT  periodicity_tp =  kvector.at(target);

	const TOKEN_UNIT  NoKPeriodic_i_a   =  dataflow->getEdgeIn(c);
	const TOKEN_UNIT  NoKPeriodic_o_a   =  dataflow->getEdgeOut(c);
	const TOKEN_UNIT  i_a        = (TOKEN_UNIT) periodicity_t  * dataflow->getEdgeIn(c);
	const TOKEN_UNIT  o_a        = (TOKEN_UNIT) periodicity_tp  * dataflow->getEdgeOut(c);
	const TOKEN_UNIT  gcda       = boost::integer::gcd(i_a, o_a);
	const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);
	const TOKEN_UNIT  Moa        = commons::floor(dataflow->getPreload(c), stepa);

	const EXEC_COUNT  subphi_t = dataflow->getPhasesQuantity(source) ;
	const EXEC_COUNT  subphi_tp = dataflow->getPhasesQuantity(target);
	const EXEC_COUNT  N_t = dataflow->getNi(source) / subphi_t;
	const EXEC_COUNT  N_tp = dataflow->getNi(target) / subphi_tp;

	VERBOSE_DEBUG("Buffer " << buffername << " between " << sourceStr << " and " << targetStr );


	VERBOSE_EXTRA_DEBUG("  periodicity_t=" << periodicity_t
		     << " periodicity_tp=" << periodicity_tp
		     << " NoKPeriodic_i_a=" << NoKPeriodic_i_a
		     << " NoKPeriodic_o_a=" << NoKPeriodic_o_a
				 );

	VERBOSE_EXTRA_DEBUG("  N_t=" << N_t
		     << " N_tp=" << N_tp
		    );
	VERBOSE_EXTRA_DEBUG("  i_a=" << i_a
		     << " o_a=" << o_a
		     << " gcda=" << gcda
		     << " stepa=" << stepa
		     << " Moa=" << Moa
				 );



	for(EXEC_COUNT phase_i = 1; phase_i <= subphi_t ; phase_i++) {
	for(EXEC_COUNT period_i = 1; period_i <= periodicity_t ; period_i++) {
	for(EXEC_COUNT phase_j = 1; phase_j <= subphi_tp ; phase_j++) {
	for(EXEC_COUNT period_j = 1; period_j <= periodicity_tp ; period_j++) {

		VERBOSE_EXTRA_DEBUG("    phase_i=" << phase_i << ", phase_j=" << phase_j << ", period_i=" << period_i << ", period_j=" << period_j );


		int saicolid = g.getColumn(START_COL_STR(sourceStr,phase_i,period_i)) ;
		int sajcolid = g.getColumn(START_COL_STR(targetStr,phase_j,period_j)) ;

		const TIME_UNIT ltai = dataflow->getVertexDuration(source,phase_i);

		// /!\ /!\ Careful /!\ /!\, the equation use k to define phases, as K-Periodic is a natural extension of CSDF.


		//EXEC_COUNT k    = phase_i +  subphi_t * (period_i - 1); // current input phase
		//EXEC_COUNT kp   = phase_j +  subphi_tp * (period_j - 1); // current output phase

		//EXEC_COUNT kpm1 = (kp - 1) > 0 ? (kp - 1) : dataflow->getPhasesQuantity(target) ; // previous phase

		TOKEN_UNIT in_a_k   = dataflow->getEdgeInPhase(c, phase_i);
		TOKEN_UNIT out_a_kp = dataflow->getEdgeOutPhase(c, phase_j);
		TOKEN_UNIT out_a_kpm1 = dataflow->getEdgeOutPhase(c, (phase_j - 1) > 0 ? (phase_j - 1) : dataflow->getPhasesQuantity(target) );


		VERBOSE_EXTRA_DEBUG("    in_a_k=" << in_a_k
			     << " out_a_kp=" << out_a_kp
			     << " out_a_kpm1=" << out_a_kpm1
					 );

		// These are defined Page 59 of the B.Bodin 2013 (PhD Thesis).

		TOKEN_UNIT NoKPeriodic_O_a_tpkp_1 = std::accumulate(dataflow->getEdgeOutVector(c).begin(), dataflow->getEdgeOutVector(c).begin() + phase_j , 0) ;
		TOKEN_UNIT NoKPeriodic_I_a_tk_1   = std::accumulate(dataflow->getEdgeInVector(c).begin(),  dataflow->getEdgeInVector(c).begin()  + phase_i , 0) ;

		TOKEN_UNIT O_a_tpkp_1 = NoKPeriodic_O_a_tpkp_1 + (period_j - 1) * NoKPeriodic_o_a ; // sum of consumption up to k
		TOKEN_UNIT I_a_tk_1 = NoKPeriodic_I_a_tk_1 + (period_i - 1) * NoKPeriodic_i_a ; // sum of production up to k

		TOKEN_UNIT I_a_Pre_tk_1 = I_a_tk_1 - in_a_k ; // sum of production up to k - 1


		VERBOSE_EXTRA_DEBUG("    NoKPeriodic_O_a_tpkp_1=" << NoKPeriodic_O_a_tpkp_1
			     << " NoKPeriodic_I_a_tk_1=" << NoKPeriodic_I_a_tk_1
					 );


		VERBOSE_EXTRA_DEBUG("    O_a_tpkp_1=" << O_a_tpkp_1
			     << " I_a_tk_1=" << I_a_tk_1
			     << " I_a_Pre_tk_1=" << I_a_Pre_tk_1
					 );

		// About threshold execution, Defined Page 129 of the B.Bodin 2013 (PhD Thesis).

		TOKEN_UNIT thr_a_kp   = 0;
		TOKEN_UNIT thr_a_kpm1 = 0;
		//TOKEN_UNIT up_a_k_kp  = thr_a_kp + in_a_k ;
		TOKEN_UNIT low_a_k_kp = std::max(thr_a_kp, thr_a_kpm1 + in_a_k - out_a_kp ) ;


		VERBOSE_EXTRA_DEBUG("    thr_a_kp=" << thr_a_kp
			     << " thr_a_kpm1=" << thr_a_kpm1
			     << " low_a_k_kp=" << low_a_k_kp
					 );
		// This definition of alphamin and alphamax is available page 130 of B.Bodin 2013 (PhD Thesis).

		TOKEN_UNIT alphamin = commons::ceil( low_a_k_kp + O_a_tpkp_1 - I_a_tk_1 - Moa , gcda) ;
		TOKEN_UNIT alphamax = commons::floor( thr_a_kp  + O_a_tpkp_1 - I_a_Pre_tk_1 - Moa - 1 , gcda ) ;


		VERBOSE_EXTRA_DEBUG("    Test alphamin (" << alphamin << ") <=  alphamax (" << alphamax << ")");

		if (alphamin <= alphamax) { // check if contraint exist

				// This definition of this constraint is available page 133 of B.Bodin 2013 (PhD Thesis).

				const std::string pred_row_name = PRED_ROW_STR(buffername,phase_i,period_i,phase_j,period_j);
				TIME_UNIT omega_coef =  ((TIME_UNIT) alphamax) / ( (TIME_UNIT) N_t * (TIME_UNIT) NoKPeriodic_i_a );
				int rowid = g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) ltai ));

				VERBOSE_EXTRA_DEBUG("      omega_coef =  " << omega_coef );

				g.fastAddCoef(rowid ,OMEGA_COL    , (double) - omega_coef      );
				if ( sajcolid != saicolid) {
					  g.fastAddCoef(rowid ,sajcolid    ,  1        );
					  g.fastAddCoef(rowid ,saicolid    , -1        );
				}
				if (false)
				if (dataflow->getEdgeType(c) == EDGE_TYPE::BUFFERLESS_EDGE) {
					const std::string pred_row_name = BL_PRED_ROW_STR(buffername,phase_i,period_i,phase_j,period_j);
					int rowid = g.addRow(pred_row_name,commons::bound_s(commons::FIX_BOUND, (double) ltai ));
					if ( sajcolid != saicolid) {
						g.fastAddCoef(rowid ,sajcolid    ,  1        );
						g.fastAddCoef(rowid ,saicolid    , -1        );
					}
				}
			}

	}}}}

	return true;
}

bool add_reentrancy_constraints (commons::GLPSol &g, const models::Dataflow* const dataflow, const periodicity_vector_t& kvector, const Vertex t) {

	const std::string name = dataflow->getVertexName(t);

	const EXEC_COUNT phase_quantity = dataflow->getPhasesQuantity(t);
	const EXEC_COUNT periodicity_factor = kvector.at(t);
	const EXEC_COUNT repetition_factor = dataflow->getNi(t);

    std::string last_col_name = "";
    TIME_UNIT last_col_time = 0;

	    		for(EXEC_COUNT k = 1; k <= periodicity_factor ; k++) {
	    			for(EXEC_COUNT a = 1; a <= phase_quantity ; a++) {
	    				const TIME_UNIT       ltai    = last_col_time;
	    				std::string new_col_name = START_COL_STR(name,a,k);
	    				std::string row_name = "Task_order_"  + name + "_" + new_col_name;
	    				if (last_col_name != "") {
	    					g.addRow(row_name,commons::bound_s(commons::LOW_BOUND, ltai ));
	    					g.addCoef(row_name , last_col_name   , - 1     );
	    					g.addCoef(row_name , new_col_name    ,   1     );
	    				}
	    				last_col_name = new_col_name;
	    				last_col_time = dataflow->getVertexDuration(t,a);
	    			}
	    		}

	    	    // Last one for reentrancy

	    		if (dataflow->getReentrancyFactor(t)) {
					TIME_UNIT omega_coef =  - ((TIME_UNIT) (phase_quantity * periodicity_factor)  / (TIME_UNIT) repetition_factor ) ;
	    			const std::string last_row_name = "Task_reentracy_" + name;
	    			g.addRow(last_row_name,commons::bound_s(commons::LOW_BOUND, dataflow->getVertexDuration(t,dataflow->getPhasesQuantity(t)) ));
	    			std::string source_col =  START_COL_STR(name,phase_quantity, periodicity_factor);
	    			std::string target_col =  START_COL_STR(name,1,1);
	    			if (target_col != source_col) {
	    				g.addCoef(last_row_name , source_col   , - 1     );
	    				g.addCoef(last_row_name , target_col    ,   1    );
	    			}
	    			g.addCoef(last_row_name , OMEGA_COL_STR(), (double) - omega_coef     );
	    		}

	    		return true;
}


models::Scheduling collect_lp_results (const commons::GLPSol &g, const models::Dataflow* const dataflow, const periodicity_vector_t& kvector) {
	 scheduling_t res_schedule;
	TIME_UNIT OMEGA = g.getValue(OMEGA_COL_STR ());

    {ForEachVertex(dataflow,t) {
        std::string vname = dataflow->getVertexName(t);
        ARRAY_INDEX vid   = dataflow->getVertexId(t);
    	const EXEC_COUNT periodicity_factor = kvector.at(t);

		res_schedule[dataflow->getVertexId(t)].first = OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) periodicity_factor );

		for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(t) ; a++) {
			for(EXEC_COUNT k = 1; k <= periodicity_factor ; k++) {
				auto starting_time_col_str = START_COL_STR(vname,a,k);
				auto starting_time = g.getValue(starting_time_col_str);
				res_schedule[vid].second.push_back(starting_time);
			}
		}
    }}

	return models::Scheduling(dataflow, OMEGA, res_schedule);
}


models::Scheduling  algorithms::scheduling::CSDF_KPeriodicScheduling_LP    (const models::Dataflow* const dataflow, const periodicity_vector_t& kvector) {


    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(dataflow->is_repetition_vector(),"Please generate the repetition vector before using this function.");
    EXEC_COUNT sumNi = std::accumulate(dataflow->getVertices().first, dataflow->getVertices().second, 0, [&](EXEC_COUNT sum, Vertex t){return  sum + dataflow->getNi(t);}) ;
    EXEC_COUNT sumKi = accumulate(kvector.begin(), kvector.end(), 0, [](int v, const periodicity_vector_t::value_type& p){return v + p.second;});

    VERBOSE_INFO("Start CSDF_KPeriodicScheduling_LP with SumKi=" << sumKi << " and sumNi=" << sumNi);

    // We do not process empty graph.
    if (dataflow->getVerticesCount() == 0) return models::Scheduling ();

    //##################################################################
    // Linear program generation
    //##################################################################

        const std::string problemName =  "CSDF_KPeriodicScheduling_LP_" + dataflow->getName();
        commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);

        // Hyper Period
        //******************************************************************
        auto OMEGA_COL = g.addColumn(OMEGA_COL_STR(),commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),1);

        // Starting times
        //******************************************************************
        {ForEachVertex(dataflow,t) {
            std::string name = dataflow->getVertexName(t);

            // TODO: Additionally, there will be initial phases later

    		for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(t) ; a++) {
    			for(EXEC_COUNT k = 1; k <= kvector.at(t); k++) {
    				g.addColumn(START_COL_STR(name,a,k),commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
    			}
    		}

        }}


        // Voir Bodin 2013 Page 105-106 for a full example of Periodic schedule of CSDF with bi-weighted graph


        // Reentrancy Constraints
        //******************************************************************

        {ForEachVertex(dataflow,t) {
        	VERBOSE_ASSERT(add_reentrancy_constraints (g, dataflow, kvector, t), "Error when generating reentrancy constraints for task " << t);
        }}

        // Communication Constraints
        //******************************************************************

        {ForEachEdge(dataflow,c) {
        	VERBOSE_ASSERT(add_precedences_constraints (g, OMEGA_COL, dataflow, kvector, c),"Error when generating reentrancy constraints for edge " << c);
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


        VERBOSE_INFO("Solving problem ...");

        bool sol = g.solve();

        VERBOSE_INFO("Solved, gathering results ...");

        //##################################################################
        // GATHERING RESULTS
        //##################################################################

        // SCHEDULING RESULT
        //******************************************************************

        models::Scheduling persched;
        if (sol) {
        	persched = collect_lp_results (g,  dataflow, kvector) ;
        } else {
            VERBOSE_ERROR("No feasible solution");
        }

        return persched;


}



 const periodicity_vector_t algorithms::scheduling::generateNPeriodicVector(const models::Dataflow* dataflow) {

	VERBOSE_ASSERT(dataflow->is_repetition_vector(),"Need repetition vector.");
	periodicity_vector_t kvector;
	for (Vertex v : dataflow->vertices()){
	        kvector[v] = dataflow->getNi(v);
	}
	return kvector;
}


 const periodicity_vector_t algorithms::scheduling::generate1PeriodicVector(const models::Dataflow* dataflow) {
	 return algorithms::scheduling::generateKPeriodicVector(dataflow, 1);
}


 const periodicity_vector_t algorithms::scheduling::generateKPeriodicVector(const models::Dataflow* dataflow, int k) {

	periodicity_vector_t kvector;
	for (Vertex v : dataflow->vertices()){
	        kvector[v] = k;
	}
	return kvector;
}

 void algorithms::scheduling::CSDF_1PeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t )  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	models::Scheduling res = CSDF_KPeriodicScheduling_LP    (dataflow, generate1PeriodicVector(dataflow));

    TIME_UNIT omega = res.getGraphPeriod();
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}

 void algorithms::scheduling::CSDF_NPeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t )  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	models::Scheduling res = CSDF_KPeriodicScheduling_LP    (dataflow, generateNPeriodicVector(dataflow));

    TIME_UNIT omega = res.getGraphPeriod();
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}


 void algorithms::scheduling::CSDF_NPeriodicScheduling (models::Dataflow*  dataflow, parameters_list_t )  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    //STEP 1 - Generate Event Graph
	auto kvector = generateNPeriodicVector(dataflow);
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
    TIME_UNIT omega = 1/howard_res.first;

    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}

