// -*- coding:utf-8; mode:c++; mode:auto-fill; fill-column:80; -*-

/// @file      memory_mapping.hpp
/// @brief     Platform-independent memory mapping.
/// @author    J. Arrieta <juan.arrieta@nablazerolabs.com>
/// @date      January, 08, 2019
/// @copyright (C) 2019 Nabla Zero Labs

#pragma once

// C++ Standard Library
#include <cstdint>
#include <string>

namespace nzl {

class MemoryMapping {
 public:
  /// @brief Create a read-only MemoryMaping of file.
  /// @param path Filesystem path to the file.
  /// @throws std::system_error (Linux/macOS) if mapping cannot be created.
  /// @throws std::runtime_error (Windows) if mapping cannot be created.
  MemoryMapping(const std::string& path);

  /// @brief Move constructor.
  MemoryMapping(MemoryMapping&& other) noexcept;

  /// @brief Move assignment.
  MemoryMapping& operator=(MemoryMapping&& other) noexcept;

  /// @brief Destructor.
  ~MemoryMapping() noexcept;

  /// @brief Return a pointer to the beginning of the data block.
  const std::byte* data() const noexcept;

  /// @brief Return a pointer to the beginning of the data block.
  const std::byte* begin() const noexcept;

  /// @brief Return a pointer to the end of the data block.
  const std::byte* end() const noexcept;

  /// @brief Return a pointer to the beginning
  const std::byte* cbegin() const noexcept;

  /// @brief Return a pointer to the end of the data block.
  const std::byte* cend() const noexcept;

  /// @brief Return the size of the mapped region in bytes.
  std::size_t size() const noexcept;

  /// @brief Swap this and other MemoryMapping
  /// @param other The MemoryMapping to swap with this one.
  void swap(MemoryMapping& other) noexcept;

 private:
  std::byte*  m_data{nullptr};
  std::size_t m_size{0};
};

/// @brief Swap two @link MemoryMapping MemoryMappings@endlink
/// @param lhs First MemoryMapping to swap.
/// @param rhs Second MemoryMapping to swap.
void swap(MemoryMapping& lhs, MemoryMapping& rhs) noexcept;

}  // namespace nzl
