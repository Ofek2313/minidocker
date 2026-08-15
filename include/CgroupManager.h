#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <sched.h>
class CgroupManager {

  using filePath = std::filesystem::path;

private:
  std::filesystem::path CgroupPath;

public:
  CgroupManager(const std::string &groupName);
  void CreateCgroup();
  void LimitMemoryUsage(std::string_view memory);
  void AddProc(pid_t processId);
  void LimitCpuBanwidth(double cores, int period);
  ~CgroupManager();
};
