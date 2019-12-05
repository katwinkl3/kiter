/*
 * stdout.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef STDOUT_H_
#define STDOUT_H_


#include <commons/commons.h>
#include <string>
#include <models/Dataflow.h>
#include <models/Scheduling.h>


namespace printers {
std::string GenerateDOT       (models::Dataflow* const  dataflow  , bool simple = true);
std::string PeriodicScheduling2DOT    (models::Dataflow* const  dataflow, models::Scheduling& periodic_scheduling,   TIME_UNIT last_execution_end_at,  bool full , double xscale , double yscale );
void printGraph         (models::Dataflow* const  dataflow, parameters_list_t = std::map<std::string,std::string>());
void printGraphAsKiterScript (models::Dataflow* const  dataflow, parameters_list_t = std::map<std::string,std::string>());
void printInfos         (models::Dataflow* const  dataflow, parameters_list_t = std::map<std::string,std::string>());
void printXML         (models::Dataflow* const  dataflow, parameters_list_t = std::map<std::string,std::string>());
}

#endif /* STDOUT_H_ */
