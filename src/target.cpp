#include "target.h"
#include "config.h"
#include "raylib.h"
#include "shape_drawer.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <ostream>

Target::Target(const Vector2 &_translation, float _readyDelta,
               float _disappearDelta, float _chargingDelta)
    : GameObject(_translation, {32, 32},
                 new OrientedRectangle(_translation, {32, 32})),
      readyDelta(_readyDelta), disappearDelta(_disappearDelta), ready(false),
      reached(false), initialTime(GetTime()), chargingDelta(_chargingDelta) {
  readyTime = GetTime() + readyDelta;
}

void Target::Update() {
  bool isDisappeared = GetTime() - readyTime > disappearDelta;
  if (!reached && isDisappeared && enabled) {
    Config::Get().scoreMult = 1;
  }
  if (reached || isDisappeared) {
    enabled = false;
  }
  if (GetTime() >= readyTime) {
    ready = true;
  }
}

void Target::Reach() {
  if (!enabled)
    return;
  reached = true;
  reachedTime = GetTime();
  float missDelta = reachedTime - readyTime;
  float targetScoreMult = (disappearDelta - missDelta) / disappearDelta;
  isPerfect = missDelta < 0.3f;
  Config::Get().score += Config::Get().scoreMult * targetScoreMult +
                         isPerfect * Config::Get().perfectScoreMult;
  Config::Get().scoreMult += 1;
}

void Target::Draw() {
  if (!enabled) {
    if (reached && isPerfect) {
      float factor =
          std::clamp((float)((GetTime() - reachedTime) / 2.0f), 0.0f, 1.0f);
      unsigned char opacity =
          static_cast<unsigned char>((factor * (1 - factor) * 255));
      Color current = {255, 255, 255, opacity};
      DrawText("Perfect", translation.x - size.x / 2 - 20,
               translation.y - size.y / 2 - 35, 20, current);
    }
    return;
  }
  if (Config::Get().gizmosUIEnabled) {
    ShapeDrawer::DrawShape(collider_shape);
  }
  Color startChargeColor = BLUE;
  Color endChargeColor = DARKPURPLE;
  Color readyColor = WHITE;
  Color initialColor = {30, 30, 30, 255};
  if (!ready) {
    float startChargeTime = readyTime - chargingDelta;
    if (GetTime() > startChargeTime) {
      float factor = (GetTime() - startChargeTime) / chargingDelta;
      Color current = ColorLerp(startChargeColor, endChargeColor, factor);
      DrawRectangleV(Utils::subtract(translation, Utils::multiply(size, 0.5f)),
                     size, current);
    } else {
      float factor =
          (GetTime() - startChargeTime + chargingDelta) / chargingDelta;
      Color current = ColorLerp(initialColor, startChargeColor, factor);
      DrawRectangleV(Utils::subtract(translation, Utils::multiply(size, 0.5f)),
                     size, current);
    }
  } else {
    float factor = (std::min(GetTime() - readyTime, (double)disappearDelta)) /
                   disappearDelta;
    unsigned char opacity =
        static_cast<unsigned char>((factor * (1 - factor) * 255));
    Color current = ColorLerp(readyColor, {255, 255, 255, opacity}, factor);
    DrawRectangleV(Utils::subtract(translation, Utils::multiply(size, 0.5f)),
                   size, current);
  }
}
