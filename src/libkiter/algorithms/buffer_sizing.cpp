/*
 * buffer_sizing.cpp
 *
 */

#include <boost/math/common_factor_rt.hpp>  // for boost::math::gcd, lcm
#include <algorithms/buffer_sizing.h>
#include <models/Dataflow.h>

EXEC_COUNT *minStepSizes;
EXEC_COUNT *minChannelSizes;
int minDistributionSize;

void findMinimumStepSz(models::Dataflow *dataflow) {
  std::cout << "Calculating minimal channel step sizes..." << std::endl;
  minStepSizes = new int [dataflow->getEdgesCount()]; // need to calculate minimum step size per channel
  int iter = 0;
  std::cout << "Number of channels: " << dataflow->getEdgesCount() << std::endl;
  {ForEachEdge(dataflow, c) { // get GCD of all possible combinations of rates of production and consumption in channel
      EXEC_COUNT minStepSz;
      minStepSz = (EXEC_COUNT) dataflow->getEdgeInVector(c)[0]; // initialise with first value
      for (int i = 0; i < dataflow->getEdgeInPhasesCount(c); i++)
        minStepSz = boost::math::gcd(minStepSz, (EXEC_COUNT) dataflow->getEdgeInVector(c)[i]);
      for (int i = 0; i < dataflow->getEdgeOutPhasesCount(c); i++)
        minStepSz = boost::math::gcd(minStepSz, (EXEC_COUNT) dataflow->getEdgeOutVector(c)[i]);
      minStepSizes[iter] = minStepSz;
      std::cout << "Min. step size for channel " << dataflow->getEdgeName(c)
                << ": " << minStepSz << std::endl;
      iter++;
  }}
}

void findMinimumChannelSz(models::Dataflow *dataflow) {
  std::cout << "Calculating minimal channel sizes (for postive throughput)..."
            << std::endl;
  minChannelSizes = new int [dataflow->getEdgesCount()];
  
  {ForEachEdge(dataflow, c) {
      // initialise channel size to maximum int size (should use ULONG_MAX but it's a really large value)
      minChannelSizes[dataflow->getEdgeId(c) - 1] = INT_MAX; // use (EdgeID - 1) to for array index
      EXEC_COUNT ratePeriod = boost::math::gcd(dataflow->getEdgeInPhasesCount(c),
                                               dataflow->getEdgeOutPhasesCount(c));
      std::cout << "Rate period for " << dataflow->getEdgeName(c) << ": "
                << ratePeriod << std::endl;
      for (int i = 0; i < ratePeriod; i++) {
        // might want to change variables to p, c, and t for legibility
        EXEC_COUNT tokensProduced = dataflow->getEdgeInVector(c)[i % dataflow->getEdgeInPhasesCount(c)];
        EXEC_COUNT tokensConsumed = dataflow->getEdgeOutVector(c)[i % dataflow->getEdgeOutPhasesCount(c)];
        EXEC_COUNT tokensInitial = dataflow->getPreload(c);
        std::cout << "p, c, t: " << tokensProduced << ", " << tokensConsumed << ", " << tokensInitial << std::endl;
        EXEC_COUNT lowerBound;

        if (boost::math::gcd(tokensProduced, tokensConsumed)) {
          lowerBound = tokensProduced + tokensConsumed -
            boost::math::gcd(tokensProduced, tokensConsumed) +
            tokensInitial % boost::math::gcd(tokensProduced, tokensConsumed);
        } else {
          lowerBound = tokensProduced + tokensConsumed -
            boost::math::gcd(tokensProduced, tokensConsumed);
        }
        lowerBound = (lowerBound > tokensInitial ? lowerBound : tokensInitial);
        
        // take the lowest bound amongst phases of prod/cons
        if (lowerBound < minChannelSizes[dataflow->getEdgeId(c) - 1]) {
          minChannelSizes[dataflow->getEdgeId(c) - 1] = lowerBound;
        }
      }
      std::cout << "Minimum channel size for " << dataflow->getEdgeName(c)
                << ": " << minChannelSizes[dataflow->getEdgeId(c) - 1] << std::endl;
      
  }}
}

void findMinimumDistributionSz(models::Dataflow *dataflow) {
  minDistributionSize = 0;
  // std::cout << dataflow->getEdgesCount() << std::endl;
  for (int i = 0; i < dataflow->getEdgesCount(); i++) {
    std::cout << minChannelSizes[i] << std::endl;
    minDistributionSize += minChannelSizes[i];
  }
  std::cout << "Lower bound distribution size: " << minDistributionSize << std::endl;
}

// void findMaximumThroughput(models::Dataflow *dataflow) {
//   // compute_Kperiodic_throughput(dataflow, parameters);
//   // might have to find throughput outside of function
// }

// void initSearchParameters(models::Dataflow *dataflow) {
//   // TODO: findMinStepSz (per channel), findMinChannelSz, findMinDistSz (sum of min channel sz), findMaxThr (compute kperiodic)
// }
