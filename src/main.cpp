#include "app/App.h"
#include <cli/Cli.h>
#include <complex>

int main(int argc, char *argv[]) {

  // App app;
  // app.Init();
  // app.Run();
  Cli cli(argc, argv);
  cli.GetParameter("--name");
}
