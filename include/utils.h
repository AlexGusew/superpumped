#pragma once

#include "raylib.h"
#include <vector>

class Utils {
public:
  static float Dot(const Vector2 &a, const Vector2 &b);
  static Vector2 Normalize(const Vector2 &v);
  static Vector2 Add(const Vector2 &a, const Vector2 &b);
  static Vector2 Subtract(const Vector2 &a, const Vector2 &b);
  static Vector2 Multiply(const Vector2 &v, float scalar);
  static float Magnitude(const Vector2 &v);
  static float Deg2Rad(float deg);
  static std::vector<Vector2>
  GenerateRotatedEllipseSector(float a, float b, float phi, float thetaStart,
                               float thetaEnd, int steps);
};
