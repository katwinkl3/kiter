/*
 * GroupList.h
 *
 *  Created on: Apr 29, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_COMMONS_GROUPLIST_H_
#define SRC_LIBKITER_COMMONS_GROUPLIST_H_

#include <map>
#include <vector>
#include <set>

/**
 *
 * Could not figure out how to do it efficiently so did this terrible mess
 *
 */
template <typename T, typename R>
class GroupList {
private :
	std::map<T , T> _components;
	std::map<T , std::set<R>* > _values;
	std::set< std::set<R>* > _bags;
public:
	GroupList() {VERBOSE_DEBUG("Start empty GroupList");};
	~GroupList() {
		for (auto b : _bags) {
			delete b;
		}
	};
	void add (const T& l, const T& r, const R& v) {
		VERBOSE_DEBUG("GroupList add " << l << "-" << r << " with " << v);
		// We make sure that l has a group
		if (_components.count(l) == 0) {
			std::set<R>* nvec = new std::set<R>({v});
			_bags.insert(nvec);
			_components[l] = l;
			_values[l] = nvec;
		}

		// We make sure that r has a group
		if (_components.count(r) == 0) {
			std::set<R>* nvec = new std::set<R>({v});
			_bags.insert(nvec);
			_components[r] = r;
			_values[r] = nvec;
		}


		if (_values[l] != _values[r]) {
			VERBOSE_ASSERT(_values[l] != _values[r], "MERGE SHOULD MAKE SENSE");

			//We merge them
			std::set<R>* nvec = _values[l];
			for (auto v : *_values[r]) {
				nvec->insert(v);
			}

			_bags.erase(_values[r]);
			delete _values[r];

			// merge
			T oldlc = _components[l];
			T oldrc = _components[r];
			for (auto item : _components) {
				if (item.second == oldlc or item.second == oldrc) {
					_values[item.first] = nvec;
					_components[item.first] = l;
				}
			}



			VERBOSE_ASSERT(_values[l] == _values[r], "MERGE FAILED");

		} else {
			_values[l]->insert(v);
		}
	};

	std::vector<std::set<R>> getall () {
		std::vector<std::set<R>> res;
		for (auto item : _components) {
			VERBOSE_DEBUG("item " << item.first << " inside " << item.second << " with values " << commons::toString( *_values[item.first] ));
			if (item.first == item.second) {
				res.push_back(*_values[item.first]);
			}
		}
		return res;
	}
};



#endif /* SRC_LIBKITER_COMMONS_GROUPLIST_H_ */
