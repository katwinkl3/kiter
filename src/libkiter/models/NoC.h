/*
 * NoC.h
 *
 *  Created on: Jul 24, 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_MODELS_NOC_H_
#define SRC_LIBKITER_MODELS_NOC_H_

#include <map>
#include <vector>
#include <commons/verbose.h>

typedef unsigned long          bank_id_t;
typedef          long          noc_id_t;
typedef noc_id_t               node_id_t;
typedef noc_id_t               edge_id_t;
	typedef std::vector<edge_id_t> route_t;
	//inline std::ostream& operator<< (std::ostream &out,const route_t& r)		{bool first = true; for (edge_id_t e : r) { if (!first) {out << "->";}  out << e ; first = false;} return out;}
	enum NetworkNodeType { Core, Router, Unknown };

	struct NetworkComponent {
		noc_id_t id;
		NetworkComponent (noc_id_t id) : id(id) {};

	};
	struct NetworkNode : public NetworkComponent {
		//node_id_t id;
		NetworkNodeType type;
		double       x;
		double       y;
		NetworkNode (node_id_t id, NetworkNodeType type, double x , double y)  :  NetworkComponent(id),	 type(type), x(x),	 y (y) {};
	};
	inline std::ostream& operator<< (std::ostream &out,const NetworkNode& n) {
		if (n.type == NetworkNodeType::Router) {
			out << "Router(" << n.id << ", x=" << n.x << ", y=" << n.y << ")";
		} else if (n.type == NetworkNodeType::Core) {
			out << "Core(" << n.id << ", x=" << n.x  << ", y=" << n.y << ")";
		} else {
			out << "UnknownNetworkNode(" << n.id << ", x=" << n.x  << ", y=" << n.y << ")";
		}
		return out;
	}

	struct NetworkEdge : public NetworkComponent{
		//edge_id_t id;
		node_id_t src;
		node_id_t dst;
		NetworkEdge(edge_id_t id,
				noc_id_t src,
				noc_id_t dst) :  NetworkComponent(id),	 src(src),	 dst (dst) {};
	};
	inline std::ostream& operator<< (std::ostream &out,const NetworkEdge& e) { out << "NetworkEdge(" << e.id << ", src=" << e.src  << ", dest=" << e.dst << ")"; return out;}


class NoC {

private :
	int _XSIZE, _YSIZE;
	std::map<std::pair<node_id_t,node_id_t>,NetworkEdge> _medges;
	std::map<edge_id_t,NetworkEdge> _mid2edges;
	std::map<node_id_t,NetworkNode> _mnodes;
	std::vector<NetworkEdge> _vedges;
	std::vector<NetworkNode> _vnodes;

	NoC () : _XSIZE(0), _YSIZE(0) {
	} ;
public:
	const std::vector<NetworkEdge>& getEdges() const {return this->_vedges;};
	const std::vector<NetworkNode>& getNodes() const {return this->_vnodes;};
	      bool           hasNode(node_id_t id) const {return (this->_mnodes.count(id) > 0);};
	      bool           hasEdge(edge_id_t id)  const {return (this->_mid2edges.count(id) > 0);};

	const NetworkNode&   getNode(node_id_t id) const {
		VERBOSE_ASSERT(this->_mnodes.count(id), "Impossible to find node " << id << " inside the NoC.");
		return this->_mnodes.at(id);
	};

	const NetworkEdge& getEdge(edge_id_t e) const {return this->_mid2edges.at(e);};
	const NetworkEdge& getEdge(node_id_t src,node_id_t dst) const {
		const std::pair<node_id_t,node_id_t> pair = {src,dst};
		VERBOSE_ASSERT(this->_medges.count(pair), "Impossible to find edge from " << src  << " to " << dst  << " inside the NoC.");
		return this->_medges.at(pair);
	};

	const NetworkEdge& getLeftNeighbour(node_id_t src) const {
		for (const auto &it : this->_vedges){
			if ((it.src == src) && (this->getNode(it.src).y == this->getNode(it.dst).y) 
				&& (this->getNode(it.src).x > this->getNode(it.dst).x)){
					return it;
			}
		}
	};
	const NetworkEdge& getRightNeighbour(node_id_t src) const {
		for (const auto &it : this->_vedges){
			if ((it.src == src) && (this->getNode(it.src).y == this->getNode(it.dst).y) 
				&& (this->getNode(it.src).x < this->getNode(it.dst).x)){
					return it;
			}
		}
	};
	const NetworkEdge& getTopNeighbour(node_id_t src) const {
		for (const auto &it : this->_vedges){
			if ((it.src == src) && (this->getNode(it.src).x == this->getNode(it.dst).x) 
				&& (this->getNode(it.src).y < this->getNode(it.dst).y)){
					return it;
			}
		}
	};
	const NetworkEdge& getBottomNeighbour(node_id_t src) const {
		for (const auto &it : this->_vedges){
			if ((it.src == src) && (this->getNode(it.src).x == this->getNode(it.dst).x) 
				&& (this->getNode(it.src).y > this->getNode(it.dst).y)){
					return it;
			}
		}
	};
	const NetworkEdge& getSrcNeighbour(node_id_t src) const {
		//Match associated router
		for (const auto &it : this->_vedges){
			if (it.src == src){
					return it;
			}
		}
	};
	const NetworkEdge& getDstNeighbour(node_id_t dst) const {
		//Match associated router
		for (const auto &it : this->_vedges){
			if (it.dst == dst){
					return it;
			}
		}
	};


		//the routers are from (0 to (NXN)-1)
		//while the cores are marked from (NXN) to 2*(NXN) - 1

		NoC (int XSIZE, int YSIZE) ;


public :
	int size      () const {return _XSIZE * _YSIZE;}
	int getXSize  () const {return _XSIZE;}
	int getYSize  () const {return _YSIZE;}





};


#endif /* SRC_LIBKITER_MODELS_NOC_H_ */
