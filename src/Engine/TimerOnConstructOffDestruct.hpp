#pragma once

#include "Time.hpp"

class TimerOnConstructOffDestruct {

public:
	TimerOnConstructOffDestruct() = delete;
	explicit TimerOnConstructOffDestruct(std::string const &);
	~TimerOnConstructOffDestruct();
	TimerOnConstructOffDestruct(TimerOnConstructOffDestruct &camera) = delete;

private:
	std::string name_;
	Timer timer_;
};

