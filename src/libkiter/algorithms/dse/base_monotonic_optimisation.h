/*
 * monotonic_optimisation.h
 *
 *  Created on: Sep 28, 2020
 *      Author: jkmingwen
 */

#ifndef SRC_LIBKITER_ALGORITHMS_DSE_BASE_MONOTONIC_OPTIMISATION_H_
#define SRC_LIBKITER_ALGORITHMS_DSE_BASE_MONOTONIC_OPTIMISATION_H_

#include <commons/KiterRegistry.h>
#include <algorithms/schedulings.h>
#include <algorithms/throughput/kperiodic.h>

namespace models {
	class Dataflow;
}

namespace algorithms {
  StorageDistribution select(models::Dataflow* const dataflow,
                             StorageDistributionSet infeasibleSet,
                             StorageDistributionSet kneeSet,
                             StorageDistributionSet feasibleSet,
                             bool &foundPoint,
                             bool isSat,
                             TOKEN_UNIT &mult,
                             StorageDistribution &kMinPoint);
  StorageDistributionSet base_monotonic_optimised_Kperiodic_throughput_dse(models::Dataflow* const dataflow,
                                                                           StorageDistribution initDist,
                                                                           TIME_UNIT targetThr,
                                                                           long int &computation_counter,
                                                                           parameters_list_t params);
        
}

// ADD_TRANSFORMATION(MonotonicOptimisation,
// 		transformation_t({"MonotonicOptimisation", "Use monotonic optimisation to reduce search space of buffer sizing and throughput exploration, using K-periodic scheduling method", algorithms::monotonic_optimised_Kperiodic_throughput_dse}));
#endif /* SRC_LIBKITER_ALGORITHMS_DSE_BASE_MONOTONIC_OPTIMISATION_H_ */
