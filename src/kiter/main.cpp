/*
 *  This program is a Dataflow Analysis Tool : Kiter
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


std::vector<algo_t> algorithmslist =               {

		      { "1PeriodicThroughput"              , "Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.",
			algorithms::compute_1Kperiodic_throughput} ,
		      { "2PeriodicThroughput"              , "Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.",
			algorithms::compute_2Kperiodic_throughput} ,
		      { "NKPeriodicThroughput"             , "Optimal Throughput evaluation of CSDF by using N-periodic method.",
			algorithms::compute_NKperiodic_throughput} ,
		      { "NPeriodicThroughput"              , "Optimal Throughput evaluation of SDF by using Munier1993 method.",
			algorithms::compute_NPeriodic_throughput} ,
		      { "NCleanPeriodicThroughput"         , "Optimal Throughput evaluation of SDF by using Munier1993 method combined with deGroote2012 reduction.",
			algorithms::compute_NCleanPeriodic_throughput} ,
		      { "KPeriodicThroughput"              , "Optimal Throughput evaluation of CSDF by K-Periodic scheduling method 2.",
			algorithms::compute_Kperiodic_throughput} ,
		      { "PrintKPeriodicThroughput"           , "Optimal Throughput evaluation of CSDF by K-Periodic scheduling method X with starting times.",
			algorithms::print_kperiodic_scheduling} ,		      
		      { "PrintInfos"                       , "Just print some graph informations.",
			printers::printInfos},
		      { "PrintGraph"                       , "Print DOT file",
			printers::printGraph},
		      { "deGrooteThroughput"               , "Throughput analysis from deGroote2012 paper except event graph reduction.",
			algorithms::compute_deGroote_throughput},
		      { "deGrooteCleanThroughput"          , "Throughput analysis from deGroote2012 paper.",
			algorithms::compute_deGrooteClean_throughput},
		      { "SymbolicExecution"                       , "Execute task in ASAP fashion and print out the scheduling.",
		    algorithms::symbolic_execution},
		      { "SoftwareControlledNoC"                       , "Perform NoC scheduling after deciding task mapping and routing.",
		    algorithms::software_noc},
		      { "SymbolicExecutionWP"                       , "Execute task in ASAP fashion and print out the scheduling.",
		    algorithms::symbolic_execution_with_packets},
			{ "PeriodicSizing"                  , "Minimal Buffer size estimation by periodic scheduling method.",
					algorithms::compute_csdf_1periodic_memory} ,
			{ "BurstSizing"                    , "Minimal Buffer size estimation by periodic scheduling with BURST policy.",
					algorithms::compute_burst_memory} ,
			{ "AverageSizing"                  , "Minimal Buffer size estimation by periodic scheduling with AVERAGE policy.",
					algorithms::compute_average_memory} ,
			{ "MinMaxSizing"                  , "Minimal Buffer size estimation by periodic scheduling with MINMAX policy.",
					algorithms::compute_minmax_memory} ,
			{ "WiggersSizing"                  , "Minimal Buffer size estimation by periodic scheduling with Wiggers policy.",
					algorithms::compute_wiggers_memory} ,
			{ "1PeriodicSizing"                  , "Minimal Buffer size estimation by periodic scheduling method.",
					algorithms::compute_1Periodic_memory} ,
			{ "NPeriodicSizing"              , "Optimal Sizing evaluation of SDF by using N-Periodic method.",
					algorithms::compute_NPeriodic_memory} ,
			{ "KPeriodicSizing"              , "Optimal Sizing evaluation of SDF by using K-Periodic method.",
					algorithms::compute_KPeriodic_memory} ,
			{ "PrintPeriodic"                   , "Print reduced XML version and DOT definition of the event graph produced by periodic method.",
					algorithms::print_periodic_eventgraph  },
			{ "Print2Periodic"                   , "Print reduced XML version and DOT definition of the event graph produced by periodic method.",
					algorithms::print_2periodic_eventgraph  },
			{ "Print1Periodic"                   , "Print reduced XML version and DOT definition of the event graph produced by periodic method.",
					algorithms::print_1periodic_eventgraph  },
			{ "PrintNPeriodic"                   , "Print reduced XML version and DOT definition of the event graph produced by periodic method.",
					algorithms::print_Nperiodic_eventgraph  },
			{ "PrintNKPeriodic"                   , "Print reduced XML version and DOT definition of the event graph produced by periodic method.",
					algorithms::print_NKperiodic_eventgraph  },
			{ "BackPressureSizing"              , "Buffer sizing method from Wiggers et al DAC 2007 paper.",
					algorithms::compute_backpressure_memory_sizing}
                  
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
	std::vector<std::string> algos;
	parameters_list_t parameters;
	commons::set_verbose_mode(commons::WARNING_LEVEL); // default verbose mode is ...

	// **  Retreive arguments **
	int c;
    while ((c = getopt(argc, (char **)argv, "f:a:v:p:")) != -1) {
    	if (c == 'f') {
    		filename = optarg;
    	}
    	if (c == 'a') {
    		algos.push_back(optarg);
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

    VERBOSE_INFO("Parameter parsing is done.");

    /*
     * Here, we gathered two important values :
     *   - filename (default is stdin)
     *   - algo (default is print)
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
    for ( std::vector<algo_t>::iterator lit = algorithmslist.begin() ; lit != algorithmslist.end() ; lit++ ) {
        for ( std::vector<std::string>::iterator it = algos.begin() ; it != algos.end() ; it++ ) {
            if (*it == lit->name) {
               VERBOSE_INFO ("Run " << lit->name);
               tock();
               lit->fun(csdf,parameters);
               double duration = tock();
               VERBOSE_INFO (lit->name  << " duration=" << duration);
               nothing = false;
            }
        }
    }

    if (nothing) {
            std::cerr << " Unsupported algorithm (-a NAME), list of supported algorithms is " << std::endl;
	    for ( std::vector<algo_t>::iterator lit = algorithmslist.begin() ; lit != algorithmslist.end() ; lit++ ) {	      
                std::cerr << " - " <<  lit->name  << " : " << lit->desc << std::endl;
            }
            exit(1);
    }

    exit(0);

}
