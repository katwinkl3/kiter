/*
    XY Routing Algorithm
 */

#include <models/Dataflow.h>
#include <algorithms/mappings.h>
#include <models/NoC.h>

void algorithms::mapping::randomRouting (models::Dataflow* const  dataflow, parameters_list_t params) {	

    VERBOSE_INFO("randomRouting");
    // std::vector<edge_id_t> memoize_vec;

    ForEachEdge(dataflow, e){
            // 1. Extract values
            std::vector<edge_id_t> path;
            NetworkEdge start = dataflow->getNoC().getSrcNeighbour(dataflow->getMapping(dataflow->getEdgeSource(e)));
            path.push_back(start.id);
            node_id_t node_id = start.dst;
            NetworkNode node = dataflow->getNoC().getNode(node_id);
            NetworkEdge last = dataflow->getNoC().getDstNeighbour(dataflow->getMapping(dataflow->getEdgeTarget(e)));
            node_id_t dest_id = last.src; 
            double dest_x = dataflow->getNoC().getNode(dest_id).x; 
            double dest_y = dataflow->getNoC().getNode(dest_id).y; 

            while (node.x != dest_x || node.y != dest_y){
                bool step = std::rand() % 2;
                if (step){ // on x axis
                    if (node.x < dest_x){ // travel right
                        NetworkEdge nb = dataflow->getNoC().getRightNeighbour(node.id); //change to pt; this is ugly
                        // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                        path.push_back(nb.id);
                        node = dataflow->getNoC().getNode(nb.dst);
                    }
                    if (node.x > dest_x) { //travel left
                        NetworkEdge nb = dataflow->getNoC().getLeftNeighbour(node.id); 
                        // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                        path.push_back(nb.id);
                        node = dataflow->getNoC().getNode(nb.dst);
                    }
                } else {
                    if (node.y < dest_y){ // travel up
                        NetworkEdge nb = dataflow->getNoC().getTopNeighbour(node.id);
                        // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                        path.push_back(nb.id);
                        node = dataflow->getNoC().getNode(nb.dst);
                    } 
                    if (node.y > dest_y) { //travel down
                        NetworkEdge nb = dataflow->getNoC().getBottomNeighbour(node.id);
                        // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                        path.push_back(nb.id);
                        node = dataflow->getNoC().getNode(nb.dst);
                    }
                }
                
            }
            path.push_back(last.id);
            dataflow->setRoute (e, path);
	}

}
