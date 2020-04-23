/*
 * merging.h
 *
 *  Created on: Nov 13, 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_MERGING_H_
#define SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_MERGING_H_

#include <string>
#include <vector>
#include <commons/commons.h>
#include <commons/KiterRegistry.h>


// ./Debug/bin/kiter  -f ./sdf3mem/example.xml  -a Merge -ptasks=a,b -pname=new

namespace models {
	class Dataflow;
	class Scheduling;
}

namespace algorithms {
	namespace transformation {
		bool mergeCSDFFromKperiodicSchedule(models::Dataflow* to, std::string name , std::vector< ARRAY_INDEX >& mergeNodes);
		bool mergeCSDFFromSchedule(models::Dataflow* to, std::string name , const std::vector< ARRAY_INDEX >& mergeNodes, const models::Scheduling* scheduling_res);
		void merge_tasks    (models::Dataflow* const dataflow, parameters_list_t  parameters  );
	}
}

ADD_TRANSFORMATION(Merge,
		transformation_t({ "Merge" , "Merging tasks using Kperiodic scheduling and Initialization phases", algorithms::transformation::merge_tasks}));
#endif /* SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_MERGING_H_ */
