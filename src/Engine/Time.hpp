#pragma once
///
/// \file       Time.hpp
/// \author     Noé
/// \version    1.0
///

#include <chrono>
#include <vector>
#include <iostream>

/// \brief For play with Time
/// \details    Timer is like a stopwatch
/// \section HowToUse
/// \snippet snippetTime.cpp Timer example

class Timer {
    friend class Time;
    friend std::ostream &operator<<(std::ostream &os, Timer const &timer);
public:
    /// \param canStart : If true, the timer is started just after construction. Otherwise, it will not be automatically started.
    explicit Timer(bool canStart = true);
    Timer(Timer const &other) = delete;
    Timer& operator=(Timer const &other) = delete;
    virtual ~Timer();

    /// \brief Start/resume the timer.
    void start();

    /// \brief Stop/pause the timer.
    void stop();

    /// \brief Set speed of Timer
    /// \param speed : Speed to set
    /// \details Can set any speed (Negative/Positive) to timer
    void setSpeed(float speed);

    /// \brief Get speed of Timer
    /// \return Speed of Timer
    float getSpeed() const;


    /// \brief Reset Timer
    /// \details Timer need to be start after a reset
    void reset();


    /// \brief Return the elapsed time.
    /// \details duration_t :
    ///           The duration type used to return the time elapsed. If not
    ///           specified, it returns the time as represented by
    ///           std::chrono::milliseconds.
    /// \return   The elapsed time in rep type . Rep is defined by duration type used (float in case of std::chrono::milliseconds)
    template <class duration_t = std::chrono::milliseconds>
    typename duration_t::rep count() const;

    /// \brief Return the duration of elapsed time
    /// \details duration_t :
    ///           The duration type used to return the time elapsed. If not
    ///           specified, it returns the time as represented by
    ///           std::chrono::milliseconds.
    /// \return   The elapsed time in duration type . std::chrono::milliseconds if not defined
    template <class duration_t = std::chrono::milliseconds>
    duration_t getDuration() const;



private:

    //Friendly
    static void worldStart_();
    static void worldStop_();
    void applyWorldStart_();
    void applyWorldStop_();
    std::chrono::steady_clock::time_point getReference_();

    bool started_;
    bool paused_;
    float speed_;
    std::chrono::steady_clock::time_point timePointTimerCreated_;
    std::chrono::steady_clock::time_point reference_;
    std::chrono::duration<long double> accumulated_;


    static std::vector< Timer * > timers_;

    static void addTimer_(Timer *timer);
    static void deleteTimer_(Timer *timer);

    static bool debug_;


};

/// \brief Singleton
/// \details Is a module who permit to Play with Time.
///             Can have "FixedUpdate", for each 16milliseconds (See example)
///             Can make a World Pause, (Pause all Timer and himself)

class Time {
    friend class Timer;
public:

	Time();

    /// \brief Update of Time module
    /// \details    Time need to be update up to your render loop
    ///             Permit to set DeltaTime, Stack the Lag (Talk to ntoniolo for understand)
	void update();

    /// \brief FixedUpdate
    /// \details    Check if Lag is > then the TimeStep (16milliseconds)
    /// \return bool : Do FixedUpdate if return true
	bool shouldUpdateLogic();
	static Time &Get();

    /// \brief Make WorldPause/WorldUnPause
    /// \details    Make a WorldPause/WorldUnPause and Apply for each Timer exist, a pause
    ///             If a Timer was already in a pause, it will remain after WorldUnPause
    /// \param b : True if you want WorldPause
    ///            False if you want WorldUnPause
	void pause(bool b);

    /// \brief Get Bool of WorldPause
    /// \return Bool of WorldPause
	bool isPause() const;

    /// \brief Timer reset each Frame
    Timer sinceWorldStartFrame;

    /// \brief Timer start during Time Constructor
    Timer sinceWorldStartProgram;

private:
    bool pause_;
    std::chrono::time_point<std::chrono::steady_clock> startPause_;
    std::chrono::milliseconds elapsedTimeInPause_;

    std::chrono::time_point<std::chrono::steady_clock> sinceRealStartProgram_;

	std::chrono::nanoseconds timeStep_;
	std::chrono::nanoseconds lag_;

	static std::unique_ptr<Time> instance_;

    static bool debug_;
};

std::ostream &operator<<(std::ostream &os, Timer const &timer);

template <class duration_t>
duration_t Timer::getDuration() const {
    if (started_) {
        if (paused_ || Time::Get().isPause()) {
            return std::chrono::duration_cast<duration_t>(accumulated_);
        } else {
            if (debug_) {
                std::cout << "Timer::getDuration() : " << std::endl;
                std::cout << "accumulated : " << std::chrono::duration_cast<duration_t>(accumulated_).count() << std::endl;
                std::cout << "now - ref : " << std::chrono::duration_cast<duration_t>((std::chrono::steady_clock::now() - reference_)).count() << std::endl;
            }
            return std::chrono::duration_cast<duration_t>(
                    accumulated_ + (std::chrono::steady_clock::now() - reference_) * speed_);
        }
    } else {
        return duration_t(0);
    }
}

template <class duration_t>
typename duration_t::rep Timer::count() const {
    if (started_) {
        if (paused_ || Time::Get().isPause()) {
            return std::chrono::duration_cast<duration_t>(accumulated_).count();
        } else {
            if (debug_) {
                std::cout << "Timer::getDuration() : " << std::endl;
                std::cout << "accumulated : " << std::chrono::duration_cast<duration_t>(accumulated_).count() << std::endl;
                std::cout << "now - ref : " << std::chrono::duration_cast<duration_t>((std::chrono::steady_clock::now() - reference_)).count() << std::endl;
            }
            return std::chrono::duration_cast<duration_t>(
                    accumulated_ + (std::chrono::steady_clock::now() - reference_) * speed_).count();
        }
    } else {
        return duration_t(0).count();
    }
}

/// \example exampleTime.cpp
/// This is an example of how to use the Time class.