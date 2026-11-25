#pragma once
#include <cstdint>
#include <string>
#include <sys/types.h>
#include <vector>
namespace minidocker {

struct RunOptions {
  bool detached = false;
  std::string name;
  std::string image;
  std::string memory = "512M";
  float cores = 1;
  int duration = 10000;
  std::vector<std::string> commands;
};
struct BuildOptions {
  std::string name;
};
} // namespace minidocker
