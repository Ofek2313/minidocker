
#pragma once
#include "tools/PipeHandler.h"
#include <filesystem>
#include <semaphore>
#include <string>
#include <variant>
#include <vector>

namespace minidocker {

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
  std::string containerName;
  FilePath workingDirectory;
  FilePath containerHostDirectory;
  bool attachFlag;
  bool copyBinary;
};

struct ChildArgs {
  PipeHandler &pipeHandler;
  PipeHandler &syncHandler;
  ContainerConfig &containerConfig;
  std::vector<std::string> commands;
};

}; // namespace minidocker
