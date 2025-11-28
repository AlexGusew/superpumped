#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>

class TextureDrawer {
private:
  Texture texture;

public:
  std::unordered_map<std::string, std::pair<int, int>> config;
  int size;

  TextureDrawer(
      const std::string& path,
      const std::unordered_map<std::string, std::pair<int, int>>& _config,
      int _size);

  ~TextureDrawer();

  void Draw(const std::string& id, Vector2 position);

  void Unload();
};
