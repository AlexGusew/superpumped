#pragma once

#include "enemy.h"
#include "player.h"
#include "target.h"
#include "terrain.h"
#include <vector>

class Config {
public:
  bool gizmosEnabled = false;
  bool gizmosUIEnabled = false;
  float score = 0;
  float scoreMult = 1;
  float perfectScoreMult = 100;
  Terrain terrain;
  std::vector<Target> targets;
  std::vector<Enemy> enemies;
  Player player;

  static Config &Get();
  Config(const Config &) = delete;
  void operator=(const Config &) = delete;
  Config() = default;
};
