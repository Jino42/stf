#include "Time.hpp"
#include <iostream>
#include <iomanip>

Timer::Timer(bool canStart) :
        started_(false),
        paused_(false),
        speed_(1.0f),
        timePointTimerCreated_(std::chrono::steady_clock::now()),
        reference_(std::chrono::steady_clock::now()),
        accumulated_(std::chrono::duration<long double>(0)) {
    if (canStart)
        start();
    Timer::addTimer_(this);
}

Timer::~Timer() {
    Timer::deleteTimer_(this);
}

void Timer::start() {
    if (!started_) {
        started_ = true;
        paused_ = false;
        accumulated_ = std::chrono::duration<long double>(0);
        reference_ = std::chrono::steady_clock::now();
    } else if (paused_) {
        reference_ = std::chrono::steady_clock::now();
        paused_ = false;
    }
}

void Timer::stop() {
    if (started_ && !paused_) {
        if (!Time::Get().isPause()) {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            accumulated_ = accumulated_ + std::chrono::duration_cast< std::chrono::duration<long double> >(now - reference_) * speed_;
        }
        paused_ = true;
    }
}

void Timer::reset() {
    if (started_) {
        started_ = false;
        paused_ = false;
        speed_ = 1.0f;
        reference_ = std::chrono::steady_clock::now();
        accumulated_ = std::chrono::duration<long double>(0);
    }
}

void Timer::setSpeed(float speed) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    accumulated_ = accumulated_ + std::chrono::duration_cast< std::chrono::duration<long double> >(now - reference_) * speed_;
    reference_ = std::chrono::steady_clock::now();
    speed_ = speed;
}

float Timer::getSpeed() const {
    return speed_;
}

void Timer::applyWorldStart() {
    if (!started_)
        return ;
    if (!Time::Get().isPause()) {
        if (!paused_)
            reference_ = std::chrono::steady_clock::now();
    }
}
void Timer::applyWorldStop() {
    if (!started_ || paused_)
        return ;
    if (Time::Get().isPause()) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        accumulated_ = accumulated_ + std::chrono::duration_cast< std::chrono::duration<long double> >(now - reference_) * speed_;
    }
}

void Timer::worldStart() {
    for (auto *timer : Timer::timers_)
        timer->applyWorldStart();
}
void Timer::worldStop() {
    for (auto *timer : Timer::timers_)
        timer->applyWorldStop();
}

void Timer::addTimer_(Timer *timer) {
    Timer::timers_.push_back(timer);
}

void Timer::deleteTimer_(Timer *timer) {
    auto position = std::find(Timer::timers_.begin(), Timer::timers_.end(), timer);
    if (position == Timer::timers_.end())
        throw std::runtime_error("Timer not found in timers_ vector.");
    Timer::timers_.erase(position);
}

std::vector< Timer *> Timer::timers_;

///
///
///

Time::Time() :
        pause_(false),
        elapsedTimeInPause_(0),
		timeStep_(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(16))),
		lag_(std::chrono::nanoseconds(0)){
}

void Time::update() {
    std::chrono::high_resolution_clock::duration deltaTime = std::chrono::high_resolution_clock::now() - sinceWorldStartFrame.getReference();
//  std::chrono::nanoseconds deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()) - sinceWorldStartFrame_.getDuration<std::chrono::nanoseconds>();
    sinceWorldStartFrame.reset();
    sinceWorldStartFrame.start();
    lag_ += std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime);

}
bool Time::shouldUpdateLogic() {
	if (lag_ >= timeStep_) {
		lag_ -= timeStep_;
		return true;
	}
	return false;
}

float Time::getFloatStartProgramTimePoint() {
	return (sinceWorldStartFrame.count() - sinceWorldStartProgram.count()) / 1000.f;
}

float Time::getFloatSinceTimePoint(std::chrono::time_point<std::chrono::steady_clock> &point) {
    std::chrono::steady_clock::duration delta =  sinceWorldStartFrame.getReference() - point;
    return (std::chrono::duration_cast<std::chrono::milliseconds>(delta) - elapsedTimeInPause_).count() / 1000.f;

}

std::chrono::steady_clock::time_point Timer::getReference()
{
    return reference_;
}

Time &Time::Get() {
	if (!Time::instance_) {
        Time::instance_ = std::make_unique<Time>();
	}
	return *Time::instance_;
}

std::unique_ptr<Time> Time::instance_ = nullptr;

bool  Time::debug_ = false;
bool  Timer::debug_ = false;

time_t steady_clock_to_time_t(std::chrono::steady_clock::time_point const &t)
{
    return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
            + std::chrono::duration_cast<std::chrono::system_clock::duration>(t - std::chrono::steady_clock::now()));
}

std::ostream &operator<<(std::ostream &os, Timer const &timer) {
    os << std::boolalpha;
    time_t timeCratedTimer = steady_clock_to_time_t(timer.timePointTimerCreated_);
    time_t timeReferenceTimer = steady_clock_to_time_t(timer.reference_);

    std::string strTimeCreatedTimer(std::ctime(&timeCratedTimer));
    strTimeCreatedTimer.erase(std::remove(strTimeCreatedTimer.begin(), strTimeCreatedTimer.end(), '\n'), strTimeCreatedTimer.end());

    std::string strTimeReferenceTimer(std::ctime(&timeReferenceTimer));
    strTimeReferenceTimer.erase(std::remove(strTimeReferenceTimer.begin(), strTimeReferenceTimer.end(), '\n'), strTimeReferenceTimer.end());

    os << "Timer {";
    os << "World pause [" << Time::Get().isPause() << "]," << std::endl;
    os << "Timer created at : [" << strTimeCreatedTimer << "]," << std::endl;
    os << "Started_[" << timer.started_ << "]," << std::endl;
    os << "paused_ [" << timer.paused_ << "], " << std::endl;
    os << "reference_ [" << strTimeReferenceTimer << "]," << std::endl;
    os << "accumulated_ [ms:" << (std::chrono::duration_cast<std::chrono::milliseconds>(timer.accumulated_).count() / 1000.f) << "] }" << std::endl;
    os << "Timer actual ms : [" << timer.count() << "]" << std::endl;
    os << "Speed : [" << timer.speed_ << "]" << std::endl;
    os << "}" << std::endl;
    return os;
}