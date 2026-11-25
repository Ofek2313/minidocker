#include "app/App.h"
#include <cli/Cli.h>

int main(int argc, char *argv[]) {

  // App app;
  // app.Init();
  // app.Build();
  // app.Run();

  Cli cli(argc, argv);
  cli.Interpret();
}
