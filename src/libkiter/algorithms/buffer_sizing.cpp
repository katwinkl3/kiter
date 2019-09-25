/*
 * buffer_sizing.cpp
 *
 */

#include <boost/math/common_factor_rt.hpp>  // for boost::math::gcd, lcm
#include <algorithms/buffer_sizing.h>
#include <models/Dataflow.h>

StorageDistribution::StorageDistribution() {
  this->edge_count = 0;
  this->thr = 0;
  this->distribution_size = 0;
}

StorageDistribution::StorageDistribution(unsigned int edge_count, // TODO: Refactor constructors to use ":" syntax
                                         TIME_UNIT thr,
                                         std::map<Edge, TOKEN_UNIT> input_quantities,
                                         TOKEN_UNIT distribution_size) {
  this->edge_count = edge_count;
  this->thr = thr;
  this->channel_quantities = input_quantities;
  this->distribution_size = distribution_size; // TODO: check if can replace this with updateDistributionSize
}

void StorageDistribution::setChannelQuantity(Edge e,
                                             TOKEN_UNIT quantity) {
  this->channel_quantities[e] = quantity;
  this->updateDistributionSize();
}

void StorageDistribution::setDistributionSize(TOKEN_UNIT sz) {
  this->distribution_size = sz;
}

void StorageDistribution::setThroughput(TIME_UNIT thr) {
  this->thr = thr;
}

TOKEN_UNIT StorageDistribution::getChannelQuantity(Edge e) const {
  assert(channel_quantities.find(e) != channel_quantities.end()); // edge quantity not in map
  return this->channel_quantities.at(e);
}

TOKEN_UNIT StorageDistribution::getDistributionSize() const {
  return this->distribution_size;
}

TIME_UNIT StorageDistribution::getThroughput() const {
  return this->thr;
}

unsigned int StorageDistribution::getEdgeCount() const {
  return this->edge_count;
}

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
  // can this overload be simplified to return !(this == distribution)?
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
 * Called whenever channel quantity has been altered
 */
void StorageDistribution::updateDistributionSize() {
  TOKEN_UNIT new_dist_sz = 0;
  for (auto it = this->channel_quantities.begin();
       it != this->channel_quantities.end(); it++) {
    new_dist_sz += it->second;
  }
  if (new_dist_sz != this->getDistributionSize()) {
    this->setDistributionSize(new_dist_sz);
  }
}

// Prints member data of StorageDistribution for debugging
void StorageDistribution::print_info() { // BUG: I once got an inconsistent result here (expected sd: 2 6 6 2, actual: 2 2 6 6). should probably take a look but not sure how to recreate --- seems to have gone away after commenting out copy constructor declaration?
  std::cout << "Current StorageDistribution info:" << std::endl;
  std::cout << "Number of edges: " << this->edge_count << std::endl;
  std::cout << "Channel quantities: " << std::endl;
  for (auto it = this->channel_quantities.begin();
       it != this->channel_quantities.end(); it++) {
    std::cout << it->second << " ";
  }
  std::cout << std::endl;
  std::cout << "Distribution size: " << this->distribution_size << std::endl;
  std::cout << "Throughput: " << this->thr << std::endl;
}

StorageDistributionSet::StorageDistributionSet() {
  // TODO: Implement default constructor
}

StorageDistributionSet::StorageDistributionSet(TOKEN_UNIT dist_sz,
                                               StorageDistribution distribution) {
  this->set[dist_sz].push_back(distribution);
}

/*
 * Add a new storage distribution to the set of storage distributions
 * Will not add storage distribution if an identical one already exists in the set
 */
void StorageDistributionSet::addStorageDistribution(StorageDistribution new_distribution) {
  std::cout << "Attempting to add storage distribution of dist sz: "
            << new_distribution.getDistributionSize() << std::endl;
  if (this->set.find(new_distribution.getDistributionSize()) == this->set.end()) { // first storage distribution of this distribution size
    std::cout << "First of this distribution size: adding new distribution" << std::endl;
    this->set[new_distribution.getDistributionSize()].push_back(new_distribution);
  } else { // there's already a storage distribution with the same distribution size
    for (auto &distribution : this->set[new_distribution.getDistributionSize()]) {
      // don't add storage distributions that are already in checklist
      if (new_distribution == distribution) {
        std::cout << "Found matching distribution: not adding new distribution" << std::endl;
        return;
      }
    }
    // no matching storage distributions
    std::cout << "Adding new distribution" << std::endl;
    this->set[new_distribution.getDistributionSize()].push_back(new_distribution);
  }
}

void StorageDistributionSet::removeStorageDistribution( // TODO: check if erase-remove idiom is more efficient than manually checking
StorageDistribution dist_to_rm) {
  // Use erase-remove idiom to remove matching storage distribution
  this->set[dist_to_rm.getDistributionSize()].erase(std::remove(this->set[dist_to_rm.getDistributionSize()].begin(),
                                                                this->set[dist_to_rm.getDistributionSize()].end(),
                                                                dist_to_rm), this->set[dist_to_rm.getDistributionSize()].end());
  // remove distribution size from set if there aren't any storage distributions left
  if (this->set[dist_to_rm.getDistributionSize()].size() == 0) {
    std::cout << "No more storage dist of dist sz "
              << dist_to_rm.getDistributionSize()
              << ": removing key" << std::endl;
    removeDistributionSize(dist_to_rm.getDistributionSize());
  }
}

void StorageDistributionSet::removeDistributionSize(TOKEN_UNIT dist_sz) {
  this->set.erase(dist_sz);
}

StorageDistribution StorageDistributionSet::getNextDistribution() const {
  return this->set.begin()->second.front();
}

// returns number of unique distribution sizes in storage distribuiton set
size_t StorageDistributionSet::getSize() const {
  return this->set.size();
}

/* Removes the new storage distribution from the storage distribution set if:
   - there exists a storage distribution with equal distribution size with >= throughput 
   OR
   - the new storage distribution has throughput that is <= throughput of storage 
   distributions with a smaller distribution size
 */
void StorageDistributionSet::minimizeStorageDistributions(StorageDistribution newDist) {
  // compare to storage distributions with same distribution sizes
  for (auto &it : this->set[newDist.getDistributionSize()]) {
    if (newDist.getThroughput() < it.getThroughput()) {
      std::cout << "Found storage distribution with < max thr of set: removing "
                << newDist.getDistributionSize() << std::endl;
      this->removeStorageDistribution(newDist);
      return;
    }
  }
  // compare to storage distributions with smaller distribution sizes
  TOKEN_UNIT curr_dist_sz = newDist.getDistributionSize() - 1;
  while (curr_dist_sz >= this->set.begin()->first) { // bound by smallest distribution size
    for (auto &it : this->set[curr_dist_sz]) {
      if (newDist.getThroughput() <= it.getThroughput()) {
        std::cout << "Throughput of new storage distribution ("
                  << newDist.getDistributionSize()
                  << ") <= throughput of smaller storage distribution: removing"
                  << std::endl;
        this->removeStorageDistribution(newDist);
        return;
      }
    }
    curr_dist_sz--;
  }
}

void StorageDistributionSet::print_distributions(TOKEN_UNIT dist_sz) {
  assert(set.find(dist_sz) != set.end());

  std::cout << "Printing storage distributions of distribution size: "
            << dist_sz << std::endl;
  for (auto &i : set[dist_sz]) {
    i.print_info();
  }
}

void StorageDistributionSet::print_distributions() {
  for (auto &it : this->set) {
    print_distributions(it.first);
  }
}

// Returns the minimum step size for each channel in the given dataflow graph
void findMinimumStepSz(models::Dataflow *dataflow,
                       std::map<Edge, TOKEN_UNIT> &minStepSizes) {
  std::cout << "Calculating minimal channel step sizes..." << std::endl;
  // minStepSizes = new EXEC_COUNT [dataflow->getEdgesCount()]; // need to calculate minimum step size per channel
  std::cout << "Number of channels: " << dataflow->getEdgesCount() << std::endl;
  {ForEachEdge(dataflow, c) { // get GCD of all possible combinations of rates of production and consumption in channel
      TOKEN_UNIT minStepSz;
      minStepSz = dataflow->getEdgeInVector(c)[0]; // initialise with first value
      for (int i = 0; i < dataflow->getEdgeInPhasesCount(c); i++)
        minStepSz = boost::math::gcd(minStepSz, dataflow->getEdgeInVector(c)[i]);
      for (int i = 0; i < dataflow->getEdgeOutPhasesCount(c); i++)
        minStepSz = boost::math::gcd(minStepSz, dataflow->getEdgeOutVector(c)[i]);
      minStepSizes[c] = minStepSz;
      std::cout << "Min. step size for channel " << dataflow->getEdgeName(c)
                << ": " << minStepSz << std::endl;
  }}
  std::cout << "Minimum step sizes calculated!\n" << std::endl;
  // return minStepSizes;
}

void findMinimumChannelSz(models::Dataflow *dataflow,
                          std::map<Edge, TOKEN_UNIT> &minChannelSizes) {
  std::cout << "Calculating minimal channel sizes (for postive throughput)..."
            << std::endl;
  // minChannelSizes = new EXEC_COUNT [dataflow->getEdgesCount()];
  
  {ForEachEdge(dataflow, c) {
      // initialise channel size to maximum int size (should use ULONG_MAX but it's a really large value)
      minChannelSizes[c] = INT_MAX; // use (EdgeID - 1) to for array index
      TOKEN_UNIT ratePeriod = (TOKEN_UNIT) boost::math::gcd(dataflow->getEdgeInPhasesCount(c),
                                               dataflow->getEdgeOutPhasesCount(c));
      // std::cout << "Rate period for " << dataflow->getEdgeName(c) << ": "
      //           << ratePeriod << std::endl;
      for (int i = 0; i < ratePeriod; i++) { // might be able to use ForEachPhase here
        // might want to change variables to p, c, and t for legibility
        TOKEN_UNIT tokensProduced = dataflow->getEdgeInVector(c)[i % dataflow->getEdgeInPhasesCount(c)];
        TOKEN_UNIT tokensConsumed = dataflow->getEdgeOutVector(c)[i % dataflow->getEdgeOutPhasesCount(c)];
        TOKEN_UNIT tokensInitial = dataflow->getPreload(c);
        // std::cout << "p, c, t: " << tokensProduced << ", " << tokensConsumed << ", " << tokensInitial << std::endl;
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
        if (lowerBound < minChannelSizes[c]) {
          minChannelSizes[c] = lowerBound;
        }
      }
      std::cout << "Minimum channel size for " << dataflow->getEdgeName(c)
                << ": " << minChannelSizes[c] << std::endl;      
  }}
}

/*
 * Returns the distribution size of a given graph by summing the channel quantities 
 * on every channel
 */
TOKEN_UNIT findMinimumDistributionSz(models::Dataflow *dataflow,
                                     std::map<Edge, TOKEN_UNIT> minChannelSizes) {
  TOKEN_UNIT minDistributionSize = 0;
  // for (int i = 0; i < dataflow->getEdgesCount(); i++) {
  //   minDistributionSize += minChannelSizes[i];
  // }
  for (auto it = minChannelSizes.begin(); it != minChannelSizes.end(); it++) {
    minDistributionSize += it->second;
  }
  std::cout << "Lower bound distribution size: " << minDistributionSize << std::endl;
  return minDistributionSize;
}

void initSearchParameters(models::Dataflow *dataflow,
                          std::map<Edge, TOKEN_UNIT> &minStepSizes,
                          std::map<Edge, TOKEN_UNIT> &minChannelSizes) {
  findMinimumStepSz(dataflow, minStepSizes);
  findMinimumChannelSz(dataflow, minChannelSizes);

  // Cleanup (TEMPORARY)
  // delete [] minStepSizes;
  // delete [] minChannelSizes;
}

