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
                                            bool &isInit,
                                            bool &foundPoint,
                                            TIME_UNIT thrCurrent,
                                            TIME_UNIT thrTarget,
                                            TOKEN_UNIT &mult,
                                            std::map<Edge, TOKEN_UNIT> &step,
                                            StorageDistribution &kMinPoint) {
  if (isInit && thrCurrent < thrTarget) {
    mult *= 2;
    StorageDistribution newDist(kMinPoint);
    {ForEachEdge(dataflow, c) {
        if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) {
          newDist.setChannelQuantity(c,
                                     (newDist.getChannelQuantity(c) + (mult * step[c])));
        }
      }}
    if (!(feasibleSet.isInForeCone(newDist) || feasibleSet.hasStorageDistribution(newDist))) {
      std::cout << "SELECT: new point not in S+:\n" << newDist.printInfo(dataflow) << std::endl;
      return newDist;
    }
  }

  mult = 1;
  kMinPoint = kneeSet.getNextDistribution();
  StorageDistribution newDist(kMinPoint);
  StorageDistribution sMinPoint(feasibleSet.getNextDistribution());
  {ForEachEdge(dataflow, c) {
      if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) {
        step[c] = (sMinPoint.getChannelQuantity(c) - kMinPoint.getChannelQuantity(c)) / 2;
        newDist.setChannelQuantity(c,
                                   newDist.getChannelQuantity(c) + step[c]);
      }
    }}
  if (!isInit) {
    isInit = true;
  }
  if ((infeasibleSet.isInBackCone(newDist) || infeasibleSet.hasStorageDistribution(newDist))
      ||
      (feasibleSet.isInForeCone(newDist) || feasibleSet.hasStorageDistribution(newDist))) {
    // we've gotten to the end of the optimization phase
    foundPoint = true;
    std::cout << "SELECT: Cannot find new point, ending search" << std::endl;
    return newDist;
  } else {
    std::cout << "SELECT: new point found:\n" << newDist.printInfo(dataflow) << std::endl;
    return newDist;
  }
}

StorageDistributionSet algorithms::monotonic_optimised_Kperiodic_throughput_dse(models::Dataflow* const dataflow,
                                                                                StorageDistribution initDist,
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
  // models::Dataflow* dataflow_prime = new models::Dataflow(*dataflow);
  long int computation_counter = 0;
  
  // // create channels in new graph to model bounded channel quantities
  // dataflow_prime->reset_computation(); // make graph writeable to alter channel size
  // {ForEachEdge(dataflow, c) {
  //     auto new_edge = dataflow_prime->addEdge(dataflow_prime->getEdgeTarget(c),
  //                                             dataflow_prime->getEdgeSource(c));
  //     dataflow_prime->setEdgeInPhases(new_edge,
  //                                     dataflow_prime->getEdgeOutVector(c));
  //     dataflow_prime->setEdgeOutPhases(new_edge,
  //                                      dataflow_prime->getEdgeInVector(c));
  //     dataflow_prime->setPreload(new_edge, dataflow->getPreload(c));
  //     dataflow_prime->setEdgeName(new_edge,
  //                                 dataflow_prime->getEdgeName(c) + "_prime");
  //   }}

  // // initialise search parameters
  // std::map<Edge, TOKEN_UNIT> minStepSizes;
  // std::map<Edge, std::pair<TOKEN_UNIT, TOKEN_UNIT>> minChannelSizes;
  // TOKEN_UNIT minDistributionSize;
  
  // initSearchParameters(dataflow,
  //                      minStepSizes,
  //                      minChannelSizes);
  // dataflow->reset_computation();
  // // set lower bound channel quantities
  // {ForEachEdge(dataflow, c) {
  //     if (dataflow->getEdgeId(c) <= dataflow->getEdgesCount()/2) {
  //       minChannelSizes[c].second = 0; // original edges must have capacity 0
  //     }
  //     minChannelSizes[c].first = dataflow->getPreload(c);
  //   }}

  // minDistributionSize = findMinimumDistributionSz(dataflow,
  //                                                 minChannelSizes);

  // // initialise and store initial storage distribution state
  // StorageDistribution initDist(dataflow->getEdgesCount(),
  //                              0,
  //                              minChannelSizes,
  //                              minDistributionSize);
  
  // // // initialise modelled graph with lower bound distribution
  // {ForEachEdge(dataflow, c) {
  //     if (dataflow->getEdgeId(c) <= dataflow->getEdgesCount()/2) { // original channels
  //       dataflow->setPreload(c, initDist.getInitialTokens(c));
  //     } else {
  //       // subtract initial tokens from buffer size to model any initial tokens in buffer
  //       dataflow->setPreload(c, (initDist.getChannelQuantity(c) -
  //                                      initDist.getInitialTokens(c)));
  //     }
  //   }}

  // // calculate max throughput and current throughput with lower bound distribution
  // kperiodic_result_t result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);

  // /* a negative throughput indicates a deadlocked graph and so 
  //    we set it to 0 to avoid bugs from having a negative throughput */
  // if (result.throughput < 0) {
  //   initDist.setThroughput(0);
  // } else { // set throughput to one computed from initial distribution otherwise
  //   initDist.setThroughput(result.throughput);
  // }

  // add initial distribution to list of storage distributions
  // StorageDistributionSet checkList(initDist.getDistributionSize(),
  //                                  initDist);
  TIME_UNIT thrCurrent = initDist.getThroughput();
  StorageDistribution newDist(initDist);
  kperiodic_result_t result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
  StorageDistributionSet infeasibleSet;
  StorageDistributionSet kneeSet;
  StorageDistributionSet feasibleSet;
  while (thrCurrent < thrTarget) {
    VERBOSE_DSE("SD sending to handleInfeasible:\n" << newDist.printInfo(dataflow) << std::endl);
    handleInfeasiblePoint(dataflow, infeasibleSet, kneeSet, newDist, result);
    VERBOSE_DSE("Current infeasible set:\n" << infeasibleSet.printDistributions(dataflow)
                << std::endl);
    VERBOSE_DSE("Current knee set:\n" << kneeSet.printDistributions(dataflow)
                << std::endl);
    for (std::set<Edge>::iterator it = (result.critical_edges).begin();
         it != (result.critical_edges).end(); it++) {
      // only increase channel quantity on "modelled" channels
      if (dataflow->getEdgeId(*it) > dataflow->getEdgesCount()/2) {
        VERBOSE_DSE("\tFound storage dependency in channel "
                    << dataflow->getEdgeName(*it) << std::endl);
        // double new modelled storage distribution channels with storage dependencies
        newDist.setChannelQuantity(*it, (newDist.getChannelQuantity(*it) * 2));
        VERBOSE_DSE("\t\tIncreasing channel size of "
                    << dataflow->getEdgeName(*it) << " to "
                    << newDist.getChannelQuantity(*it) << std::endl);
      }
    }
    VERBOSE_DSE("Analysing new storage distribution: " << std::endl);
    dataflow->reset_computation(); // make graph writeable to alter channel size
    {ForEachEdge(dataflow, c) {
        if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) { // only modelled buffer preloads change
          dataflow->setPreload(c, (newDist.getChannelQuantity(c) -
                                         newDist.getInitialTokens(c))); // always account for initial tokens in buffer
        }
      }}
    result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
    if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
      newDist.setThroughput(0);
    } else {
      newDist.setThroughput(result.throughput);
    }
    VERBOSE_DSE(newDist.printInfo(dataflow));
    thrCurrent = newDist.getThroughput();
  }
  VERBOSE_DSE("Found first feasible SD:\n" << newDist.printInfo(dataflow)
              << std::endl);
  VERBOSE_DSE("Current infeasible set:\n" << infeasibleSet.printDistributions(dataflow)
              << std::endl);
  VERBOSE_DSE("Current knee set:\n" << kneeSet.printDistributions(dataflow)
              << std::endl);

  // Optimization phase
  feasibleSet.updateFeasibleSet(newDist);
  VERBOSE_DSE("Current feasible set:\n"
              << feasibleSet.printDistributions(dataflow) << std::endl);
  StorageDistribution kMin;
  TOKEN_UNIT mult = 1;
  std::map<Edge, TOKEN_UNIT> step;
  bool isInit = false;
  bool foundPoint = false;
  StorageDistribution currDist;
  
  StorageDistribution checkDist = selectNewSD(dataflow,
                                              infeasibleSet, kneeSet, feasibleSet,
                                              isInit, foundPoint,
                                              thrCurrent, thrTarget,
                                              mult, step, kMin);
  VERBOSE_DSE("Next SD to check:\n" << checkDist.printInfo(dataflow)
              << std::endl);
  
  while (!foundPoint) {
    VERBOSE_DSE("Analysing new storage distribution: " << std::endl);
    dataflow->reset_computation(); // make graph writeable to alter channel size
    {ForEachEdge(dataflow, c) {
        if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) { // only modelled buffer preloads change
          dataflow->setPreload(c, (checkDist.getChannelQuantity(c) -
                                         checkDist.getInitialTokens(c))); // always account for initial tokens in buffer
        }
      }}
    result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
    if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
      checkDist.setThroughput(0);
    } else {
      checkDist.setThroughput(result.throughput);
    }
    VERBOSE_DSE(checkDist.printInfo(dataflow));
    thrCurrent = checkDist.getThroughput();
    std::cout << "thrCurrent, thrTarget: " << thrCurrent << ", " << thrTarget << std::endl;
    if (thrCurrent < thrTarget) {
      handleInfeasiblePoint(dataflow, infeasibleSet, kneeSet,
                            checkDist, result);
    } else {
      feasibleSet.updateFeasibleSet(checkDist);
    }
    currDist = checkDist;
    checkDist = selectNewSD(dataflow,
                            infeasibleSet, kneeSet, feasibleSet,
                            isInit, foundPoint,
                            thrCurrent, thrTarget,
                            mult, step, kMin);
    if (!foundPoint) {
      currDist = checkDist;
    }
    VERBOSE_DSE("Current infeasible set:\n" << infeasibleSet.printDistributions(dataflow)
                << std::endl);
    VERBOSE_DSE("Current knee set:\n" << kneeSet.printDistributions(dataflow)
                << std::endl);
    VERBOSE_DSE("Current feasible set:\n"
                << feasibleSet.printDistributions(dataflow) << std::endl);
  }

  dataflow->reset_computation(); // make graph writeable to alter channel size
  {ForEachEdge(dataflow, c) {
      if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) { // only modelled buffer preloads change
        dataflow->setPreload(c, (currDist.getChannelQuantity(c) -
                                       currDist.getInitialTokens(c))); // always account for initial tokens in buffer
      }
    }}
  result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
  if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
    currDist.setThroughput(0);
  } else {
    currDist.setThroughput(result.throughput);
  }
  VERBOSE_DSE("SD to send to DSE:\n" << currDist.printInfo(dataflow)
              << std::endl);
  return kneeSet;
}
