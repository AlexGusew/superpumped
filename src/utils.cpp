#include "utils.h"
#include <cmath>

float Utils::dot(const Vector2 &a, const Vector2 &b) {
  return a.x * b.x + a.y * b.y;
}

Vector2 Utils::normalize(const Vector2 &v) {
  float length = sqrtf(v.x * v.x + v.y * v.y);
  if (length == 0)
    return {0, 0};
  return {v.x / length, v.y / length};
}

Vector2 Utils::add(const Vector2 &a, const Vector2 &b) {
  return {a.x + b.x, a.y + b.y};
}

Vector2 Utils::subtract(const Vector2 &a, const Vector2 &b) {
  return {a.x - b.x, a.y - b.y};
}

Vector2 Utils::multiply(const Vector2 &v, float scalar) {
  return {v.x * scalar, v.y * scalar};
}

float Utils::magnitude(const Vector2 &v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}
