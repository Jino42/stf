/// [Timer example]
Timer timer1;
Timer timer2(false);

// ...

std::cout << "Time elapsed in milliseconds : " << test.count() << std::endl;
std::cout << "Time elapsed in seconds : " << test.count() / 1000.f << std::endl;
std::cout << "Time elapsed in seconds (rounded): " << test.count<std::chrono::seconds>() << std::endl;


timer2.setSpeed(2.0f);
timer2.start();

// ...

float gapBetweenTimerInMicro = (timer1.getDuration<std::chrono::microseconds>() - timer2.getDuration<std::chrono::microseconds>()).count();

std::cout << "Gap Between Timer1 and Timer2" << gapBetweenTimerInMicro << std::endl;


/// [Timer example]