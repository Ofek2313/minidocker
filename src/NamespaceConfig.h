
#include "sched.h"
#include <csignal>

class NamespaceConfig {
public:
  NamespaceConfig &isolatePid();
  NamespaceConfig &isolateMount();
  int getFlags();

private:
  int flags = SIGCHLD;
};
