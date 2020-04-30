/*
 * This program is a Dataflow Analysis Tool : Kiter
 * TODO : remove all these deps, dataflow should not be required
 *
 *
 * */

#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <commons/KiterRegistry.h>
#include <commons/verbose.h>
#include <commons/SDF3Wrapper.h>


inline double tock() {
	static struct timespec last;
	struct timespec clockData;
	clock_gettime(CLOCK_MONOTONIC, &clockData);
	double diff = clockData.tv_sec - last.tv_sec + (clockData.tv_nsec - last.tv_nsec) / 1000000000.0;
	last = clockData;
	return diff;
}
//!< List of algorithms







inline void activate_verbose(const std::string p) {

	if (commons::fromString<int>(p) < 0) {
		std::exit(EXIT_FAILURE);
	}
	commons::set_verbose_mode(commons::fromString<int>((optarg)));
}

int main (int argc, char **argv)
{

	// ** default arguments
	std::string filename = "";
	std::vector<std::pair<std::string,parameters_list_t>> algos;
	std::vector<std::pair<std::string,parameters_list_t>> generators;
	parameters_list_t parameters;
	commons::set_verbose_mode(commons::WARNING_LEVEL); // default verbose mode is ...

	// ** Retreive arguments **
	int c;
	while ((c = getopt(argc, (char **)argv, "f:g:a:v:p:h")) != -1) {
		if (c == 'f') {
			filename = optarg;
		}
		if (c == 'a') {
			VERBOSE_INFO("Store algo " << optarg);
			algos.push_back(std::pair<std::string,parameters_list_t>(optarg,parameters));
			parameters.clear();
		}
		if (c == 'g') {
			VERBOSE_INFO("Store generator " << optarg);
			generators.push_back(std::pair<std::string,parameters_list_t>(optarg,parameters));
			parameters.clear();
		}
		if (c == 'v') {
			activate_verbose(optarg);
		}
		if (c == 'p') {
			std::string arg = optarg;
			if (arg.find('=',0) != arg.npos) {
				parameters[arg.substr(0,arg.find('=',0))] = arg.substr(arg.find('=',0)+1,arg.size());
			}

		}
		if (c == 'h') {
			VERBOSE_INFO("-f FILENAME or -g GENERATOR, then -a ALGORITHM.");
		}
	}

	VERBOSE_DEBUG("Parameter parsing is done: " << algos.size() << " algo identified.");

	/*
	 * Here, we gathered two important values :
	 * - filename (default is stdin)
	 * - algo (default is print)
	 *
	 * */

	models::Dataflow* csdf = nullptr;


	// Step 1 = Load XML file if any
	if (filename != "") {
		VERBOSE_INFO("Read XML file : " << filename);
		csdf = commons::readSDF3File(filename);
	}

	if (generators.size() > 1) {
		VERBOSE_ERROR("Unsupported case, only one generator can be used.");
	}

	if (csdf and (generators.size() > 0)) {
		VERBOSE_ERROR("Unsupported case, cannot used file (-f) and generator (-g) at the same time.");
	}

	if (!csdf) {
		for ( std::vector<std::pair<std::string,parameters_list_t>>::iterator it = generators.begin() ; it != generators.end() ; it++ ) {
			std::string name = (*it).first;
			const generator_t* generator = KiterRegistry<generator_t>::get(name);
			if (generator)  {
				VERBOSE_INFO ("Run " << generator->name);
				tock();
				csdf = generator->fun(it->second);
				double duration = tock();
				VERBOSE_INFO (generator->name << " duration=" << duration);
			} else {
				std::cerr << " Unsupported generator (-g NAME) or no filename (-f FILENAME), list of supported generator is " << std::endl;
				KiterRegistry<generator_t>::print();
				exit(1);
			}

		}
	}


	if (!csdf) {
		VERBOSE_ERROR("Error with XML file or Generator.");
		std::cerr << " No input file or enable to read it, please us -f FILENAME or -g GENERATOR." << std::endl;
		exit(1);
	}


	// Step 2 = Check parameters
	for ( parameters_list_t::iterator it = parameters.begin() ; it != parameters.end() ; it++ ) {
		VERBOSE_DEBUG("Additionnal parameter found: '" << it->first << "' = '" << it->second << "'");
	}




	// Step 4 = Apply selected algorithm
	for ( std::vector<std::pair<std::string,parameters_list_t>>::iterator it = algos.begin() ; it != algos.end() ; it++ ) {
		std::string name = (*it).first;
		const transformation_t* transformation = KiterRegistry<transformation_t>::get(name);
		if (!transformation) {
			std::cerr << " Unsupported algorithm (-a " << name << "), list of supported algorithms is " << std::endl;
			KiterRegistry<transformation_t>::print();
			exit(1);
		} else {
			VERBOSE_INFO ("Run " << transformation->name);
			tock();
			(*it).second.insert(parameters.begin(),parameters.end());
			transformation->fun(csdf,(*it).second);
			double duration = tock();
			VERBOSE_INFO (transformation->name << " duration=" << duration);
		}
	}



	exit(0);

}
