/*
 * NoC.cpp
 *
 *  Created on: Apr 22, 2020
 *      Author: toky
 */

#include <models/NoC.h>
#include <set>
#include <limits>
#include <algorithm>

bool NoC::check_ids () {

	VERBOSE_DEBUG("Start auto-check of NoC model");

	std::set<noc_id_t> ids;

	for (auto e : this->getEdges()) {
		VERBOSE_DEBUG("Edge " << e);
		VERBOSE_ASSERT(ids.count(e.id) == 0, "Two NoC elements must not share the same id!");
		ids.insert(e.id);
	}
	for (auto n : this->getNodes()) {
		VERBOSE_DEBUG("Node " << n);
		VERBOSE_ASSERT(ids.count(n.id) == 0, "Two NoC elements must not share the same id!");
		ids.insert(n.id);
	}

	VERBOSE_DEBUG("End auto-check of NoC model");

	return (ids.size() == this->getEdges().size() + this->getNodes().size());

}

void NoC::generate() {

	_vnodes.clear();
	_mnodes.clear();

	_vedges.clear();
	_medges.clear();
	_mid2edges.clear();

	_adj.clear();


	auto XSIZE = this->getXSize();
	auto YSIZE = this->getYSize();

	VERBOSE_DEBUG( "start NoC with dim=" << XSIZE << "x" << YSIZE  );

				const int NodeCount = XSIZE * YSIZE * 2;

				for (int X = 0 ; X < XSIZE ; X ++) {

					for (int Y = 0 ; Y < YSIZE ; Y ++) {

						const int source_id = X + Y * XSIZE ;
						const int core_id   = source_id + this->size();

						// Add Core and Router nodes
						this->addNetworkNode(NetworkNode(source_id,  NetworkNodeType::Router, (double)(X)+0.5, (double)(Y)+0.5));
						this->addNetworkNode(NetworkNode(core_id, NetworkNodeType::Core, (double)(X), (double)(Y)));

						// Connect them
						this->addNetworkEdge(NetworkEdge(NodeCount + source_id*NodeCount + core_id, source_id, core_id));
						this->addNetworkEdge(NetworkEdge(NodeCount + core_id*NodeCount + source_id , core_id, source_id));


						if (X < (XSIZE - 1)) {
							const int dest_right = (X + 1) + Y * XSIZE ;

							// Connect with previous nodes on the left
							this->addNetworkEdge(NetworkEdge(NodeCount + source_id*NodeCount + dest_right, source_id, dest_right));
							this->addNetworkEdge(NetworkEdge(NodeCount + dest_right*NodeCount + source_id, dest_right, source_id));

						}

						if (Y < (YSIZE - 1)) {
							int dest_bottom = X + (Y+1) * XSIZE ;

							// Connect with previous nodes on the top
							this->addNetworkEdge(NetworkEdge(NodeCount + source_id*NodeCount + dest_bottom, source_id, dest_bottom));
							this->addNetworkEdge(NetworkEdge(NodeCount + dest_bottom*NodeCount + source_id, dest_bottom, source_id));

						}

					}
				}
				VERBOSE_DEBUG( "dim=" << XSIZE << "x" << YSIZE << ",edges=" << _vedges.size() << ",medges=" << _medges.size() );



}



