
#pragma once
#include "tools/PipeHandler.h"
#include <cerrno>
#include <filesystem>
#include <semaphore>
#include <string>
#include <system_error>
#include <variant>
#include <vector>

namespace minidocker {

template <typename T> void checkErr(T ret, const std::string &msg) {
  if (ret == -1)
    throw std::system_error(errno, std::generic_category(), msg);
}

using FilePath = std::filesystem::path;
using CpuDuration = int64_t;
using CpuQuota = int64_t;
using CpuCores = float;
using Memory = std::string;

enum class BaseImage { None, Alpine, Ubuntu, Arch, Fedora };

struct CgroupConfig {
  minidocker::CpuDuration period;
  minidocker::CpuCores cores;
  minidocker::Memory memoryLimit;
};

struct ContainerConfig {

  CgroupConfig cgroupConfig;
  std::string imageName;
  bool attachFlag;
};

struct ChildArgs {

  PipeHandler &pipeHandler;
  PipeHandler &syncHandler;
  ContainerConfig &containerConfig;
  std::vector<std::string> commands;
  size_t containerId;
};

}; // namespace minidocker
