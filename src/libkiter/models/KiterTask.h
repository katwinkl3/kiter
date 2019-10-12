/*
 * KiterTask.h
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_MODELS_KiterTask_H_
#define SRC_LIBKITER_MODELS_KiterTask_H_

#include <map>
#include <string>

namespace models {class Dataflow;}

typedef std::map<std::string,std::string> parameters_list_t;

namespace models {
	class KiterTask {

	protected:
		const std::string _name;
		const std::string _desc;
		virtual void process (models::Dataflow*, parameters_list_t) = 0;

	public:
		KiterTask(std::string name,std::string desc) : _name(name), _desc(desc) {};
		virtual ~KiterTask(){};

};
}



#endif /* SRC_LIBKITER_MODELS_KiterTask_H_ */
