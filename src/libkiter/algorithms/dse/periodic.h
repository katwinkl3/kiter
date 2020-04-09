/*
 * periodic.h
 *
 *  Created on: Apr 9, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_
#define SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_

#include <commons/commons.h>

namespace models {
	class Dataflow;
}

namespace algorithms {
	void compute_csdf_dse_periodic   (models::Dataflow* const  dataflow, parameters_list_t params) ;
}

#endif /* SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_ */
