#pragma once

#include "raylib.h"
#include <vector>

class GameManager {
public:
  Music track;
  float playedTime;
  float startTimePadding;
  float bpm;
  float timePerBit;
  int currentBeat;

  std::vector<std::vector<double>> times;
  std::vector<float> perfectDashTimes;

  void Init(float bpm, float startTimePadding);
  void Reset();
  void FinishGame(bool win);
  void Start();
  void Update();
  void Destroy();
  void DrawUI();
  void Dash();

  void SetMusicTime(float newTime);
};
