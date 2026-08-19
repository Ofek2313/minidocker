#include "Config.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <sched.h>

class CgroupManager {

private:
  minidocker::FilePath CgroupPath;

  void CreateCgroup();
  void LimitMemoryUsage(std::string_view memory);
  void LimitCpuBanwidth(minidocker::CpuCores cores,
                        minidocker::CpuDuration period);
  void LimitMemorySwap(minidocker::Memory swapLimit);

public:
  CgroupManager(const std::string &groupName, minidocker::CgroupConfig &config);
  void SetupCgroup(minidocker::CgroupConfig &config, pid_t procsId);

  void AddProc(pid_t processId);
  ~CgroupManager();
};
