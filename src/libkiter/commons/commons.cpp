/*
 * commons.cpp
 *
 *  Created on: 1 févr. 2011
 *      Author: toky
 */

#include <commons/commons.h>
#include <commons/verbose.h>



// REALLY STRANGE BUGFIX : For Andrea config (old boost version in fc10)
namespace std {
bool operator<(const boost::detail::edge_desc_impl<boost::bidirectional_tag, unsigned int>& lh, const boost::detail::edge_desc_impl<boost::bidirectional_tag, unsigned int>& rh)
{
  if (lh.m_source == rh.m_source) {return lh.m_target < rh.m_target;} else
	  {return lh.m_source < rh.m_source;}
}
}
namespace commons {


const std::string getFilename(const std::string s) { return s.substr(s.find_last_of("/\\")+1);}

TIME_UNIT roundIt(TIME_UNIT val,TIME_UNIT p) {
  std::stringstream s;
  s<<std::setprecision((int)p)<<std::setiosflags(std::ios_base::fixed)<<val;
   s>>val;
   return val;
}
template<>
std::string toString< std::vector<TOKEN_UNIT> >(const std::vector<TOKEN_UNIT>& v)
{
        return commons::join(v.begin(),v.end(),std::string(","));
}
template<>
std::string toString< std::vector<long double> >(const std::vector<long double>& v)
{
        return commons::join(v.begin(),v.end(),std::string(","));
}
template<>
std::string toString< std::vector<unsigned int> >(const std::vector<unsigned int>& v)
{
        return commons::join(v.begin(),v.end(),std::string(","));
}

template<>
	char *fromString<char*>(const std::string&)
{
		VERBOSE_BAD_PROGRAMMING(); // niak niak niak ....
		return 0;
}
template<>
	const char *fromString<const char*>(const std::string& str)
{
      return str.c_str();
}

char * string2char(const std::string& str) {
	char * ret;
	ret = new char[str.length() + 1];
	strcpy(ret, str.c_str());
	return ret;
}
std::string convBase(unsigned long v, unsigned long base)
	{
		std::string digits = "0123456789abcdef";
		std::string result;
		if((base < 2) || (base > 16)) {
			result = "Error: base out of range.";
		}
		else {
			do {
				result = digits[v % base] + result;
				v /= base;
			}
			while(v);
		}
		return result;
	}
bool is_readable( const std::string & file )
{
    std::ifstream fichier( file.c_str() );
    return !fichier.fail();
}

 std::string getEnv(std::string name) {
    char const* tmp = getenv( name.c_str() );
    if ( tmp == NULL ) {
        return "";
    } else {
        std::string res (tmp);
        return res;
    }
}

 std::string latexize(std::string text) {
     std::string nouveau = text;
     size_t position = nouveau.find( "_" ); // find first

     while ( position != std::string::npos )
        {
         nouveau.replace( position, 1, "\\_" );
         position = nouveau.find( "_", position + 2);
        }
     return nouveau;
 }


 std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
     std::stringstream ss(s);
     std::string item;
     while(std::getline(ss, item, delim)) {
         elems.push_back(item);
     }
     return elems;
 }

 std::vector<std::string> split(const std::string &s, char delim) {
     std::vector<std::string> elems;
     return split(s, delim, elems);
 }

 std::vector<std::string> splitSDF3List(const std::string &s) {
     std::vector<std::string> sub,ssub;
     std::vector<std::string> elems;
     sub = split (s, ',');
     for (std::vector<std::string>::iterator it = sub.begin() ; it != sub.end() ; it++) {
         ssub = split (*it, '*');
         if (ssub.size() == 1) {
             elems.push_back((ssub.at(0)));
         } else if (ssub.size() == 2) {
             for (int i = 0 ; i < commons::fromString<int>(ssub.at(0)) ; i++) {elems.push_back((ssub.at(1)));}
         } else {
             VERBOSE_FAILURE();
         }
     }
     return elems;
 }

 int fibo (int index) {
     int fib_n  = 0;
     int fib_np = 1;
     for (int i = 0; i < index ; i++){
          int swap = fib_np;
          fib_n = fib_np;
          fib_np = swap + fib_np;
     }
     return fib_n;
 }

 const std::string runCmd( const std::string com , TIME_UNIT  max_time ) {

	 std::string sendme = com;
	 if (max_time > 0)
		 sendme = "timeout " + commons::toString<unsigned int>((unsigned int)max_time) + " " + sendme + " 2>/dev/null";

	 VERBOSE_INFO("launch command : " << sendme );

	 FILE * fp = popen (sendme.c_str(),"r");
	 char buffer[1024];
	 std::string res;
	 size_t readcount = 0;
	 do {
		 readcount = fread(buffer,sizeof(char),sizeof(buffer),fp);
		 res += std::string(buffer).substr (0,readcount);
	 } while (readcount) ;

	 pclose(fp);

	 return res;

 }



}
