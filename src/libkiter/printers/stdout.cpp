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

void  printers::printGraphAsKiterScript (models::Dataflow* const  dataflow, parameters_list_t params) {


	  std::ostringstream returnStream;

	  bool verbose = params.count("verbose");

	  returnStream << "// Auto-generate by Kiter for Kiter" << std::endl;
	  returnStream << "// " << std::endl;

	  returnStream << "models::Dataflow* generateGraph () {"   << std::endl;



	  if (verbose) returnStream << "std::cout << \"generate a dataflow graph...\" << std::endl;"  << std::endl;
	  returnStream << "models::Dataflow* new_graph = new models::Dataflow();" << std::endl;
	  returnStream << "" << std::endl;

	  {ForEachVertex(dataflow,t) {
		  PHASE_INDEX phase_count = dataflow->getPhasesQuantity(t);
		  PHASE_INDEX init_phase_count = dataflow->getInitPhasesQuantity(t);
		  returnStream << "{" << std::endl;
		  if (verbose) returnStream << "std::cout << \"generate a task ...\" << std::endl;"  << std::endl;
		  returnStream << "auto new_vertex = new_graph->addVertex(" << dataflow->getVertexId(t) << ");" << std::endl;

		  returnStream << " new_graph->setVertexName(new_vertex,\"" << dataflow->getVertexName(t) << "\");" << std::endl;
		  returnStream << " new_graph->setInitPhasesQuantity(new_vertex," <<init_phase_count << ");" << std::endl;
		  returnStream << " new_graph->setPhasesQuantity(new_vertex," <<phase_count << ");" << std::endl;
		  returnStream << " new_graph->setReentrancyFactor(new_vertex," << dataflow->getReentrancyFactor(t)<< "); " << std::endl;
		  if (init_phase_count) {
		  std::string init_duration_string = "";
		  for (PHASE_INDEX phase = 1 ; phase <= init_phase_count ; phase++) {
			  if (phase > 1)  init_duration_string += ",";
			  init_duration_string += commons::toString<TIME_UNIT>(dataflow->getVertexInitDuration(t,phase));
		  }
		  init_duration_string = "{" + init_duration_string + "}";
		  returnStream << " new_graph->setVertexInitDuration(new_vertex," << init_duration_string << ");" << std::endl;
		  }
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
		  if (verbose) returnStream << "std::cout << \"generate a buffer ...\" << std::endl;"  << std::endl;

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


		  EXEC_COUNT init_in_phase_count = dataflow->getInitPhasesQuantity(in_vertex);
		  EXEC_COUNT init_out_phase_count = dataflow->getInitPhasesQuantity(out_vertex);

		  if (init_in_phase_count) {
	      std::string init_in_rates = "";
	      for (PHASE_INDEX init_phase = 1 ; init_phase <= init_in_phase_count ; init_phase++) {
	      			  if (init_phase > 1)  init_in_rates += ",";
	      			init_in_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeInInitPhase(c,init_phase));
	      }
	      init_in_rates = "{" + init_in_rates + "}";
		  returnStream << " new_graph->setEdgeInInitPhases(new_edge," << init_in_rates << ");" << std::endl;
		  }


		  if (init_out_phase_count) {
	      std::string init_out_rates = "";
	      for (PHASE_INDEX init_phase = 1 ; init_phase <= init_out_phase_count ; init_phase++) {
	      			  if (init_phase > 1)  init_out_rates += ",";
	      			init_out_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeOutInitPhase(c,init_phase));
	      }
	      init_out_rates = "{" + init_out_rates + "}";
		  returnStream << " new_graph->setEdgeOutInitPhases(new_edge," << init_out_rates << ");" << std::endl;
		  }



		  returnStream << " new_graph->setEdgeInPhases(new_edge," << in_rates << ");" << std::endl;
		  returnStream << " new_graph->setEdgeOutPhases(new_edge," << out_rates << ");" << std::endl;
		  returnStream << " new_graph->setPreload(new_edge," << dataflow->getPreload(c) << ");" << std::endl;
		  returnStream << " new_graph->setEdgeName(new_edge,\"" << dataflow->getEdgeName(c) << "\");" << std::endl;

		  returnStream << "}" << std::endl;

	    }}
	  returnStream << std::endl;
	  returnStream << "}"   << std::endl;
	  std::cout << returnStream.str();

}



std::string printers::GenerateNoCDOT    (models::Dataflow* const  dataflow , bool simple) {

	double max_penwidth = 4.0;

  VERBOSE_DEBUG("Start NoC DOT generation");
  const NoC & noc = dataflow->getNoC();


  std::ostringstream returnStream;

  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with neato for an optimal visu\n" << std::endl;
  returnStream << "digraph G {\n" << std::endl;

  //returnStream <<  "  graph [label=\"" << "Auto-generate by the Kiter"<< "\",overlap=scale,splines=true]\n";
  //returnStream << "   edge [labelangle=15,labeldistance=1,len=1.5,fontsize=8,labelsize=4,color=grey]" << std::endl;

  returnStream << std::endl;


  for (const NetworkNode node : noc.getNodes()) {
	  std::string label = commons::toString(node.id);
	  double x = node.x;
	  double y = node.y;
	  if (node.type == NetworkNodeType::Core) {
		  returnStream << node.id << "[label=\"Core\\n" << label << "\", fontsize=\"10\", shape=\"box\",  fixedsize=\"shape\", width=0.5, height=0.5, pos=\"" << x << "," << y << "!\", pin=TRUE, notranslate=TRUE];" << std::endl;
	  } else if (node.type == NetworkNodeType::Router) {
		  returnStream << node.id << "[label=\"" << label << "\", fontsize=\"5\", shape=\"box\",  fixedsize=\"shape\", width=0.2, height=0.2, pos=\"" << x << "," << y << "!\", pin=TRUE, notranslate=TRUE];" << std::endl;
	  }
  }



  for (const Vertex v : dataflow->vertices()) {
	  ARRAY_INDEX task_id = dataflow->getVertexId(v);
	  node_id_t core_id = dataflow->getMapping(v);
	  VERBOSE_DEBUG("task_id=" << task_id << " core_id = " <<  core_id);
	  const NetworkNode& core = dataflow->getNoC().getNode(core_id);
	  double x = core.x;
	  double y = core.y;
	  returnStream <<  "task_" << task_id << "[label=\"Task\\n" << task_id << "\", style=\"filled\", color=\"red\", fontsize=\"10\", shape=\"circle\", pos=\"" << x << "," << y << "!\", fixedsize=\"shape\", width=0.5, height=0.5];" << std::endl;
  }


  std::map<edge_id_t,std::vector<Edge>> link_usage;
  std::map<Edge,std::string> edge_color;
  std::vector<std::string> available_colors = {"#ACFF54","#41E869","#53FFEE","#4191E8","#4191E8","#634BFA", "#FAF93C",
		                                       "#DB42FF","#E83154","#FF7843","#E8A131","#FADC39", "#8736FF"
};

  for (const NetworkEdge e : noc.getEdges()) {
	  link_usage[e.id];
      //returnStream << e.src << "->" << e.dst << std::endl;
  }



  {ForEachChannel(dataflow,c){
	  edge_color[c] = available_colors[edge_color.size() % available_colors.size()];
	  ARRAY_INDEX edge_id = dataflow->getEdgeId(c);
	  Vertex source_vtx   = dataflow->getEdgeSource(c);
	  Vertex target_vtx   = dataflow->getEdgeTarget(c);
	  const std::vector<edge_id_t>& route = dataflow->getRoute(c);
	  VERBOSE_DEBUG("edge_id=" << edge_id << "from tasks " <<  dataflow->getVertexId(source_vtx) << " and " <<   dataflow->getVertexId(target_vtx)  << " route = " <<  route);

	  for (edge_id_t e : route) {
		  const NetworkEdge& nedge = dataflow->getNoC().getEdge(e);
		  VERBOSE_DEBUG("  - edge_id_t=" << e << " connects " <<  nedge.src << "->" << nedge.dst);
	      //returnStream << nedge.src << "->" << nedge.dst<< "[style = \"bold\", color=\"red\", splines=\"false\" ];" << std::endl;

		  link_usage[nedge.id].push_back(c);
	  }
  }}

  std::vector<Edge>::size_type max_cnt = 0;
  for (const NetworkEdge e : noc.getEdges()) {
	  max_cnt = std::max(max_cnt , link_usage[e.id].size());
  }

  for (const NetworkEdge e : noc.getEdges()) {
	  int cnt = link_usage[e.id].size();
	  if (cnt == 0) {
		  returnStream << e.src << "->" << e.dst << std::endl;
	  } else {
		  std::string color = "";
		  for (Edge c : link_usage[e.id]) {
			  if (color != "") color += "::";
			  color += edge_color[c];
		  }
		  double pw = 1;
	      returnStream << e.src << "->" << e.dst<< "[style = \"bold\", color=\"" << color << "\", splines=\"false\"  , penwidth=" << pw << " ];" << std::endl;
	  }
  }


  returnStream <<  "}" << std::endl;

  returnStream << std::endl;

  return returnStream.str();

}




std::string printers::GenerateGraphDOT    (models::Dataflow* const  dataflow , bool simple) {

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

  std::ostringstream returnStream;
  
  VERBOSE_DEBUG("Start Graph DOT generation");

  
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
      Vertex edgeIn  = (dataflow->getEdgeSource(c));
      Vertex edgeOut = (dataflow->getEdgeTarget(c));
      ARRAY_INDEX edgeInId  = dataflow->getVertexId(dataflow->getEdgeSource(c));
      ARRAY_INDEX edgeOutId = dataflow->getVertexId(dataflow->getEdgeTarget(c));
      returnStream << "  t_" << edgeInId << " -> t_" << edgeOutId << " [";
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
      if (dataflow->getInitPhasesQuantity(edgeOut) > 0) {returnStream << "(" << commons::toString(dataflow->getEdgeInitOutVector(c)) << ")"  << ";" ;}
      returnStream <<  commons::toString(dataflow->getEdgeOutVector(c)) << "\"," << std::endl;
      returnStream << "    taillabel=\"" ;
      if (dataflow->getInitPhasesQuantity(edgeIn) > 0) {returnStream << "("  <<  commons::toString(dataflow->getEdgeInitInVector(c))  << ")" << ";" ;}
      returnStream <<  commons::toString(dataflow->getEdgeInVector(c)) << "\"," << std::endl;
      returnStream << " ] ;" << std::endl;
    }}
  returnStream << std::endl;
  
  returnStream <<  "}" << std::endl;
  
  return returnStream.str();

}


void printers::printMapping    (models::Dataflow* const  dataflow, parameters_list_t ) {

  std::cout << printers::GenerateNoCDOT    ( dataflow ) ;

}

void printers::printGraph    (models::Dataflow* const  dataflow, parameters_list_t ) {

  std::cout << printers::GenerateGraphDOT    ( dataflow ) ;

}
void printers::printXML    (models::Dataflow* const  dataflow, parameters_list_t params ) {

	static const std::string filename_argument = "filename";

	if (params.find(filename_argument) != params.end() ) {
		std::string filename = params[filename_argument];
		commons::writeSDF3File(filename, dataflow );
	} else {
		 std::cout << commons::generateSDF3XML(dataflow) << std::endl;
	}

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


