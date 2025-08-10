#pragma once

#include "collision_solver.h"
#include "raylib.h"

class GameObject {
public:
  Vector2 translation;
  Vector2 size;
  Shape *collider_shape;
  bool use_physics;

  inline GameObject(const Vector2 &_translation, const Vector2 &_size,
                    Shape *_collider_shape = nullptr,
                    const bool &_use_physics = false)
      : translation(_translation), size(_size),
        collider_shape(_collider_shape) {}

  virtual void Draw() {};
  virtual void DrawGizmos() {};
  virtual void Update() {};

  virtual ~GameObject() = default;
};
