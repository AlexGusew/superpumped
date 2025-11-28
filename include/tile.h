#pragma once

#include "game_object.h"
#include "raylib.h"
#include "texture_drawer.h"

class Tile : public GameObject {
public:
  std::string id;
  Tile(const std::string& tile_id, const Vector2& _translation,
       const Vector2& _size, Shape* _colliderShape,
       const bool& _usePhysics = false);
  void Draw(TextureDrawer& drawer);
  void DrawGizmos() override;
};
