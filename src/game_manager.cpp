#include "game_manager.h"
#include "config.h"
#include "constants.h"
#include "raygui.h"
#include "raylib.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <ostream>

void GameManager::Init(float _bpm, float _startTimePadding) {
  track = LoadMusicStream("Sub Urban - Cradles [NCS Release].mp3");
  // track = LoadMusicStream("Sub Urban - Cradles [NCS Release] - short.mp3");
  track.looping = false;
  playedTime = 0.0f;
  bpm = _bpm;
  startTimePadding = _startTimePadding;
  currentBeat = 1;
  timePerBit = 60.f / bpm;

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

  playedTime = 0.0f;
  currentBeat = 1;
  SetMusicTime(0.0f);
  config.player.Reset();
  player.translation = {h / 2, w / 2};
  perfectDashTimes.clear();

  StopMusicStream(track);
}

void GameManager::SetMusicTime(float newTime) {
  SeekMusicStream(track, newTime + startTimePadding);
}

void GameManager::Start() {
  Config &config = Config::Get();
  config.Reset();
  config.gameState = GameState::PLAY;
  float h = GetScreenHeight();

  PlayMusicStream(track);

  for (size_t i = 0; i < std::size(times); i++) {
    float el_h = GetRandomValue(200, 300);
    float el_w = GetRandomValue(200, 300);

    float phi = Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
    float thetaStart =
        Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
    float thetaWidth =
        Utils::Deg2Rad(static_cast<float>(GetRandomValue(0, 360)));
    float thetaEnd = thetaStart + thetaWidth;

    int steps = std::rand() % 100 + 3;
    std::cout << steps << std::endl;
    // int steps = times[i].size();

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

void GameManager::FinishGame(bool win) {
  Config &config = Config::Get();
  config.gameState = GameState::GAME_OVER;
  config.win = win;
  config.lastHighScore = std::max(config.score, config.lastHighScore);
}

void GameManager::Update() {
  Config &config = Config::Get();
  if (IsMusicStreamPlaying(track)) {
    playedTime += GetFrameTime();
  }

  UpdateMusicStream(track);

  currentBeat = (playedTime + startTimePadding) / timePerBit;

  if (playedTime >= GetMusicTimeLength(track)) {
    FinishGame(true);
    GameManager::Reset();
  } else if (IsKeyPressed(KEY_R)) {
    GameManager::Reset();
    GameManager::Start();
  } else if (config.player.hp <= 0) {
    FinishGame(false);
    GameManager::Reset();
  }
}

void GameManager::Destroy() {
  UnloadMusicStream(track);
  CloseAudioDevice();
}

void GameManager::DrawUI() {
  float w = GetScreenWidth();
  float h = GetScreenHeight();
  DrawText(TextFormat("Beat %d", Config::Get().gameManager.currentBeat + 1),
           w - 180.0f, h - 40.0f, 20, WHITE);
  float bitStartTime = currentBeat * timePerBit;
  float delta = playedTime - bitStartTime;
  float factor = 1 - delta / timePerBit;
  DrawCircle(
      w - 30, h - 30, 20,
      {255, 0, 0, static_cast<unsigned char>(100 + factor * (255 - 100))});
  for (auto &time : perfectDashTimes) {
    if (playedTime - time < 2) {
      float shift = (playedTime - time) / 2;
      DrawText(
          "Perfect dash +x1", w - 180.0f, h - 100.0f - (shift * 40), 20,
          {255, 255, 255, static_cast<unsigned char>(255 * (1.0f - shift))});
    }
  }
}

void GameManager::Dash() {
  float perfectDashThreashold = 0.2f;
  float bitStartTime = currentBeat * timePerBit;
  float delta = playedTime - bitStartTime;
  float factor = delta / timePerBit;

  if (factor < perfectDashThreashold || 1 - factor < perfectDashThreashold) {
    Config::Get().scoreMult += 1;
    perfectDashTimes.push_back(playedTime);
  }
}
