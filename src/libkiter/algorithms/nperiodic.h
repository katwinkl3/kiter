/*
 * nperiodic.h
 *
 *  Created on: 5 juil. 2013
 *      Author: toky
 */

#ifndef NPERIODIC_H_
#define NPERIODIC_H_

#include <commons/KiterRegistry.h>

namespace models {
	class EventGraph;
	class Dataflow;
}

namespace algorithms {

    std::map<Vertex,EXEC_COUNT>  get_Kvector(models::Dataflow *  const);
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
ADD_TRANSFORMATION(UnPeriodicSizing,
		transformation_t({ "1PeriodicSizing" , "Minimal Buffer size estimation by periodic scheduling method.", algorithms::compute_1Periodic_memory}));
ADD_TRANSFORMATION(NPeriodicSizing,
		transformation_t({ "NPeriodicSizing" , "Optimal Sizing evaluation of SDF by using N-Periodic method.", algorithms::compute_NPeriodic_memory}));
ADD_TRANSFORMATION(KPeriodicSizing,
		transformation_t({ "KPeriodicSizing" , "Optimal Sizing evaluation of SDF by using K-Periodic method.", algorithms::compute_KPeriodic_memory}));
ADD_TRANSFORMATION(NCleanPeriodicThroughput,
		transformation_t({ "NCleanPeriodicThroughput" , "Optimal Throughput evaluation of SDF by using Munier1993 method combined with deGroote2012 reduction.", algorithms::compute_NCleanPeriodic_throughput}));

ADD_TRANSFORMATION(NPeriodicThroughput,
		transformation_t({ "NPeriodicThroughput" , "Optimal Throughput evaluation of SDF by using Munier1993 method.", algorithms::compute_NPeriodic_throughput}));

#endif /* NPERIODIC_H_ */
