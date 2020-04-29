/*
 * NoC.cpp
 *
 *  Created on: Apr 22, 2020
 *      Author: toky
 */

#include <models/NoC.h>


		NoC::NoC (int XSIZE, int YSIZE) : _XSIZE(XSIZE), _YSIZE(YSIZE) {

			int NodeCount = XSIZE * YSIZE * 2;

			for (int X = 0 ; X < XSIZE ; X ++) {

				for (int Y = 0 ; Y < YSIZE ; Y ++) {

					int source = X + Y * XSIZE ;
					int core_id = source + this->size();
					NetworkNode source_node = NetworkNode(source,  NetworkNodeType::Router, (double)(X)+0.5, (double)(Y)+0.5);
					NetworkNode core_node =  NetworkNode(core_id, NetworkNodeType::Core, (double)(X), (double)(Y));


					_vnodes.push_back(source_node);
					_vnodes.push_back(core_node);
					_mnodes.insert({source,source_node});
					_mnodes.insert({core_id,core_node});

					//add edges between source core and router
					NetworkEdge ep = NetworkEdge(NodeCount + core_id*NodeCount + source , core_id, source);
					_mid2edges.insert({ep.id,ep});
					_medges.insert({std::pair<node_id_t,node_id_t>(core_id , source),ep});
					_vedges.push_back(ep);



					NetworkEdge e = NetworkEdge(NodeCount + source*NodeCount + core_id, source, core_id);
					_mid2edges.insert({e.id,e});
					_medges.insert({std::pair<node_id_t,node_id_t>(source, core_id),e});
					_vedges.push_back(e);



					if (X < (XSIZE - 1)) {
						int dest_right = (X + 1) + Y * XSIZE ;

						NetworkEdge e = NetworkEdge(NodeCount + source*NodeCount + dest_right, source, dest_right);
						_mid2edges.insert({e.id,e});
						_medges.insert({std::pair<node_id_t,node_id_t>( source, dest_right),e});
						_vedges.push_back(e);

						NetworkEdge ep = NetworkEdge(NodeCount + dest_right*NodeCount + source, dest_right, source);
						_mid2edges.insert({ep.id,ep});
						_medges.insert({std::pair<node_id_t,node_id_t>(  dest_right , source),ep});
						_vedges.push_back(ep);

					}

					if (Y < (YSIZE - 1)) {
						int dest_bottom = X + (Y+1) * XSIZE ;

						NetworkEdge e = NetworkEdge(NodeCount + source*NodeCount + dest_bottom, source, dest_bottom);
						_mid2edges.insert({e.id,e});
						_medges.insert({std::pair<node_id_t,node_id_t>( source, dest_bottom),e});
						_vedges.push_back(e);


						NetworkEdge ep = NetworkEdge(NodeCount + dest_bottom*NodeCount + source, dest_bottom, source);
						_mid2edges.insert({ep.id,ep});
						_medges.insert({std::pair<node_id_t,node_id_t>( dest_bottom ,  source),ep});
						_vedges.push_back(ep);

					}

				}
			}
			VERBOSE_INFO( "dim=" << XSIZE << "X" << YSIZE << ",edges=" << _vedges.size() << ",medges=" << _medges.size() );

			VERBOSE_INFO("Start auto-check of NoC model");

			std::set<noc_id_t> ids;

			for (auto e : this->getEdges()) {
				VERBOSE_INFO("Edge " << e);
				VERBOSE_ASSERT(ids.count(e.id) == 0, "Two NoC elements must not share the same id!");
				ids.insert(e.id);
			}
			for (auto n : this->getNodes()) {
				VERBOSE_INFO("Node " << n);
				VERBOSE_ASSERT(ids.count(n.id) == 0, "Two NoC elements must not share the same id!");
				ids.insert(n.id);
			}


		}


