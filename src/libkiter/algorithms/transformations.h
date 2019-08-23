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
	}
}

#endif /* SRC_LIBKITER_ALGORITHMS_TRANSFORMATIONS_H_ */
