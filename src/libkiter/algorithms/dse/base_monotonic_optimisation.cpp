/*
 * base_monotonic_optimisation.cpp
 * Adapted from Hendricks et al. (2019)
 *
 * Created on: Nov 13, 2020
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
#include "base_monotonic_optimisation.h"
#include "kperiodic.h"
#include <string>
#include <chrono> // to take computation timings

// TODO handle infeasible function
//      inputs: U, K, x, dependencies
//      behaviour: - return error if no dependencies,
//                 - expand U according to channels without dependencies in x
//                 - call update knee point function
//                 - call update infeasible SDs function

StorageDistribution algorithms::select(models::Dataflow* const dataflow,
                                       StorageDistributionSet infeasibleSet,
                                       StorageDistributionSet kneeSet,
                                       StorageDistributionSet feasibleSet,
                                       bool &foundPoint,
                                       bool isSat,
                                       TOKEN_UNIT &mult,
                                       StorageDistribution &kMinPoint) {
  StorageDistribution hpDist(kneeSet.getNextDistribution());
  StorageDistribution hyperPlanePoint(feasibleSet.getNextDistribution());
  if (!isSat) {
    mult *= 2;
    StorageDistribution newDist = createPoint(dataflow, kMinPoint, hpDist, mult);
    if (feasibleSet.satContains(newDist)) {
      return newDist;
    }
  }
  // explore smallest knee
  kMinPoint = kneeSet.getNextDistribution();
  // compute closest hyperplane point
  TOKEN_UNIT k = kMinPoint.getDistributionSize() - hyperPlanePoint.getDistributionSize();
  {ForEachEdge(dataflow, e) {
      if (dataflow->getEdgeId(e) > dataflow->getEdgesCount()/2) {
        hyperPlanePoint.setChannelQuantity(e, kMinPoint.getChannelQuantity(e) - k);
      }
    }}
  {ForEachEdge(dataflow, e) {
      if (dataflow->getEdgeId(e) > dataflow->getEdgesCount()/2) {
        hpDist.setChannelQuantity(e, (hyperPlanePoint.getChannelQuantity(e) - hpDist.getChannelQuantity(e))/2);
      }
    }}
  mult = 1;
  StorageDistribution newDist = createPoint(dataflow, kMinPoint, hpDist, mult);
  if (feasibleSet.satContains(newDist) || infeasibleSet.unsatContains(newDist)) {
    foundPoint = true;
    return newDist;
  }
  return newDist;
 
}

StorageDistributionSet algorithms::base_monotonic_optimised_Kperiodic_throughput_dse(models::Dataflow* const dataflow,
                                                                                     StorageDistribution initDist,
                                                                                     TIME_UNIT targetThr,
                                                                                     parameters_list_t parameters) {
  TIME_UNIT thrTarget = targetThr;
  long int computation_counter = 0;
  TIME_UNIT thrCurrent;
  StorageDistribution newDist(initDist);
  StorageDistribution originDist(initDist);
  StorageDistribution lbDist(initDist);
  kperiodic_result_t result;
  StorageDistributionSet infeasibleSet;
  StorageDistributionSet kneeSet;
  StorageDistributionSet feasibleSet;
  std::map<Edge, TOKEN_UNIT> bufferInc;
  
  // initialise infeasible set, knee set, feasible set
  for (auto &e : originDist.getEdges()) {
    originDist.setChannelQuantity(e, 0); // NOTE might have to be 1 cause no sense having 0 buffer size
    lbDist.setChannelQuantity(e, INT_MAX);
    bufferInc[e] = 1;
  }
  kneeSet.addStorageDistribution(originDist);
  // initialise mapping of lower bounds of buffer sizes (to account for storage dependencies)
  for (auto &e : initDist.getEdges()) {
    StorageDistribution tempDist(lbDist);
    tempDist.setChannelQuantity(e, initDist.getChannelQuantity(e) - 1);
    infeasibleSet.add(tempDist, kneeSet);
  }

  result = compute_Kperiodic_throughput_and_cycles(dataflow, parameters);
  thrCurrent = result.throughput;
  while (thrCurrent < thrTarget) {
    infeasibleSet.add(newDist, kneeSet);
    infeasibleSet.addCut(dataflow, newDist, result, kneeSet);
    for (auto &e : result.critical_edges) {
      if (dataflow->getEdgeId(e) > dataflow->getEdgesCount()/2) {
        bufferInc[e] = bufferInc[e] * 2;
        newDist.setChannelQuantity(e, newDist.getChannelQuantity(e) + bufferInc[e]);
      }
    }
    std::cout << "BASE_M_OPT: trying:\n" << newDist.printInfo(dataflow) << std::endl;
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
  feasibleSet.addStorageDistribution(newDist);

  bool foundPoint = false;
  bool isSat = true; // true if thrCurrent >= thrTarget
  TOKEN_UNIT mult = 1;
  StorageDistribution kMin;
  StorageDistribution currDist;
  StorageDistribution checkDist = select(dataflow,
                                         infeasibleSet, kneeSet, feasibleSet,
                                         foundPoint, isSat, mult,
                                         kMin);
  while (!foundPoint) {
    VERBOSE_DSE("Analysing new storage distribution: " << std::endl);
    dataflow->reset_computation();
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
    isSat = thrCurrent >= thrTarget;
    if (!isSat) {
      infeasibleSet.add(checkDist, kneeSet);
      infeasibleSet.addCut(dataflow, checkDist, result, kneeSet);
    } else {
      feasibleSet.addToSat(checkDist);
    }
    checkDist = select(dataflow,
                       infeasibleSet, kneeSet, feasibleSet,
                       foundPoint, isSat, mult,
                       kMin);
  }
  StorageDistributionSet augmentedKneeSet;
  for (auto &dist_sz : kneeSet.getSet()) {
    for (auto &sd : dist_sz.second) {
      StorageDistribution tempSD(sd);
      {ForEachEdge(dataflow, c) {
          if (dataflow->getEdgeId(c) > dataflow->getEdgesCount()/2) {
            if (tempSD.getChannelQuantity(c) < INT_MAX) {              
              tempSD.setChannelQuantity(c, (sd.getChannelQuantity(c) + 1));
            }
          }
        }}
      if (tempSD.getDistributionSize() < (feasibleSet.getNextDistribution()).getDistributionSize()) {
        augmentedKneeSet.addStorageDistribution(tempSD);
      }
    }
  }
  // return kneeSet;
  return augmentedKneeSet;
}
