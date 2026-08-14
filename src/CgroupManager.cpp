#include "CgroupManager.h"
#include <cerrno>
#include <filesystem>
#include <string>
#include <system_error>

CgroupManager::CgroupManager(const std::string &groupName) {

  CgroupPath = "/sys/fs/cgroup/" + groupName;
}

void CgroupManager::CreateCgroup() {

  if (std::filesystem::is_directory(CgroupPath)) {
    // Cgroup name is already taken

  } else {
    if (!std::filesystem::create_directory(CgroupPath)) {
      throw std::system_error(errno, std::generic_category(),
                              "Cgroup Failed To Create");
    }
  }
}
