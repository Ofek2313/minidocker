
#include "NamespaceConfig.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int child_function(void *arg) {
  std::cout << getpid() << std::endl;
  return 0;
}

int main(int argc, char *argv[]) {
  NamespaceConfig ns;
  ns.isolatePid().isolateMount();
  std::vector<uint8_t> childsStack(1024 * 1024);

  pid_t pid = clone(child_function, childsStack.data() + 1024 * 1024,
                    ns.getFlags(), NULL);

  wait(NULL);

  return 0;
}
