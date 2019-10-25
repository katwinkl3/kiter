/*
 * kperiodic.cpp
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */


#include <map>
#include <vector>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/kperiodic.h>
#include <algorithms/repetition_vector.h>



bool algorithms::sameset(models::Dataflow* const dataflow, std::set<Edge> *cc1 , std::set<Edge>* cc2) {

    VERBOSE_DEBUG_ASSERT(cc1,"cc1 is not valid");
    VERBOSE_DEBUG_ASSERT(cc2,"cc2 is not valid");

    if (cc1->size() != cc2->size()) return false;

    bool result = false;

    for (std::set<Edge>::iterator it1 = cc1->begin() ; it1 != cc1->end() ; it1++ ) {
        result = false;
        for (std::set<Edge>::iterator it2 = cc2->begin() ; it2 != cc2->end() ; it2++ ) {
            if (dataflow->getEdgeId(*it1) == dataflow->getEdgeId(*it2)) result = true;
        }
        if (result == false) return false;
    }
    return true;
}



std::string algorithms::cc2string  (models::Dataflow* const dataflow,std::set<Edge>* cc) {
    std::ostringstream returnStream;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end() ; it++ ) {
        returnStream <<  dataflow->getEdgeName(*it) <<
	  " (" << dataflow->getVertexName(dataflow->getEdgeSource(*it)) << "->"
	       << dataflow->getVertexName(dataflow->getEdgeTarget(*it)) << "), ";
    }
    return returnStream.str();

}

std::string algorithms::print_schedule (models::EventGraph* eg, models::Dataflow* const  dataflow,  std::map<Vertex,EXEC_COUNT> & kvector , TIME_UNIT res ) {
	std::ostringstream returnStream;


    eg->computeStartingTime (res);
    TIME_UNIT omega = 1 / res ;
    returnStream << "\\begin{scheduling}{" << dataflow->getVerticesCount() <<  "}{" << 0 <<  "}{3.2}{5}" << std::endl;

    {ForEachVertex(dataflow,v) {
    	returnStream << "\\taskname{"  << dataflow->getVertexId(v) <<  "}{"  << dataflow->getVertexName(v) <<  "}" << "% ki=" << kvector[v]   << " Ni=" << dataflow->getNi(v)  << std::endl;
    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        EXEC_COUNT tp = se.getTaskPhase();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
        TIME_UNIT period = kvector[v] *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
        //if (start + duration <= SCHEDULING_SIZE){
        returnStream << kvector[v]  << " *  " << dataflow->getPhasesQuantity(v) <<  " * " <<  omega <<  "/" <<  dataflow->getNi(v)  << std::endl;
        	returnStream << "\\addexecution[premier]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{duration = "  << duration <<  "}{ start="  << start <<  "}{ period="  << period <<  "}"  << std::endl;
        //}
    }}

    returnStream << "\\end{scheduling}"  << std::endl;
    return returnStream.str();
}
void print_function    (models::Dataflow* const  dataflow,  std::map<Vertex,EXEC_COUNT> & kvector , TIME_UNIT res , bool printXML = false, bool printTikz = false , bool printSchedule = false ) {


    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);

    if (printXML)      std::cout << eg->printXML();
    if (printTikz)     std::cout << eg->printTikz();
    if (printSchedule) std::cout << algorithms::print_schedule(eg,dataflow,kvector,res);

}

void algorithms::print_kperiodic_expansion_graph    (models::Dataflow* const  dataflow, parameters_list_t param_list) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    bool print_dot = (param_list.find("DOT")!= param_list.end());
    bool print_xml = (param_list.find("XML")!= param_list.end());
    bool print_tikz = (param_list.find("TIKZ")!= param_list.end());

    VERBOSE_INFO("Please note you can specify the values of K and DOT,XML, and TIKZ paramters.");
    if ( not (print_dot || print_xml || print_tikz) )
        VERBOSE_ERROR("You did not ask for any kind of output, please specify parameters DOT,XML, or TIKZ ");


    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,v) {
        kvector[v] = 1;
        if (param_list.count(dataflow->getVertexName(v)) == 1) {
            std::string str_value = param_list[dataflow->getVertexName(v)];
            kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
        }
    }}
    kperiodic_result_t result = KSchedule(dataflow,&kvector);

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);

    if (print_dot)      std::cout << eg->printDOT();
    if (print_xml)      std::cout << eg->printXML();
    if (print_tikz)      std::cout << eg->printTikz();

}

scheduling_t period2scheduling    (models::Dataflow* const  dataflow,  std::map<Vertex,EXEC_COUNT> & kvector , TIME_UNIT throughput) {


	scheduling_t scheduling_result;

    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    eg->computeStartingTime (throughput);
    TIME_UNIT omega = 1 / throughput ;


    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT period = kvector[v] *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
        scheduling_result[dataflow->getVertexId(v)].first = period;

        VERBOSE_DEBUG("Task " << dataflow->getVertexName(v) << " " << period  );

    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        scheduling_result[dataflow->getVertexId(v)].second.push_back( start );
        VERBOSE_DEBUG("Task " << dataflow->getVertexName(v) << " " << start  );


    }}



    return scheduling_result ;

}

void algorithms::print_kperiodic_scheduling    (models::Dataflow* const  dataflow, parameters_list_t param_list) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    VERBOSE_INFO("Please note you can specify the values of K.");
    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,v) {
        kvector[v] = 1;
        if (param_list.count(dataflow->getVertexName(v)) == 1) {
            std::string str_value = param_list[dataflow->getVertexName(v)];
            kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
        }
    }}
    kperiodic_result_t result = KSchedule(dataflow,&kvector);
    print_function    ( dataflow, kvector , result.first , true, true, true);
    VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

}

kperiodic_result_t algorithms::KSchedule(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , TIME_UNIT bound )  {

	kperiodic_result_t result;

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");


    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,kvector);

    VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = (bound > 0) ? eg->MinCycleRatio(bound) : eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }


    TIME_UNIT frequency = howard_res.first;

    // /* return the best omega found in sdf3 way */
    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );
    //VERBOSE_INFO("  -> considering task " << dataflow->getVertexName(first) << ", mu_t = " << mui );
    //VERBOSE_INFO("  -> then sdf3 normalize frequency is " << thg );


    //result.first = thg;
    result.first = frequency;
    delete eg;

    return result;
}

kperiodic_result_t algorithms::KScheduleBufferLess(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , TIME_UNIT bound )  {

    kperiodic_result_t result;

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");


    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,kvector, true);

    VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = (bound > 0) ? eg->MinCycleRatio(bound) : eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }


    TIME_UNIT frequency = howard_res.first;

    // /* return the best omega found in sdf3 way */
    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );
    //VERBOSE_INFO("  -> considering task " << dataflow->getVertexName(first) << ", mu_t = " << mui );
    //VERBOSE_INFO("  -> then sdf3 normalize frequency is " << thg );


    //result.first = thg;
    result.first = frequency;
    delete eg;

    return result;
}


models::EventGraph* algorithms::generateKPeriodicEventGraph(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues , bool doBufferLessEdges) {


    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    models::EventGraph * g = new models::EventGraph();


    /* generate nodes */
    {ForEachVertex(dataflow,t) {
        EXEC_COUNT phase_count = dataflow->getPhasesQuantity(t);
        for (EXEC_COUNT j = 1; j <= kValues->at(t) ; j++ ) {
            for (EXEC_COUNT i = 1; i <= phase_count ; i++ ) {
                g->addEvent(models::SchedulingEvent(dataflow->getVertexId(t),i,j));
            }
        }
    }}


    // DEFINITION DES REENTRANCES
    //******************************************************************

    {ForEachVertex(dataflow,pTask) {
        EXEC_COUNT start_count = kValues->at(pTask);
        generateKperiodicSelfloop(dataflow,start_count,g,pTask);
    }}


    // DEFINITION DES CONTRAINTES DE PRECEDENCES
    //******************************************************************
    {ForEachChannel(dataflow,c) {
        generateKPeriodicConstraint(dataflow , kValues,   g ,  c, doBufferLessEdges);
    }}



    return g;



}

void algorithms::print_NKperiodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");


    // STEP 0.2
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = dataflow->getNi(t);
    }}
    // STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    // STEP 2 - print it


    std::cout << eg->printXML();


}

void algorithms::print_2periodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");


    // STEP 0.2
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 2;
    }}
    // STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    // STEP 2 - print it


    std::cout << eg->printXML();


}


void algorithms::print_1periodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");


    // STEP 0.2
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    // STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    // STEP 2 - print it


    std::cout << eg->printXML();


}



bool algorithms::updateVectorWithFineNi( models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * oldkvector, std::set<Edge>* cc ) {

    VERBOSE_ASSERT(dataflow,"error");
    VERBOSE_ASSERT(oldkvector,"error");
    VERBOSE_ASSERT(cc,"error");

    std::map<Vertex,EXEC_COUNT>  kvector ;
    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        kvector[it->first] = it->second;
    }

    // STEP 1 = GENERATE OLD KI AND NEW KI VECTORS == GENERATE addconstraint

    if (!updateVectorWithLocalNi(dataflow,&kvector,cc)) return false;

    models::EventGraph*  cycle_eg =  generateCycleOnly( dataflow , &kvector,   cc ) ;
    //
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = cycle_eg->MinCycleRatio();
    VERBOSE_INFO("With this cycle the new bound is " << howard_res.first);


    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        oldkvector->at(it->first) = kvector.at(it->first);
    }

    delete cycle_eg;

    return true;

}








models::EventGraph* algorithms::updateEventGraph( models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * oldkvector, std::set<Edge>* cc, models::EventGraph* g, bool verbose = false) {



    VERBOSE_ASSERT(dataflow,"error");
    VERBOSE_ASSERT(oldkvector,"error");
    VERBOSE_ASSERT(g,"error");

    EXEC_COUNT addconstraint = 0;

    // Compute old and new vector


    VERBOSE_INFO("Update event graph - Step 0 - Compute new K and check changed");

    std::map<Vertex,EXEC_COUNT>  kvector ;
    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        kvector[it->first] = it->second;
    }

    // STEP 1 = GENERATE OLD KI AND NEW KI VECTORS == GENERATE addconstraint


    bool changed = updateVectorWithLocalNi(dataflow,&kvector,cc);

    if (!changed) {
        VERBOSE_INFO("Unchanged ...");
        return NULL;
    }


    VERBOSE_INFO("Update event graph - Step 1 - Delete edges and add task");
    // STEP 1
    //remove all connected edges
    EXEC_COUNT total = cc->size();
    EXEC_COUNT current = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        current ++ ;
        if (verbose) {std::cout << "\rdelete edges ... " << std::setw(3) << ((current* 100)  / (total))   << "%" << std::setw( 11 ) << " ";std::cout.flush();}
        Vertex source = dataflow->getEdgeSource(*it);
        EXEC_COUNT ki = oldkvector->at(source);
        EXEC_COUNT newki =  kvector.at(source);
        if (ki < newki) {
            ARRAY_INDEX task_id= dataflow->getVertexId(source);
            for (EXEC_COUNT p = 1 ; p <= dataflow->getPhasesQuantity(source) ; p++) {
                for(ARRAY_INDEX k = 1 ; k <= ki ; k++) {
                    g->removeConnectedEdges(g->getEventGraphVertex(task_id,p,k));
                }
            }

            for (EXEC_COUNT p = 1 ; p <= dataflow->getPhasesQuantity(source) ; p++) {
                g->addEventGroup(task_id,p,ki+1,newki);
            }
            if (newki > dataflow->getNi(source)) {
                VERBOSE_ERROR("newki=" << newki << " Ni=" << dataflow->getNi(source));
            }
            VERBOSE_ASSERT(newki <= dataflow->getNi(source),"Bad idea !");
        }
    }



    VERBOSE_INFO("Update event graph - Step 2 - Reentrancy");

    current = 0;

    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        if (verbose) {std::cout << "\rreentrency edges ... " << std::setw(3) << ((current* 100)  / (total) ) << "%" << std::setw( 11 ) << " ";std::cout.flush();}
        current ++ ;
        Vertex t = dataflow->getEdgeSource(*it);

        const EXEC_COUNT oldki =  oldkvector->at(t);
        const EXEC_COUNT newki =     kvector.at(t);

        // SKIP IF KI NOT CHANGED (no loopback to create, because loopback not deleted)
        if (oldki == newki) continue;




        VERBOSE_DEBUG("generate reentrancy loops for task " <<  t << " with newki=" << newki);
        EXEC_COUNT start_count = kvector.at(t);
        generateKperiodicSelfloop(dataflow,start_count,g,t);

    }


    VERBOSE_INFO("Update event graph - Step 3 - add " << addconstraint << " constraints.");

    // STEP 3
    // add all edges

    current = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end() ; it++) {
        if (verbose) {std::cout << "\rconstraint edges ... " << std::setw(3) << ((current* 100)  / (total) )  << "% (" << std::setw( 11 ) << total << " edges)";std::cout.flush();}
        current ++ ;
        std::set<Edge>::iterator it_previous = it;
        if (it == cc->begin())  {
            it_previous = cc->end();
        }
        it_previous--;
        Edge current = *it;
        Edge previous = *it_previous;
        Vertex source = dataflow->getEdgeSource(current);
        Vertex target = dataflow->getEdgeTarget(current);
        const EXEC_COUNT oldki =  oldkvector->at(source);
        const EXEC_COUNT newki =     kvector.at(source);
        const EXEC_COUNT oldkj =  oldkvector->at(target);
        const EXEC_COUNT newkj =     kvector.at(target);

        // SKIP IF KI NOT CHANGED (no loopback to create, because loopback not deleted
        if (oldki == newki) continue;

        {ForOutputEdges(dataflow,source,c) {
            if ((c != current) || (oldkj == newkj)) {
                generateKPeriodicConstraint(dataflow , &kvector,   g ,  c);
            }

        }}

        {ForInputEdges(dataflow,source,c) {
            if (c != previous) {
                generateKPeriodicConstraint(dataflow , &kvector,   g ,  c);
            } else {
                generateKPeriodicConstraint(dataflow , &kvector,   g ,  c); //speed up here only
            }
        }}
    }


    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        oldkvector->at(it->first) = kvector.at(it->first);
    }

    if (verbose) {std::cout << "End of generation" << std::setw(30)  << "\r";std::cout.flush();}


    return g;

}






models::EventGraph*  algorithms::generateCycleOnly(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,   std::set<Edge> * cc ) {


    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    models::EventGraph * g = new models::EventGraph();

    VERBOSE_DEBUG_ASSERT(dataflow->is_normalized() == false,"looser");


    /* generate nodes */
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex pTask = dataflow->getEdgeSource(*it);
        EXEC_COUNT start_count = kValues->at(pTask);
        {ForEachPhase(dataflow,pTask,p) {
            VERBOSE_DEBUG("generate " << start_count << " node for task " << dataflow->getVertexId(pTask) << " phase " << p);
            g->addEventGroup(dataflow->getVertexId(pTask),p,start_count);
        }}


    }


    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex pTask = dataflow->getEdgeSource(*it);
        EXEC_COUNT start_count = kValues->at(pTask);
        generateKperiodicSelfloop(dataflow,start_count,g,pTask);

    }




    // DEFINITION DES CONTRAINTES DE PRECEDENCES
    //******************************************************************


    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        generateKPeriodicConstraint(dataflow , kValues,   g ,  *it);
    }




    return g;

}



void algorithms::generateKPeriodicConstraint(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,  models::EventGraph* g , Edge c, bool doBufferLessEdges) {


    VERBOSE_DEBUG("Constraint for " << dataflow->getEdgeName(c) );

    ARRAY_INDEX id = dataflow->getEdgeId(c);

    Vertex source = dataflow->getEdgeSource(c);
    Vertex target = dataflow->getEdgeTarget(c);

    const ARRAY_INDEX source_id = dataflow->getVertexId(source);
    const ARRAY_INDEX target_id = dataflow->getVertexId(target);

    EXEC_COUNT source_phase_count = dataflow->getEdgeInPhasesCount(c);
    EXEC_COUNT target_phase_count = dataflow->getEdgeOutPhasesCount(c);

    const EXEC_COUNT  maxki        = kValues->at(source) ;
    const EXEC_COUNT  maxkj        = kValues->at(target) ;

    TOKEN_UNIT mop     =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

    const TOKEN_UNIT  Wc        =  dataflow->getEdgeIn(c) * maxki;
    const TOKEN_UNIT  Rc        =  dataflow->getEdgeOut(c) * maxkj;

    const TOKEN_UNIT  gcdz       = boost::math::gcd((Wc),(Rc));
    const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);

    TOKEN_UNIT normdapkm1 = 0;
    TOKEN_UNIT normdapk   = 0;



    for (EXEC_COUNT ki = 1; ki <= maxki ; ki++ ) {
        for (EXEC_COUNT pi = 1; pi <= source_phase_count ; pi++ ) {

            TIME_UNIT  d =  dataflow->getVertexDuration(source, pi); // L(a) = l(ti)
            TOKEN_UNIT normdamkp = 0;
            const TOKEN_UNIT wak        = dataflow->getEdgeInPhase(c,pi)   ;
            normdapk += wak;
            models::EventGraphVertex source_event = g->getEventGraphVertex(source_id,pi,ki);


            for (EXEC_COUNT kj = 1; kj <= maxkj ; kj++ ) {
                for (EXEC_COUNT pj = 1; pj <= target_phase_count ; pj++ ) {

                    const TOKEN_UNIT vakp       = dataflow->getEdgeOutPhase(c,pj) ;
                    normdamkp += vakp;

#ifdef NOT_OPTIMIZED
                    const TOKEN_UNIT  alphamin = (TOKEN_UNIT)  commons::ceil(std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp,gcdz);
                    const TOKEN_UNIT alphamax  =  commons::floor( - mop - normdapkm1 + normdamkp - stepa ,gcdz);
#else

                    const TOKEN_UNIT lpimax =    ( - mop - normdapkm1 + normdamkp - stepa) ;
                    const TOKEN_UNIT rpimax =     lpimax % gcdz;
                    const TOKEN_UNIT alphamax =   (rpimax >= 0) ? ( lpimax - rpimax ) : ( lpimax - rpimax - gcdz );
                    const TOKEN_UNIT lpimin =    (std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp) ;

                    //if (lpimin > alphamax ) continue; // ca ne fera qu'empirer

                    const TOKEN_UNIT rpimin =     lpimin % gcdz;
                    const TOKEN_UNIT alphamin =   (rpimin <= 0) ? ( lpimin - rpimin ) : ( lpimin - rpimin + gcdz );
#endif

                    models::EventGraphVertex target_event = g->getEventGraphVertex(target_id,pj,kj);
                    VERBOSE_DEBUG("  stepa=" << stepa);
                    VERBOSE_DEBUG("  ki=" << ki<<" kj=" << kj << " (" <<  source_event  << "," << target_event  << ")");
                    VERBOSE_DEBUG("  alphamin=" << alphamin <<"   alphamax=" << alphamax );
                    if (alphamin <= alphamax) {


                        TIME_UNIT w = ((TIME_UNIT) alphamax * source_phase_count * maxki ) / ( (TIME_UNIT) Wc  * (TIME_UNIT) dataflow->getNi(source) );
                        VERBOSE_DEBUG("   w = (" << alphamax << " * " << dataflow->getPhasesQuantity(source) * maxki << ") / (" << Wc << " * " << dataflow->getNi(source) / maxki << ")");
                        VERBOSE_DEBUG("   d = (" << d << ")");

                        if (doBufferLessEdges) {
                            g->addEventConstraint(target_event ,source_event,0,-d,id);
                            g->addEventConstraint(source_event ,target_event,0,d,id);
                        } else {
                            g->addEventConstraint(source_event ,target_event,-w,d,id);
                        }
                    }
                }
            }
            normdapkm1 = normdapk;
        }
    }
}


void algorithms::generateKperiodicSelfloop(models::Dataflow * const dataflow , EXEC_COUNT ki,  models::EventGraph* g , Vertex t  ) {

    const ARRAY_INDEX task_id = dataflow->getVertexId(t);

    const TIME_UNIT timefactor = (dataflow->getReentrancyFactor(t) <= 0)?0:1;

    VERBOSE_DEBUG("generate reentrancy loops for task " <<  dataflow->getVertexId(t) << " with ki=" << ki);


    models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,1,1);
    models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,1,1);
    EXEC_COUNT pq = dataflow->getPhasesQuantity(t);
    TIME_UNIT lti = timefactor * dataflow->getVertexDuration(t,1);
    EXEC_COUNT i = 1;
    EXEC_COUNT p = 1;
    while (i <= ki) {
        p += 1;
        if (p > pq) {p = 1; i += 1;}
        if (i > ki) continue;

        target_event = g->getEventGraphVertex(task_id,p,i);

        TIME_UNIT d =  lti;
        TIME_UNIT w =  0;
        g->addEventConstraint(source_event ,target_event,w,d,0);

        source_event = target_event;
        lti = timefactor * dataflow->getVertexDuration(t,p);
    }






    if (dataflow->getReentrancyFactor(t) > 0) // consider only a factor 1
    {

        // constraintes last_k --> 1

        EXEC_COUNT p = dataflow->getPhasesQuantity(t);
        EXEC_COUNT i = ki;

        EXEC_COUNT np = 1;
        EXEC_COUNT ni = 1;

        const TIME_UNIT lti = timefactor * dataflow->getVertexDuration(t,p);

        models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,p,i);
        models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,np,ni);


        const TIME_UNIT       d     =  lti;
        const TIME_UNIT       w     =   -  (TIME_UNIT) (p * i) / (  (TIME_UNIT) dataflow->getNi(t)  );


        VERBOSE_DEBUG("add task constraint :" << dataflow->getVertexName(t) << " : " <<   target_event  << " --> " <<  source_event << " : " <<  " (- " << w << " / " << dataflow->getNi(t)  << ") ," << d );
        g->addEventConstraint(source_event ,target_event,-w,d,0);
    }



}




void algorithms::updateVectorWithFullNi(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , std::set<Edge> * cc)  {

    EXEC_COUNT before = 0;
    EXEC_COUNT after = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex source = dataflow->getEdgeSource(*it);
        Vertex target = dataflow->getEdgeTarget(*it);
        before += kvector->at(source) * kvector->at(target);
        kvector->at(source) = dataflow->getNi(source);
        after += dataflow->getNi(source) * dataflow->getNi(target);
    }

    VERBOSE_INFO("   Up complexity from " << before << " to " << after);

}

bool algorithms::updateVectorWithLocalNi(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , std::set<Edge> * cc)  {

    bool changed = false;

    EXEC_COUNT gcdNi = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex source = dataflow->getEdgeSource(*it);
        gcdNi = boost::math::gcd(gcdNi,dataflow->getNi(source) );
    }

    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex source = dataflow->getEdgeSource(*it);
        Vertex target = dataflow->getEdgeTarget(*it);

        EXEC_COUNT Ni =  dataflow->getNi(source);
        EXEC_COUNT Nj =  dataflow->getNi(target);

        EXEC_COUNT newki = boost::math::lcm( kvector->at(source),  Ni / gcdNi);

        if (newki != kvector->at(source) ) changed = true;

        kvector->at(source) = newki;
        VERBOSE_ASSERT(kvector->at(source) <= Ni , "Bad idea");
        VERBOSE_ASSERT(kvector->at(target) <= Nj , "Bad idea");
    }

    return changed;

}

void algorithms::compute_NKperiodic_throughput            (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    // STEP 0.2 - Assert SDF
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = dataflow->getNi(t);
    }}

    kperiodic_result_t result = KSchedule(dataflow,&kvector);

    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;


}



void algorithms::compute_2Kperiodic_throughput            (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0.2 - Assert SDF
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 2;
    }}

    kperiodic_result_t result = KSchedule(dataflow,&kvector);
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;


}


void algorithms::compute_1Kperiodic_throughput            (models::Dataflow* const  dataflow, parameters_list_t param_list) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    bool printRequired = (param_list.count("print") == 1);

    VERBOSE_INFO("Please note you cna use the print parameter.");

    // STEP 0.2 - Assert SDF
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}

    kperiodic_result_t result = KSchedule(dataflow,&kvector);

    if (printRequired) {
    	print_function    ( dataflow, kvector , result.first , false,false,true);
    } else {
    	TIME_UNIT res = result.first;
    	std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    	std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;
    }

}



std::map<Vertex,EXEC_COUNT> algorithms::get_Kvector(models::Dataflow *  const dataflow ) {

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    EXEC_COUNT iteration_count = 0;

    // STEP 1 - generate initial vector
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}


    kperiodic_result_t result;



    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }

    TIME_UNIT frequency = howard_res.first;

    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

    result.first = frequency;

    ////////////// SCHEDULE CALL // END

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            ////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

            kperiodic_result_t resultprime;

            //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
            VERBOSE_INFO("KPeriodic EventGraph generation");

            //STEP 1 - Generate Event Graph and update vector
            if (!updateEventGraph( dataflow ,  &kvector, &(result.second), eg)) break ;

            VERBOSE_INFO("KPeriodic EventGraph generation Done");

            //STEP 2 - resolve the MCRP on this Event Graph
            std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();

            std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

            //STEP 3 - convert CC(eg) => CC(graph)
            VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
            for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
                VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
                ARRAY_INDEX channel_id = eg->getChannelId(*it);
                try {
                    Edge        channel    = dataflow->getEdgeById(channel_id);
                    resultprime.second.insert(channel);
                } catch(...) {
                    VERBOSE_DEBUG("      is loopback");
                }
            }

            TIME_UNIT frequency = howard_res_bis.first;

            VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
            VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

            resultprime.first = frequency;

            ////////////// SCHEDULE CALL // END

            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;

                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    }


    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");
    delete eg;

    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    return kvector;
}

void algorithms::compute_KperiodicSlow_throughput    (models::Dataflow* const dataflow, parameters_list_t) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    kperiodic_result_t result = KSchedule(dataflow,&kvector);

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            updateVectorWithLocalNi(dataflow,&kvector,&(result.second));
            kperiodic_result_t resultprime = KSchedule(dataflow,&kvector);
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;



}

/**
 *
 * Compute K-periodic schedule by using an update module
 *
 */

//std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> 

scheduling_t algorithms::generateKperiodicSchedule    (models::Dataflow* const dataflow , bool verbose) {

	//std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>>
	scheduling_t scheduling_result;

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");


    EXEC_COUNT sumNi = 0;
    EXEC_COUNT sumKi = dataflow->getVerticesCount();

    {ForEachTask(dataflow,t) {
        sumNi += dataflow->getNi(t) ;
    }}

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    EXEC_COUNT iteration_count = 0;

    // STEP 1 - generate initial vector
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}


    kperiodic_result_t result;



    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }

    TIME_UNIT frequency = howard_res.first;

    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

    result.first = frequency;

    ////////////// SCHEDULE CALL // END


    if (result.second.size() != 0) {


        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {



            iteration_count++;
            ////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

            kperiodic_result_t resultprime;

            //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
            VERBOSE_INFO("KPeriodic EventGraph generation");

            //STEP 1 - Generate Event Graph and update vector
            if (!updateEventGraph( dataflow ,  &kvector, &(result.second), eg, verbose)) break ;

            VERBOSE_INFO("KPeriodic EventGraph generation Done");

            //STEP 2 - resolve the MCRP on this Event Graph
            std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();

            std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

            //STEP 3 - convert CC(eg) => CC(graph)
            VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
            for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
                VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
                ARRAY_INDEX channel_id = eg->getChannelId(*it);
                try {
                    Edge        channel    = dataflow->getEdgeById(channel_id);
                    resultprime.second.insert(channel);
                } catch(...) {
                    VERBOSE_DEBUG("      is loopback");
                }
            }

            TIME_UNIT frequency = howard_res_bis.first;

            VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
            VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

            resultprime.first = frequency;

            ////////////// SCHEDULE CALL // END

            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;

                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");



        }

    }


    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");


    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << sumKi << " total_ni=" << sumNi );

    TIME_UNIT res = result.first;

    eg->computeStartingTime (res);
    TIME_UNIT omega = 1 / res ;


    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT period = kvector[v] *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
        scheduling_result[dataflow->getVertexId(v)].first = period;

        VERBOSE_INFO("Task " << dataflow->getVertexName(v) << " " << period  );

    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        scheduling_result[dataflow->getVertexId(v)].second.push_back( start );
        VERBOSE_INFO("Task " << dataflow->getVertexName(v) << " " << start  );


    }}



    return scheduling_result ;


}


void algorithms::compute_Kperiodic_throughput    (models::Dataflow* const dataflow, parameters_list_t  parameters  ) {


	  bool do_buffer_less =    (parameters.find("do_buffer_less") != parameters.end() ) ;

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    bool verbose = false;
    if (parameters.find("PRINT") != parameters.end() ) {
        verbose = true;
    }
    VERBOSE_INFO("Please note you can use the PRINT parameter");

    EXEC_COUNT sumNi = 0;
    EXEC_COUNT sumKi = dataflow->getVerticesCount();

    {ForEachTask(dataflow,t) {
        sumNi += dataflow->getNi(t) ;
    }}

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    EXEC_COUNT iteration_count = 0;

    // STEP 1 - generate initial vector
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}


    kperiodic_result_t result;



    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector,do_buffer_less);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }

    TIME_UNIT frequency = howard_res.first;

    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

    result.first = frequency;

    ////////////// SCHEDULE CALL // END


    if (verbose) {
        std::cout << "Iteration "<< std::fixed << std::setw( 4 ) << iteration_count <<  "      period = "  << std::fixed << std::setw( 15 ) << std::setprecision( 2 ) << 1.0/result.first  <<  "      complexity = "  << std::setw( 4 )  << (sumKi * 100) / sumNi << std::endl ;
    }

    if (result.second.size() != 0) {


        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {



            iteration_count++;
            ////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

            kperiodic_result_t resultprime;

            //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
            VERBOSE_INFO("KPeriodic EventGraph generation");

            //STEP 1 - Generate Event Graph and update vector
            if (!updateEventGraph( dataflow ,  &kvector, &(result.second), eg, verbose)) break ;

            VERBOSE_INFO("KPeriodic EventGraph generation Done");

            //STEP 2 - resolve the MCRP on this Event Graph
            std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();

            std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

            //STEP 3 - convert CC(eg) => CC(graph)
            VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
            for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
                VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
                ARRAY_INDEX channel_id = eg->getChannelId(*it);
                try {
                    Edge        channel    = dataflow->getEdgeById(channel_id);
                    resultprime.second.insert(channel);
                } catch(...) {
                    VERBOSE_DEBUG("      is loopback");
                }
            }

            TIME_UNIT frequency = howard_res_bis.first;

            VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
            VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

            resultprime.first = frequency;

            ////////////// SCHEDULE CALL // END

            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;

                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");



            if (verbose) {
                sumKi = 0;
                {ForEachVertex(dataflow,t) {
                    sumKi += kvector[t];
                }}

                std::cout << "Iteration "<< std::fixed << std::setw( 4 ) << iteration_count <<  "      period = "  << std::fixed << std::setw( 15 ) << std::setprecision( 2 ) << 1.0/result.first  <<  "      complexity = "  << std::setw( 4 )  << (sumKi * 100) / sumNi << std::endl ;
            }

        }

    }


    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");
    delete eg;

    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ki += kvector[t];
    }}
    sumKi = 0;
    {ForEachVertex(dataflow,t) {
    	sumKi += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << sumKi << " total_ni=" << sumNi );

    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

    if (verbose) {
    	scheduling_t persched = period2scheduling    (dataflow,  kvector , res);
    	for (auto  key : persched) {
    			auto task = key.first;
    			auto task_vtx = dataflow->getVertexById(key.first);
    			std::cout <<  "Task " <<  dataflow->getVertexName(task_vtx)
    					<<  " : duration=" << dataflow->getVertexTotalDuration(task_vtx)
						<<  " period=" <<  persched[task].first
						<<  " Ni=" << dataflow->getNi(task_vtx)
    					<<  " starts=[ " << commons::toString(persched[task].second) << "]" << std::endl;

    		}

    }


}




/**
 *
 * Compute K-periodic schedule by using an update module
 *
 */
void algorithms::compute_KperiodicSlow2_throughput    (models::Dataflow* const dataflow, parameters_list_t) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    kperiodic_result_t result = KSchedule(dataflow,&kvector);

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            updateVectorWithFineNi(dataflow,&kvector,&(result.second));
	    {ForEachVertex(dataflow,v) {
		VERBOSE_INFO("New vector " << dataflow->getVertexName(v) << " = " << kvector[v]  << "( Ni=" << dataflow->getNi(v) << ")" );
	      }}
	    kperiodic_result_t resultprime = KSchedule(dataflow,&kvector);
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );

    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

}

EXEC_COUNT algorithms::test_Kperiodic_throughput    (models::Dataflow* const dataflow, parameters_list_t) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    kperiodic_result_t result = KSchedule(dataflow,&kvector);

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            updateVectorWithLocalNi(dataflow,&kvector,&(result.second));
            kperiodic_result_t resultprime = KSchedule(dataflow,&kvector);
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

    return iteration_count;

}
