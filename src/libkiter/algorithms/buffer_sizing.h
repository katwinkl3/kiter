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

void findMinimumStepSz(models::Dataflow *dataflow, EXEC_COUNT *minStepSizes);
void findMinimumChannelSz(models::Dataflow *dataflow, EXEC_COUNT *minChannelSizes);
int findMinimumDistributionSz(models::Dataflow *dataflow,
                               EXEC_COUNT *minChannelSizes);
void initSearchParameters(models::Dataflow *dataflow,
                          EXEC_COUNT *minStepSizes,
                          EXEC_COUNT *minChannelSizes,
                          int &minDistributionSize);

#endif /* BUFFER_SIZING_H_ */
