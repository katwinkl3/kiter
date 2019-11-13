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

#endif /* SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_MERGING_H_ */
