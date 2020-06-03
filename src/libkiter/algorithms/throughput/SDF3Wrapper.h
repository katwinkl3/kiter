/*
 * sdf3wrapper.h
 *
 *  Created on: Jun 3, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_THROUGHPUT_SDF3WRAPPER_H_
#define SRC_LIBKITER_ALGORITHMS_THROUGHPUT_SDF3WRAPPER_H_

#include <commons/commons.h>

namespace models {
class Dataflow;
}
namespace algorithms {
	TIME_UNIT			  runSDF3Throughput(models::Dataflow* const  dataflow, std::string SDF3_binary = "./sdf3/sdf3/build/release/Linux/bin/sdf3analysis-csdf");
}


#endif /* SRC_LIBKITER_ALGORITHMS_THROUGHPUT_SDF3WRAPPER_H_ */
