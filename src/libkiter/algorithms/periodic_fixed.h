/*
 * throughput.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef PERIODIC_FIXED_H_
#define PERIODIC_FIXED_H_

namespace models {
	class Dataflow;
	class EventGraph;
}

namespace algorithms {

    bool ctnewMinMax(models::Dataflow * dataflow,  TIME_UNIT OMEGA, Vertex t, std::vector<TIME_UNIT>& res) ;
    void checkOffsets (models::Dataflow * const dataflow, TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & offsets) ;
    bool generateBurstOffsets(models::Dataflow * const dataflow,std::map<Vertex,std::vector<TIME_UNIT> > & res);
    bool generateAverageOffsets(models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res);
    bool generateMinMaxOffsets(models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res);
    bool generateWiggersOffsets(models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res);

    void compute_burst_memory                                   (models::Dataflow* const  dataflow, parameters_list_t params);
    void compute_average_memory                                 (models::Dataflow* const  dataflow, parameters_list_t params);
    void compute_minmax_memory                                  (models::Dataflow* const  dataflow, parameters_list_t params);
    void compute_wiggers_memory                                 (models::Dataflow* const  dataflow, parameters_list_t params);

    void compute_periodic_fixed_memory                          (models::Dataflow* const  dataflow, std::map<Vertex,std::vector<TIME_UNIT> > & res, parameters_list_t params);

} // end of namespace algorithms

#endif /* PERIODIC_FIXED_H_ */
