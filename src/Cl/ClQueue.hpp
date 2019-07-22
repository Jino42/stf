#pragma once

#include <NTL.hpp>
#include "ClContext.hpp"


class ClQueue {
public:
	ClQueue() : queue_(ClContext::Get().context, ClContext::Get().deviceDefault) {}

	cl::Event &getEvent() {
		event_.emplace_back();
		return event_.back();
	}
	cl::CommandQueue &getQueue() {
		return queue_;
	}
	void wait() {
		cl::Event::waitForEvents(event_);
		event_.clear();
	}
	std::vector<cl::Event> &getEventBuffer() {
		return event_;
	}
private:
	cl::CommandQueue queue_;
	std::vector<cl::Event> event_;
};
