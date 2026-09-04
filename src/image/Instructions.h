#pragma once
#include "Config.h"
namespace minidocker::image::instuctions {

enum class BaseImage { None, Alpine, Ubuntu, Arch, Fedora };

struct ImageConfig {
  BaseImage baseImage;
};

struct From {
  BaseImage baseImage;
};
struct Copy {
  minidocker::FilePath source;
  FilePath destination;
};
struct Add {
  FilePath folderPath;
};
struct Cwd {
  FilePath WorkingDirectory;
};
struct Env {
  std::string name;
  std::string value;
};
using Instruction = std::variant<Copy, Add, From, Cwd, Env>;

}; // namespace minidocker::image::instuctions
