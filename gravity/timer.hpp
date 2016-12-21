
#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>

class Timer {
  using clock = std::chrono::high_resolution_clock;

public:
  Timer() : start(clock::now()) {}
  ~Timer() {
    auto d = clock::now() - start;
    std::cout << "Elapsed time: " << std::setw(16) << d.count() << "ns" << std::endl;
  }

private:
  std::chrono::time_point<clock> start;
};
