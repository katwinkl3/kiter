/*
 * commons.h
 *
 *  Created on: 1 févr. 2011
 *      Author: toky
 *
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <stack>
#include <iostream>
#include <cmath>
#include <string>
#include <cstring>
#include <limits>
#include <map>
#include <vector>
#include <cstdlib>
#include <boost/functional/hash.hpp>
#include <boost/integer/common_factor_ct.hpp>
//#define CHECK_BOOST_BIMAP (BOOST_VERSION >= 103800)
#define CHECK_BOOST_BIMAP false
// REALLY STRANGE BUGFIX : For Andrea config (old boost version in fc10)
#include <boost/graph/detail/edge.hpp>
#include <boost/rational.hpp>

#include <commons/basic_types.h>

namespace boost {struct bidirectional_tag;}
namespace std {
bool operator<(const boost::detail::edge_desc_impl<boost::bidirectional_tag, unsigned int>& lh, const boost::detail::edge_desc_impl<boost::bidirectional_tag, unsigned int>& rh);
}


typedef         boost::rational<EXEC_COUNT> EXEC_COUNT_FRACT   ;
typedef    boost::rational<TOKEN_UNIT>      TOKEN_FRACT        ;
typedef    boost::rational<DATA_UNIT>       DATA_FRACT         ;





namespace commons
{



 const std::string getFilename(const std::string s);



// en cours de reflexion ...
template<class Type>
	void deleteByIterating(Type IteratingContain) {
	for (Type it = IteratingContain->begin(); it != IteratingContain->end() ; it++) {delete (*it).second;};
}

// from http://stackoverflow.com/questions/4442658/c-parse-int-from-string
template<class T>
    T fromString(const std::string& s)
{
     std::istringstream stream (s);
     T t;
     stream >> t;
     return t;
}

template<>
char *fromString<char*>(const std::string& str);

template<>
const char *fromString<const char*>(const std::string& str);



template <class T>
bool findInVector( std::vector<T> vect,  T v) {
    for (typename std::vector<T>::iterator it = vect.begin() ; it != vect.end() ; it++) {
        if (*it == v) return true;
    }
    return false;
}

template <class T>
bool findInVector( std::vector<T> * vect,  T v) {
    for (typename std::vector<T>::iterator it = vect->begin() ; it != vect->end() ; it++) {
        if (*it == v) return true;
    }
    return false;
}

template<class T>
    std::string toString(const T& t)
{
     std::ostringstream stream;
     stream << t;
     return stream.str();
}

template<class T>
    std::string toString(const std::vector<T>& t)
{
	 std::stringstream s;
	 for (auto myt : t) {
		 s << commons::toString(myt) + " ";
	 }
	 return s.str();
}

template<>
std::string toString< std::vector < std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> > >(const std::vector < std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> >& v);

template<>
std::string toString< std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> >(const std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX>& v);


template<>
std::string toString< std::pair<TIME_UNIT , ARRAY_INDEX> >(const std::pair<TIME_UNIT , ARRAY_INDEX>& v);

template<>
std::string toString< std::vector<int> >(const std::vector<int>& v);
template<>
std::string toString< std::vector<ARRAY_INDEX> >(const std::vector<ARRAY_INDEX>& v);

template<>
std::string toString< std::vector<TOKEN_UNIT> >(const std::vector<TOKEN_UNIT>& v);

template<>
std::string toString< std::vector<long double> >(const std::vector<long double>& v);

template<>
std::string toString< std::vector<unsigned int> >(const std::vector<unsigned int>& v);

// from http://stackoverflow.com/questions/1430757/c-array-to-string
template <class Str, class It>
Str join(It begin, const It end, const Str &sep)
{
  typedef typename Str::value_type     char_type;
  typedef typename Str::traits_type    traits_type;
  typedef typename Str::allocator_type allocator_type;
  typedef std::basic_ostringstream<char_type,traits_type,allocator_type>
                                       ostringstream_type;
  ostringstream_type result;

  if(begin!=end)
    result << *begin++;
  while(begin!=end) {
    result << sep;
    result << *begin++;
  }
  return result.str();
}



inline bool isInteger(TIME_UNIT v){
    TIME_UNIT intpart;
    TIME_UNIT remain = std::modf((TIME_UNIT)  v, (TIME_UNIT*) &intpart);

    return ( (TIME_UNIT) remain >= (TIME_UNIT) 0.0) && ((TIME_UNIT)  remain <= (TIME_UNIT)  0.0);
}



std::string convBase    (   unsigned long       v,
                            long                base);

bool        is_readable (   const std::string&  file );

char *      string2char (   const std::string&  str);

// http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
bool        AreSame     (   double a,
                            double b);
template <typename T>
inline bool AreSame(T a, T b) {
    return std::fabs((a - b)) < std::numeric_limits<T>::epsilon();
}

template<typename A,typename B>
 inline long double division(A a,B b) {return  (long double)a /(long double)b;}


 std::string getEnv(std::string name) ;

std::string latexize(std::string) ;


template <typename entier,typename flottant>
   inline entier ceil  ( flottant v , entier c){ return (entier) std::ceil ((flottant) v / (flottant) c) * c; }
template <typename entier,typename flottant>
   inline entier floor ( flottant v , entier c){ return (entier) std::floor((flottant) v / (flottant) c) * c; }
template <typename entier>
   inline entier ceil  ( entier v , entier c){ return (entier) std::ceil ((TIME_UNIT) v / (TIME_UNIT) c) * c; }
template <typename entier>
   inline const entier floor ( entier v , entier c){ return (entier) std::floor((TIME_UNIT) v / (TIME_UNIT) c) * c; }
template <typename entier>
   inline entier flooru  ( entier v , entier c){ return (entier) std::floor(commons::division(v,c)) * c; }
template <typename entier>
   inline entier ceilu  ( entier v , entier c){ return (entier) std::ceil(commons::division(v,c)) * c; }
template <typename entier>
   inline entier ceild  ( entier v , entier c){ if  ((v % c) == 0) {return v +c ;} else {return ceilu(v,c);};}

template <typename entier>
   inline entier floord  ( entier v , entier c){ if  ((v % c) == 0) {return v -c ;} else {return flooru(v,c);};}

inline std::string ConvertRGBtoHex(int num) {
         static std::string hexDigits = "0123456789ABCDEF";
         std::string rgb;
         for (int i=(3*2) - 1; i>=0; i--) {
             rgb += hexDigits[((num >> i*4) & 0xF)];
         }
         return rgb;
     }

inline std::string ConvertRGBtoHex(int r, int g, int b) {
      int rgbNum = ((r & 0xff) << 16)
          | ((g & 0xff) << 8)
          | (b & 0xff);

       return ConvertRGBtoHex(rgbNum);
  }
/**
 * Stupid way to do it, but easy to find ...*
 * other way welcome !
 */
TIME_UNIT roundIt(TIME_UNIT val,TIME_UNIT p);
//
std::vector<std::string> &split(const std::string &s, const char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, const char delim) ;
std::vector<std::string> splitSDF3List(const std::string &s);

int fibo (int index);
 
const std::string runCmd( const std::string sendme , TIME_UNIT timeout) ;



unsigned int getRandomSeed(void);

template <typename entier>
inline entier modulo(entier a, entier b) {
  const entier result = a % b;
  return result >= 0 ? result : result + b;
}



}// end of commons namespace

#endif /* COMMONS_H_ */
