#include "Debug.hpp"
#include <iostream>

void Debug::setDebug(std::string const &name, bool &ref) {
	if (mapBool_.find(name) == mapBool_.end())
		mapBool_.insert(std::pair<std::string, bool &>(name, ref));
}

void Debug::debug(std::string const &name, bool b) {
	std::map<std::string, bool &>::iterator itr;

	itr = mapBool_.find(name);
	if (itr != mapBool_.end()) {
		itr->second = b;
	}
}

void Debug::flipDebug(std::string const &name) {
	std::map<std::string, bool &>::iterator itr;

	itr = mapBool_.find(name);
	if (itr != mapBool_.end()) {
		itr->second = !itr->second;
	}
}

std::unique_ptr<Debug> Debug::instance_ = nullptr;