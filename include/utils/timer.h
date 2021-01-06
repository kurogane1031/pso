#pragma once
#include <chrono>
#include <fmt/core.h>

class Timer{
public:
  Timer() = default;

  void Start()
  {
    m_StartTimepoint = std::chrono::high_resolution_clock::now();
  }
  void Stop()
  {
    m_EndTimepoint = std::chrono::high_resolution_clock::now();
    computeTime();
  }

  void computeTime()
  {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(m_EndTimepoint - m_StartTimepoint).count();
    auto ms = duration * 0.001;
    fmt::print("{} ms ({} us)\n", ms, duration);
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTimepoint;
};
