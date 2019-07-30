/*
 * stdout.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <algorithms/repetition_vector.h>
#include <printers/stdout.h>

std::string add_block ( std::string name , TIME_UNIT start, TIME_UNIT duration, double node_height,  double current_task_y_pos, double line_width =  0.1 , bool fill = false) {
		std::ostringstream returnStream;
		returnStream<< "\"" << name << 0  << "\""
				<< " [label=\"\", width=\"" << duration / 72 << "\", penwidth=\"" << line_width << "\"";
		if (fill) returnStream <<  ", style=\"filled\" , color=black" ;
		returnStream <<  ", height=\"" << node_height << "\", fixedsize=true, pos=\"" << start + (duration/2) << ","<< current_task_y_pos << "\",shape=box]" << std::endl;
		return returnStream.str();
}

std::string printers::PeriodicScheduling2DOT    (models::Dataflow* const  dataflow, std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> periodic_scheduling , bool full,  double xscale , double yscale ) {

  std::ostringstream returnStream;

  static double label_node_width  =   1;
  static double label_node_height = yscale *  0.25;

  static double node_height = yscale * 0.25;
  static double pixel_start =  (-(72/2) - 1);
  static double line_width =  0.1;

  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with -Knop only!\n" << std::endl;
  returnStream << "digraph {\n" << std::endl;
  returnStream << "graph [sep = \"0\", nodesep = \"0\"];" << std::endl;

  TIME_UNIT last_execution_end_at = 0;
  {ForEachVertex(dataflow,t) {
  	  VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");
  	  auto Ni = dataflow->getNi(t);
  	  auto period = periodic_scheduling[t].first;
  	  auto duration = dataflow->getVertexDuration(t);
  	  auto starts = periodic_scheduling[t].second;

        for (EXEC_COUNT iter = 0 ; iter < (Ni/starts.size()) ; iter++) {
      	  for (auto  start : starts) {
      		last_execution_end_at = std::max(last_execution_end_at, duration + (start + period * iter));
      	  }
        }

      }}

  double idx = 0;
  {ForEachVertex(dataflow,t) {
	  VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");
	  auto Ni = dataflow->getNi(t);
	  auto period = periodic_scheduling[t].first;
	  auto duration = dataflow->getVertexDuration(t);
	  auto starts = periodic_scheduling[t].second;

	  double current_task_y_pos = yscale * -20 * idx ;

      returnStream << "\""<< dataflow->getVertexName(t) << "\"" << " [label=\"" <<  dataflow->getVertexName(t) << "\\r\", margin=\"0,0\", width=\"" << label_node_width << "\", height=\"" << label_node_height << "\", fixedsize=true, pos=\"" << pixel_start << ","<< current_task_y_pos << "\",shape=plaintext]" << std::endl;

      EXEC_COUNT execution_index = 0;
      for (EXEC_COUNT iter = 0 ; ((starts[0] + period * iter)) < last_execution_end_at ; iter++) {
    	  for (auto  start : starts) {
    		  bool fill = (execution_index >= Ni) ;
    		  if (!full) if (!fill) iter = (EXEC_COUNT) std::round(last_execution_end_at);
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

std::string printers::GenerateDOT    (models::Dataflow* const  dataflow ) {

  std::ostringstream returnStream;
  
  
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
      returnStream << "    label = \" " << dataflow->getVertexName(t) << "(id:" << tid << " duration:" << dataflow->getVertexDuration(t) << ")" << "\"" << std::endl;
      
      returnStream  << "  ];" << std::endl;
      returnStream << std::endl;
    }}


  
  {ForEachChannel(dataflow,c){
      ARRAY_INDEX edgeIn  = dataflow->getVertexId(dataflow->getEdgeSource(c));
      ARRAY_INDEX edgeOut = dataflow->getVertexId(dataflow->getEdgeTarget(c));
      returnStream << "  t_" << edgeIn << " -> t_" << edgeOut << " [";
      returnStream << std::endl;
      returnStream << "    headlabel=\"" << dataflow->getEdgeOut(c) << "\"," << std::endl;
      returnStream << "    taillabel=\"" << dataflow->getEdgeIn(c) << "\"," << std::endl;
      returnStream << " ] ;" << std::endl;
    }}
  returnStream << std::endl;
  
  returnStream <<  "}" << std::endl;
  
  return returnStream.str();

}


void printers::printGraph    (models::Dataflow* const  dataflow, parameters_list_t ) {

  std::cout << printers::GenerateDOT    ( dataflow ) ;

}


void printers::printInfos    (models::Dataflow* const  dataflow, parameters_list_t ) {

                VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
		EXEC_COUNT total = 0;
		std::cout << "Graph name             = " <<  dataflow->getName() << std::endl;
		std::cout << "Task count             = " <<  dataflow->getVerticesCount() << std::endl;
		std::cout << "Channels total         = " <<  dataflow->getEdgesCount() << std::endl;

              {ForEachTask(dataflow,t) {
		  total += dataflow->getNi(t) ;
                  std::cout << " - " <<  dataflow->getVertexName(t) << " N=" << dataflow->getNi(t) << std::endl;

              }}

              std::cout << "Complexity         = " <<  total << std::endl;
}


