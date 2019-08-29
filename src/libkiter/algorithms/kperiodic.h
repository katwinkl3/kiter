/*
 * kperiodic.h
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */

#ifndef KPERIODIC_H_
#define KPERIODIC_H_

namespace algorithms {

std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> generateKperiodicSchedule    (models::Dataflow* const dataflow , bool verbose) ;
    void print_kperiodic_scheduling         (models::Dataflow* const  dataflow, parameters_list_t);
    void print_kperiodic_expansion_graph    (models::Dataflow* const  dataflow, parameters_list_t);
  void generateKperiodicSelfloop(models::Dataflow * const dataflow , EXEC_COUNT ki, models::EventGraph* g  , Vertex t  );

  void generateKPeriodicConstraint(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,  models::EventGraph* g, Edge c) ;

  void fastGenerateKPeriodicConstraint(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,  models::EventGraph* g, Edge c) ;
  void newGenerateKPeriodicConstraint(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,  models::EventGraph* g, Edge c) ;

  models::EventGraph*  updateEventGraph( models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kvector, std::set<Edge>* cc, models::EventGraph* g, bool verbose) ;

models::EventGraph* generateKPeriodicEventGraphWithNormalize  (models::Dataflow *  const,  std::map<Vertex,EXEC_COUNT> * kvector, bool noRe  );
models::EventGraph* generateKPeriodicEventGraph               (models::Dataflow *  const,  std::map<Vertex,EXEC_COUNT> * kvector );


 void updateKPeriodicEventGraph(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,   std::set<Edge> * cc, models::EventGraph* g  );
 models::EventGraph*  generateCycleOnly(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,   std::set<Edge> * cc );

 std::map<Vertex,EXEC_COUNT>  get_Kvector(models::Dataflow *  const);
void                compute_Kperiodic_throughput              (models::Dataflow *  const , parameters_list_t);

    void                compute_1Kperiodic_throughput   		  (models::Dataflow* const  dataflow, parameters_list_t);
    void                compute_2Kperiodic_throughput             (models::Dataflow* const  dataflow, parameters_list_t);
    void                compute_NKperiodic_throughput             (models::Dataflow* const  dataflow, parameters_list_t);
    void                compute_GKperiodic_throughput  		      (models::Dataflow* const  dataflow, parameters_list_t);

    std::pair<TIME_UNIT, std::set<Edge> > KSchedule   (models::Dataflow *  const ,std::map<Vertex,EXEC_COUNT> * kvector  , TIME_UNIT bound = 0) ;

    void                                     updateVectorWithFullNi   (models::Dataflow *  const  ,std::map<Vertex,EXEC_COUNT> *  , std::set<Edge> * ) ;
    bool                                     updateVectorWithLocalNi  (models::Dataflow *  const  ,std::map<Vertex,EXEC_COUNT> *  , std::set<Edge> * ) ;
    bool 									 updateVectorWithFineNi   (models::Dataflow * const   , std::map<Vertex,EXEC_COUNT> * , std::set<Edge>*  );

    TIME_UNIT compute_Kperiodic_optimal_throughput    (models::Dataflow* const dataflow) ;

    
    void compute_KperiodicSlow_throughput    (models::Dataflow* const dataflow, parameters_list_t) ;
    void compute_KperiodicSlow2_throughput    (models::Dataflow* const dataflow, parameters_list_t) ;

    void print_1periodic_eventgraph                                 (models::Dataflow* const  dataflow, parameters_list_t);
    void print_2periodic_eventgraph                                 (models::Dataflow* const  dataflow, parameters_list_t);
    void print_NKperiodic_eventgraph                                (models::Dataflow *  const , parameters_list_t);
    EXEC_COUNT test_Kperiodic_throughput    (models::Dataflow* const dataflow, parameters_list_t) ;
    void compute_Kperiodic_throughput_dse (models::Dataflow* const  dataflow, parameters_list_t);
}

#endif /* KPERIODIC_H_ */
