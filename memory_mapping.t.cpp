// -*- coding:utf-8; mode:c++; mode:auto-fill; fill-column:80; -*-

/// @file      memory_mapping.t.cpp
/// @brief     Test driver for platform-independent memory mapping.
/// @author    J. Arrieta <juan.arrieta@nablazerolabs.com>
/// @date      January, 08, 2019
/// @copyright (C) 2019 Nabla Zero Labs

// Nabla Zero Labs Library.
#include "memory_mapping.hpp"

// C++ Standard Library
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

void using_file(const std::string& path) {
  std::ifstream fp(path, std::ios::in | std::ios::binary);

  std::istreambuf_iterator<char>       it(fp);
  const std::istreambuf_iterator<char> it_end{};

  std::size_t n{0};
  while (it != it_end) {
    if (*it++ == '\n') ++n;
  }
  std::cout << "[f] " << std::setw(8) << n << " " << path << "\n";
}

void using_mapping(const std::string& path) {
  nzl::MemoryMapping mm{path};

  std::size_t n{0};
  for (auto&& c : mm) {
    if (c == std::byte('\n')) ++n;
  }
  std::cout << "[m] " << std::setw(8) << n << " " << path << "\n";
}

int main(int argc, char* argv[]) {
  using namespace std::chrono;

  bool use_map = std::getenv("NZL_USE_MAP") != nullptr;
  if (use_map) {
    std::cout << "using memory mapping\n";
    auto cpu_beg = system_clock::now();
    std::for_each(argv + 1, argv + argc, using_mapping);
    auto cpu_end = system_clock::now();
    auto cpu_dur = nanoseconds(cpu_end - cpu_beg).count();
    std::cout << cpu_dur << " nanoseconds elapsed\n";

  } else {
    std::cout << "using file\n";
    auto cpu_beg = system_clock::now();
    std::for_each(argv + 1, argv + argc, using_file);
    auto cpu_end = system_clock::now();
    auto cpu_dur = nanoseconds(cpu_end - cpu_beg).count();
    std::cout << cpu_dur << " nanoseconds elapsed\n";
  }
}
