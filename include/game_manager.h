#pragma once

#include "raylib.h"
#include <vector>

class GameManager {
public:
  Sound track;
  std::vector<std::vector<double>> times;

  void Init();
  void Reset();
  void Start();
  void Update();
  void Destroy();
};
