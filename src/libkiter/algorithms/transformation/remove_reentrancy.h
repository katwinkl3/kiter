/*
 * remove_reentrancy.h
 *
 *  Created on: Feb 24, 2021
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_REMOVE_REENTRANCY_H_
#define SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_REMOVE_REENTRANCY_H_



#include <string>
#include <vector>
#include <commons/commons.h>
#include <commons/KiterRegistry.h>



namespace models {
	class Dataflow;
	class Scheduling;
}

namespace algorithms {
	namespace transformation {
		void remove_reentrancy    (models::Dataflow* const dataflow, parameters_list_t  parameters  );
	}
}

ADD_TRANSFORMATION(RemoveReentrancy,
		transformation_t({ "RemoveReentrancy" , "Set reentrancy to 0 for every task or selected tasks.", algorithms::transformation::remove_reentrancy}));


#endif /* SRC_LIBKITER_ALGORITHMS_TRANSFORMATION_REMOVE_REENTRANCY_H_ */
