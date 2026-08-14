#include <filesystem>
#include <string_view>
class CgroupManager {

private:
  std::filesystem::path CgroupPath;

public:
  CgroupManager(const std::string &groupName);
  void CreateCgroup();
  void LimitMemory();
  void LimitCpu();
};
