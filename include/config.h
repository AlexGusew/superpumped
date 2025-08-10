#pragma once

#include "target.h"
#include "terrain.h"
#include <vector>

class Config {
public:
  bool gizmosEnabled = true;
  bool gizmosUIEnabled = true;
  Terrain terrain;
  std::vector<Target> targets;

  static Config &Get();

  Config(const Config &) = delete;
  void operator=(const Config &) = delete;

  Config() = default;
};
