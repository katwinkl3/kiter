/*
 *  This program is a Dataflow Analysis Tool : Kiter
 *
 *
 *
 * */

#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <commons/SDF3Wrapper.h>

models::Dataflow * generateSample () {
	// Auto-generate by Kiter for Kiter
	//
	std::cout << "generate a dataflow graph..." << std::endl;
	models::Dataflow* new_graph = new models::Dataflow();

	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(1);
	 new_graph->setVertexName(new_vertex,"a");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(2);
	 new_graph->setVertexName(new_vertex,"b");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{2});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(3);
	 new_graph->setVertexName(new_vertex,"c");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{2});
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(2));
	 new_graph->setEdgeInPhases(new_edge,{2});
	 new_graph->setEdgeOutPhases(new_edge,{3});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"alpha");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(3));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{2});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"beta");
	}

	 return new_graph;
}


inline void activate_verbose(const std::string p) {

    if (commons::fromString<int>(p) < 0) {
        std::exit(EXIT_FAILURE);
    }
    commons::set_verbose_mode(commons::fromString<int>((optarg)));
}

int main (int argc, char ** argv)
{

	// ** default arguments
		std::string filename = "";
		std::vector<std::pair<std::string,parameters_list_t>> algos;
		parameters_list_t parameters;
		commons::set_verbose_mode(commons::WARNING_LEVEL); // default verbose mode is ...

		// **  Retreive arguments **
		int c;
	    while ((c = getopt(argc, (char **)argv, "f:v:p:")) != -1) {
	    	if (c == 'f') {
	    		filename = optarg;
	    	}
	    	if (c == 'v') {
	    		activate_verbose(optarg);
	    	}
	        if (c == 'p') {
	            std::string arg = optarg;
	            if (arg.find('=',0) != arg.npos) {
	                parameters[arg.substr(0,arg.find('=',0))] = arg.substr(arg.find('=',0)+1,arg.size());
	            }

	        }
	    }

	    VERBOSE_INFO("Parameter parsing is done.");



	VERBOSE_INFO("Read XML file : " << filename);
	models::Dataflow* dataflow = filename.length() ? commons::readSDF3File(filename) : generateSample () ;
	std::cout << "Done" << std::endl;


	std::cout << "Done" << std::endl;
	algorithms::scheduling::KPeriodic_scheduling_bufferless (dataflow, parameters);
	std::cout << "Done" << std::endl;




}
