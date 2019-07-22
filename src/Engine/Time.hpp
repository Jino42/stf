#pragma once
///
/// \file       Time.hpp
/// \author     Noé
/// \version    1.0
///

#include <chrono>
#include <vector>

/// \brief Singleton
/// \details Time get somes Function for play with Time.
///             Can have FixedUpdate, for each 16milliseconds (See example)
///             Can make a World Pause

class Timer {

public:

    /**
     * Constructor.
     *
     * @param   start
     *          If true, the timer is started just after construction.
     *          Otherwise, it will not be automatically started.
     */
    explicit Timer(bool canStart = true);

    Timer(Timer const &other) = delete;
    Timer& operator=(Timer const &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Timer();

    /**
     * Start/resume the timer.
     */
    void start();

    /**
     * Stop/pause the timer.
     */
    void stop();

    void applyWorldStart();
    void applyWorldStop();

    /**
     * Reset the timer.
     */
    void reset();

    std::chrono::steady_clock::time_point getReference();

    /**
     * Return the elapsed time.
     *
     * @param   duration_t
     *          The duration type used to return the time elapsed. If not
     *          specified, it returns the time as represented by
     *          std::chrono::milliseconds.
     *
     * @return  The elapsed time.
     */
    template <class duration_t = std::chrono::milliseconds>
    typename duration_t::rep count() const {
        if (started_) {
            if (paused_) {
                return std::chrono::duration_cast<duration_t>(accumulated_).count();
            } else {
                return std::chrono::duration_cast<duration_t>(
                        accumulated_ + (std::chrono::steady_clock::now() - reference_)).count();
            }
        } else {
            return duration_t(0).count();
        }
    }

    template <class duration_t = std::chrono::milliseconds>
    duration_t getDuration() const {
        if (started_) {
            if (paused_) {
                return std::chrono::duration_cast<duration_t>(accumulated_);
            } else {
                return std::chrono::duration_cast<duration_t>(
                        accumulated_ + (std::chrono::steady_clock::now() - reference_));
            }
        } else {
            return duration_t(0);
        }
    }

    static void worldStart();
    static void worldStop();

private:

    bool started_;
    bool paused_;
    std::chrono::steady_clock::time_point reference_;
    std::chrono::duration<long double> accumulated_;
    static std::vector< Timer * > timers_;

    static void addTimer_(Timer *timer);
    static void deleteTimer_(Timer *timer);
};

class Time {
public:
	Time();

	void update();
	bool shouldUpdateLogic();
	float getFloatStartProgramTimePoint();
    float getFloatSinceTimePoint(std::chrono::time_point<std::chrono::steady_clock> &point);
	static Time &Get();
	void pause(bool b) {
        pause_ = b;
	    if (b) {
            startPause_ = std::chrono::steady_clock::now();
            Timer::worldStop();
	    }
	    else {
            elapsedTimeInPause_ += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPause_);
            Timer::worldStart();
        }
	}
	bool isPause() const {
	    return pause_;
	}

private:
    bool pause_;
    std::chrono::time_point<std::chrono::steady_clock> startPause_;
    std::chrono::milliseconds elapsedTimeInPause_;

    Timer sinceWorldStartFrame_;
    Timer sinceWorldStartProgram_;
    std::chrono::time_point<std::chrono::steady_clock> sinceRealStartProgram_;

	std::chrono::nanoseconds timeStep_;
	std::chrono::nanoseconds lag_;

	static std::unique_ptr<Time> instance_;
};

/// \example exampleTime.cpp
/// This is an example of how to use the Time class.