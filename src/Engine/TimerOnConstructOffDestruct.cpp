//
// Created by ntoniolo on 25/11/2019.
//

#include "TimerOnConstructOffDestruct.hpp"

TimerOnConstructOffDestruct::TimerOnConstructOffDestruct(std::string const &name) :
name_(name) {
	std::cout << name_ << " [Start]" << std::endl;
	timer_.start();
}

TimerOnConstructOffDestruct::~TimerOnConstructOffDestruct() {
	timer_.stop();
	std::cout << name_ << " [Finish] : time : " << timer_.count() << "ms" << std::endl;
}
