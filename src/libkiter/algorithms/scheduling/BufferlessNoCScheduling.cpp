/*
 * BufferlessNoCScheduling.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */


#include <models/NoC.h>
#include <vector>
#include <iostream>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/software_noc.h>
#include <algorithms/repetition_vector.h>
#include <algorithms/kperiodic.h>
#include <algorithms/mappings.h>
#include <cstdlib>
#include <stack>
#include <climits>
#include <algorithms/schedulings.h>
#include <set>
#include <queue>
#include <tuple>
#include <commons/SDF3Wrapper.h>
#include <unordered_map>

static TIME_UNIT NULL_DURATION = 0;

typedef std::pair<ARRAY_INDEX, ARRAY_INDEX>  mypair;
typedef std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> mytuple;

static void print_graph (models::Dataflow * to, std::string suffix = "none") {

	static int counter = 0;
	counter ++ ;
	VERBOSE_INFO("=========== Write file " << counter << "\n");

	std::string sfilename = "bufferless_noc_schedule_"+ commons::toString(counter) + "_" + suffix + "";



	{

		std::string filename = sfilename + "_noc";
		std::ofstream myfile;
		myfile.open (filename  + ".dot");
		myfile << printers::GenerateNoCDOT(to);
		myfile.close();

		std::string cmd = "dot " + filename + ".dot -Gdpi=300  -Kneato -T png -o " + filename + ".png";
		auto out_err = system(cmd.c_str());
		if(out_err) {
			VERBOSE_INFO ("System call returns error\n");
		}

	}

	commons::writeSDF3File(sfilename+ ".xml", to);
	to->reset_computation();

}

static LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b)
{
	if (x == 0) //Base Case
	{
		*a = 0;
		*b = 1;
		return y;
	}

	LARGE_INT a1, b1; // To store results of recursive call
	LARGE_INT gcd = gcdExtended(y%x, x, &a1, &b1);
	// Update x and y using results of recursive  call
	*a = b1 - (y/x) * a1;
	*b = a1;

	return gcd;
}


//print_graph(to, original_df);
void algorithms::BufferlessNoCScheduling(models::Dataflow* const  _dataflow, parameters_list_t   ) {


	models::Dataflow* to = new models::Dataflow(*_dataflow);

	print_graph(to, "start");

	/// Store a list of origina l actors id.
	std::vector <ARRAY_INDEX> original_vertex_ids;
	for (auto v : to->vertices()) {
		original_vertex_ids.push_back(to->getVertexId(v));
	}


	VERBOSE_INFO ("Task mapping = ");
	for (auto v : to->vertices()) {
		VERBOSE_INFO ("  - " << to->getVertexId(v) << " mapped to "  << to->getMapping(v));
	}


	algorithms::ModelNoCConflictFreeCommunication(to) ;


	print_graph(to, "modeledNoC");

	/// Store a list of origina l actors id.
	std::map <edge_id_t, std::vector <ARRAY_INDEX> > links_usage;
	std::map <node_id_t, std::vector <ARRAY_INDEX> > router_usage;
	std::map <std::pair<edge_id_t,edge_id_t>, std::vector <ARRAY_INDEX> > router_xbar_usage;

	for (auto v : to->vertices()) {
		ARRAY_INDEX tid = to->getVertexId(v);
		auto current_mapping =  (to->getMapping(v));
		VERBOSE_ASSERT(current_mapping >= 0, "UNSUPPORTED CASE, EVERY TASK NEED TO BE MAPPED");
		VERBOSE_ASSERT(to->getNoC().hasEdge(current_mapping) xor to->getNoC().hasNode(current_mapping), "UNSUPPORTED CASE, NOC NODE AND NOC EDGE WITH SIMILAR ID : " << current_mapping);

		VERBOSE_INFO("Process task " << tid << " - " << to->getVertexName(v) << " mapped to " << current_mapping);

		if (to->getNoC().hasEdge(current_mapping)) {
			VERBOSE_INFO("  - Is a NoC Edge: add to links_usage");
			links_usage[current_mapping].push_back(tid);
		} else if (to->getNoC().hasNode(current_mapping)) {
			VERBOSE_INFO("  - Is a NoC Node: add to router_usage and router_xbar_usage");
			if (to->getNoC().getNode(current_mapping).type == NetworkNodeType::Router) {
				router_usage[current_mapping].push_back(tid);

				VERBOSE_ASSERT(to->getVertexInDegree(v) == 1, "The NoC has bot been modelled has expected, every NoCRouter-task should have one input NoCEdge Task. The task " << to->getVertexName(v) << " has " << to->getVertexInDegree(v));
				VERBOSE_ASSERT(to->getVertexOutDegree(v) == 1, "The NoC has bot been modelled has expected, every NoCRouter-task should have one output NoCEdge Task. The task " << to->getVertexName(v) << " has " << to->getVertexOutDegree(v));

				Vertex inputEdgeTask = to->getEdgeSource(*(to->getInputEdges(v).first));
				Vertex outputEdgeTask = to->getEdgeTarget(*(to->getOutputEdges(v).first));

				edge_id_t input_edge = to->getMapping(inputEdgeTask);
				edge_id_t output_edge = to->getMapping(outputEdgeTask);

				VERBOSE_ASSERT(to->getNoC().hasEdge(to->getMapping(inputEdgeTask)), "The NoC has bot been modelled has expected, every  NoCEdge Task should be properly mapped");
				VERBOSE_ASSERT(to->getNoC().hasEdge(to->getMapping(outputEdgeTask)), "The NoC has bot been modelled has expected, every  NoCEdge Task should be properly mapped");

				router_xbar_usage[std::pair<edge_id_t,edge_id_t> (input_edge, output_edge) ].push_back(tid);

			}else {
				VERBOSE_INFO("!! SKIP because node is not a ROUTER ");
			}
		} else {
			VERBOSE_INFO("!! SKIP because not found in the NoC ");
		}
	}

	VERBOSE_INFO ("Router_usage = ");
	for (auto item : router_usage) {

		VERBOSE_INFO ("  - " << item.first << ":"  << commons::toString(item.second));
	}
	VERBOSE_INFO ("Router_Xbar usage = ");
	for (auto item : router_xbar_usage) {
		edge_id_t src =  item.first.first ;
		edge_id_t dst =  item.first.second ;
		std::vector<ARRAY_INDEX> vertex_ids = item.second ;
		VERBOSE_ASSERT(to->getNoC().getEdge(src).dst == to->getNoC().getEdge(dst).src, "NoC Graph doesnt match NoC");
		VERBOSE_INFO ("  - " << src << "::" << dst << " : "  << commons::toString(item.second) << " = " );

		for (auto id : vertex_ids) {
			Vertex router_node = to->getVertexById(id);
			VERBOSE_INFO ("        x " << to->getVertexName(router_node) << "(" << id << ")");
		}

	}


	VERBOSE_INFO ("links_usage = ");
	for (auto item : links_usage) {

		VERBOSE_INFO ("  - " << item.first << ":"  << commons::toString(item.second));
	}





}




