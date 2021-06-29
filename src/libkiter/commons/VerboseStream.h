#include <iostream>
#include <sstream>
#include <vector>

typedef std::vector<std::string> commands;

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
            (*this).debug_commands.clear();
            return *this;
        }

        // Singleton null
        VerboseStream& null() {
            (*this).sstream << "null";
            return *this;
        }

        // Appending Commands
        void addcommands(commands c){
            (*this).debug_commands.insert((*this).debug_commands.end(), c.begin(), c.end());
         }

        // Defining "<<" operator
        template<typename T>
        VerboseStream& operator<<(T val) {
            if ((*this).sstream.str() != "null"){
                (*this).sstream << val; // 
                std::cout << (*this).sstream.str();
            }
            /* Not using clean commands bc. need to maintain
            the debug_commands */
            s.sstream.str("");
            s.sstream.clear();
            return *this;                
        }

    private:
        // Variables
        commands debug_commands;
        std::ostringstream sstream;

        // Init function
        VerboseStream () {};

        // Singleton Instance
        static VerboseStream stream;
};

// Creating the singleton instance (globally?)
VerboseStream VerboseStream::stream;

namespace Verbose {

    VerboseStream& info(commands c = {}) {
        VerboseStream& out = VerboseStream::get();
        out.clean();
        out.addcommands(c);
        return out;
    }

    VerboseStream& debug(commands c = {}) {
        c.insert(c.begin(), "DEBUG");
        VerboseStream& out = Verbose::info(c);
        return out;
    }

    VerboseStream& assert(bool statement, commands c = {}) {
        VerboseStream& out = Verbose::info(c);
        if (~statement){
            return out;
        } else {
            return out.null();
        }
    }
    
}

// int main(){
//     Verbose::info({"debug"}) << "egeee";
//     Verbose::debug() << "egeee2" << 5 << "XX";
//     Verbose::assert(false) << "agaga";
    
// }