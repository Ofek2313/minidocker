#pragma once
#include <cstdint>
#include <string>
#include <sys/types.h>
namespace minidocker {

struct RunOptions {
  bool detached;
  std::string name;
  std::string image;
  std::string memory;
  float cores;
  int64_t duration;
};
struct BuildOptions {
  std::string name;
};
} // namespace minidocker
