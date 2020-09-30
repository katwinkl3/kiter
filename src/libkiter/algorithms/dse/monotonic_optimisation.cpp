/*
 * monotonic_optimisation.cpp
 *
 * Created on: Sep 28, 2020
 * Author: jkmingwen
 */

#include <map>
#include <vector>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <printers/SDF3Wrapper.h> // to write XML files
#include <commons/commons.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
// #include <algorithms/throughput/kperiodic.h>
#include <models/repetition_vector.h>
#include "buffer_sizing.h"
#include "monotonic_optimisation.h"
#include <chrono> // to take computation timings

// TODO update set of infeasible SDs (U)
//      inputs: set of infeasible SDs (U), new point (x)
//      behaviour: update max SDs of U, given x

// TODO knee point updating function
//      inputs: set of infeasible SDs (U), set of knee points (K), new point (x)
//      behaviour: if x is a knee point, update K to include K and remove whichever knee points are no longer in K

// TODO handle infeasible function
//      inputs: U, K, x, dependencies
//      behaviour: - return error if no dependencies,
//                 - expand U according to channels without dependencies in x
//                 - call update knee point function
//                 - call update infeasible SDs function

void algorithms::monotonic_optimised_Kperiodic_throughput_dse(models::Dataflow* const dataflow,
                                                              parameters_list_t parameters) {
  
  // graph to model bounded channel quantities
  models::Dataflow* dataflow_prime = new models::Dataflow(*dataflow);
  long int computation_counter = 0;
  
  // create channels in new graph to model bounded channel quantities
  {ForEachEdge(dataflow, c) {
      auto new_edge = dataflow_prime->addEdge(dataflow_prime->getEdgeTarget(c),
                                              dataflow_prime->getEdgeSource(c));
      dataflow_prime->setEdgeInPhases(new_edge,
                                      dataflow_prime->getEdgeOutVector(c));
      dataflow_prime->setEdgeOutPhases(new_edge,
                                       dataflow_prime->getEdgeInVector(c));
      dataflow_prime->setPreload(new_edge, dataflow->getPreload(c));
      dataflow_prime->setEdgeName(new_edge,
                                  dataflow_prime->getEdgeName(c) + "_prime");
    }}

  // initialise search parameters
  std::map<Edge, TOKEN_UNIT> minStepSizes;
  std::map<Edge, std::pair<TOKEN_UNIT, TOKEN_UNIT>> minChannelSizes;
  TOKEN_UNIT minDistributionSize;
  
  initSearchParameters(dataflow_prime,
                       minStepSizes,
                       minChannelSizes);

  // set lower bound channel quantities
  {ForEachEdge(dataflow_prime, c) {
      if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
        minChannelSizes[c].second = 0; // original edges must have capacity 0
      }
      minChannelSizes[c].first = dataflow_prime->getPreload(c);
    }}

  minDistributionSize = findMinimumDistributionSz(dataflow_prime,
                                                  minChannelSizes);

  // initialise and store initial storage distribution state
  StorageDistribution initDist(dataflow_prime->getEdgesCount(),
                               0,
                               minChannelSizes,
                               minDistributionSize);
  
  // initialise modelled graph with lower bound distribution
  {ForEachEdge(dataflow_prime, c) {
      if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) { // original channels
        dataflow_prime->setPreload(c, initDist.getInitialTokens(c));
      } else {
        // subtract initial tokens from buffer size to model any initial tokens in buffer
        dataflow_prime->setPreload(c, (initDist.getChannelQuantity(c) -
                                       initDist.getInitialTokens(c)));
      }
    }}
}
