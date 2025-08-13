#include "game_manager.h"
#include "config.h"
#include "constants.h"
#include "utils.h"
#include <iostream>

void GameManager::Init() {
  track = LoadSound("Sub Urban - Cradles [NCS Release].mp3");
  times = {{3.037974684},
           {4.556962025},
           {6.075949367},
           {7.594936709, 8.35443038, 9.113924051},
           {10.63291139},
           {12.15189873, 12.91139241, 13.67088608},
           {15.18987342, 15.56962025, 15.94936709, 16.32911392},
           {18.2278481, 18.60759494, 18.98734177, 19.36708861},
           {21.26582278, 21.64556962, 22.02531646, 22.40506329},
           {24.30379747, 24.6835443, 25.06329114, 25.44303797},
           {27.34177215, 27.72151899, 28.10126582, 28.48101266},
           {30.37974684, 30.75949367, 31.13924051, 31.51898734},
           {33.41772152, 33.79746835, 34.17721519, 34.55696203},
           {36.4556962, 36.83544304, 37.21518987, 37.59493671}};
};

void GameManager::Reset() {
  Config &config = Config::Get();
  Player &player = config.player;
  float h = GetScreenHeight();
  float w = GetScreenWidth();

  config.player.Reset();
  player.translation = {h / 2, w / 2};
  StopSound(track);
}

void GameManager::Start() {
  Config &config = Config::Get();
  config.Reset();
  config.gameState = GameState::PLAY;
  float h = GetScreenHeight();

  PlaySound(track);

  for (size_t i = 0; i < std::size(times); i++) {
    float el_h = GetRandomValue(200, 300);
    float el_w = GetRandomValue(200, 300);

    float phi = Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
    float thetaStart =
        Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
    float thetaWidth =
        Utils::Deg2Rad(static_cast<float>(GetRandomValue(30, 160)));
    float thetaEnd = thetaStart + thetaWidth;

    int steps = times[i].size();

    Vector2 center{static_cast<float>(i * 200), static_cast<float>(h) * 0.5f};
    if (i % 4 == 0) {
      config.enemies.push_back(Enemy(center, 300));
    }
    std::vector<Vector2> points = Utils::GenerateRotatedEllipseSector(
        el_w, el_h, phi, thetaStart, thetaEnd, steps);
    for (int j = 0; j < size(times[i]); j++) {
      config.targets.push_back(
          Target(Utils::Add(points[j], center), times[i][j], 4, 1.2f));
    }
  }
}

void GameManager::Update() {
  Config &config = Config::Get();

  if (IsKeyPressed(KEY_R)) {
    GameManager::Reset();
    GameManager::Start();
  }

  if (config.player.hp <= 0) {
    config.gameState = GameState::GAME_OVER;
    config.win = false;
    config.lastHighScore = std::max(config.score, config.lastHighScore);

    GameManager::Reset();
  }
}

void GameManager::Destroy() {
  UnloadSound(track);
  CloseAudioDevice();
}
