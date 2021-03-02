/*
 * csdf_strictly_periodic.h
 *
 *  Created on: Feb 22, 2021
 *      Author: toky
 */


#include <commons/KiterRegistry.h>
#include <models/Dataflow.h>
#include <map>

namespace models {
	class EventGraph;
}

namespace algorithms {
	models::EventGraph* generate_csdf_strictly_periodic_event_graph(const models::Dataflow * const dataflow);
	void compute_SPeriodic_throughput    (models::Dataflow*  dataflow, parameters_list_t );

}


ADD_TRANSFORMATION(SPeriodicThroughput,
		transformation_t({ "SPeriodicThroughput" , "Experimental scheduling technique", algorithms::compute_SPeriodic_throughput}));
