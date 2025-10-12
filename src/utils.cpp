#include "utils.h"
#include <cmath>

float Utils::Dot(const Vector2 &a, const Vector2 &b) {
  return a.x * b.x + a.y * b.y;
}

Vector2 Utils::Normalize(const Vector2 &v) {
  float length = sqrtf(v.x * v.x + v.y * v.y);
  if (length == 0)
    return {0, 0};
  return {v.x / length, v.y / length};
}

Vector2 Utils::Add(const Vector2 &a, const Vector2 &b) {
  return {a.x + b.x, a.y + b.y};
}

Vector2 Utils::Subtract(const Vector2 &a, const Vector2 &b) {
  return {a.x - b.x, a.y - b.y};
}

Vector2 Utils::Multiply(const Vector2 &v, float scalar) {
  return {v.x * scalar, v.y * scalar};
}

float Utils::Magnitude(const Vector2 &v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

float Utils::Deg2Rad(float deg) { return deg * PI / 180.0f; }

std::vector<Vector2> Utils::GenerateRotatedEllipseSector(
    float a, float b, float phi, float thetaStart, float thetaEnd, int steps) {
  std::vector<Vector2> ellipsePoints;
  if (steps <= 0)
    return ellipsePoints;

  if (thetaEnd <= thetaStart)
    thetaEnd += 2.0f * PI;

  for (int i = 0; i <= steps; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(steps); // 0..1
    float theta = thetaStart + t * (thetaEnd - thetaStart);

    float x = a * cosf(theta);
    float y = b * sinf(theta);

    float xr = x * cosf(phi) - y * sinf(phi);
    float yr = x * sinf(phi) + y * cosf(phi);

    ellipsePoints.push_back({xr, yr});
  }

  return ellipsePoints;
}
