/*
 * buffer_sizing.h
 *
 */

#ifndef BUFFER_SIZING_H_
#define BUFFER_SIZING_H_

namespace models {
  class Dataflow;
}

void findMinimumStepSz(models::Dataflow *dataflow);
void findMinimumChannelSz(models::Dataflow *dataflow);
void findMinimumDistributionSz(models::Dataflow *dataflow);
void findMaxThroughput(models::Dataflow *dataflow);

#endif /* BUFFER_SIZING_H_ */
