/*
 * periodic.h
 *
 *  Created on: Apr 9, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_
#define SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_

#include <commons/KiterRegistry.h>

namespace models {
	class Dataflow;
}

namespace algorithms {
	void compute_csdf_dse_periodic   (models::Dataflow* const  dataflow, parameters_list_t params) ;
}

ADD_TRANSFORMATION(PeriodicDSE,
		transformation_t({ "PeriodicDSE" , "Bodin2013 Periodic DSE", algorithms::compute_csdf_dse_periodic}));
#endif /* SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_ */
