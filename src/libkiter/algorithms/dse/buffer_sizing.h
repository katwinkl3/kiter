/*
 * buffer_sizing.h
 *
 */

#ifndef BUFFER_SIZING_H_
#define BUFFER_SIZING_H_

#include <models/Dataflow.h>
#include <commons/commons.h>

namespace models {
  class Dataflow;
}

class StorageDistribution {
public:
  StorageDistribution();
  StorageDistribution(unsigned int edge_count,
                      TIME_UNIT thr,
                      std::map<Edge,
                      std::pair<TOKEN_UNIT, TOKEN_UNIT>> channel_quantities,
                      TOKEN_UNIT distribution_size);

  void setChannelQuantity(Edge e, TOKEN_UNIT quantity);
  void setInitialTokens(Edge e, TOKEN_UNIT token_count);
  void setDistributionSize(TOKEN_UNIT sz);
  void setThroughput(TIME_UNIT thr);
  TOKEN_UNIT getChannelQuantity(Edge e) const;
  TOKEN_UNIT getInitialTokens(Edge e) const;
  TOKEN_UNIT getDistributionSize() const;
  TIME_UNIT getThroughput() const;
  unsigned int getEdgeCount() const;
  std::vector<Edge> getEdges() const;
  bool operator==(const StorageDistribution& distribution) const;
  bool operator!=(const StorageDistribution& distribution) const;
  void updateDistributionSize();
  std::string printInfo(models::Dataflow* const dataflow);
  std::string print_quantities_csv(models::Dataflow* const dataflow);
  std::string printGraph(models::Dataflow* const dataflow);
private:
  unsigned int edge_count;
  TIME_UNIT thr; // throughput of given storage distribution
  std::map<Edge, std::pair<TOKEN_UNIT, // initial tokens
                           TOKEN_UNIT>> channel_quantities; // amount of space (in tokens per channel)
  TOKEN_UNIT distribution_size; // should be equal to sum of channel quantities
};

class StorageDistributionSet {
public:
  StorageDistributionSet();
  StorageDistributionSet(TOKEN_UNIT dist_sz,
                         StorageDistribution distribution);
  void addStorageDistribution(StorageDistribution new_distribution); // add storage distribution to set
  void removeStorageDistribution(StorageDistribution dist_to_rm); // remove storage distribution from set
  void removeDistributionSize(TOKEN_UNIT dist_sz); /* removes all storage distributions of
                                                      distribution size dist_sz */
  StorageDistribution getNextDistribution() const;
  size_t getSize() const;
  void minimizeStorageDistributions(StorageDistribution newDist);
  bool hasDistribution(TOKEN_UNIT dist_sz);
  bool isSearchComplete(StorageDistributionSet checklist, TIME_UNIT target_thr);
  void removeNonMaximum(StorageDistribution checkDist);
  void updateInfeasibleSet(StorageDistribution new_sd); // add new SD to infeasible set of SDs for monotonic optimisation
  std::string printDistributions(TOKEN_UNIT dist_sz,
				 models::Dataflow* const dataflow); /* prints info of all storage distributions 
								       of given distribution size */
  std::string printDistributions(models::Dataflow* const dataflow); // prints info of all storage distributions in set
  void writeCSV(std::string filename, models::Dataflow* const dataflow); // writes to a CSV file for plots
  void printGraphs(models::Dataflow* const dataflow, std::string filename); // iterate through storage distribution set and print graphs
  
private:
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> set; /* store storage distributions 
                                                                 by distribution size*/
  std::pair<TOKEN_UNIT, TIME_UNIT> p_max; /* stores the current maximum throughput and corresponding
                                             distribution size of set */
};

// to search parameters
void findMinimumStepSz(models::Dataflow *dataflow,
                       std::map<Edge, TOKEN_UNIT> &minStepSizes);
void findMinimumChannelSz(models::Dataflow *dataflow,
                          std::map<Edge,
                          std::pair<TOKEN_UNIT, TOKEN_UNIT>> &minChannelSizes); // initial tokens in first element of pair and channel quantity in second element of pair
TOKEN_UNIT findMinimumDistributionSz(models::Dataflow *dataflow,
                                     std::map<Edge,
                                     std::pair<TOKEN_UNIT, TOKEN_UNIT>> minChannelSizes);
void initSearchParameters(models::Dataflow *dataflow,
                          std::map<Edge, TOKEN_UNIT> &minStepSizes,
                          std::map<Edge,
                          std::pair<TOKEN_UNIT, TOKEN_UNIT>> &minChannelSizes);
std::string timeToString(TIME_UNIT t);

#endif /* BUFFER_SIZING_H_ */
