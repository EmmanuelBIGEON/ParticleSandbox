#pragma once 

#include <chrono>

//! \class Chrono
//! \brief Class to measure time.
class Chrono
{
public:
    Chrono();
    ~Chrono();

    void Start();
    void Stop();
    void Reset();
    double GetElapsedTime();
    void PrintElapsedTime();

  private:
    std::chrono::system_clock::time_point m_startTime;
    std::chrono::system_clock::time_point m_stopTime;

    bool started;
    bool finished;
};