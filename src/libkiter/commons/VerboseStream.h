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
#include <vector>
#include <string>
#include <set>

const std::string cPURPLE = "\033[1;35m";
const std::string cRED    = "\033[1;31m";
const std::string cYELLOW = "\033[0;33m";
const std::string cGREEN  = "\033[1;32m";
const std::string cBLUE   = "\033[1;34m";
const std::string cRESET  = "\033[0m";

const std::set<std::string> cOLORS = {cPURPLE, cRED, cYELLOW, cGREEN, cBLUE, cRESET};


// const inline std::string _verbosegetFilename(const std::string s) { return s.substr(s.find_last_of("/\\")+1);}
// #define __SHOW_LEVEL "[ "<< __RELEASE__ <<"  " << _verbosegetFilename(__FILE__) << ":" << __LINE__ << "]" << RESET_COLOR << " "


typedef std::vector<std::string> debug_commands;

class VerboseStream {
    public:
        // Singleton Caller
        static VerboseStream& get(){
            return stream;
        }

        VerboseStream(const VerboseStream &) = delete;

        // Singleton Clear
        VerboseStream& clean(){
            (*this).sstream.str("");
            (*this).sstream.clear();
            (*this).commands.clear();
            return *this;
        }

        // Singleton null
        VerboseStream& null() {
            (*this).sstream << "null";
            return *this;
        }

        // Appending Commands
        void addcommands(debug_commands c){
            (*this).commands.insert((*this).commands.end(), c.begin(), c.end());
        }

        // Adding to output string
        void add2stream(std::string s){
            (*this).sstream << s;
        }

        // Defining "<<" operator
        template<typename T>
        VerboseStream& operator<<(T val) {
            if ((*this).sstream.str() != "null"){
                (*this).sstream << val;

                // Color definition
                std::string color;
                for (std::string c : (*this).commands){
                    if (cOLORS.count(c) == 1){
                        // Should only be one color defined
                        color = c;
                        std::cout << color << (*this).sstream.str();
                    }
                }
            } else {
                BOOST_REQUIRE(false);
            }
            /* Not using clean commands bc. need to maintain
            the debug_commands */
            (*this).sstream.str("");
            (*this).sstream.clear();
            return *this;                
        }

    private:
        // Variables
        debug_commands commands;
        std::ostringstream sstream;

        // Init function
        VerboseStream () {};

        // Singleton Instance
        static VerboseStream stream;


};

// Creating the singleton instance (globally?)
VerboseStream VerboseStream::stream;

namespace Verbose {

    VerboseStream& Info(debug_commands c = {}) {
        VerboseStream& out = VerboseStream::get();
        out.clean();
        out.addcommands(c);
        return out;
    }

    VerboseStream& Debug(debug_commands c = {}) {
        c.insert(c.begin(), "DEBUG");
        VerboseStream& out = Verbose::Info(c);
        return out;
    }

    VerboseStream& Assert(bool statement, debug_commands c = {}) {
        VerboseStream& out = Verbose::Info(c);
        if (!statement){
            out.add2stream("Assertion failed : ");
            return out;
        } else {
            return out.null();
        }
    }
    
}


#endif /* VERBOSE_STREAM_H_ */




// int main(){
//     Verbose::info({"debug"}) << "egeee";
//     Verbose::debug() << "egeee2" << 5 << "XX";
//     Verbose::assert(false) << "agaga";
    
// }