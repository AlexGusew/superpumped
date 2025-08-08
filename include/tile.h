#pragma once

#include "game_object.h"
#include "raylib.h"
#include "texture_drawer.h"

class Tile : public GameObject {
public:
  std::string id;
  Tile(const std::string &tile_id, const Vector2 &_translation);
  void Draw(TextureDrawer &drawer);
};
