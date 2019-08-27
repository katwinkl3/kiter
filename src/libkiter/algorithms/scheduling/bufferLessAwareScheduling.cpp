/*
 * bufferLessAwareScheduling.cpp
 *
 *  Created on: Aug 22, 2019
 *      Author: toky
 */


#include <algorithms/repetition_vector.h>
#include <algorithms/schedulings.h>
#include <algorithms/mappings.h>
#include <commons/glpsol.h>

void bufferless_scheduling (models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector, std::vector<std::vector <Vertex> > task_sequences) {


    commons::ValueKind CONTINUE_OR_INTEGER = commons::KIND_CONTINUE;
   // With gurobi might be needed, need to fix that.
    //CONTINUE_OR_INTEGER = commons::KIND_INTEGER;

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        VERBOSE_INFO(" - " << name << " k=" <<  kvector[t]);
        VERBOSE_ASSERT(kvector[t], "periodicity vector must be positive non zero.");
    }}
    VERBOSE_INFO("Getting period ...");

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    VERBOSE_INFO("Generate Graph ...");

    //##################################################################
    // Linear program generation
    //##################################################################
    const std::string problemName =  "BLKPeriodicSizing_" + dataflow->getName() + "_" + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "INT" : "");
    commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);

    // Hyper Period
    //******************************************************************

    auto OMEGA_COL = g.addColumn("OMEGA",commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),1);

    // Starting times
    //******************************************************************
    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
            g.addColumn("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
        }
    }}


    for (std::vector<Vertex> sequence : task_sequences) {
        VERBOSE_INFO("One sequence of " << sequence.size() << " task to force inline");
        if (sequence.size() == 0) continue;
        EXEC_COUNT force_k = kvector[sequence[0]];

        std::vector<std::string> previous;
        std::string name = dataflow->getVertexName(sequence[0]);

        for(EXEC_COUNT k = 1; k <= kvector[sequence[0]] ; k++) {
        	previous.push_back("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name);
        }

        for (Vertex t : sequence) {
        	VERBOSE_ASSERT(force_k == kvector[t], "Edges from the same sequence must have the same k values.");

            for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {

            	// ADD sequence between between current task k and previous task k.
            	std::string previous_name = previous[k-1];
            	std::string current_name = "s_" + commons::toString<EXEC_COUNT>(k) + "_" + dataflow->getVertexName(t);

            	if (previous_name != current_name)  {
            		// add constraint
            		std::string row_name = "sequence_" + previous_name + "_to_" + current_name;
            		g.addRow(row_name,commons::bound_s(commons::FIX_BOUND, 1 ));
            		g.addCoef(row_name , previous_name   , - 1     );
            		g.addCoef(row_name , current_name    ,   1     );
                    VERBOSE_INFO("Add " << row_name);
            	}
            	previous[k-1] = current_name;
            }

        }


    }






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



                    int rowid = g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) ltai ));
                    VERBOSE_INFO("Add " << pred_row_name);



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
        	         	<< "  period=" <<  OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] )
						<< "  end_of_execution_of_last_instance_of_hyper_period=" <<
						starting_time + dataflow->getVertexDuration(t) + (dataflow->getNi(t)-1)  * ((OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] ))) << std::endl ;
            }
        }}


    } else {
        VERBOSE_ERROR("No feasible solution");
    }

    VERBOSE_INFO("Done");
    return;


}


//remove the current edge between nodes
//add intermediate nodes based on the path between them
std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > & returnValue) {

	VERBOSE_ASSERT(not d->is_read_only(), "The graph must be writable to use addPathNode.");

	std::vector<Vertex> new_vertices;
	// We store infos about edge to be deleted
	auto source_vtx = d->getEdgeSource(c);
	auto target_vtx = d->getEdgeTarget(c);

	auto source_vtx_name = d->getVertexName(source_vtx);
	auto target_vtx_name = d->getVertexName(target_vtx);

	//Find the core index
	auto source = d->getMapping(source_vtx);
	auto target = d->getMapping(target_vtx);

	//use the inrate and route of the edges ans use it when creating the edges
	auto inrates = d->getEdgeInVector(c);
	auto outrates = d->getEdgeOutVector(c);
	auto preload = d->getPreload(c);  // preload is M0

	bool flag = true;
	if (source == target) //ignore this case
		return new_vertices;

	// we delete the edge
	d->removeEdge(c);

	//for every link in the path, add a corresponding node
	auto list = d->getNoC()->get_route(source, target);
	for (auto e : list) {
		//std::cout << e << " --> " ;
		// we create a new vertex "middle"
		auto middle = d->addVertex();
		new_vertices.push_back(middle);

		std::stringstream ss;
		ss << "mid_" << source_vtx_name << "_" << target_vtx_name << "_" << e;

		std::pair<Vertex, Vertex> pair_temp;
		pair_temp.first = middle;
		pair_temp.second = source_vtx;

		returnValue[(unsigned int)e].push_back(pair_temp);
		d->setVertexName(middle,ss.str());

		d->setPhasesQuantity(middle,1); // number of state for the actor, only one in SDF
		d->setVertexDuration(middle,{1}); // is specify for every state , only one for SDF.
		d->setReentrancyFactor(middle,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

		// we create a new edge between source and middle,
		auto e1 = d->addEdge(source_vtx, middle);

		if(flag)
		{
			d->setEdgeInPhases(e1,inrates);  // we specify the production rates for the buffer
			flag = false;
		}
		else
		{
			d->setEdgeInPhases(e1,{1});
		}

		d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
		d->setPreload(e1,preload);  // preload is M0


		source_vtx = middle;
	}

	//find the final edge
	auto e2 = d->addEdge(source_vtx, target_vtx);
	d->setEdgeOutPhases(e2,outrates);
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,0);  // preload is M0
	return new_vertices;

}

void algorithms::scheduling::KPeriodic_scheduling_bufferless (models::Dataflow* const  dataflow,  parameters_list_t   param_list) {

	//algorithms::mapping::moduloMapping (dataflow,  param_list);
    VERBOSE_INFO("Generate KVector");

	std::map<Vertex,EXEC_COUNT> kvector;
	{ForEachVertex(dataflow,v) {
		kvector[v] = 1;
		if (param_list.count(dataflow->getVertexName(v)) == 1) {
			std::string str_value = param_list[dataflow->getVertexName(v)];
			kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
		}
	    VERBOSE_INFO("Task " << dataflow->getVertexName(v) << " - k="<< kvector[v] << " - mapping="<< dataflow->getMapping(v));

	}}

    VERBOSE_INFO("Generate New graph");


	std::vector<std::vector <Vertex> >  sequences;
	models::Dataflow* to = new models::Dataflow(*dataflow);
	to->reset_repetition_vector();
	to->set_writable();
	std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEdges;

	std::vector<Edge> edges_list;
	{ForEachEdge(to,e) {
		edges_list.push_back(e);
	}}

	for(auto e: edges_list) {
		auto seq = addPathNode(to, e, conflictEdges);
		sequences.push_back(seq);
		for (Vertex v : seq) {
			kvector[v] = 1;
		}
	}

    VERBOSE_INFO("bufferless_scheduling (to,  kvector, sequences);");

	bufferless_scheduling (to,  kvector, sequences);

}

