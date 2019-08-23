/*
 * bufferLessAwareScheduling.cpp
 *
 *  Created on: Aug 22, 2019
 *      Author: toky
 */


#include <algorithms/repetition_vector.h>
#include <algorithms/schedulings.h>
#include <commons/glpsol.h>

void algorithms::scheduling::KPeriodic_scheduling_bufferless (models::Dataflow* const  dataflow,  parameters_list_t   param_list) {

	 std::map<Vertex,EXEC_COUNT> kvector;
	    {ForEachVertex(dataflow,v) {
	        kvector[v] = 1;
	        if (param_list.count(dataflow->getVertexName(v)) == 1) {
	            std::string str_value = param_list[dataflow->getVertexName(v)];
	            kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
	        }
	    }}

    commons::ValueKind CONTINUE_OR_INTEGER = commons::KIND_CONTINUE;
   // With gurobi might be needed, need to fix that.
    //CONTINUE_OR_INTEGER = commons::KIND_INTEGER;

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);


    VERBOSE_INFO("Getting period ...");

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    VERBOSE_INFO("Generate Graph ...");

    //##################################################################
    // Linear program generation
    //##################################################################
    const std::string problemName =  "KPeriodicSizing_" + dataflow->getName() + "_" + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "INT" : "");
    commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);

    // Starting times
    //******************************************************************
    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
            g.addColumn("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
        }
    }}

    auto OMEGA_COL = g.addColumn("OMEGA",commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),1);





    // Constraints
    //******************************************************************

    {ForEachEdge(dataflow,c) {
        const Vertex source   = dataflow->getEdgeSource(c);
        const Vertex target   = dataflow->getEdgeTarget(c);

        const std::string  buffername= dataflow->getEdgeName(c);
        const std::string  sourceStr = dataflow->getVertexName(source);
        const std::string  targetStr = dataflow->getVertexName(target);
        const EXEC_COUNT  Ni        =  dataflow->getNi(source);

        const TOKEN_UNIT  in_b        = dataflow->getEdgeIn(c);
        const TOKEN_UNIT  ou_b        = dataflow->getEdgeOut(c);

        const TOKEN_UNIT  gcdb      = boost::math::gcd((in_b),(ou_b));
        const TOKEN_UNIT  gcdk      = boost::math::gcd( kvector[source]  * (in_b), kvector[target] * (ou_b));

        const TOKEN_UNIT  mop      =  commons::floor(dataflow->getPreload(c),gcdb);



        const TIME_UNIT       ltai    = dataflow->getVertexDuration(source,1);
        const TOKEN_UNIT  Ha        =   std::max((TOKEN_UNIT)0, in_b - ou_b);

        for(EXEC_COUNT ai = 1; ai <= kvector[source] ; ai++) {
            int saicolid = g.getColumn("s_" + commons::toString<EXEC_COUNT>(ai) + "_"+ sourceStr );
            for(EXEC_COUNT  aj = 1; aj <= kvector[target] ; aj++) {
                int sajcolid = g.getColumn("s_" + commons::toString<EXEC_COUNT>(aj) + "_"+ targetStr );


                // *** Normal Buffer constraint computation
                const TOKEN_UNIT  alphamin  =   commons::ceil(Ha + (TOKEN_UNIT) aj * ou_b - (TOKEN_UNIT) ai * in_b - mop,gcdk);
                const TOKEN_UNIT  alphamax  =   commons::floor(  in_b + (TOKEN_UNIT)aj * ou_b - (TOKEN_UNIT)ai * in_b  - mop - 1 ,gcdk);


                if (alphamin <= alphamax) { // check if contraint exist
                    const std::string pred_row_name = "precedence_" + buffername + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj);
                    TIME_UNIT coef =  ((((TIME_UNIT) alphamax) / ( (TIME_UNIT) Ni  * (TIME_UNIT) in_b )));


                    					//	( (TIME_UNIT) kvector[source] / (TIME_UNIT) Ni  /  ((TIME_UNIT) in_b* (double) kvector[source]));
                    //VERBOSE_DEBUG("LP : s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " >= " << ltai << " + " << (TIME_UNIT) alphamax << " * (" << mu_i << "/" << in_b <<  ")");
                    //VERBOSE_DEBUG("     s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " >= " << coef);

                    int rowid = g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) ltai ));



                    g.fastAddCoef(rowid ,OMEGA_COL    , (double) - coef      );

                    if ( (ai != aj) || (source != target)) {
                        g.fastAddCoef(rowid ,sajcolid    ,  1        );
                        g.fastAddCoef(rowid ,saicolid    , -1        );
                    }
                }
            }
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


    VERBOSE_INFO("Solving problem ...");

    bool sol = g.solve();

    VERBOSE_INFO("Solved, gathering results ...");

    //##################################################################
    // GATHERING RESULTS
    //##################################################################

    // BUFFER SIZES
    //******************************************************************
    if (sol) {

    	TIME_UNIT OMEGA = g.getValue("OMEGA");

        std::cout << "OMEGA : " << OMEGA << std::endl ;

        {ForEachVertex(dataflow,t) {
            std::string name = dataflow->getVertexName(t);
            for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
                auto starting_time = g.getValue("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name);

                std::cout << "s_" << k  << "_" << name <<  "=" << starting_time
                		<< "  NI=" <<  dataflow->getNi(t)
        	         	<< "  period=" <<  OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] ) << std::endl ;
            }
        }}


    } else {
        VERBOSE_ERROR("No feasible solution");
    }

    VERBOSE_INFO("Done");
    return;


}
