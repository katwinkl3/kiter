/*
 * SDF3Wrapper.h
 *
 *  Created on: 15 mars 2012
 *      Author: toky
 */

#ifndef SDF3WRAPPER_H_
#define SDF3WRAPPER_H_

#include <string>
#include <commons/basic_types.h>

namespace models {
	class Dataflow;
}

namespace commons {

models::Dataflow*     readSDF3File          (const std::string f) ;
void                  writeSDF3File         (const std::string f, const models::Dataflow* ) ;
TIME_UNIT			  runSDF3Throughput(models::Dataflow* const  dataflow, std::string SDF3_binary = "./sdf3/sdf3/build/release/Linux/bin/sdf3analysis-csdf");

} // end of namespace commons

#endif /* SDF3WRAPPER_H_ */
