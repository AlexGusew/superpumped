#pragma once

#include "collision_solver.h"
#include "raylib.h"

class GameObject {
public:
  Vector2 translation;
  Vector2 size;
  Shape* colliderShape;
  bool usePhysics;
  bool enabled;

  inline GameObject(const Vector2& _translation, const Vector2& _size,
                    Shape* _colliderShape = nullptr,
                    const bool& _usePhysics = false)
      : translation(_translation), size(_size), colliderShape(_colliderShape),
        enabled(true) {}

  virtual void Draw() {};
  virtual void DrawGizmos() {};
  virtual void Update() {};

  inline virtual void Reset() {
    usePhysics = false;
    enabled = true;
  };

  virtual ~GameObject() = default;
};
