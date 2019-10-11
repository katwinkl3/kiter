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
#include <commons/NoCGraph.h>

typedef unsigned long  bank_id_t;
typedef          long  node_id_t;
typedef unsigned long edge_id_t;
typedef std::vector<edge_id_t> route_t;

struct edge {
	edge_id_t id;
	node_id_t src;
	node_id_t dst;
	edge(edge_id_t id,
			node_id_t src,
			node_id_t dst) :  id(id),	 src(src),	 dst (dst) {};
};

class NoC {
private :
	int _XSIZE, _YSIZE, _BANKCOUNT;
	std::map<std::pair<node_id_t,node_id_t>,edge> _medges;
	std::vector<edge> _vedges;
	NoCGraph* g;

	NoC () : _XSIZE(1), _YSIZE(1), _BANKCOUNT(1) {g = new NoCGraph(1);} ;

public :
	int size () const {return _XSIZE * _YSIZE;}
	int bank_count () {return _BANKCOUNT;}
	int getMeshSize () {return size();}

	void printPaths(int s, int d) 
	{
		auto length = get_route(s, d).size();
		g->printAllPaths((int)s + size(), (int)d + size(), (int)length);
	}

	void generateShortestPaths() { g->generateAllShortestPaths(); }
	std::vector< std::vector<int> > getShortestPaths(int s, int d) { return g->getShortestPaths(s, d); }
	//std::map<int, int> getLinkUtil() { return g->getLinkUtil(); }
	std::vector<int> getLinkUtil() { return g->getLinkUtil(); }
	//void setLinkUtil(std::map<int, int> u) { g->setLinkUtil(u); }
	void setLinkUtil(std::vector<int> u) { g->setLinkUtil(u); }

	void clear() { g->clear(); }
	int getMapIndex(int x, int y) { return g->getMapIndex(x,y); }
	void getMapIndexPair(int index, int* x, int* y) { return g->getMapIndexPair(index, x, y); }
	int getPathLength(int src, int dst) { return g->getPathLength(src, dst); }
	std::map<int, int> edgeIndexMap;

	//the routers are from (0 to (NXN)-1)
	//while the cores are marked from (NXN) to 2*(NXN) - 1

	NoC (int XSIZE, int YSIZE, int BANKCOUNT) : _XSIZE(XSIZE), _YSIZE(YSIZE), _BANKCOUNT(BANKCOUNT)  {
		
		g = new NoCGraph((int)size()*2);

		for (int X = 0 ; X < XSIZE ; X ++) {

			for (int Y = 0 ; Y < YSIZE ; Y ++) {

				int source = X + Y * XSIZE ;
				int core_id = source + this->size();

				//add edges between source core and router
				edge ep = edge(_medges.size(), core_id, source);
				_medges.insert({std::pair<node_id_t,node_id_t>(core_id , source),ep});
				_vedges.push_back(ep);
				g->addEdge(core_id, source);


				edge e = edge(_medges.size(), source, core_id);
				_medges.insert({std::pair<node_id_t,node_id_t>(source, core_id),e});
				_vedges.push_back(e);
				g->addEdge(source, core_id);


				if (X < (XSIZE - 1)) {
					int dest_right = (X + 1) + Y * XSIZE ;

					edge e = edge(_medges.size(), source, dest_right);
					_medges.insert({std::pair<node_id_t,node_id_t>( source, dest_right),e});
					_vedges.push_back(e);
					g->addEdge(source, dest_right);

					edge ep = edge(_medges.size(), dest_right, source);
					_medges.insert({std::pair<node_id_t,node_id_t>(  dest_right , source),ep});
					_vedges.push_back(ep);
					g->addEdge(dest_right, source);
				}

				if (Y < (YSIZE - 1)) {
					int dest_bottom = X + (Y+1) * XSIZE ;
					
					edge e = edge(_medges.size(), source, dest_bottom);
					_medges.insert({std::pair<node_id_t,node_id_t>( source, dest_bottom),e});
					_vedges.push_back(e);
					g->addEdge(source, dest_bottom);
					
					
					edge ep = edge(_medges.size(), dest_bottom, source);
					_medges.insert({std::pair<node_id_t,node_id_t>( dest_bottom ,  source),ep});
					_vedges.push_back(ep);
					g->addEdge(dest_bottom, source);
				}

			}
		}
		VERBOSE_INFO( "dim=" << XSIZE << "X" << YSIZE << ",edges=" << _vedges.size() << ",medges=" << _medges.size() );
	}
	std::vector<edge>& edges () {
		return _vedges;
	}

	edge_id_t find_edge (node_id_t src,node_id_t dst) const {

		//Remove the min operation as edge A->B is different from edge B->A
		//auto res = _medges.find (std::pair<node_id_t,node_id_t>(std::min(src,dst),std::max(src,dst))) ;
		auto res = _medges.find (std::pair<node_id_t,node_id_t>(src,dst)) ;
		if (res != _medges.end()) return res->second.id;
		ERROR ();
	}

	route_t get_route (node_id_t src,node_id_t dst) const {

		auto srcx = src % _XSIZE;
		auto srcy  = src / _XSIZE;
		auto dstx = dst % _XSIZE;
		auto dsty = dst / _XSIZE;
		route_t res;

		node_id_t src_core = src + this->size();
		node_id_t dst_core = dst + this->size();

		edge_id_t t_e = find_edge (src_core, src);
		res.push_back(t_e);

		while (srcy < dsty) {
			// move down
			// look up for srcx + (srcy * XSIZE) to  srcx  + ((srcy + 1) * XSIZE)
			edge_id_t e = find_edge ( srcx + (srcy * _XSIZE) ,  srcx  + ((srcy + 1) * _XSIZE));
					res.push_back(e);
			srcy++;
		}

		while (srcy > dsty) {
			// move up<< std::endl
			// look up for srcx + (srcy * XSIZE) to  srcx  + ((srcy - 1) * XSIZE)
			edge_id_t e = find_edge (srcx + (srcy * _XSIZE) ,  srcx  + ((srcy - 1) * _XSIZE) );
					res.push_back(e);
			srcy--;
		}


		while (srcx < dstx) {
			// move right
			// look up for srcx + (srcy * XSIZE) to  1 + srcx + (srcy * XSIZE)
			edge_id_t e = find_edge (srcx + (srcy * _XSIZE) ,  1 + srcx + (srcy * _XSIZE));
			res.push_back(e);
			srcx++;
		}

		while (srcx > dstx) {
			// move left
			// look up for srcx + (srcy * XSIZE) to  srcx - 1 + (srcy * XSIZE)
			edge_id_t e = find_edge (srcx + (srcy * _XSIZE) ,  srcx - 1 + (srcy * _XSIZE));
					res.push_back(e);
			srcx--;
		}

		t_e = find_edge (dst, dst_core);
		res.push_back(t_e);

		return res;
	}

};


#endif /* SRC_LIBKITER_MODELS_NOC_H_ */
