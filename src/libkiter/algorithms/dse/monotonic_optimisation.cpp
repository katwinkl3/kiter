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
                                            StorageDistribution &kMinPoint,
                                            std::map<Edge, TOKEN_UNIT> bufferLb) {
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
  if ((infeasibleSet.isInBackCone(newDist, bufferLb) || infeasibleSet.hasStorageDistribution(newDist))
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
                                                                                TIME_UNIT targetThr,
                                                                                parameters_list_t parameters) {
  TIME_UNIT thrTarget = targetThr;
  long int computation_counter = 0;
  TIME_UNIT thrCurrent = initDist.getThroughput();
  StorageDistribution newDist(initDist);
  kperiodic_result_t result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
  StorageDistributionSet infeasibleSet;
  StorageDistributionSet kneeSet;
  StorageDistributionSet feasibleSet;
  // initialise mapping of lower bounds of buffer sizes (to account for storage dependencies)
  std::map<Edge, TOKEN_UNIT> bufferLowerBounds;
  for (auto &e : initDist.getEdges()) {
    bufferLowerBounds[e] = 0;
  }
  while (thrCurrent < thrTarget) {
    VERBOSE_DSE("SD sending to handleInfeasible:\n" << newDist.printInfo(dataflow) << std::endl);
    handleInfeasiblePoint(dataflow, infeasibleSet, kneeSet, newDist, result, bufferLowerBounds);
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
        // if (bufferLowerBounds[*it] < newDist.getChannelQuantity(*it)) {
        //   bufferLowerBounds[*it] = newDist.getChannelQuantity(*it); // update lower bounds of channel          
        // }
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
                                              mult, step, kMin,
                                              bufferLowerBounds);
  VERBOSE_DSE("Next SD to check:\n" << checkDist.printInfo(dataflow)
              << std::endl);
  if (foundPoint) { // first call of select function can't find a point between U and S
    currDist = checkDist;
  }
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
                            checkDist, result, bufferLowerBounds);
    } else {
      feasibleSet.updateFeasibleSet(checkDist);
    }
    currDist = checkDist;
    checkDist = selectNewSD(dataflow,
                            infeasibleSet, kneeSet, feasibleSet,
                            isInit, foundPoint,
                            thrCurrent, thrTarget,
                            mult, step, kMin,
                            bufferLowerBounds);
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
  VERBOSE_DSE("Knee set to send to DSE:\n" << kneeSet.printDistributions(dataflow)
              << std::endl);
  StorageDistributionSet augmentedKneeSet;
  for (auto &distSz : kneeSet.getSet()) {
    for (auto &sd : distSz.second) {
      StorageDistribution tempSD(sd);
      {ForEachEdge(dataflow, c) {
          if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) {
            tempSD.setChannelQuantity(c, (sd.getChannelQuantity(c) + 1));
          }
        }}
      augmentedKneeSet.addStorageDistribution(tempSD);
    }
  }
  VERBOSE_DSE("Augmented knee set:\n" << augmentedKneeSet.printDistributions(dataflow)
              << std::endl);
  return augmentedKneeSet;
}
