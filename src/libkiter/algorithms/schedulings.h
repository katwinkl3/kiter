/*
 * schedulings.h
 *
 *  Created on: Aug 23, 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_

#include <map>
#include <models/Dataflow.h>


struct DelayConstraint {
	Vertex src;
	Vertex dst;
	TIME_UNIT delay;
};


void sdf_bufferless_scheduling (models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector, std::vector<std::vector <Vertex> > task_sequences);
void bufferless_scheduling (models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector, std::vector< DelayConstraint > task_sequences);

namespace algorithms {
	namespace scheduling {
		void KPeriodic_scheduling_bufferless (models::Dataflow* const  dataflow,   parameters_list_t   param_list);
	}
}




#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_ */
