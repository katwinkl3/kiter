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

void  printers::printGraphAsKiterScript (models::Dataflow* const  dataflow, parameters_list_t ) {


	  std::ostringstream returnStream;


	  returnStream << "// Auto-generate by Kiter for Kiter" << std::endl;
	  returnStream << "// " << std::endl;

	  returnStream << "std::cout << \"generate a dataflow graph...\" << std::endl;"  << std::endl;
	  returnStream << "models::Dataflow* new_graph = new models::Dataflow();" << std::endl;
	  returnStream << "" << std::endl;

	  {ForEachVertex(dataflow,t) {
		  EXEC_COUNT phase_count = dataflow->getPhasesQuantity(t);
		  returnStream << "{" << std::endl;
		  returnStream << "std::cout << \"generate a task ...\" << std::endl;"  << std::endl;
		  returnStream << "auto new_vertex = new_graph->addVertex(" << dataflow->getVertexId(t) << ");" << std::endl;

		  returnStream << " new_graph->setVertexName(new_vertex,\"" << dataflow->getVertexName(t) << "\");" << std::endl;
		  returnStream << " new_graph->setPhasesQuantity(new_vertex," <<phase_count << ");" << std::endl;
		  returnStream << " new_graph->setReentrancyFactor(new_vertex," << dataflow->getReentrancyFactor(t)<< "); " << std::endl;

		  std::string duration_string = "";
		  for (ARRAY_INDEX phase = 1 ; phase <= phase_count ; phase++) {
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
	      for (ARRAY_INDEX phase = 1 ; phase <= in_phase_count ; phase++) {
	      			  if (phase > 1)  in_rates += ",";
	      			in_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeInPhase(c,phase));
	      }
	      in_rates = "{" + in_rates + "}";



	      std::string out_rates = "";
	      for (ARRAY_INDEX phase = 1 ; phase <= out_phase_count ; phase++) {
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

std::string printers::GenerateDOT    (models::Dataflow* const  dataflow ) {

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
void printers::printXML    (models::Dataflow* const  dataflow, parameters_list_t params ) {
	std::string filename = params["filename"];
	commons::writeSDF3File(filename, dataflow );
}


void printers::printInfos    (models::Dataflow* const  dataflow, parameters_list_t ) {

                VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
		EXEC_COUNT total = 0;
		std::cout << "Graph name             = " <<  dataflow->getName() << std::endl;
		std::cout << "Task count             = " <<  dataflow->getVerticesCount() << std::endl;
		std::cout << "Channels total         = " <<  dataflow->getEdgesCount() << std::endl;

              {ForEachTask(dataflow,t) {
		  total += dataflow->getNi(t) ;
                  std::cout << " - " <<  dataflow->getVertexName(t)
                		  << " N=" << dataflow->getNi(t)
						  << " Mapping=" << dataflow->getMapping(t)
                		  << std::endl;

              }}

              std::cout << "Complexity         = " <<  total << std::endl;
}


