/*
 * SDF3Wrapper.h
 *
 *  Created on: 15 mars 2012
 *      Author: toky
 */

#ifndef SDF3WRAPPER_H_
#define SDF3WRAPPER_H_

#include <string>

namespace models {
	class Dataflow;
}

namespace commons {

models::Dataflow*  readSDF3File          (const std::string f) ;
std::string        writeSDF3File         (const models::Dataflow* f) ;

} // end of namespace commons

#endif /* SDF3WRAPPER_H_ */
