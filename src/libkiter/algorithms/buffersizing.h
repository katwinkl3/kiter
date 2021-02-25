/*
 * buffersizing.h
 *
 *  Created on: Feb 25, 2021
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_BUFFERSIZING_H_
#define SRC_LIBKITER_ALGORITHMS_BUFFERSIZING_H_

#include <commons/basic_types.h>
#include <functional>

namespace models {
class Dataflow;
}

class BufferSizingResult {

	bool       _valid;
	TOKEN_UNIT _total_size;
public:
	BufferSizingResult(TOKEN_UNIT t) : _valid (true), _total_size(t) {}
	BufferSizingResult() : _valid (false), _total_size(0) {}
	bool is_valid() const {return _valid;}
	TOKEN_UNIT total_size() const {return _total_size;}
};

typedef std::function< BufferSizingResult(models::Dataflow* const, TIME_UNIT) > BufferSizingFun;

#endif /* SRC_LIBKITER_ALGORITHMS_BUFFERSIZING_H_ */
