#include "container/CgroupManager.h"
#include "Config.h"
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sched.h>
#include <stdexcept>
#include <string>
#include <system_error>

CgroupManager::CgroupManager(size_t containerId,
                             minidocker::CgroupConfig &config) {

  cgroupPath_ = "/sys/fs/cgroup/minidocker-" + std::to_string(containerId);
  CreateCgroup();
  LimitMemoryUsage(config.memoryLimit);
  LimitCpuBanwidth(config.cores, config.period);
  LimitMemorySwap("0");
}

void CgroupManager::CreateCgroup() {

  if (!std::filesystem::is_directory(cgroupPath_)) {
    if (!std::filesystem::create_directory(cgroupPath_)) {
      throw std::system_error(errno, std::generic_category(),
                              "Failed to create Cgroup");
    }
  }
}

void CgroupManager::LimitMemoryUsage(std::string_view memory) {
  minidocker::FilePath memoryPath = cgroupPath_ / ("memory.max");
  std::ofstream outFile(memoryPath, std::ios::binary);
  if (!outFile.is_open()) {
    throw std::runtime_error("Unable to open memory file");
  }
  outFile << memory << '\n';

  if (!outFile) {
    throw std::runtime_error("File Writting Failed");
  }
  // filePath memorySwap = CgroupPath / ("memory.swap.max");
  // std::ofstream outFile2(memorySwap, std::ios::binary);
  // outFile2 << 0 << '\n';
}
void CgroupManager::LimitMemorySwap(minidocker::Memory swapLimit) {
  minidocker::FilePath memorySwapPath = cgroupPath_ / ("memory.swap.max");
  std::ofstream outFile(memorySwapPath, std::ios::binary);

  if (!outFile.is_open())
    throw std::runtime_error("Unable to open memory swap file");

  outFile << swapLimit << '\n';
  if (!outFile)
    throw std::runtime_error("File Writting Failed");
}

void CgroupManager::AddProc(pid_t processId) {
  minidocker::FilePath procPath = cgroupPath_ / ("cgroup.procs");
  std::ofstream outFile(procPath, std::ios::binary);

  outFile << processId << '\n';
}
void CgroupManager::LimitCpuBanwidth(minidocker::CpuCores cores,
                                     minidocker::CpuQuota period = 100000) {

  minidocker::FilePath cpuPath = cgroupPath_ / ("cpu.max");
  minidocker::CpuQuota quota = cores * period;

  std::string cpuwidth = std::to_string(quota) + " " + std::to_string(period);

  std::ofstream outFile(cpuPath, std::ios::trunc);
  if (!outFile.is_open()) {
    throw std::runtime_error("Unable to open cpu bandwith file");
  }
  outFile << cpuwidth << '\n';

  if (!outFile) {
    throw std::runtime_error("File writting Failed");
  }
}
void CgroupManager::SetupCgroup(minidocker::CgroupConfig &config,
                                pid_t procsId) {

  CreateCgroup();
  LimitMemoryUsage(config.memoryLimit);
  LimitCpuBanwidth(config.cores, config.period);
  AddProc(procsId);
}

CgroupManager::~CgroupManager() noexcept {
  // std::filesystem::remove(CgroupPath);
}
