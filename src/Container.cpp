#include "Container.h"
#include "CgroupManager.h"
#include "NamespaceConfig.h"
#include "RootFileSystem.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

struct FileDescriptorArgs {
  int readFd;
  int writeFd;
};

int Container::child_function(void *args) {
  FileDescriptorArgs *Fd = static_cast<FileDescriptorArgs *>(args);
  close(Fd->readFd);
  try {
    RootFileSystem rfs;
    if (!rfs.IsRootFsInitialized()) {
      rfs.CreateRootDirectory();
      rfs.DownloadAlpineEnvironment();
    } // If Root FileSystem Not Initialized download and setup environment;

    rfs.setRoot(getpid());
    rfs.MountProcFolder();
    // auto memoryBuffer =
    // std::make_shared<std::vector<std::byte>>(1024 * 1024 * 1024);

    // std::fill_n(memoryBuffer->data(), 1024 * 1024 * 1024, std::byte{0xA});
    char *arg[] = {(char *)"/bin/ps", (char *)"aux", nullptr};
    execvp(arg[0], arg);
  } catch (const std::exception &ex) {
    write(Fd->writeFd, ex.what(), std::strlen(ex.what()));
    close(Fd->writeFd);
    _exit(1);
  }

  return 0;
}

void Container::InitContainer() {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();
  int fd[2];
  pipe(fd);
  FileDescriptorArgs Fd;
  Fd.readFd = fd[0];
  Fd.writeFd = fd[1];
  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);

  CgroupManager CgroupManager("minidocker");
  CgroupManager.CreateCgroup();
  CgroupManager.LimitMemoryUsage("512M");
  CgroupManager.LimitCpuBanwidth(0.5, 100000);
  CgroupManager.AddProc(getpid());
  pid_t pid =
      clone(child_function, stack.get() + stackSize, ns.getFlags(), &Fd);
  close(fd[1]);
  processId = pid;
  char errBuffer[8192];
  std::string errText = "";
  int bytes = read(fd[0], errBuffer, sizeof(errBuffer));
  if (bytes > 0) {
    errText.append(errBuffer);
    while ((bytes = read(fd[0], errBuffer, sizeof(errBuffer)) > 0)) {
      errText.append((errBuffer));
    }
    std::cerr << errText << std::endl;
  }
  close(fd[0]);
  std::cout << "Container Setup!" << std::endl;
  wait(NULL);
}
