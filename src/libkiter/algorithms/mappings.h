/*
 * mappings.h
 *
 *  Created on: Aug 21, 2019
 *      Author: toky
 */

#ifndef _MAPPINGS_H_
#define _MAPPINGS_H_

#include <commons/commons.h>

namespace models {
	class Dataflow;
}
namespace algorithms {
	namespace mapping {
	void randomMapping (models::Dataflow* const  dataflow, parameters_list_t params);
	void moduloMapping (models::Dataflow* const  dataflow, parameters_list_t params);
	}
}

#endif
