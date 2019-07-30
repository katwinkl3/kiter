/*
 * backpressure.h
 *
 *  Created on: 25 avr. 2013
 *      Author: toky
 */

#ifndef BACKPRESSURE_H_
#define BACKPRESSURE_H_

#include <commons/commons.h>

namespace models {
    class EventGraph;
}
namespace algorithms {
std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> back_pressure_start_timing (models::Dataflow* const  dataflow, TIME_UNIT PERIOD);
    void compute_backpressure_memory_sizing (models::Dataflow* const  dataflow, parameters_list_t);
}


#endif /* BACKPRESSURE_H_ */
