// -*- coding:utf-8; mode:c++; mode:auto-fill; fill-column:80; -*-

/// @file      memory_mapping.cpp
/// @brief     Platform-independent memory mapping.
/// @author    J. Arrieta <juan.arrieta@nablazerolabs.com>
/// @date      January, 08, 2019
/// @copyright (C) 2019 Nabla Zero Labs

// Related header
#include "memory_mapping.hpp"

// C++ Standard Library
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <utility>

// Platform-specific libraries.
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(_WIN64)
#include <Windows.h>
#else
#error "Your platform is not supported"
#endif

namespace nzl {
namespace {

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
auto make_mapping(const char* path) {
  auto fp = ::open(path, O_RDONLY);
  if (fp == -1) {
    throw std::system_error(errno, std::system_category(), path);
  }

  struct stat sb;
  if (::fstat(fp, &sb) == -1) {
    ::close(fp);
    throw std::system_error(errno, std::system_category(), path);
  }

  auto data = ::mmap(nullptr, sb.st_size, PROT_READ, MAP_SHARED, fp, 0);
  ::close(fp);  // closing file descriptor does not unmap region

  if (data == MAP_FAILED) {
    throw std::system_error(errno, std::system_category(), path);
  }

  return std::make_pair(static_cast<std::byte*>(data),
                        static_cast<std::size_t>(sb.st_size));
}

auto clear_mapping(std::byte* data, std::size_t size) noexcept {
  if (data != nullptr) {
    ::munmap(static_cast<void*>(data), size);
    data = nullptr;
    size = 0;
  }
  return std::make_pair(data, size);
}

#elif defined(_WIN64)
// Get the last Windows error as a C++ std::string.
std::string last_error() {
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0) {
    return {};
  } else {
    LPSTR  messageBuffer = nullptr;
    size_t size          = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, nullptr);
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
  }
}

auto make_mapping(const char* path) {
  HANDLE hfile = ::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, 0, nullptr);
  if (hfile == INVALID_HANDLE_VALUE) {
    std::ostringstream oss;
    oss << "error opening \"" << path << "\": " << last_error();
    throw std::runtime_error(oss.str());
  }

  LARGE_INTEGER size_value;
  if (!::GetFileSizeEx(hfile, &size_value)) {
    std::ostringstream oss;
    oss << "cannot obtain size of \"" << path << "\": " << last_error();
    throw std::runtime_error(oss.str());
  }
  auto size = size_value.QuadPart;

  HANDLE hmap =
      ::CreateFileMapping(hfile, nullptr, PAGE_READONLY, 0, 0, nullptr);
  // closing the file handle does not affect the mapping
  if (!::CloseHandle(hfile) || (hmap == nullptr)) {
    std::ostringstream oss;
    oss << "error creating file mapping of \"" << path
        << "\": " << last_error();
    throw std::runtime_error(oss.str());
  }

  auto data = ::MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, size);
  // closing the map handle does not unmap the region
  if (!::CloseHandle(hmap) || (data == nullptr)) {
    std::ostringstream oss;
    oss << "error finalizing creation of file mapping of \"" << path
        << "\": " << last_error();
    throw std::runtime_error(oss.str());
  }

  return std::make_pair(static_cast<std::byte*>(data),
                        static_cast<std::size_t>(size));
}

auto clear_mapping(std::byte* data, std::size_t size) noexcept {
  if (data != nullptr) {
    ::UnmapViewOfFile(data);
    data = nullptr;
    size = 0;
  }
  return std::make_pair(data, size);
}
#else
#error "Your platform is not supported"
#endif

}  // anonymous namespace

MemoryMapping::MemoryMapping(const std::string& path) {
  std::tie(m_data, m_size) = make_mapping(path.c_str());
}

MemoryMapping::MemoryMapping(MemoryMapping&& other) noexcept
    : m_data{other.m_data}, m_size{other.m_size} {
  other.m_data = nullptr;
  other.m_size = 0;
}

MemoryMapping& MemoryMapping::operator=(MemoryMapping&& other) noexcept {
  m_data = other.m_data;
  m_size = other.m_size;

  other.m_data = nullptr;
  other.m_size = 0;

  return *this;
}

MemoryMapping::~MemoryMapping() noexcept {
  std::tie(m_data, m_size) = clear_mapping(m_data, m_size);
}

const std::byte* MemoryMapping::data() const noexcept { return m_data; }

const std::byte* MemoryMapping::begin() const noexcept { return cbegin(); }

const std::byte* MemoryMapping::end() const noexcept { return cend(); }

const std::byte* MemoryMapping::cbegin() const noexcept { return m_data; }

const std::byte* MemoryMapping::cend() const noexcept {
  return std::next(cbegin(), size());
}

std::size_t MemoryMapping::size() const noexcept { return m_size; }

void MemoryMapping::swap(MemoryMapping& other) noexcept {
  using std::swap;
  swap(m_data, other.m_data);
  swap(m_size, other.m_size);
}

void swap(MemoryMapping& lhs, MemoryMapping& rhs) noexcept { lhs.swap(rhs); }

}  // namespace nzl
