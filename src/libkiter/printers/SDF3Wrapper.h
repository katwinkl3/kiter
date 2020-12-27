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

namespace printers {

std::string           generateSDF3XML       (const models::Dataflow* dataflow)  ;
models::Dataflow*     parseSDF3XML          (const std::string data) ;
models::Dataflow*     readSDF3File          (const std::string f) ;
void                  writeSDF3File         (const std::string f, const models::Dataflow* ) ;

} // end of namespace commons

#endif /* SDF3WRAPPER_H_ */
