#pragma once

#include "raylib.h"

class Utils {
public:
  static float dot(const Vector2 &a, const Vector2 &b);
  static Vector2 normalize(const Vector2 &v);
  static Vector2 add(const Vector2 &a, const Vector2 &b);
  static Vector2 subtract(const Vector2 &a, const Vector2 &b);
  static Vector2 multiply(const Vector2 &v, float scalar);
  static float magnitude(const Vector2 &v);
};
