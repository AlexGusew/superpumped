#pragma once

#include "raylib.h"

class GameObject {
public:
  Vector2 translation;
  Vector2 size;
  bool isCollider;

  virtual void Draw() {};
  virtual void DrawGizmos() {};
  virtual void Update() {};

  virtual ~GameObject() = default;
};
