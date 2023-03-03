#include "Chrono.h"

#include <chrono>
#include <iostream>

Chrono::Chrono()
{
}

Chrono::~Chrono()
{
}

void Chrono::Start()
{
  m_startTime = std::chrono::system_clock::now();
  started = true;
}

void Chrono::Stop()
{
  if(started)
  {
    m_stopTime = std::chrono::system_clock::now();
    finished = true;
  }
}

void Chrono::Reset()
{
  started = false;
  finished = false;
  // m_startTime = std::chrono::system_clock::time_point();
  // m_stopTime = std::chrono::system_clock::time_point();
}

double Chrono::GetElapsedTime()
{
  if(!finished) return -1;
  std::chrono::duration<double> elapsed = m_stopTime - m_startTime;
  return elapsed.count();
}

void Chrono::PrintElapsedTime()
{
  std::chrono::duration<double> elapsed = m_stopTime - m_startTime;
  std::cout << elapsed.count() << " seconds.." << std::endl;
}