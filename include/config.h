#pragma once

#include "terrain.h"

class Config {
public:
  bool gizmosEnabled = true;
  bool gizmosUIEnabled = true;
  Terrain terrain;

  static Config &Get();

  Config(const Config &) = delete;
  void operator=(const Config &) = delete;

  Config() = default;
};
