/*
 * RandomGenerator.h
 *
 *  Created on: Apr 30, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_GENERATORS_RANDOMGENERATOR_H_
#define SRC_LIBKITER_GENERATORS_RANDOMGENERATOR_H_


#include <models/Dataflow.h>
#include <commons/KiterRegistry.h>

struct RandomGeneratorConfiguration {

	ARRAY_INDEX min_vertices_count        = 10;
	ARRAY_INDEX min_edges_count           = 15;
	ARRAY_INDEX max_vertices_count        = 10;
	ARRAY_INDEX max_edges_count           = 15;
	TOKEN_UNIT  max_weight                = 100;
	TIME_UNIT   max_duration              = 1;
	PHASE_INDEX max_phase_quantity        = 1;
	PHASE_INDEX max_init_phase_quantity   = 0;

	bool        no_reentrancy             = false;
	bool        no_self_loop              = true;

};

class RandomGenerator {

private :
	RandomGeneratorConfiguration _configuration;
public :
	RandomGenerator (const RandomGeneratorConfiguration & configuration) : _configuration (configuration) {

	}

	models::Dataflow * generate (int seed = 123) ;

};


namespace generators {
	models::Dataflow*  generate_random_sdf(parameters_list_t   param_list);
}

ADD_GENERATOR(GenerateRandomSDF,
		generator_t(
				{ "GenerateRandomSDF" , "generate LTE SDF.", generators::generate_random_sdf})
);



#endif /* SRC_LIBKITER_GENERATORS_RANDOMGENERATOR_H_ */
