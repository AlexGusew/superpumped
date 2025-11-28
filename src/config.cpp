#include "config.h"

Config& Config::Get() {
  static Config instance;
  return instance;
}

void Config::Reset() {
  Config::Get().targets.clear();
  Config::Get().enemies.clear();
  Config::Get().score = 0;
  Config::Get().scoreMult = 1;
}
