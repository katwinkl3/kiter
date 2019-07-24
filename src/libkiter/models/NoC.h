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

typedef unsigned long  bank_id_t;
typedef unsigned long  node_id_t;
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
	NoC () : _XSIZE(1), _YSIZE(1), _BANKCOUNT(1) {} ;
public :
	int size () {return _XSIZE * _YSIZE;}
	int bank_count () {return _BANKCOUNT;}

	NoC (int XSIZE, int YSIZE, int BANKCOUNT) : _XSIZE(XSIZE), _YSIZE(YSIZE), _BANKCOUNT(BANKCOUNT)  {
	for (int X = 0 ; X < XSIZE ; X ++) {

		for (int Y = 0 ; Y < YSIZE ; Y ++) {

			int source = X + Y * XSIZE ;

			if (X < (XSIZE - 1)) {
				int dest_right = X + 1 + Y * XSIZE ;

				edge e = edge(_medges.size(), source, dest_right);
				_medges.insert({std::pair<node_id_t,node_id_t>( source, dest_right),e});
				_vedges.push_back(e);

				edge ep = edge(_medges.size(), dest_right , source);
				_medges.insert({std::pair<node_id_t,node_id_t>(  dest_right , source),ep});
				_vedges.push_back(ep);

			}

			if (Y < (YSIZE - 1)) {
				int dest_bottom = X + (Y+1) * XSIZE ;
				{
				edge e = edge(_medges.size(), source, dest_bottom);
				_medges.insert({std::pair<node_id_t,node_id_t>( source, dest_bottom),e});
				_vedges.push_back(e);
				}
				{
				edge e = edge(_medges.size(), dest_bottom ,  source );
				_medges.insert({std::pair<node_id_t,node_id_t>( dest_bottom ,  source),e});
				_vedges.push_back(e);
				}

			}
		}
	}
	}
	std::vector<edge>& edges () {
		return _vedges;
	}

	edge_id_t find_edge (node_id_t src,node_id_t dst) {
		auto res = _medges.find (std::pair<node_id_t,node_id_t>(std::min(src,dst),std::max(src,dst))) ;
		if (res != _medges.end()) return res->second.id;
		ERROR ();
	}

	route_t get_route (node_id_t src,node_id_t dst) {

		auto srcx = src % _XSIZE;
		auto srcy  = src / _XSIZE;
		auto dstx = dst % _XSIZE;
		auto dsty = dst / _XSIZE;
		route_t res;


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



		return res;
	}


};


#endif /* SRC_LIBKITER_MODELS_NOC_H_ */
