
#include "Container.h"
#include "ImageCreator.h"
#include "NamespaceConfig.h"
#include "SettingsManager.h"
#include <filesystem>
#include <iostream>

#include <sched.h>
#include <string>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  SettingsManager settingsManager;

  ImageCreator imageCreator(settingsManager.GetInstructions());
  imageCreator.CreateImage();

  Container container;
  container.Init();
  std::vector<std::string> a = {"ps", "aux"};
  container.Run(a);
  return 0;
}
