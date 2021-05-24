/*
 * periodic.h
 *
 *  Created on: Apr 9, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_
#define SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_

#include <commons/KiterRegistry.h>
#include <commons/basic_types.h>
#include <algorithms/buffersizing.h>
#include <functional>

namespace models {
	class Dataflow;
}

namespace algorithms {
	void compute_csdf_dse_periodic        (models::Dataflow* const  dataflow, parameters_list_t params) ;
	void compute_csdf_dse_speriodic       (models::Dataflow* const  dataflow, parameters_list_t params) ;
	void compute_csdf_dse_from_function   (models::Dataflow* const  dataflow, const BufferSizingFun& sizing_fun, std::ostream& dseLog) ;
}

ADD_TRANSFORMATION(PeriodicDSE,
		transformation_t({ "PeriodicDSE" , "Bodin2013 Periodic DSE", algorithms::compute_csdf_dse_periodic}));
ADD_TRANSFORMATION(SPeriodicDSE,
		transformation_t({ "SPeriodicDSE" , "Experimental Strictly Periodic DSE", algorithms::compute_csdf_dse_speriodic}));
#endif /* SRC_LIBKITER_ALGORITHMS_DSE_PERIODIC_H_ */
