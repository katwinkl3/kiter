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
	models::Dataflow*  generate_random_sdf(parameters_list_t   param_list) {
		RandomGeneratorConfiguration config;
		config.min_vertices_count       = 10;
		config.max_vertices_count       = 20;
		config.min_edges_count          = 20;
		config.max_edges_count          = 30;
		config.max_weight               = 10;
		config.max_duration             = 1;
		config.max_phase_quantity       = 1;
		config.max_init_phase_quantity  = 0;

		if (param_list.count("min_vertices_count")) { config.min_vertices_count = (commons::fromString<ARRAY_INDEX>(param_list["min_vertices_count"])); }
		if (param_list.count("min_edges_count"))    { config.min_edges_count   = (commons::fromString<ARRAY_INDEX>(param_list["min_edges_count"])); }

		if (param_list.count("max_vertices_count")) { config.max_vertices_count = (commons::fromString<ARRAY_INDEX>(param_list["max_vertices_count"])); }
		if (param_list.count("max_edges_count")) { config.max_edges_count = (commons::fromString<ARRAY_INDEX>(param_list["max_edges_count"])); }
		if (param_list.count("min_vertices_count")) { config.min_vertices_count = (commons::fromString<ARRAY_INDEX>(param_list["min_vertices_count"])); }
		if (param_list.count("min_edges_count")) { config.min_edges_count = (commons::fromString<ARRAY_INDEX>(param_list["min_edges_count"])); }
		if (param_list.count("max_weight")) { config.max_weight = (commons::fromString<ARRAY_INDEX>(param_list["max_weight"])); }
		if (param_list.count("max_duration")) { config.max_duration = (commons::fromString<TIME_UNIT>(param_list["max_duration"])); }
		if (param_list.count("max_phase_quantity")) { config.max_phase_quantity = (commons::fromString<ARRAY_INDEX>(param_list["max_phase_quantity"])); }
		if (param_list.count("max_init_phase_quantity")) { config.max_init_phase_quantity = (commons::fromString<ARRAY_INDEX>(param_list["max_init_phase_quantity"])); }

		RandomGenerator generator (config);
		int seed = 124;
		if (param_list.count("seed")) {
			seed = (commons::fromString<int>(param_list["seed"]));
		}
		return generator.generate(seed);
	}
}

ADD_GENERATOR(GenerateRandomSDF,
		generator_t(
				{ "GenerateRandomSDF" , "generate LTE SDF.", generators::generate_random_sdf})
);



#endif /* SRC_LIBKITER_GENERATORS_RANDOMGENERATOR_H_ */
