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
#include <algorithms/kperiodic.h>
#include <algorithms/nperiodic.h>
#include <algorithms/degroote.h>

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
//#define ALGORITHMS_COUNT 10

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
		      { "deGrooteThroughput"               , "Throughput analysis from deGroote2012 paper except event graph reduction.",
			algorithms::compute_deGroote_throughput},
		      { "deGrooteCleanThroughput"          , "Throughput analysis from deGroote2012 paper.",
			algorithms::compute_deGrooteClean_throughput}

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


    // Step 4 = Apply selected algorithm
    bool nothing = true;
    for ( std::vector<algo_t>::iterator lit = algorithmslist.begin() ; lit != algorithmslist.end() ; lit++ ) {
        for ( std::vector<std::string>::iterator it = algos.begin() ; it != algos.end() ; it++ ) {
            if (*it == lit->name) {
               std::cerr << "Run " << lit->name << std::endl;
               tock();
               lit->fun(csdf,parameters);
               double duration = tock();
               std::cerr << lit->name  << " duration=" << duration << std::endl;
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
