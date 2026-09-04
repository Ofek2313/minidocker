#include "container/NamespaceConfig.h"
#include <sched.h>

NamespaceConfig &NamespaceConfig::isolatePid() {
  flags |= CLONE_NEWPID;
  return *this;
}
NamespaceConfig &NamespaceConfig::isolateMount() {
  flags |= CLONE_NEWNS;
  return *this;
}
int NamespaceConfig::getFlags() { return flags; }
