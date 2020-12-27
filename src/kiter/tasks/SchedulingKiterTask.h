/*
 * SchedulingKiterTask.h
 *
 *  Created on: 12 Oct 2019
 *      Author: toky
 */

#ifndef SchedulingKiterTask_H_
#define SchedulingKiterTask_H_

#include <models/Dataflow.h>
#include <models/KiterTask.h>

typedef std::map<ARRAY_INDEX,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>> scheduling_t;

namespace models {
	class SchedulingKiterTask : public KiterTask {

	private :
		virtual scheduling_t scheduling_function (const models::Dataflow*, parameters_list_t) = 0;

	protected :
		void process (models::Dataflow* dataflow, parameters_list_t params) {
			scheduling_t res = this->scheduling_function (dataflow, params);
		};

		SchedulingKiterTask(std::string name,std::string desc) : KiterTask(name,desc) {};
	};
}




#endif /* SchedulingKiterTask_H_ */
