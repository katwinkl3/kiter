/*
 * stdout.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <algorithms/repetition_vector.h>
#include <printers/stdout.h>
#include <commons/commons.h>
#include <commons/SDF3Wrapper.h>

std::string add_block ( std::string name , TIME_UNIT start, TIME_UNIT duration, double node_height,  double current_task_y_pos, double line_width =  0.1 , bool fill = false) {
		std::ostringstream returnStream;
		returnStream<< "\"" << name << 0  << "\""
				<< " [label=\"\", width=\"" << duration / 72 << "\", penwidth=\"" << line_width << "\"";
		if (fill) returnStream <<  ", style=\"filled\" , fillcolor=blue" ;
		returnStream <<  ", height=\"" << node_height << "\", fixedsize=true, pos=\"" << start + (duration/2) << ","<< current_task_y_pos << "\",shape=box]" << std::endl;
		return returnStream.str();
}

std::string printers::PeriodicScheduling2DOT    (models::Dataflow* const  dataflow,  models::Scheduling& sched ,   TIME_UNIT last_execution_end_at, bool full,  double xscale , double yscale ) {

  std::ostringstream returnStream;
  scheduling_t periodic_scheduling = sched.getTaskSchedule();
  static double label_node_width  =   1;
  static double label_node_height = yscale *  0.25;

  static double node_height = yscale * 0.25;
  static double pixel_start =  (-(72/2) - 1);
  static double line_width =  0.1;

  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with -Knop only!\n" << std::endl;
  returnStream << "digraph {\n" << std::endl;
  returnStream << "graph [sep = \"0\", nodesep = \"0\"];" << std::endl;


  if (last_execution_end_at == 0)
  {ForEachVertex(dataflow,t) {
      auto tid = dataflow->getVertexId(t); 
      //VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");
  	  auto Ni = dataflow->getNi(t);

  	  auto period = periodic_scheduling[tid].first;
  	  auto duration = dataflow->getVertexTotalDuration(t);
  	  auto starts = periodic_scheduling[tid].second;

        for (EXEC_COUNT iter = 0 ; iter < (Ni/(EXEC_COUNT)starts.size()) ; iter++) {
      	  for (auto  start : starts) {
      		last_execution_end_at = std::max(last_execution_end_at, duration + (start + period * iter));
      	  }
        }

      }}

  double idx = 0;
  {ForEachVertex(dataflow,t) {
      auto tid = dataflow->getVertexId(t); 
      //VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");

	  auto period = periodic_scheduling[tid].first;
	  auto starts = periodic_scheduling[tid].second;

	  double current_task_y_pos = yscale * -20 * idx ;

      returnStream << "\""<< dataflow->getVertexName(t) << "\"" << " [label=\"" <<  dataflow->getVertexName(t) << "\\r\", margin=\"0,0\", width=\"" << label_node_width << "\", height=\"" << label_node_height << "\", fixedsize=true, pos=\"" << pixel_start << ","<< current_task_y_pos << "\",shape=plaintext]" << std::endl;

      EXEC_COUNT execution_index = 0;

      bool fill_init  = true;
      bool fill_first = true;
      bool fill_per   = false;

      for (EXEC_COUNT init_phase = 0 ; init_phase < dataflow->getInitPhasesQuantity(t) ; init_phase++) {
    	  TIME_UNIT start = starts[init_phase];
		  auto duration = dataflow->getVertexInitDuration(t,init_phase+1);
		  returnStream << add_block (
				  dataflow->getVertexName(t) + std::to_string(execution_index++),
				  xscale * (start),
				  xscale * (duration),  node_height,   current_task_y_pos,  line_width , fill_init ) ;
      }


      EXEC_COUNT first_start_index = dataflow->getInitPhasesQuantity(t);

      for (EXEC_COUNT iter = 0 ;  ((starts[first_start_index] + period * iter)) < last_execution_end_at ; iter++) {
    	  for (EXEC_COUNT sidx = first_start_index; sidx < (EXEC_COUNT) starts.size() ; sidx++) {
    		  auto current_phase = 1 +  sidx % dataflow->getPhasesQuantity(t);
    		  auto duration = dataflow->getVertexDuration(t,current_phase);
    		  bool fill = iter == 0 ? fill_first : fill_per;
        	  TIME_UNIT start = starts[sidx];
    		  returnStream << add_block ( dataflow->getVertexName(t) + std::to_string(execution_index++),
    		     				  xscale * (start + period * iter),
    		 					  xscale * (duration),  node_height,   current_task_y_pos,  line_width , fill ) ;
    	  }
      }

      idx+=1;

    }}

  returnStream << std::endl;

  returnStream <<  "}" << std::endl;

  return returnStream.str();

}

void  printers::printGraphAsKiterScript (models::Dataflow* const  dataflow, parameters_list_t ) {


	  std::ostringstream returnStream;


	  returnStream << "// Auto-generate by Kiter for Kiter" << std::endl;
	  returnStream << "// " << std::endl;

	  returnStream << "std::cout << \"generate a dataflow graph...\" << std::endl;"  << std::endl;
	  returnStream << "models::Dataflow* new_graph = new models::Dataflow();" << std::endl;
	  returnStream << "" << std::endl;

	  {ForEachVertex(dataflow,t) {
		  PHASE_INDEX phase_count = dataflow->getPhasesQuantity(t);
		  returnStream << "{" << std::endl;
		  returnStream << "std::cout << \"generate a task ...\" << std::endl;"  << std::endl;
		  returnStream << "auto new_vertex = new_graph->addVertex(" << dataflow->getVertexId(t) << ");" << std::endl;

		  returnStream << " new_graph->setVertexName(new_vertex,\"" << dataflow->getVertexName(t) << "\");" << std::endl;
		  returnStream << " new_graph->setPhasesQuantity(new_vertex," <<phase_count << ");" << std::endl;
		  returnStream << " new_graph->setReentrancyFactor(new_vertex," << dataflow->getReentrancyFactor(t)<< "); " << std::endl;

		  std::string duration_string = "";
		  for (PHASE_INDEX phase = 1 ; phase <= phase_count ; phase++) {
			  if (phase > 1)  duration_string += ",";
			  duration_string += commons::toString<TIME_UNIT>(dataflow->getVertexDuration(t,phase));
		  }
		  duration_string = "{" + duration_string + "}";

		  returnStream << " new_graph->setVertexDuration(new_vertex," << duration_string << ");" << std::endl;

		  returnStream << "}" << std::endl;

	   }}



	  {ForEachChannel(dataflow,c){

	      Vertex in_vertex  = dataflow->getEdgeSource(c);
	      Vertex out_vertex = dataflow->getEdgeTarget(c);

	      ARRAY_INDEX in_vertex_id  = dataflow->getVertexId(in_vertex);
	      ARRAY_INDEX out_vertex_id = dataflow->getVertexId(out_vertex);



		  EXEC_COUNT in_phase_count = dataflow->getPhasesQuantity(in_vertex);
		  EXEC_COUNT out_phase_count = dataflow->getPhasesQuantity(out_vertex);

		  returnStream << "{" << std::endl;
		  returnStream << "std::cout << \"generate a buffer ...\" << std::endl;"  << std::endl;

	      returnStream << "auto new_edge = new_graph->addEdge(new_graph->getVertexById(" << in_vertex_id << "), new_graph->getVertexById(" << out_vertex_id << "));" << std::endl;


	      std::string in_rates = "";
	      for (PHASE_INDEX phase = 1 ; phase <= in_phase_count ; phase++) {
	      			  if (phase > 1)  in_rates += ",";
	      			in_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeInPhase(c,phase));
	      }
	      in_rates = "{" + in_rates + "}";



	      std::string out_rates = "";
	      for (PHASE_INDEX phase = 1 ; phase <= out_phase_count ; phase++) {
	      			  if (phase > 1)  out_rates += ",";
	      			out_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeOutPhase(c,phase));
	      }
	      out_rates = "{" + out_rates + "}";


		  returnStream << " new_graph->setEdgeInPhases(new_edge," << in_rates << ");" << std::endl;
		  returnStream << " new_graph->setEdgeOutPhases(new_edge," << out_rates << ");" << std::endl;
		  returnStream << " new_graph->setPreload(new_edge," << dataflow->getPreload(c) << ");" << std::endl;
		  returnStream << " new_graph->setEdgeName(new_edge,\"" << dataflow->getEdgeName(c) << "\");" << std::endl;

		  returnStream << "}" << std::endl;

	    }}
	  returnStream << std::endl;
	  std::cout << returnStream.str();

}

std::string printers::GenerateDOT    (models::Dataflow* const  dataflow , bool simple) {

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

  std::ostringstream returnStream;
  
  VERBOSE_DEBUG("Start DOT generation");

  
  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with circo for an optimal visu\n" << std::endl;
  returnStream << "digraph G {\n" << std::endl;
  
  returnStream <<  "  graph [label=\"" << "Auto-generate by the Kiter"
	       << "\",overlap=scale,splines=true]\n";
  returnStream << "  edge [labelangle=15,labeldistance=1,len=1.5,fontsize=8,labelsize=4,color=grey]" << std::endl;

  returnStream << std::endl;


  {ForEachVertex(dataflow,t) {

      ARRAY_INDEX tid =  dataflow->getVertexId(t);

      returnStream << "  t_" << tid << " [" << std::endl;
      returnStream << "    shape=circle," << std::endl;
      returnStream << "    label = \"" << dataflow->getVertexName(t);
      if (!simple) {
    	  returnStream	  << "\nid:" << tid
    		  << "\nPhases:" << commons::toString(dataflow->getPhasesQuantity(t))
    		  << "\nNi:" << commons::toString(dataflow->getNi(t))
    		  << "\nduration:" << commons::toString(dataflow->getVertexPhaseDuration(t))
              << "\nreentrancy:" << commons::toString(dataflow->getReentrancyFactor(t))  ;
      }
      returnStream  << "\"" << std::endl;
      returnStream  << "  ];" << std::endl;
      returnStream << std::endl;
    }}


  
  {ForEachChannel(dataflow,c){
      ARRAY_INDEX edgeIn  = dataflow->getVertexId(dataflow->getEdgeSource(c));
      ARRAY_INDEX edgeOut = dataflow->getVertexId(dataflow->getEdgeTarget(c));
      returnStream << "  t_" << edgeIn << " -> t_" << edgeOut << " [";
      returnStream << std::endl;
      std::string bl = dataflow->getEdgeTypeStr(c) ;
      ARRAY_INDEX eid = dataflow->getEdgeId(c) ;
      returnStream << "    label=\"" ;
      if (!simple) {
    	  returnStream	  << bl
    		  	  << "\nid:" << eid
				  << "\npreload:"  << commons::toString(dataflow->getPreload(c)) ;
      }
      returnStream << "\"," << std::endl;
      returnStream << "    headlabel=\"" ;
      if (dataflow->getInitPhasesQuantity(edgeOut) > 0) {returnStream <<  commons::toString(dataflow->getEdgeInitOutVector(c)) << ";" ;}
      returnStream <<  commons::toString(dataflow->getEdgeOutVector(c)) << "\"," << std::endl;
      returnStream << "    taillabel=\"" ;
      if (dataflow->getInitPhasesQuantity(edgeIn) > 0) {returnStream <<  commons::toString(dataflow->getEdgeInitInVector(c)) << ";" ;}
      returnStream <<  commons::toString(dataflow->getEdgeInVector(c)) << "\"," << std::endl;
      returnStream << " ] ;" << std::endl;
    }}
  returnStream << std::endl;
  
  returnStream <<  "}" << std::endl;
  
  return returnStream.str();

}


void printers::printGraph    (models::Dataflow* const  dataflow, parameters_list_t ) {

  std::cout << printers::GenerateDOT    ( dataflow ) ;

}
void printers::printXML    (models::Dataflow* const  dataflow, parameters_list_t params ) {

	static const std::string filename_argument = "filename";
	VERBOSE_ASSERT (params.find(filename_argument) != params.end() , "Please provide a filename using -pfilename=X.");
	std::string filename = params[filename_argument];
	commons::writeSDF3File(filename, dataflow );
}


void printers::printInfos    (models::Dataflow* const  dataflow, parameters_list_t ) {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
		std::cout << "Graph name             = " <<  dataflow->getName() << std::endl;
		std::cout << "Task count             = " <<  dataflow->getVerticesCount() << std::endl;
		std::cout << "Channels total         = " <<  dataflow->getEdgesCount() << std::endl;


		EXEC_COUNT total = 0;
		{ForEachTask(dataflow,t) {
			total += dataflow->getNi(t) ;
		}}
        std::cout << "Complexity         = " <<  total << std::endl;


		std::cout << "== Tasks ==" << std::endl;

              {ForEachTask(dataflow,t) {

                  std::cout << "  - "
                		  << std::setw(20) << dataflow->getVertexName(t) << "(" << dataflow->getVertexId(t) << ")"
                          << " T= " << std::fixed << std::setw(20) << dataflow->getVertexTotalDuration(t)
                		  << " N=" << std::setw(5)<< dataflow->getNi(t)
                		  << " P=" << std::setw(5)<< dataflow->getPhasesQuantity(t)
						  << " Mapping=" << std::setw(5)<< dataflow->getMapping(t)
                		  << std::endl;

              }}

}


