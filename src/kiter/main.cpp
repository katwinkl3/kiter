/*
 * This program is a Dataflow Analysis Tool : Kiter
 *
 *
 *
 * */

#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <commons/verbose.h>
#include <commons/SDF3Wrapper.h>
#include <models/EventGraph.h>
#include <models/Dataflow.h>
#include <printers/stdout.h>
#include <algorithms/periodic.h>
#include <algorithms/periodic_fixed.h>
#include <algorithms/kperiodic.h>
#include <algorithms/software_noc.h>
#include <algorithms/nperiodic.h>
#include <algorithms/degroote.h>
#include <algorithms/symbolicExecution.h>
#include <algorithms/backpressure.h>
#include <algorithms/mappings.h>
#include <algorithms/schedulings.h>
#include <algorithms/transformations.h>

struct algo_t {
	std::string name;
	std::string desc;
	void (*fun)(models::Dataflow*, parameters_list_t);
};
inline double tock() {
	static struct timespec last;
	struct timespec clockData;
	clock_gettime(CLOCK_MONOTONIC, &clockData);
	double diff = clockData.tv_sec - last.tv_sec + (clockData.tv_nsec - last.tv_nsec) / 1000000000.0;
	last = clockData;
	return diff;
}
//!< List of algorithms


std::vector<algo_t> algorithmslist = {

		// Printers
		{ "PrintXML" , "Print XML file", printers::printXML},		{ "PrintKiter" , "Generate C++ code to internally generate the graph inside Kiter.", printers::printGraphAsKiterScript} ,
		{ "PrintInfos" , "Just print some graph informations.", printers::printInfos},
		{ "PrintGraph" , "Print DOT file", printers::printGraph},

		// Helpers to map vertex to cores, need a way to also consider routers
		{ "randomMapping" , "This command will associate a mapping to each task of the graph. Task unspecified as parameters will be randomly allocated to a core.", algorithms::mapping::randomMapping} ,
		{ "moduloMapping" , "This command will associate a mapping to each task of the graph. Task unspecified as parameters will be randomly allocated to a core.", algorithms::mapping::moduloMapping} ,

		// Throughput techniques
		{ "SDFKPeriodic" , "Build an Exansion graph given a set of value K (1 by default), and compute its MCRP.", algorithms::scheduling::SDFKPeriodicScheduling} ,
		{ "BufferlessKPeriodicScheduling" , "Run Bufferless Kperiodic", algorithms::scheduling::bufferlessKPeriodicScheduling} ,
		{ "1PeriodicThroughput" , "Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.", algorithms::compute_1Kperiodic_throughput} ,
		{ "2PeriodicThroughput" , "Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.", algorithms::compute_2Kperiodic_throughput} ,
		{ "NKPeriodicThroughput" , "Optimal Throughput evaluation of CSDF by using N-periodic method.", algorithms::compute_NKperiodic_throughput} ,
		{ "NPeriodicThroughput" , "Optimal Throughput evaluation of SDF by using Munier1993 method.", algorithms::compute_NPeriodic_throughput} ,
		{ "NCleanPeriodicThroughput" , "Optimal Throughput evaluation of SDF by using Munier1993 method combined with deGroote2012 reduction.", algorithms::compute_NCleanPeriodic_throughput} ,
		{ "KPeriodicThroughput" , "Optimal Throughput evaluation of CSDF by K-Periodic scheduling method 2.", algorithms::compute_Kperiodic_throughput} ,
		{ "deGrooteThroughput" , "Throughput analysis from deGroote2012 paper except event graph reduction.", algorithms::compute_deGroote_throughput},
		{ "deGrooteCleanThroughput" , "Throughput analysis from deGroote2012 paper.", algorithms::compute_deGrooteClean_throughput},



		//Buffer sizing techniques
		{ "PeriodicSizing" , "Minimal Buffer size estimation by periodic scheduling method.", algorithms::compute_csdf_1periodic_memory} ,
		{ "BurstSizing" , "Minimal Buffer size estimation by periodic scheduling with BURST policy.", algorithms::compute_burst_memory} ,
		{ "AverageSizing" , "Minimal Buffer size estimation by periodic scheduling with AVERAGE policy.", algorithms::compute_average_memory} ,
		{ "MinMaxSizing" , "Minimal Buffer size estimation by periodic scheduling with MINMAX policy.", algorithms::compute_minmax_memory} ,
		{ "WiggersSizing" , "Minimal Buffer size estimation by periodic scheduling with Wiggers policy.", algorithms::compute_wiggers_memory} ,
		{ "1PeriodicSizing" , "Minimal Buffer size estimation by periodic scheduling method.", algorithms::compute_1Periodic_memory} ,
		{ "NPeriodicSizing" , "Optimal Sizing evaluation of SDF by using N-Periodic method.", algorithms::compute_NPeriodic_memory} ,
		{ "KPeriodicSizing" , "Optimal Sizing evaluation of SDF by using K-Periodic method.", algorithms::compute_KPeriodic_memory} ,
		{ "BackPressureSizing" , "Buffer sizing method from Wiggers et al DAC 2007 paper.", algorithms::compute_backpressure_memory_sizing},

		// Various
		{ "SymbolicExecution" , "Execute task in ASAP fashion and print out the scheduling.", algorithms::symbolic_execution},
		{ "genLTESDF" , "generate LTE SDF.", algorithms::generate_lte_sdf},
		{ "SoftwareControlledNoCBufferless" , "Perform Bufferless NoC scheduling after deciding task mapping and routing.", algorithms::software_noc_bufferless},
		{ "SymbolicExecutionWP" , "Execute task in ASAP fashion and print out the scheduling.", algorithms::symbolic_execution_with_packets},

		// Trasnformation
		{ "Merge" , "Merging tasks using Kperiodic scheduling and Initialization phases", algorithms::transformation::merge_tasks},


		// Recent stuff
		{ "LP1" , "Rewriting Bodin2013 Threshold CSDF Periodic Scheduling", algorithms::scheduling::CSDF_1PeriodicScheduling_LP},
		{ "LPN" , "Rewriting Bodin2013 Threshold CSDF Periodic Scheduling", algorithms::scheduling::CSDF_NPeriodicScheduling_LP},
		{ "EGN" , "Rewriting Bodin2013 Threshold CSDF Periodic Scheduling", algorithms::scheduling::CSDF_NPeriodicScheduling}


};




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
	parameters_list_t parameters;
	commons::set_verbose_mode(commons::WARNING_LEVEL); // default verbose mode is ...

	// ** Retreive arguments **
	int c;
	while ((c = getopt(argc, (char **)argv, "f:a:v:p:")) != -1) {
		if (c == 'f') {
			filename = optarg;
		}
		if (c == 'a') {
			VERBOSE_INFO("Store algo " << optarg);
			algos.push_back(std::pair<std::string,parameters_list_t>(optarg,parameters));
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
	}

	VERBOSE_INFO("Parameter parsing is done: " << algos.size() << " algo identified.");

	/*
	 * Here, we gathered two important values :
	 * - filename (default is stdin)
	 * - algo (default is print)
	 *
	 * */


	// Step 1 = Load XML file

	VERBOSE_INFO("Read XML file : " << filename);
	models::Dataflow* csdf = commons::readSDF3File(filename);

	if (!csdf) {
		VERBOSE_ERROR("Error with XML file.");
		std::cerr << " No input file or enable to read it, please us -f FILENAME." << std::endl;
		exit(1);
	}


	// Step 2 = Check parameters
	for ( parameters_list_t::iterator it = parameters.begin() ; it != parameters.end() ; it++ ) {
		VERBOSE_INFO("Additionnal parameter found: '" << it->first << "' = '" << it->second << "'");
	}

	// Step 3 - Compute maximum periodic throughput

	TIME_UNIT FREQUENCY = 0;

	if (parameters.find("FREQUENCY") != parameters.end() ) {
		VERBOSE_ASSERT(parameters.find("FREQUENCY") != parameters.end() , "FREQUENCY is not set, need one." );
		FREQUENCY = commons::fromString<TIME_UNIT>(parameters["FREQUENCY"]);
		VERBOSE_ASSERT(FREQUENCY > 0,"Need a postive FREQUENCY, '"<< parameters["FREQUENCY"] << "' is not .");
	} else {

		if (parameters.find("RATIO") != parameters.end() ) {
			FREQUENCY = algorithms::optimal_1periodic_throughput(csdf);
		}
	}

	if (parameters.find("RATIO") != parameters.end() ) {
		FREQUENCY = FREQUENCY * commons::fromString<TIME_UNIT>(parameters["RATIO"]);
	}

	csdf->setPeriod(1.0 / FREQUENCY );
	VERBOSE_INFO("PERIOD =" << csdf->getPeriod());



	// Step 4 = Apply selected algorithm
	bool nothing = true;
	for ( std::vector<std::pair<std::string,parameters_list_t>>::iterator it = algos.begin() ; it != algos.end() ; it++ ) {
		for ( std::vector<algo_t>::iterator lit = algorithmslist.begin() ; lit != algorithmslist.end() ; lit++ ) {
			if ((*it).first == lit->name) {
				VERBOSE_INFO ("Run " << lit->name);
				tock();
				(*it).second.insert(parameters.begin(),parameters.end());
				lit->fun(csdf,(*it).second);
				double duration = tock();
				VERBOSE_INFO (lit->name << " duration=" << duration);
				nothing = false;
			}
		}
	}

	if (nothing) {
		std::cerr << " Unsupported algorithm (-a NAME), list of supported algorithms is " << std::endl;
		for ( std::vector<algo_t>::iterator lit = algorithmslist.begin() ; lit != algorithmslist.end() ; lit++ ) {
			std::cerr << " - " << lit->name << " : " << lit->desc << std::endl;
		}
		exit(1);
	}

	exit(0);

}
