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
#include "kperiodic.h"
#include <string>
#include <chrono> // to take computation timings

// TODO handle infeasible function
//      inputs: U, K, x, dependencies
//      behaviour: - return error if no dependencies,
//                 - expand U according to channels without dependencies in x
//                 - call update knee point function
//                 - call update infeasible SDs function

StorageDistribution algorithms::selectNewSD(models::Dataflow* const dataflow,
                                            StorageDistributionSet infeasibleSet,
                                            StorageDistributionSet kneeSet,
                                            StorageDistributionSet feasibleSet,
                                            TIME_UNIT throughput) {
  // pick smallest knee point and feasible point (they're stored in ascending order)
  StorageDistribution kMinPoint(kneeSet.getNextDistribution());
  StorageDistribution sMinPoint(feasibleSet.getNextDistribution());
  StorageDistribution midPoint(kMinPoint);
  {ForEachEdge(dataflow, c) {
            TOKEN_UNIT newBufferSize = kMinPoint.getChannelQuantity(c) +
              ((sMinPoint.getChannelQuantity(c) - kMinPoint.getChannelQuantity(c)) / 2);
            midPoint.setChannelQuantity(c, newBufferSize);
    }}
  return midPoint;
}

void algorithms::monotonic_optimised_Kperiodic_throughput_dse(models::Dataflow* const dataflow,
                                                              parameters_list_t parameters) {
  // get target throughput
  bool thrTargetSpecified = false;
  TIME_UNIT thrTarget;
  if (parameters.find("THR") != parameters.end()) {
    thrTargetSpecified = true;
    thrTarget = std::stold(parameters.find("THR")->second);
    std::cout << "Target throughput set to " << thrTarget << std::endl;
  } else {
    std::cout << "Specify target throughput with '-p THR=n' flag" << std::endl;
  }
  
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

  // // set lower bound channel quantities
  {ForEachEdge(dataflow_prime, c) {
      if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
        minChannelSizes[c].second = 0; // original edges must have capacity 0
      }
      minChannelSizes[c].first = dataflow_prime->getPreload(c);
    }}

  minDistributionSize = findMinimumDistributionSz(dataflow_prime,
                                                  minChannelSizes);

  // // initialise and store initial storage distribution state
  StorageDistribution initDist(dataflow_prime->getEdgesCount(),
                               0,
                               minChannelSizes,
                               minDistributionSize);
  
  // // initialise modelled graph with lower bound distribution
  {ForEachEdge(dataflow_prime, c) {
      if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) { // original channels
        dataflow_prime->setPreload(c, initDist.getInitialTokens(c));
      } else {
        // subtract initial tokens from buffer size to model any initial tokens in buffer
        dataflow_prime->setPreload(c, (initDist.getChannelQuantity(c) -
                                       initDist.getInitialTokens(c)));
      }
    }}

  // // // calculate max throughput and current throughput with lower bound distribution
  kperiodic_result_t result_max = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
  VERBOSE_DSE("Max throughput: " << result_max.throughput << std::endl);
  if (thrTarget > result_max.throughput) {
    std::cerr << "ERROR: Specified target throughput (" << thrTarget
              << ") is larger than maximum throughput (" << result_max.throughput
              << ")" << std::endl;
  }
  kperiodic_result_t result = compute_Kperiodic_throughput_and_cycles(dataflow_prime, parameters);

  // /* a negative throughput indicates a deadlocked graph and so 
  //    we set it to 0 to avoid bugs from having a negative throughput */
  if (result.throughput < 0) {
    initDist.setThroughput(0);
  } else { // set throughput to one computed from initial distribution otherwise
    initDist.setThroughput(result.throughput);
  }

  // add initial distribution to list of storage distributions
  // StorageDistributionSet checkList(initDist.getDistributionSize(),
  //                                  initDist);
  TIME_UNIT thrCurrent = initDist.getThroughput();
  StorageDistribution newDist(initDist);
  StorageDistributionSet infeasibleSet;
  StorageDistributionSet kneeSet;
  StorageDistributionSet feasibleSet;
  while (thrCurrent < thrTarget) {
    handleInfeasiblePoint(dataflow_prime, infeasibleSet, kneeSet, newDist, result);
    VERBOSE_DSE("Current infeasible set:\n" << infeasibleSet.printDistributions(dataflow_prime)
                << std::endl);
    VERBOSE_DSE("Current knee set:\n" << kneeSet.printDistributions(dataflow_prime)
                << std::endl);
    for (std::set<Edge>::iterator it = (result.critical_edges).begin();
         it != (result.critical_edges).end(); it++) {
      // only increase channel quantity on "modelled" channels
      if (dataflow_prime->getEdgeId(*it) > dataflow->getEdgesCount()) {
        VERBOSE_DSE("\tFound storage dependency in channel "
                    << dataflow_prime->getEdgeName(*it) << std::endl);
        // double new modelled storage distribution channels with storage dependencies
        newDist.setChannelQuantity(*it, (newDist.getChannelQuantity(*it) * 2));
        VERBOSE_DSE("\t\tIncreasing channel size of "
                    << dataflow_prime->getEdgeName(*it) << " to "
                    << newDist.getChannelQuantity(*it) << std::endl);
      }
    }
    VERBOSE_DSE("Analysing new storage distribution: " << std::endl);
    dataflow_prime->reset_computation(); // make graph writeable to alter channel size
    {ForEachEdge(dataflow_prime, c) {
        if (dataflow_prime->getEdgeId(c) > dataflow->getEdgesCount()) { // only modelled buffer preloads change
          dataflow_prime->setPreload(c, (newDist.getChannelQuantity(c) -
                                         newDist.getInitialTokens(c))); // always account for initial tokens in buffer
        }
      }}
    result = compute_Kperiodic_throughput_and_cycles(dataflow_prime, parameters);
    if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
      newDist.setThroughput(0);
    } else {
      newDist.setThroughput(result.throughput);
    }
    VERBOSE_DSE(newDist.printInfo(dataflow_prime));
    thrCurrent = newDist.getThroughput();
  }
  VERBOSE_DSE("Found first feasible SD:\n" << newDist.printInfo(dataflow_prime)
              << std::endl);
  VERBOSE_DSE("Current infeasible set:\n" << infeasibleSet.printDistributions(dataflow_prime)
              << std::endl);
  VERBOSE_DSE("Current knee set:\n" << kneeSet.printDistributions(dataflow_prime)
              << std::endl);

  feasibleSet.updateFeasibleSet(newDist);
  VERBOSE_DSE("Current feasible set:\n"
              << feasibleSet.printDistributions(dataflow_prime) << std::endl);
  StorageDistribution checkDist = selectNewSD(dataflow_prime, infeasibleSet, kneeSet, feasibleSet, thrCurrent);
  VERBOSE_DSE("Next SD to check:\n" << checkDist.printInfo(dataflow_prime)
              << std::endl);
  // if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
  //   checkDist.setThroughput(0);
  // } else {
  //   checkDist.setThroughput(result.throughput);
  // }
  // VERBOSE_DSE(checkDist.printInfo(dataflow_prime));

  // // Create new storage distributions for every storage dependency found; add new storage distributions to checklist
  // for (std::set<Edge>::iterator it = (result.critical_edges).begin();
  //      it != (result.critical_edges).end(); it++) {
  //   StorageDistribution newDist(checkDist);
  //   // only increase channel quantity on "modelled" channels
  //   if (dataflow_prime->getEdgeId(*it) > dataflow->getEdgesCount()) {
  //     VERBOSE_DSE("\tFound storage dependency in channel "
  //                 << dataflow_prime->getEdgeName(*it) << std::endl);
  //     // make new modelled storage distribution according to storage dependencies
  //     newDist.setChannelQuantity(*it, (newDist.getChannelQuantity(*it) +
  //                                      minStepSizes[*it]));
  //     VERBOSE_DSE("\t\tIncreasing channel size of "
  //                 << dataflow_prime->getEdgeName(*it) << " to "
  //                 << newDist.getChannelQuantity(*it) << std::endl);
  //     VERBOSE_DSE("\tUpdating checklist with new storage distribution..."
  //                 << std::endl);
  //     checklist.addStorageDistribution(newDist);
  //   }
  // }

  // std::map<Edge, std::pair<TOKEN_UNIT, TOKEN_UNIT>> channelSizes;
  // int i = 0;
  // int sz0[2] = {3,3};
  // StorageDistribution x0;
  // {ForEachEdge(dataflow_prime, c) {
  //     if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
  //       channelSizes[c].second = 0;
  //     } else {
  //       x0.setChannelQuantity(c, sz0[i]);
  //       i++;
  //     }
  //   }}
  // i = 0;
  // int sz[2] = {4,3};
  // StorageDistribution x1;
  // {ForEachEdge(dataflow_prime, c) {
  //     if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
  //       channelSizes[c].second = 0;
  //     } else {
  //       x1.setChannelQuantity(c, sz[i]);
  //       i++;
  //     }
  //   }}
  // i = 0;
  // int sz2[2] = {7,2};
  // StorageDistribution x2;
  // {ForEachEdge(dataflow_prime, c) {
  //     if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
  //       channelSizes[c].second = 0;
  //     } else {
  //       x2.setChannelQuantity(c, sz2[i]);
  //       i++;
  //     }
  //   }}
  // i = 0;
  // int sz3[2] = {4,4};
  // StorageDistribution x3;
  // {ForEachEdge(dataflow_prime, c) {
  //     if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
  //       channelSizes[c].second = 0;
  //     } else {
  //       x3.setChannelQuantity(c, sz3[i]);
  //       i++;
  //     }
  //   }}
  // i = 0;
  // int sz4[2] = {2,8};
  // StorageDistribution x4;
  // {ForEachEdge(dataflow_prime, c) {
  //     if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
  //       channelSizes[c].second = 0;
  //     } else {
  //       x4.setChannelQuantity(c, sz4[i]);
  //       i++;
  //     }
  //   }}
  // i = 0;
  // int sz5[2] = {5,3};
  // StorageDistribution x5;
  // {ForEachEdge(dataflow_prime, c) {
  //     if (dataflow_prime->getEdgeId(c) <= dataflow->getEdgesCount()) {
  //       channelSizes[c].second = 0;
  //     } else {
  //       x5.setChannelQuantity(c, sz5[i]);
  //       i++;
  //     }
  //   }}
  // StorageDistributionSet infeasibleSet;
  // StorageDistributionSet kneeSet;
  // infeasibleSet.updateInfeasibleSet(x0); // 3, 3
  // infeasibleSet.updateInfeasibleSet(x1); // 4, 3
  // infeasibleSet.updateInfeasibleSet(x2); // 7, 2
  // infeasibleSet.updateInfeasibleSet(x3); // 4, 4
  // infeasibleSet.updateInfeasibleSet(x4); // 2, 8
  // infeasibleSet.updateInfeasibleSet(x5); // 5, 3
  // kneeSet.updateKneeSet(infeasibleSet);
}
