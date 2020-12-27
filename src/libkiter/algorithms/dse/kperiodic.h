/*
 * kperiodic.h
 *
 *  Created on: Aug 17, 2020
 *      Author: jkmingwen
 */

#ifndef SRC_LIBKITER_ALGORITHMS_DSE_KPERIODIC_H_
#define SRC_LIBKITER_ALGORITHMS_DSE_KPERIODIC_H_

#include <commons/KiterRegistry.h>
#include <algorithms/schedulings.h>
#include <algorithms/throughput/kperiodic.h>

namespace models {
	class Dataflow;
}

namespace algorithms {
        kperiodic_result_t compute_Kperiodic_throughput_and_cycles(models::Dataflow* const dataflow, parameters_list_t params);
        void compute_Kperiodic_throughput_dse (models::Dataflow* const  dataflow, parameters_list_t params);
}

ADD_TRANSFORMATION(KPeriodicThroughputwithDSE,
		transformation_t({"KPeriodicThroughputwithDSE", "Combine buffer space exploration with throughput evaluation of CSDF by K-periodic scheduling method", algorithms::compute_Kperiodic_throughput_dse}));
#endif /* SRC_LIBKITER_ALGORITHMS_DSE_KPERIODIC_H_ */
