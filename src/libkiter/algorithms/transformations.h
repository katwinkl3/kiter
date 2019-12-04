/*
 * transformations.h
 *
 *  Created on: Aug 21, 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_TRANSFORMATIONS_H_
#define SRC_LIBKITER_ALGORITHMS_TRANSFORMATIONS_H_



namespace models {
	class Dataflow;
}
std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > & returnValue) ;

namespace algorithms {
	namespace transformation {
		models::Dataflow* modelNoCMapping(models::Dataflow* const  dataflow) ;


		void merge_tasks    (models::Dataflow* const dataflow, parameters_list_t  parameters  );


		void compare_sdf3_throughput (models::Dataflow* const  dataflow, parameters_list_t) ;

		void remove_task (models::Dataflow* const  dataflow, parameters_list_t params);


	}
}

#endif /* SRC_LIBKITER_ALGORITHMS_TRANSFORMATIONS_H_ */
