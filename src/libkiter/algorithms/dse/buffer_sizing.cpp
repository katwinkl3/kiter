/*
 * buffer_sizing.cpp
 *
 */

#include <boost/math/common_factor_rt.hpp>  // for boost::math::gcd, lcm
#include "buffer_sizing.h"
#include <models/Dataflow.h>
#include <printers/stdout.h> // to print DOT files
#include <iostream>
#include <fstream>

StorageDistribution::StorageDistribution()
  :edge_count{0}, thr{0}, channel_quantities(), distribution_size{0} {
                          }

StorageDistribution::StorageDistribution(unsigned int edge_count,
                                         TIME_UNIT thr,
                                         std::map<Edge, std::pair<TOKEN_UNIT, TOKEN_UNIT>> channel_quantities,
                                         TOKEN_UNIT distribution_size)
  :edge_count{edge_count}, thr{thr}, channel_quantities{channel_quantities},
   distribution_size{distribution_size} {
     // NOTE: could replace distribution_size declaration with updateDistributionSize()
   }

// Set edge to given quantity
void StorageDistribution::setChannelQuantity(Edge e,
                                             TOKEN_UNIT quantity) {
  this->channel_quantities[e].second = quantity;
  this->updateDistributionSize();
}

// Set initial token count of edge
void StorageDistribution::setInitialTokens(Edge e,
                                           TOKEN_UNIT token_count) {
  this->channel_quantities[e].first = token_count;
}


// Set distribution size of storage distribution
void StorageDistribution::setDistributionSize(TOKEN_UNIT sz) {
  this->distribution_size = sz;
}

// Set throughput of storage distribution
void StorageDistribution::setThroughput(TIME_UNIT thr) {
  this->thr = thr;
}

// Return channel quantity of edge
TOKEN_UNIT StorageDistribution::getChannelQuantity(Edge e) const {
  assert(channel_quantities.find(e) != channel_quantities.end()); // edge quantity not in map
  return this->channel_quantities.at(e).second;
}

// Return initial token count of edge
TOKEN_UNIT StorageDistribution::getInitialTokens(Edge e) const {
  assert(channel_quantities.find(e) != channel_quantities.end()); // edge quantity not in map
  return this->channel_quantities.at(e).first;
}

// Return distribution size of storage distribution
TOKEN_UNIT StorageDistribution::getDistributionSize() const {
  return this->distribution_size;
}

// Return throughput of storage distribution
TIME_UNIT StorageDistribution::getThroughput() const {
  return this->thr;
}

// Return number of edges
unsigned int StorageDistribution::getEdgeCount() const {
  return this->edge_count;
}

// Return set of edges
std::vector<Edge> StorageDistribution::getEdges() const {
  std::vector<Edge> edgeSet;
  for (auto it = this->channel_quantities.begin();
       it != this->channel_quantities.end(); it++) {
    edgeSet.push_back(it->first);
  }
  return edgeSet;
}

// Storage distributions are equal if every channel has the same quantity
bool StorageDistribution::operator==(const StorageDistribution& distribution) const {
  assert(this->getEdgeCount() == distribution.getEdgeCount());

  for (auto it = this->channel_quantities.begin();
       it != this->channel_quantities.end(); it++) {
    if (this->getChannelQuantity(it->first) !=
        distribution.getChannelQuantity(it->first)) {
      return false;
    }
  }
  return true;
}

bool StorageDistribution::operator!=(const StorageDistribution& distribution) const {
  // NOTE: could this overload be simplified to return !(this == distribution)?
  assert(this->getEdgeCount() == distribution.getEdgeCount());

  for (auto it = this->channel_quantities.begin();
       it != this->channel_quantities.end(); it++) {
    if (this->getChannelQuantity(it->first) !=
        distribution.getChannelQuantity(it->first)) {
      return true;
    }
  }
  return false;
}

/* 
 * Checks if distribution size has changed and updates it accordingly
 * Called whenever channel quantity has been altered with setChannelQuantity
 */
void StorageDistribution::updateDistributionSize() {
  TOKEN_UNIT new_dist_sz = 0;
  for (auto it = this->channel_quantities.begin();
       it != this->channel_quantities.end(); it++) {
    new_dist_sz += it->second.second;
  }
  if (new_dist_sz != this->getDistributionSize()) {
    this->setDistributionSize(new_dist_sz);
  }
}

// Prints member data of StorageDistribution for debugging
std::string StorageDistribution::printInfo(models::Dataflow* const dataflow) {
  std::string sdInfo;
  unsigned int ch_count = 0;

  sdInfo += "\tCurrent StorageDistribution info:\n";
  sdInfo += "\tNumber of edges: " + std::to_string(this->edge_count) + "\n";
  sdInfo += "\tChannel quantities:\n";  
  // for (auto it = this->channel_quantities.begin();
  //      it != this->channel_quantities.end(); it++) {
  //   if (!ch_count) {
  //     sdInfo += "\t";
  //   }
  //   sdInfo += std::to_string(it->second.second) + " ";
  //   ch_count++;
  //   if (ch_count == (this->edge_count / 2)) {
  //     sdInfo += "\n\t";
  //   }
  // }
  {ForEachEdge(dataflow, c) {
      if (!ch_count) {
	sdInfo += "\t";
      }
      sdInfo += std::to_string(this->getChannelQuantity(c)) + " ";
      ch_count++;
      if (ch_count == (this->edge_count / 2)) {
	sdInfo += "\n\t";
      }
    }}
  sdInfo += "\n"; // double line return to mark end of channel quantities
  sdInfo += "\tDistribution size: " + std::to_string(this->getDistributionSize()) + "\n";
  sdInfo += "\tThroughput: " + timeToString(this->getThroughput()) + "\n";
  std::cout << "test: " << this->getThroughput() << std::endl;
  
  return sdInfo;
}

std::string StorageDistribution::print_quantities_csv(models::Dataflow* const dataflow) {
  std::string output("\"");
  std::string delim("");
  unsigned int ch_count = 0;
  // for (auto &it : this->channel_quantities) {
  //   if (ch_count >= (this->edge_count / 2)) {
  //     output += delim;
  //     output += std::to_string(it.second.second);
  //     delim = ",";
  //   }
  //   ch_count++;
  // }
  // output += "\"";
  {ForEachEdge(dataflow, c) {
      if (ch_count >= (this->edge_count / 2)) {
	output += delim;
	output += std::to_string(this->getChannelQuantity(c));
	delim = ",";
      }
      ch_count++;
    }}
  output += "\"";
  return output;
}

// Print a DOT file of the given graph modelled with a storage distribution
std::string StorageDistribution::printGraph(models::Dataflow* const dataflow) {
  // copy graph to model
  /* NOTE doesn't work with a copy:
     can't find channel quantity for given edges  */
  // models::Dataflow* modelledGraph = new models::Dataflow(*dataflow);
  dataflow->reset_computation();
  {ForEachEdge(dataflow, c) {
      // FIXME feels kinda hacky to be using half of the edge count to check for modelled edges
      if (dataflow->getEdgeId(c) > (dataflow->getEdgesCount() / 2)) { /* don't deal with non-modelled edges
                                                                         as those quantities don't change */
        dataflow->setPreload(c, (this->getChannelQuantity(c) -
                                 this->getInitialTokens(c))); // subtract initial tokens in buffer
      }
    }}
  return printers::GenerateGraphDOT(dataflow);
}

StorageDistributionSet::StorageDistributionSet() {
  // TODO: Implement default constructor
}

StorageDistributionSet::StorageDistributionSet(TOKEN_UNIT dist_sz,
                                               StorageDistribution distribution) {
  // FIXME: initialize map (using initializer_list?)// : set{{dist_sz, distribution}} {
  this->set[dist_sz].push_back(distribution);
  // TODO: initialize p_max variable
  this->p_max = std::make_pair(dist_sz, distribution.getThroughput());
}

/*
 * Add a new storage distribution to the set of storage distributions
 * Will not add storage distribution if:
 * - An identical storage distribution already exists in the set
 */
void StorageDistributionSet::addStorageDistribution(StorageDistribution new_distribution) {
  VERBOSE_DSE("\t\tAttempting to add storage distribution of dist sz: "
              << new_distribution.getDistributionSize()
              << " to set" << std::endl);
  if (!hasDistribution(new_distribution.getDistributionSize())) { // first storage distribution of this distribution size
    VERBOSE_DSE("\t\t\tFirst of this distribution size: adding new distribution" << std::endl);
    this->set[new_distribution.getDistributionSize()].push_back(new_distribution);
  } else { // there's already a storage distribution with the same distribution size
    for (auto &distribution : this->set[new_distribution.getDistributionSize()]) {
      // don't add storage distributions that are already in checklist
      if (new_distribution == distribution) {
        VERBOSE_DSE("\t\t\tFound matching distribution: not adding new distribution" << std::endl);
        return;
      }
    }
    // no matching storage distributions of equal distribution size
    this->set[new_distribution.getDistributionSize()].push_back(new_distribution);
    VERBOSE_DSE("\t\t\tNew storage distribution added!" << std::endl);
  }

  // update maximum throughput and corresponding distribution size in set
  if (new_distribution.getThroughput() > this->p_max.second) {
    this->p_max = std::make_pair(new_distribution.getDistributionSize(),
                                 new_distribution.getThroughput());
  }
}

// Removes the given storage distribution from the storage distribution set
void StorageDistributionSet::removeStorageDistribution(StorageDistribution dist_to_rm) {
  // Use erase-remove idiom to remove matching storage distribution
  this->set[dist_to_rm.getDistributionSize()].erase(std::remove(this->set[dist_to_rm.getDistributionSize()].begin(),
                                                                this->set[dist_to_rm.getDistributionSize()].end(),
                                                                dist_to_rm), this->set[dist_to_rm.getDistributionSize()].end());
  
  // remove distribution size from set if there aren't any storage distributions left
  if (this->set[dist_to_rm.getDistributionSize()].empty()) {
    removeDistributionSize(dist_to_rm.getDistributionSize());
  }
}

// Removes all storage distributions of the given distribution size from set
void StorageDistributionSet::removeDistributionSize(TOKEN_UNIT dist_sz) {
  this->set.erase(dist_sz);
}

StorageDistribution StorageDistributionSet::getNextDistribution() const {
  return this->set.begin()->second.front();
}

// returns number of storage distributions in set
size_t StorageDistributionSet::getSize() const {
  size_t total_size = 0;
  for (auto &d : this->set) {
    total_size += d.second.size();
  }
  return total_size;
}

// returns a copy of the set of storage distributions
std::map<TOKEN_UNIT, std::vector<StorageDistribution>> StorageDistributionSet::getSet() {
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
  return reference_set;
}

/* Removes the new storage distribution from the storage distribution set if:
   - there already is a storage distribution with equal distribution size with 
   throughput >= to new storage distribution's throughput
   OR
   - the new storage distribution has throughput that is <= throughput of storage 
   distributions with a smaller distribution size
*/
void StorageDistributionSet::minimizeStorageDistributions(StorageDistribution newDist) {
  // FIXME: get rid of nested for-loop
  // get distribution size and throughput early to avoid repeated function calls
  TOKEN_UNIT newDistSz = newDist.getDistributionSize();
  TIME_UNIT newThr = newDist.getThroughput();
  // make copy of minimal distribution set
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
  
  // remove non-minimal storage distributions
  /* NOTE: we don't need to update the p_max values here as the conditions ensure
     that we always store the max throughput with the minimum distribution size
     i.e. the only way we would have a falsely high p_max is if we add a storage
     distribution with an absurdly high distribution size and thus get max 
     throughput --- that is, we will only need to consider updating p_max outside
     of the addStorageDistribution function if we don't increment by minStepSizes */
  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      if ((newDistSz > storage_dist.getDistributionSize() &&
           (newThr < storage_dist.getThroughput() ||
            commons::AreSame(newThr, storage_dist.getThroughput()))) || // needed to compare floats
          (newDistSz >= storage_dist.getDistributionSize() &&
           newThr < storage_dist.getThroughput())) {
        VERBOSE_DSE("\t\tNew storage distribution of size "
                    << newDist.getDistributionSize()
                    << " found to be non-minimal: removing from set"
                    << std::endl);
        this->removeStorageDistribution(newDist);
        return; // no need to iterate through rest of set once newDist has been removed
      } else if (newDistSz == storage_dist.getDistributionSize() && // remove existing equal storage dist (size) with lower thr
                 newThr > storage_dist.getThroughput()) {
        VERBOSE_DSE("\t\tExisting storage distribution of size "
                    << storage_dist.getDistributionSize()
                    << " found to be non-minimal: removing from set"
                    << std::endl);
        this->removeStorageDistribution(storage_dist);
      }
    }
  }
}

/* Check if storage distributions of a given distribution size exist in the 
   storage distribution set 
   Returns true if storage distribution of the given distribution size is found */
bool StorageDistributionSet::hasDistribution(TOKEN_UNIT dist_sz) {
  return (this->set.find(dist_sz) != this->set.end());
}

// returns true if the given SD is in the SD set
bool StorageDistributionSet::hasStorageDistribution(StorageDistribution checkDist) {
  bool isInSet = false;
  for (auto &dist : this->set) {
    for (auto &sd : dist.second) {
      if (checkDist == sd) {
        isInSet = true;
      }
    }
  }
  return isInSet;
}

// Check if DSE completion conditions have been met
bool StorageDistributionSet::isSearchComplete(StorageDistributionSet checklist,
                                              TIME_UNIT target_thr) {
  /* search is complete when the max throughput has been found and there 
     isn't any more storage distributions of the same distribution size
     left to check */
  return ((commons::AreSame(this->p_max.second, target_thr) &&
          (!checklist.hasDistribution(this->p_max.first))) ||
          (checklist.getSize() <= 0)); // also end when we're out of distributions to check
}


/* returns true if given SD is within the backward cone of the set of SDs
   (i.e. given SD is not maximal in set) */
bool StorageDistributionSet::isInBackCone(StorageDistribution checkDist,
                                          std::map<Edge, TOKEN_UNIT> bufferLb) {
  bool isMinimal;
  std::vector<Edge> checkDistEdges = checkDist.getEdges();
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);

  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      if (checkDist != storage_dist) { // don't check against itself
        isMinimal = true;
        for (auto it = checkDistEdges.begin();
             it != checkDistEdges.end(); it++) {
          if ((checkDist.getChannelQuantity(*it) > storage_dist.getChannelQuantity(*it)) &&
              (checkDist.getChannelQuantity(*it) > bufferLb[*it])) {
            isMinimal = false;
          }
        }
      }
      /* if isMinimal = true at this point, should remove checkDist as it's def non-maximal
         if isMinimal = false at this point, should check the remaining SDs as there could
         still be another SD that is strictly larger than checkDist */
      if (isMinimal) {
        std::cout << "SD of size " << checkDist.getDistributionSize()
                  << " found to be non-maximal in set" << std::endl;
        return isMinimal;
      }
    }
  }
  isMinimal = false; // if checkDist hasn't been removed at this point, then it's not in the backward cone
  return isMinimal;
}


/* returns true if given SD is within the foreward cone of the set of SDs
   (i.e. given SD is not minimal in set) */
bool StorageDistributionSet::isInForeCone(StorageDistribution checkDist) {
  bool isMaximal;
  std::vector<Edge> checkDistEdges = checkDist.getEdges();
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);

  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      if (checkDist != storage_dist) { // don't check against itself
        isMaximal = true;
        for (auto it = checkDistEdges.begin();
             it != checkDistEdges.end(); it++) {
          if (checkDist.getChannelQuantity(*it) < storage_dist.getChannelQuantity(*it)) {
            isMaximal = false;
          }
        }
        /* if isMaximal = true at this point, should remove checkDist as it's def non-minimal
           if isMaximal = false at this point, should check the remaining SDs as there could
           still be another SD that is strictly smaller than checkDist */
        if (isMaximal) {
          std::cout << "SD of size " << checkDist.getDistributionSize()
                    << " found to be non-minimal in set" << std::endl;
          return isMaximal;
        } else {
          isMaximal = true; // reset for check against next SD in set
        }
      }
    }
  }
  isMaximal = false; // if checkDist hasn't been removed at this point, then it's not in the foreward cone
  return isMaximal;
}

void StorageDistributionSet::removeNonMaximum(StorageDistribution checkDist,
                                              std::map<Edge, TOKEN_UNIT> bufferLb) {
  // bool isMinimal = true;
  std::cout << "Removing non-maximal SDs..." << std::endl;
  if (this->getSize() <= 1) {
    std::cout << "0/1 SD in set; skipping check" << std::endl;
    return; // only check if there's more than one SD in set
  }
  else if (this->isInBackCone(checkDist, bufferLb)) {
    this->removeStorageDistribution(checkDist);
  }
}


void StorageDistributionSet::removeNonMinimum(StorageDistribution checkDist) {
  // bool isMaximal = true;
  // std::vector<Edge> checkDistEdges = checkDist.getEdges();
  // std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
  std::cout << "Removing non-minimal SDs..." << std::endl;
  if (this->getSize() <= 1) {
    std::cout << "0/1 SD in set; skipping check" << std::endl;
    return; // only check if there's more than one SD in set
  }
  else if (this->isInForeCone(checkDist)) {
    this->removeStorageDistribution(checkDist);
  }
}


// Updates the given set of knee points according to the given set of infeasible SDs
// the new SD point
void StorageDistributionSet::updateKneeSet(models::Dataflow* const dataflow,
                                           StorageDistributionSet infeasibleSet,
                                           StorageDistributionSet feasibleSet,
                                           std::map<Edge, TOKEN_UNIT> bufferLb) {
                                           // StorageDistribution newDist) {
  StorageDistributionSet checkQueue(infeasibleSet); // store queue of SDs to be compared
  StorageDistributionSet checkedSDs;
  // go through every permutation of pairs of SDs (including newly found knee points) and find local min
  /* NOTE this is a little inefficient as it generates a whole new set of knee points every time it's called
     - might be better to find a way if a new point would affect the current knee set in any way before iterating */
  bool checkFinished = false;
  if (infeasibleSet.getSize() > 1) { // no need to check if there's only one point
    while(!checkFinished) {
      StorageDistribution checkDist;
      if (!checkedSDs.getSize()) {
        checkDist = checkQueue.getNextDistribution();
      } else {
        for (auto &d_sz_a : checkQueue.getSet()) {
          for (auto &sd_a : d_sz_a.second) {
            bool isChecked = false;
            for (auto &d_sz_b : checkedSDs.getSet()) {
              for (auto &sd_b : d_sz_b.second) {
                if (sd_a == sd_b) {
                  isChecked = true;
                }
              }
            }
            if (!isChecked) {
              checkDist = sd_a;
            }
          }
        }
      }
      std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set = checkQueue.getSet();
      for (auto &distribution_sz : reference_set) {
        for (auto &storage_dist : distribution_sz.second) {
          if (checkDist != storage_dist) { // don't check against itself
            StorageDistribution kneePoint = makeMinimalSD(checkDist, storage_dist);
            this->addStorageDistribution(kneePoint);
            checkQueue.addStorageDistribution(kneePoint);
          }
        }
      }
      checkedSDs.addStorageDistribution(checkDist);
      if (checkQueue.getSet().size() == checkedSDs.getSet().size()) { // TODO replace this stand-in SD set equality check
        checkFinished = true;
        for (auto &d_sz : checkQueue.getSet()) {
          for (auto &sd : d_sz.second) {
            if (!checkedSDs.hasStorageDistribution(sd)) {
              checkFinished = false;
            }
          }
        }
      }
      // checkFinished = (checkQueue.getSet() == checkedSDs.getSet());
    }
  }
  this->addEdgeKnees(dataflow, infeasibleSet, feasibleSet, bufferLb);
  // remove knee points in backwards cone of knee set
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      this->removeNonMaximum(storage_dist, bufferLb);
    }
  }
}


// Adds the edge cases to the knee set
void StorageDistributionSet::addEdgeKnees(models::Dataflow* const dataflow,
                                          StorageDistributionSet infeasibleSet,
                                          StorageDistributionSet feasibleSet,
                                          std::map<Edge, TOKEN_UNIT> bufferLb) {
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set = infeasibleSet.getSet();
  StorageDistribution maximalSD(infeasibleSet.getNextDistribution());
  std::vector<Edge> edges = maximalSD.getEdges();
  // find maximal buffer sizes for all buffers
  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      for (auto it = edges.begin(); it != edges.end(); it++) {
        if (storage_dist.getChannelQuantity(*it) > maximalSD.getChannelQuantity(*it)) {
          maximalSD.setChannelQuantity(*it, storage_dist.getChannelQuantity(*it));
        }
      }
    }
  }
  // construct and add edge knee points
  {ForEachEdge(dataflow, c) {
      if (dataflow->getEdgeId(c) > dataflow->getEdgesCount() / 2) {
        StorageDistribution tempSD(maximalSD);
        tempSD.setChannelQuantity(c, bufferLb[c]);
        if (!infeasibleSet.hasStorageDistribution(tempSD) &&
            !feasibleSet.hasStorageDistribution(tempSD) &&
            tempSD != maximalSD) {
          std::cout << "Adding edge knee:" << std::endl;
          std::cout << tempSD.printInfo(dataflow) << std::endl;
          this->addStorageDistribution(tempSD);
        }
      }
    }}
}

// add new SD to set of infeasible SDs
void StorageDistributionSet::updateInfeasibleSet(StorageDistribution newDist,
                                                 std::map<Edge, TOKEN_UNIT> bufferLb) {
  TOKEN_UNIT newDistSz = newDist.getDistributionSize();
  TOKEN_UNIT maxDistSz;
  bool isFound = false; // track if newDist found to be maximal

  // identify current maximum distribution size
  if (!this->getSize()) { // if there aren't any other SDs in set, just add
    std::cout << "Adding SD of size " << newDist.getDistributionSize()
              << " to U" << std::endl;
    this->addStorageDistribution(newDist);
    std::cout << "Distribution sizes of U (" << this->getSize() << " distributions):" << std::endl;
    for (auto &distribution_sz : this->set) {
      for (auto &storage_dist : distribution_sz.second) {
        std::cout << storage_dist.getDistributionSize() << std::endl;
      }
    }
    return;
  } else {
    maxDistSz = this->set.rbegin()->first; // store largest SD
    std::cout << "Max size: " << maxDistSz << std::endl;
  }
  
  // if new SD has larger distribution size, then can definitely add
  if (newDistSz >= maxDistSz) {
    std::cout << "Adding SD of size " << newDist.getDistributionSize()
              << " to U" << std::endl;
    this->addStorageDistribution(newDist);
    std::cout << "Distribution sizes of U (" << this->getSize() << " distributions):" << std::endl;
    for (auto &distribution_sz : this->set) {
      for (auto &storage_dist : distribution_sz.second) {
        std::cout << storage_dist.getDistributionSize() << std::endl;
      }
    }
  } else {
    std::vector<Edge> newDistEdges = newDist.getEdges();
    std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
    // only add new SD if at least one channel has larger size than an SD currently in infeasible set
    for (auto &distribution_sz : reference_set) {
      for (auto &storage_dist : distribution_sz.second) {
        for (auto it = newDistEdges.begin();
             it != newDistEdges.end(); it++) {
          if (newDist.getChannelQuantity(*it) > storage_dist.getChannelQuantity(*it) &&
              !isFound) {
            std::cout << "Adding SD of size " << newDist.getDistributionSize()
                      << " to U" << std::endl;
            isFound = true;
            this->addStorageDistribution(newDist);
            std::cout << "Distribution sizes of U (" << this->getSize() << " distributions):" << std::endl;
            for (auto &distribution_sz : this->set) {
              for (auto &storage_dist : distribution_sz.second) {
                std::cout << storage_dist.getDistributionSize() << std::endl;
              }
            }
            break;
          }
        }
      }
    }
  }
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
  // remove SDs subsumed by edge of infeasible set (non-maximal SDs)
  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      this->removeNonMaximum(storage_dist, bufferLb);
    }
  }
  std::cout << "Distribution sizes of U (" << this->getSize() << " distributions):" << std::endl;
  for (auto &distribution_sz : this->set) {
    for (auto &storage_dist : distribution_sz.second) {
      std::cout << storage_dist.getDistributionSize() << std::endl;
    }
  }
}


// add new SD to set of feasible SDs
void StorageDistributionSet::updateFeasibleSet(StorageDistribution newDist) {
  TOKEN_UNIT newDistSz = newDist.getDistributionSize();
  TOKEN_UNIT minDistSz;
  bool isFound = false; // track if newDist found to be minimal

  // identify current minimum distribution size
  if (!this->getSize()) {
    std::cout << "Adding SD of size " << newDist.getDistributionSize()
              << " to S" << std::endl;
    this->addStorageDistribution(newDist); // if there aren't any other SDs in set, just add
    std::cout << "Distribution sizes of S (" << this->getSize() << " distributions):" << std::endl;
    for (auto &distribution_sz : this->set) {
      for (auto &storage_dist : distribution_sz.second) {
        std::cout << storage_dist.getDistributionSize() << std::endl;
      }
    }
    return;
  } else {
    minDistSz = this->set.begin()->first; // store smallest SD
    std::cout << "Min size: " << minDistSz << std::endl;
  }
  
  // if new SD has smaller distribution size, then can definitely add
  if (newDistSz <= minDistSz) {
    std::cout << "Adding SD of size " << newDist.getDistributionSize()
              << " to S" << std::endl;
    this->addStorageDistribution(newDist);
    std::cout << "Distribution sizes of S (" << this->getSize() << " distributions):" << std::endl;
    for (auto &distribution_sz : this->set) {
      for (auto &storage_dist : distribution_sz.second) {
        std::cout << storage_dist.getDistributionSize() << std::endl;
      }
    }
  } else {
    std::vector<Edge> newDistEdges = newDist.getEdges();
    std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
    // only add new SD if at least one channel has smaller size than an SD currently in feasible set
    for (auto &distribution_sz : reference_set) {
      for (auto &storage_dist : distribution_sz.second) {
        for (auto it = newDistEdges.begin();
             it != newDistEdges.end(); it++) {
          if (newDist.getChannelQuantity(*it) < storage_dist.getChannelQuantity(*it) &&
              !isFound) {
            std::cout << "Adding SD of size " << newDist.getDistributionSize()
                      << " to S" << std::endl;
            isFound = true;
            this->addStorageDistribution(newDist);
            std::cout << "Distribution sizes of S (" << this->getSize() << " distributions):" << std::endl;
            for (auto &distribution_sz : this->set) {
              for (auto &storage_dist : distribution_sz.second) {
                std::cout << storage_dist.getDistributionSize() << std::endl;
              }
            }
            break;
          }
        }
      }
    }
  }
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> reference_set(this->set);
  // remove SDs subsumed by edge of feasible set (non-minimal SDs)
  for (auto &distribution_sz : reference_set) {
    for (auto &storage_dist : distribution_sz.second) {
      this->removeNonMinimum(storage_dist);
    }
  }
  std::cout << "Distribution sizes of S(" << this->getSize() << " distributions):" << std::endl;
  for (auto &distribution_sz : this->set) {
    for (auto &storage_dist : distribution_sz.second) {
      std::cout << storage_dist.getDistributionSize() << std::endl;
    }
  }
}

// Print info of all storage distributions of a given distribution size in set
std::string StorageDistributionSet::printDistributions(TOKEN_UNIT dist_sz,
						       models::Dataflow* const dataflow) {
  assert(set.find(dist_sz) != set.end());

  std::string dInfo;
  dInfo += "Printing storage distributions of distribution size: "
    + std::to_string(dist_sz) + "\n";
  for (auto &i : set[dist_sz]) {
    dInfo += i.printInfo(dataflow);
  }
  return dInfo;
}

// Print info of all storage distributions in set
std::string StorageDistributionSet::printDistributions(models::Dataflow* const dataflow) {
  std::string allInfo;
  for (auto &it : this->set) {
    allInfo += printDistributions(it.first, dataflow);
  }
  return allInfo;
}

/* Writes storage distribution set info to a CSV file to plot data
   Takes in file name as argument --- explicitly state file 
   format (e.g. "example_filename.csv") */
void StorageDistributionSet::writeCSV(std::string filename,
				      models::Dataflow* const dataflow) {
  std::ofstream outputFile;
  outputFile.open(filename);
  outputFile << "storage distribution size,throughput,channel quantities" << std::endl; // initialise headers
  for (auto &it : this->set) {
    for (auto &sd : this->set[it.first]) {
      outputFile << it.first << "," << sd.getThroughput() << ","
		 << sd.print_quantities_csv(dataflow) << std::endl;
    }
  }
  outputFile.close();
}

/* Iterate through storage distribution set and print graphs
   in DOT format */
void StorageDistributionSet::printGraphs(models::Dataflow* const dataflow,
                                         std::string pathName) {
  std::ofstream outputFile;
  int graphCount = 0;

  for (auto &it : this->set) {
    for (auto &sd : this->set[it.first]) {
      std::string fileName = pathName + dataflow->getGraphName() +
        "_" + std::to_string(graphCount) + ".dot";
      outputFile.open(fileName);
      outputFile << sd.printGraph(dataflow);
      outputFile.close();
      graphCount++;
    }
  }
}

// Returns the minimum step size for each channel in the given dataflow graph
void findMinimumStepSz(models::Dataflow *dataflow,
                       std::map<Edge, TOKEN_UNIT> &minStepSizes) {
  VERBOSE_DSE("Calculating minimal channel step sizes..." << std::endl);
  {ForEachEdge(dataflow, c) { // get GCD of all possible combinations of rates of production and consumption in channel
      TOKEN_UNIT minStepSz;
      minStepSz = dataflow->getEdgeInVector(c)[0]; // initialise with first value
      for (EXEC_COUNT i = 0; i < dataflow->getEdgeInPhasesCount(c); i++)
        minStepSz = boost::math::gcd(minStepSz, dataflow->getEdgeInVector(c)[i]);
      for (EXEC_COUNT i = 0; i < dataflow->getEdgeOutPhasesCount(c); i++)
        minStepSz = boost::math::gcd(minStepSz, dataflow->getEdgeOutVector(c)[i]);
      minStepSizes[c] = minStepSz;
      VERBOSE_DSE("Min. step size for channel " << dataflow->getEdgeName(c)
                  << ": " << minStepSz << std::endl);
    }}
}

/* Returns the minimum channel size for each channel for which we might have non-zero throughput
   in the given dataflow graph */
void findMinimumChannelSz(models::Dataflow *dataflow,
                          std::map<Edge,
                          std::pair<TOKEN_UNIT, TOKEN_UNIT>> &minChannelSizes) {
  VERBOSE_DSE("Calculating minimal channel sizes (for postive throughput)..." << std::endl);
  
  {ForEachEdge(dataflow, c) {
      // initialise channel size to maximum int size
      minChannelSizes[c].second = INT_MAX; // NOTE (should use ULONG_MAX but it's a really large value)
      TOKEN_UNIT ratePeriod = (TOKEN_UNIT) boost::math::gcd(dataflow->getEdgeInPhasesCount(c),
                                                            dataflow->getEdgeOutPhasesCount(c));
      
      for (TOKEN_UNIT i = 0; i < ratePeriod; i++) {
        // might want to change variables to p, c, and t for legibility
        TOKEN_UNIT tokensProduced = dataflow->getEdgeInVector(c)[i % dataflow->getEdgeInPhasesCount(c)];
        TOKEN_UNIT tokensConsumed = dataflow->getEdgeOutVector(c)[i % dataflow->getEdgeOutPhasesCount(c)];
        TOKEN_UNIT tokensInitial = dataflow->getPreload(c);
        VERBOSE_DSE("p, c, t: " << tokensProduced << ", "
                    << tokensConsumed << ", " << tokensInitial << std::endl);
        TOKEN_UNIT lowerBound;

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
        if (lowerBound < minChannelSizes[c].second) {
          minChannelSizes[c].second = lowerBound;
        }
      }
      VERBOSE_DSE("Minimum channel size for " << dataflow->getEdgeName(c)
                  << ": " << minChannelSizes[c].second << std::endl);
    }}
}

/*
 * Returns the distribution size of a given graph by summing the channel quantities 
 * on every channel
 */
TOKEN_UNIT findMinimumDistributionSz(models::Dataflow *dataflow,
                                     std::map<Edge,
                                     std::pair<TOKEN_UNIT, TOKEN_UNIT>> minChannelSizes) {
  TOKEN_UNIT minDistributionSize = 0;
  
  for (auto it = minChannelSizes.begin(); it != minChannelSizes.end(); it++) {
    minDistributionSize += it->second.second;
  }
  
  VERBOSE_DSE("Lower bound distribution size: " << minDistributionSize << std::endl);
  return minDistributionSize;
}

// Calculates and writes necessary information for DSE algorithm to input arguments
void initSearchParameters(models::Dataflow *dataflow,
                          std::map<Edge, TOKEN_UNIT> &minStepSizes,
                          std::map<Edge,
                          std::pair<TOKEN_UNIT, TOKEN_UNIT>> &minChannelSizes) {
  findMinimumStepSz(dataflow, minStepSizes);
  findMinimumChannelSz(dataflow, minChannelSizes);
}

// convert TIME_UNIT to string, accounting for really small numbers
std::string timeToString(TIME_UNIT t)
{
  std::stringstream s;
  s << std::setprecision(6) << t;
  return s.str();
}

// compares 2 storage distributions and returns a SD that is the local minima of the two
// NOTE this assumes that both storage distributions have the same number of edges
StorageDistribution makeMinimalSD(StorageDistribution sd1,
                                  StorageDistribution sd2) {
  StorageDistribution minSD(sd1);
  std::vector<Edge> edgeSet = sd1.getEdges(); // use edges from sd1
  for (auto it = edgeSet.begin(); it != edgeSet.end(); it++) {
    // NOTE might need to set initial tokens to 0
    minSD.setChannelQuantity(*it, std::min(sd1.getChannelQuantity(*it),
                                           sd2.getChannelQuantity(*it)));
  }
  return minSD;
}


/* updates infeasible set and set of knee points given a new SD,
   also reduces search space using critical edges */
void handleInfeasiblePoint(models::Dataflow* const dataflow,
                           StorageDistributionSet &infeasibleSet,
                           StorageDistributionSet feasibleSet,
                           StorageDistributionSet &kneeSet,
                           StorageDistribution newSD,
                           kperiodic_result_t deps,
                           std::map<Edge, TOKEN_UNIT> &bufferLb) {
  if ((deps.critical_edges).empty()) {
    std::cerr << "ERROR: throughput requirement unreachable (no critical edges found)" << std::endl;
  }
  StorageDistribution checkSD(newSD);
  std::vector<Edge> edges = checkSD.getEdges();
  std::set<Edge> dependencies = deps.critical_edges;
  int nDeps = 0;
  for (auto edge : edges) {
    if (dataflow->getEdgeId(edge) > (edges.size() / 2)) {
      if (dependencies.find(edge) != dependencies.end()) {
        nDeps++;
      }
    }
  }
  std::cout << "Number of dependencies: " << nDeps << std::endl;
  if (nDeps == (edges.size() / 2)) {
    std::cout << "No need to update lower bounds as no non-critical channels" << std::endl;
  } else {
    std::cout << "Updating lower bounds..." << std::endl;
    for (auto edge : edges) {
      if (dataflow->getEdgeId(edge) > (edges.size() / 2)) { // only consider edges modelling bounded buffers
        if (dependencies.find(edge) != dependencies.end()) { // for edges with storage dependencies
          if (bufferLb[edge] < checkSD.getChannelQuantity(edge)) { // never decrease bounds
            std::cout << "Increasing Lb of " << dataflow->getEdgeName(edge)
                      << " to " << checkSD.getChannelQuantity(edge) << std::endl;
            bufferLb[edge] = checkSD.getChannelQuantity(edge);
          }
        }
      }
    }
  }

  std::cout << "SD sent to updateInfeasible is dist sz: " << checkSD.getDistributionSize() << std::endl;
  infeasibleSet.updateInfeasibleSet(checkSD, bufferLb);
  std::cout << "Updating knee set given updated U" << std::endl;
  kneeSet.updateKneeSet(dataflow, infeasibleSet, feasibleSet, bufferLb);
}

// BASE MONOTONIC OPTIMISATION FUNCTIONS
/* returns true if given SD is within the backward cone of the SD
   (i.e. given SD is not maximal in set) */
bool StorageDistribution::inBackConeOf(StorageDistribution checkDist) {
  for (auto &e : this->getEdges()) {
    if (this->getChannelQuantity(e) > checkDist.getChannelQuantity(e)) {
      return false;
    }
  }
  return true;
}

/* returns true if given SD is within the forward cone of the SD
   (i.e. given SD is not minimal in set) */
bool StorageDistribution::inForConeOf(StorageDistribution checkDist) {
  for (auto &e : this->getEdges()) {
    if (this->getChannelQuantity(e) < checkDist.getChannelQuantity(e)) {
      return false;
    }
  }
  return true;
}

// tries to add new SD to U; returns true if successful
bool StorageDistributionSet::addToUnsat(StorageDistribution sd) {
  StorageDistributionSet subsumed;
  for (auto &dist_sz : this->set) {
    for (auto &storage_dist : dist_sz.second) {
      if (storage_dist.inBackConeOf(sd) && storage_dist != sd) {
        subsumed.addStorageDistribution(storage_dist);
      } else if (sd.inBackConeOf(storage_dist)) {
        return false;
      }
    }
  }
  for (auto &dist_sz : subsumed.getSet()) {
    for (auto &storage_dist : dist_sz.second) {
      this->removeStorageDistribution(storage_dist);
    }
  }
  this->addStorageDistribution(sd);
  return true;
}

void StorageDistributionSet::add(StorageDistribution sd,
                                 StorageDistributionSet &kneePoints) {
  if (!this->addToUnsat(sd)) {
    std::cout << "BASE_M_OPT: SD not added to U" << std::endl;
    return;
  }
  // update knee points
  StorageDistributionSet subsumed;
  std::cout << "BASE_M_OPT: updating knees" << std::endl;
  for (auto &dist_sz : kneePoints.getSet()) {
    for (auto &knee : dist_sz.second) {
      if (knee.inBackConeOf(sd)) {
        subsumed.addStorageDistribution(knee);
      }
    }
  }
  // remove subsumed knees
  std::cout << "BASE_M_OPT: removing subsumed knees" << std::endl;
  for (auto &dist_sz : subsumed.getSet()) {
    for (auto &storage_dist : dist_sz.second) {
      kneePoints.removeStorageDistribution(storage_dist);
    }
  }
  // create extensions
  std::cout << "BASE_M_OPT: creating extensions" << std::endl;
  StorageDistributionSet extensions;
  for (auto &dist_sz : subsumed.getSet()) {
    for (auto &storage_dist : dist_sz.second) {
      for (auto &e : storage_dist.getEdges()) {
        StorageDistribution newExt(storage_dist);
        newExt.setChannelQuantity(e, sd.getChannelQuantity(e));
        addToExtensions(extensions, newExt);
      }
    }
  }
  std::cout << "BASE_M_OPT: adding extension knees" << std::endl;
  for (auto &dist_sz : extensions.getSet()) {
    for (auto &storage_dist : dist_sz.second) {
      kneePoints.addStorageDistribution(storage_dist);
    }
  }
  std::cout << "BASE_M_OPT: SD added to U" << std::endl;
  return;
}

void StorageDistributionSet::addCut(models::Dataflow *dataflow,
                                    StorageDistribution sd,
                                    kperiodic_result_t result,
                                    StorageDistributionSet &kneePoints) {
  // TODO need to check if there aren't any critical edges --- throw error if so
  if (!(result.critical_edges).empty()) {
    bool isCut = false;
    StorageDistribution tempDist(sd);
    {ForEachEdge(dataflow, e) {
        if (dataflow->getEdgeId(e) > dataflow->getEdgesCount()/2) {
          if ((result.critical_edges).find(e) == (result.critical_edges).end()) {
            tempDist.setChannelQuantity(e, INT_MAX);
            isCut = true;
          }
        }
      }}
    if (isCut) {
      this->add(tempDist, kneePoints);
    }
  }
}

void addToExtensions(StorageDistributionSet &extensions,
                     StorageDistribution sd) {
  StorageDistributionSet subsumed;
  for (auto &dist_sz : extensions.getSet()) {
    for (auto &storage_dist : dist_sz.second) {
      if (storage_dist.inForConeOf(sd)) {
        subsumed.addStorageDistribution(storage_dist);
      } else if (sd.inForConeOf(storage_dist)) {
        return;
      }
    }
  }
  for (auto &dist_sz : subsumed.getSet()) {
    for (auto &storage_dist : dist_sz.second) {
      extensions.removeStorageDistribution(storage_dist);
    }
  }
  extensions.addStorageDistribution(sd);
}
