#pragma once

#include "constants.h"
#include "enemy.h"
#include "game_manager.h"
#include "player.h"
#include "target.h"
#include "terrain.h"
#include <vector>

class Config {
public:
  bool gizmosEnabled = true;
  bool gizmosUIEnabled = true;
  bool win = true;
  float score = 0;
  float lastHighScore = 0;
  float scoreMult = 1;
  float perfectScoreMult = 100;
  Terrain terrain;
  std::vector<Target> targets;
  std::vector<Enemy> enemies;
  Player player;
  GameManager gameManager;
  GameState gameState = GameState::PLAY;

  static Config &Get();
  Config(const Config &) = delete;
  void operator=(const Config &) = delete;
  Config() = default;
  void Reset();
};
