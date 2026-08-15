#include "CgroupManager.h"
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

CgroupManager::CgroupManager(const std::string &groupName) {

  CgroupPath = "/sys/fs/cgroup/" + groupName;
}

void CgroupManager::CreateCgroup() {

  if (!std::filesystem::is_directory(CgroupPath)) {
    if (!std::filesystem::create_directory(CgroupPath)) {
      throw std::system_error(errno, std::generic_category(),
                              "Failed to create Cgroup");
    }
  }
}

void CgroupManager::LimitMemoryUsage(std::string_view memory) {
  filePath memoryPath = CgroupPath / ("memory.max");
  std::ofstream outFile(memoryPath, std::ios::binary);
  if (!outFile.is_open()) {
    throw std::runtime_error("Unable to open memory file");
  }
  outFile << memory << '\n';

  if (!outFile) {
  }
  // filePath memorySwap = CgroupPath / ("memory.swap.max");
  // std::ofstream outFile2(memorySwap, std::ios::binary);
  // outFile2 << 0 << '\n';
}
void CgroupManager::AddProc(pid_t processId) {
  filePath procPath = CgroupPath / ("cgroup.procs");
  std::ofstream outFile(procPath, std::ios::binary);

  outFile << processId << '\n';
}
void CgroupManager::LimitCpuBanwidth(double cores, int period = 100000) {
  filePath cpuPath = CgroupPath / ("cpu.max");
  int quota = cores * period;

  std::string cpuwidth = std::to_string(quota) + " " + std::to_string(period);
  std::cout << cpuwidth << std::endl;
  std::ofstream outFile(cpuPath, std::ios::trunc);

  outFile << cpuwidth << '\n';
}

CgroupManager::~CgroupManager() {

  filePath procPath = CgroupPath / ("cgroup.procs");
  std::ofstream outFile(procPath, std::ios::trunc);
}
