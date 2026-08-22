#pragma once
#include <string>
#include <vector>

class PipeHandler;
struct ChildArgs {
  PipeHandler &PipeHandler;
  std::vector<std::string> commands;
};
