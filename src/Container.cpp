#include "Container.h"
#include "NamespaceConfig.h"
#include "RootFileSystem.h"
#include <memory>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

int Container::child_function(void *args) {
  RootFileSystem rfs;
  if (!rfs.IsRootFsInitialized()) {
    rfs.CreateRootDirectory();
    rfs.DownloadAlpineEnvironment();
  } // If Root FileSystem Not Initialized download and setup environment;

  rfs.setRoot(getpid());
  rfs.MountProcFolder();
  char *arg[] = {(char *)"/bin/ps", (char *)"aux", nullptr};
  execvp(arg[0], arg);

  return 0;
}

void Container::InitContainer() {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();

  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);

  pid_t pid =
      clone(child_function, stack.get() + stackSize, ns.getFlags(), NULL);

  wait(NULL);
}
