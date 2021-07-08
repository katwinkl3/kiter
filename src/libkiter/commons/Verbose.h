/*
 *  VerboseStream.h
 *
 *  Created on: 1 Jul. 2021
 *  Author: SamKouteili
 *
 */



#ifndef VERBOSE_STREAM_H_
#define VERBOSE_STREAM_H_

#include <iostream>
#include <sstream>
#include <string>
#include <set>


const std::string cPURPLE = "\033[1;35m";
const std::string cRED    = "\033[1;31m";
const std::string cYELLOW = "\033[0;33m";
const std::string cGREEN  = "\033[1;32m";
const std::string cBLUE   = "\033[1;34m";
const std::string cRESET  = "\033[0m";

const std::set<std::string> cOLORS = {cPURPLE, cRED, cYELLOW, cGREEN, cBLUE, cRESET};

const int  MIN_LEVEL     =   0;
const int  MAX_LEVEL     =  10;

const int  NULL_LEVEL          =   0;
const int  ERROR_LEVEL         =   1;
const int  WARNING_LEVEL       =   2;
const int  PB_LEVEL 	       =   3;
const int  INFO_LEVEL          =   4;
const int  ILP_LEVEL           =   7;
const int  DEBUG_LEVEL         =   8;
const int  EXTRA_DEBUG_LEVEL   =  10;

// Currently needed for __SHOW_LEVEL, verboseFilename, EXIT_ON_FAILURE()
#include "verbose.h"
// const inline std::string _verbosegetFilename(const std::string s) { return s.substr(s.find_last_of("/\\")+1);}
// #define __SHOW_LEVEL "[ "<< __RELEASE__ <<"  " << _verbosegetFilename(__FILE__) << ":" << __LINE__ << "]" << RESET_COLOR << " "
// #define EXIT_ON_FAILURE()  {int* toto = NULL; *toto = 1;} exit(EXIT_FAILURE)

class VerboseStream {
    public:
        // Singleton Caller
        static VerboseStream& get(){
            return stream;
        }

        VerboseStream(const VerboseStream &) = delete;

        void set_lvl(int lvl){
            this->debug_lvl = lvl;
        }

        void set_statement(std::string m){
            this->assert_call = m; 
        }

        // Defining "<<" operator
        template<typename T>
        VerboseStream& operator<<(T val) {
            std::ostringstream s;
            s << val;

            switch (this->debug_lvl){
            case INFO_LEVEL:
                std::cerr << cGREEN << "[I] " << __SHOW_LEVEL << s.str() << "\n";
                break;
            case DEBUG_LEVEL:
                std::cerr << cBLUE  << "[D] " << __SHOW_LEVEL << s.str() << "\n";
                break;
            case ERROR_LEVEL:
                std::cerr << cRED   << "[E] " << __SHOW_LEVEL << this->assert_call << s.str() << "\n";
                EXIT_ON_FAILURE();
                break;
            case NULL_LEVEL:
                break;
            default:
                break;                
            }      
            return *this;                
        }

    private:
        // Variables
        int debug_lvl;
        std::string assert_call;

        // Init function
        VerboseStream () {};

        // Singleton Instance
        static VerboseStream stream;


};

// Creating the singleton instance
VerboseStream VerboseStream::stream;

namespace Verbose {

    VerboseStream& Info() {
        VerboseStream& out = VerboseStream::get();
        out.set_lvl(INFO_LEVEL);
        return out;
    }

    VerboseStream& Debug() {
        VerboseStream& out = VerboseStream::get();
        out.set_lvl(DEBUG_LEVEL);
        return out;
    }


    // How can we implement test and #test BOOST types?
    VerboseStream& Assert(bool statement) {
        VerboseStream& out = VerboseStream::get();
        if (statement){
            out.set_lvl(NULL_LEVEL);
            out.set_statement("Assertion Failure: ");
        } else {
            out.set_lvl(ERROR_LEVEL);
        }
        return out;
    }

    template<typename T>
    VerboseStream& AssertGreater(T a, T b) {
        VerboseStream& out = VerboseStream::get();
        if (a > b){
            out.set_lvl(NULL_LEVEL);
        } else {
            out.set_lvl(ERROR_LEVEL);
            std::stringstream s;
            s << "Assertion Failure: " << "(" << a + " > " << b + ") ";
            out.set_statement(s.str());
        }
        return out;
    }

    template<typename T>
    VerboseStream& AssertGreaterOrEqual(T a, T b) {
        VerboseStream& out = VerboseStream::get();
        if (a > b){
            out.set_lvl(NULL_LEVEL);
        } else {
            out.set_lvl(ERROR_LEVEL);
            std::stringstream s;
            s << "Assertion Failure: " << "(" << a + " >= " << b + ") ";
            out.set_statement(s.str());
        }
        return out;
    }


    // Not sure what this is meant to be doing? Weird function name
    // template<typename T>
    // VerboseStream& AssertContains(T a, T b) {
    //     VerboseStream& out = VerboseStream::get();
    //     if (a | b){
    //         out.set_lvl(NULL_LEVEL);
    //     } else {
    //         out.set_lvl(ERROR_LEVEL);
    //         out.set_statement("Assertion Failure: " + "(" + std::to_string(a) + " < " + std::to_string(b) + ") ");
    //     }
    //     return out;
    // }
    

}


#endif /* VERBOSE_STREAM_H_ */

