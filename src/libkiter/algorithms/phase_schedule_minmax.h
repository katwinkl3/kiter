/*
 * phase_schedule_minmax.h
 *
 *  Created on: 17 juin 2013
 *      Author: toky
 */

#ifndef PHASE_SCHEDULE_MINMAX_H_
#define PHASE_SCHEDULE_MINMAX_H_


#include <models/Dataflow.h>

namespace algorithms {
   bool PhaseScheduleMinMax_Algo(models::Dataflow * dataflow,Vertex t, std::vector<TIME_UNIT>& res);
}


#endif /* PHASE_SCHEDULE_MINMAX_H_ */
