
#pragma once
#include "PipeHandler.h"
#include <filesystem>
#include <string>
#include <vector>

namespace minidocker {

using FilePath = std::filesystem::path;
using CpuDuration = int64_t;
using CpuQuota = int64_t;
using CpuCores = float;
using Memory = std::string;

struct CgroupConfig {
  minidocker::CpuDuration period;
  minidocker::CpuCores cores;
  minidocker::Memory memoryLimit;
};

struct ContainerConfig {

  CgroupConfig cgroupConfig;
  std::string containerName;
  FilePath workingDirectory;
  bool attachFlag;
};

struct ChildArgs {
  PipeHandler &pipeHandler;
  ContainerConfig &containerConfig;
  std::vector<std::string> commands;
};

}; // namespace minidocker
