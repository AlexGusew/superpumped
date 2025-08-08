#include "config.h"

Config &Config::Get() {
  static Config instance;
  return instance;
}
