/*
 * nperiodic.h
 *
 *  Created on: 5 juil. 2013
 *      Author: toky
 */

#ifndef NPERIODIC_H_
#define NPERIODIC_H_

namespace algorithms{

	models::EventGraph* 						generateNPeriodicEventGraph         (models::Dataflow *  const) ;
	std::pair<TIME_UNIT, std::vector<Edge> > 	NSchedule							(models::Dataflow *  const dataflow ) ;
    void                						compute_NPeriodic_throughput  	    (models::Dataflow* const  dataflow, parameters_list_t);
    void                						compute_NCleanPeriodic_throughput  	(models::Dataflow* const  dataflow, parameters_list_t);



    void KPeriodic_memory   (models::Dataflow* const  dataflow,  std::map<Vertex,EXEC_COUNT>& kvector, parameters_list_t params) ;

    void compute_1Periodic_memory   (models::Dataflow* const  dataflow, parameters_list_t params) ;
    void compute_KPeriodic_memory   (models::Dataflow* const  dataflow, parameters_list_t params) ;
    void compute_NPeriodic_memory   (models::Dataflow* const  dataflow, parameters_list_t params) ;


    void                                        print_Nperiodic_eventgraph       	(models::Dataflow* const  dataflow, parameters_list_t);
}

#endif /* NPERIODIC_H_ */
