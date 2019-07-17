#include "Time.hpp"

Time::Time() :
        pause_(false),
        elapsedTimeInPause_(0),
		startProgram_(std::chrono::high_resolution_clock::now()),
		timeStep_(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(16))),
		lag_(std::chrono::nanoseconds(0)){
}

void Time::update() {
	std::chrono::high_resolution_clock::duration deltaTime = std::chrono::high_resolution_clock::now() - startFrame_;
	startFrame_ = std::chrono::high_resolution_clock::now();
	lag_ += std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime);

}
bool Time::shouldUpdateLogic() {
	if (lag_ >= timeStep_) {
		lag_ -= timeStep_;
		return true;
	}
	return false;
}

float Time::getTimeStep() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(timeStep_).count() / 1000.f;
}

std::chrono::time_point<std::chrono::high_resolution_clock> Time::getStartProgramTimePoint() {
	return startProgram_;
}

float Time::getFloatStartProgramTimePoint() {
	std::chrono::high_resolution_clock::duration delta =  startFrame_ - startProgram_;
	return std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() / 1000.f;
}

float Time::getFloatSinceTimePoint(std::chrono::time_point<std::chrono::high_resolution_clock> &point) {
    std::chrono::high_resolution_clock::duration delta =  startFrame_ - point;
    return (std::chrono::duration_cast<std::chrono::milliseconds>(delta) - elapsedTimeInPause_).count() / 1000.f;
}

Time &Time::Get() {
	if (!instance_) {
		instance_ = std::make_unique<Time>();
	}
	return *Time::instance_;
}

std::unique_ptr<Time> Time::instance_ = nullptr;