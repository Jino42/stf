#pragma once
#include <chrono>
#include <memory>

class Time {
public:
	Time();

	void update();
	bool shouldUpdateLogic();
	float getTimeStep();
	std::chrono::time_point<std::chrono::high_resolution_clock> getStartProgramTimePoint();
	float getFloatStartProgramTimePoint();
    float getFloatSinceTimePoint(std::chrono::time_point<std::chrono::high_resolution_clock> &point);
	static Time &Get();

	void pause(bool b) {
	    if (b) {
            startPause_ = std::chrono::high_resolution_clock::now();
	    }
	    else {
            elapsedTimeInPause_ += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startPause_);
	    }
	    pause_ = b;
	}

	bool isPause() const {
	    return pause_;
	}

private:
    bool pause_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startPause_;
    std::chrono::milliseconds elapsedTimeInPause_;

	std::chrono::time_point<std::chrono::high_resolution_clock> startFrame_;
	std::chrono::time_point<std::chrono::high_resolution_clock> startProgram_;

	std::chrono::nanoseconds timeStep_;
	std::chrono::nanoseconds lag_;

	static std::unique_ptr<Time> instance_;
};
