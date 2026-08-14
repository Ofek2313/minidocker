
#include <sched.h>
class Container {
private:
  static int child_function(void *args);
  pid_t processId;

public:
  void InitContainer();
};
