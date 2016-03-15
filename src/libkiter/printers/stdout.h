/*
 * stdout.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef STDOUT_H_
#define STDOUT_H_


#include <commons/commons.h>
namespace models {
	class Dataflow;
}

namespace printers {
void printGraph         (models::Dataflow* const  dataflow, parameters_list_t = std::map<std::string,std::string>());
void printInfos         (models::Dataflow* const  dataflow, parameters_list_t = std::map<std::string,std::string>());
}

#endif /* STDOUT_H_ */
