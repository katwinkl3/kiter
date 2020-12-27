/*
 * Dataflow.cpp
 *
 *  Created on: Sep 24, 2019
 *      Author: toky
 */

#include <models/Dataflow.h>

template<>
std::string commons::toString<std::map<Vertex,EXEC_COUNT>  >(const  std::map<Vertex,EXEC_COUNT> & v)
{
    std::ostringstream stream;
    for (auto item : v) {
    	stream << item.first << ":" << item.second << " ";
    }
    return stream.str();
}

