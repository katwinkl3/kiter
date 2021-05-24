/*
 * TransformationsRegistry.h
 *
 *  Created on: Apr 23, 2020
 *      Author: toky
 */

#ifndef SRC_KITER_TASKS_KiterRegistry_H_
#define SRC_KITER_TASKS_KiterRegistry_H_

#include <string>
#include <map>
#include <iostream>

typedef std::map<std::string,std::string> parameters_list_t;

namespace models {
class Dataflow;
}


struct generator_t {
	std::string name;
	std::string desc;
	models::Dataflow* (*fun)(parameters_list_t);
};

struct transformation_t {
	std::string name;
	std::string desc;
	void (*fun)(models::Dataflow*, parameters_list_t);
};



#define ADD_TRANSFORMATION(name,t) static auto name##unique = KiterRegistry<transformation_t>::add(t)
#define ADD_GENERATOR(name,t) static auto name##unique = KiterRegistry<generator_t>::add(t)

template <typename T>
class KiterRegistry {

  public:
    typedef std::map<std::string, T> transformations_map;

    static bool add(const T& t) {
    	transformations_map& map = getSingletonMap();
    	if (map.find(t.name) != map.end()) return false;
    	map[t.name] = t;
    	return true;
    }

    static const T* get(const std::string& name) {
    	transformations_map& map = getSingletonMap();
    	if (map.find(name) == map.end()) return nullptr;
    	return &(map[name]);
    }

    static void print () { //TODO; don't like this, should not access stderr like that
    	transformations_map& map = getSingletonMap();
    	for (auto i : map) {
				std::cerr << " - " << i.second.name << " : " << i.second.desc << std::endl;
    	}
    }


  private:

    static transformations_map& getSingletonMap() {
      static transformations_map map;
      return map;
    }
};


#endif /* SRC_KITER_TASKS_KiterRegistry_H_ */
