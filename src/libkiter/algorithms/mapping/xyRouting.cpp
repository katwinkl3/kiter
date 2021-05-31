/*
    XY Routing Algorithm
 */

#include <models/Dataflow.h>
#include <algorithms/mappings.h>
#include <models/NoC.h>

void algorithms::mapping::xyRouting (models::Dataflow* const  dataflow, parameters_list_t params) {	

    VERBOSE_INFO("xyRouting");
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

            // 2. Set route 
            if (node.x < dest_x){ // travel right
                while (node.x != dest_x){
                    NetworkEdge nb = dataflow->getNoC().getRightNeighbour(node.id); //change to something less ugly...
                    // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                    path.push_back(nb.id);
                    node = dataflow->getNoC().getNode(nb.dst);
                }
            } else{ // travel left
                while (node.x != dest_x){
                    NetworkEdge nb = dataflow->getNoC().getLeftNeighbour(node.id);
                    // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                    path.push_back(nb.id);
                    node = dataflow->getNoC().getNode(nb.dst);
                }
            }
            if (node.y < dest_y){ //travel up
                while (node.y != dest_y){
                    NetworkEdge nb = dataflow->getNoC().getTopNeighbour(node.id);
                    // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                    path.push_back(nb.id);
                    node = dataflow->getNoC().getNode(nb.dst);
                }
            } else{ //travel down
                while (node.y != dest_y){
                    NetworkEdge nb = dataflow->getNoC().getBottomNeighbour(node.id);
                    // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
                    path.push_back(nb.id);
                    node = dataflow->getNoC().getNode(nb.dst);
                }
            }
            // while (node.x != dest_x){
            //     NetworkEdge nb = dataflow->getNoC().getXNeighbour(node.id, x_d);
            //     // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
            //     path.push_back(nb.id);
            //     node = dataflow->getNoC().getNode(nb.dst);
            // }
            // while (node.y != dest_y){
            //     NetworkEdge nb = dataflow->getNoC().getYNeighbour(node.id, y_d);
            //     // VERBOSE_ASSERT(std::find(path.begin(), path.end(), nb.id) != path.end(), "Loop in Found Path");
            //     path.push_back(nb.id);
            //     node = dataflow->getNoC().getNode(nb.dst);
            // }
            path.push_back(last.id);
            dataflow->setRoute (e, path);
	}

}
