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
    std::cout << "SELECT: Cannot find new point, ending on:\n" << newDist.printInfo(dataflow) << std::endl;
    return newDist;
  } else {
    std::cout << "SELECT: new point found:\n" << newDist.printInfo(dataflow) << std::endl;
    return newDist;
  }
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
    VERBOSE_DSE("SD sending to handleInfeasible:\n" << newDist.printInfo(dataflow_prime) << std::endl);
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

  // Optimization phase
  feasibleSet.updateFeasibleSet(newDist);
  VERBOSE_DSE("Current feasible set:\n"
              << feasibleSet.printDistributions(dataflow_prime) << std::endl);
  StorageDistribution kMin;
  TOKEN_UNIT mult = 1;
  std::map<Edge, TOKEN_UNIT> step;
  bool isInit = false;
  bool foundPoint = false;
  
  StorageDistribution checkDist = selectNewSD(dataflow_prime,
                                              infeasibleSet, kneeSet, feasibleSet,
                                              isInit, foundPoint,
                                              thrCurrent, thrTarget,
                                              mult, step, kMin);
  VERBOSE_DSE("Next SD to check:\n" << checkDist.printInfo(dataflow_prime)
              << std::endl);
  
  while (!foundPoint) {
    VERBOSE_DSE("Analysing new storage distribution: " << std::endl);
    dataflow_prime->reset_computation(); // make graph writeable to alter channel size
    {ForEachEdge(dataflow_prime, c) {
        if (dataflow_prime->getEdgeId(c) > dataflow->getEdgesCount()) { // only modelled buffer preloads change
          dataflow_prime->setPreload(c, (checkDist.getChannelQuantity(c) -
                                         checkDist.getInitialTokens(c))); // always account for initial tokens in buffer
        }
      }}
    result = compute_Kperiodic_throughput_and_cycles(dataflow_prime, parameters);
    if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
      checkDist.setThroughput(0);
    } else {
      checkDist.setThroughput(result.throughput);
    }
    VERBOSE_DSE(checkDist.printInfo(dataflow_prime));
    thrCurrent = checkDist.getThroughput();
    std::cout << "thrCurrent, thrTarget: " << thrCurrent << ", " << thrTarget << std::endl;
    if (thrCurrent < thrTarget) {
      handleInfeasiblePoint(dataflow_prime, infeasibleSet, kneeSet,
                            checkDist, result);
    } else {
      feasibleSet.updateFeasibleSet(checkDist);
    }
    checkDist = selectNewSD(dataflow_prime,
                            infeasibleSet, kneeSet, feasibleSet,
                            isInit, foundPoint,
                            thrCurrent, thrTarget,
                            mult, step, kMin);
    VERBOSE_DSE("Current infeasible set:\n" << infeasibleSet.printDistributions(dataflow_prime)
                << std::endl);
    VERBOSE_DSE("Current knee set:\n" << kneeSet.printDistributions(dataflow_prime)
                << std::endl);
    VERBOSE_DSE("Current feasible set:\n"
                << feasibleSet.printDistributions(dataflow_prime) << std::endl);
    
  }
  dataflow_prime->reset_computation(); // make graph writeable to alter channel size
  {ForEachEdge(dataflow_prime, c) {
      if (dataflow_prime->getEdgeId(c) > dataflow->getEdgesCount()) { // only modelled buffer preloads change
        dataflow_prime->setPreload(c, (checkDist.getChannelQuantity(c) -
                                       checkDist.getInitialTokens(c))); // always account for initial tokens in buffer
      }
    }}
  result = compute_Kperiodic_throughput_and_cycles(dataflow_prime, parameters);
  if (result.throughput < 0) { // all deadlocked graphs are equal in terms of throughput
    checkDist.setThroughput(0);
  } else {
    checkDist.setThroughput(result.throughput);
  }
  VERBOSE_DSE("SD to send to DSE:\n" << checkDist.printInfo(dataflow_prime)
              << std::endl);
}
