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


typedef std::map<ARRAY_INDEX,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> scheduling_t;
//typedef std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> scheduling_t;

void sdf_bufferless_scheduling (models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector, std::vector<std::vector <Vertex> > task_sequences);
void bufferless_scheduling (models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector);

namespace algorithms {
	namespace scheduling {
		void KPeriodic_taskNoCbufferless(models::Dataflow*, parameters_list_t param_list);
		scheduling_t bufferless_scheduling(models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector);
		scheduling_t bufferless_kperiodic_scheduling(models::Dataflow* const  dataflow);
	}
}




#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_ */
