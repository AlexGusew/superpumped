#include "tile.h"
#include "config.h"
#include "raylib.h"
#include "shape_drawer.h"

using std::string;

Tile::Tile(const string &tile_id, const Vector2 &_translation,
           const Vector2 &_size, Shape *_collider_shape = nullptr,
           const bool &_use_physics)
    : GameObject(_translation, _size, _collider_shape), id(tile_id) {}

void Tile::Draw(TextureDrawer &drawer) {
  drawer.Draw(id, translation);
  if (Config::Get().gizmosUIEnabled) {
    DrawGizmos();
  }
}

void Tile::DrawGizmos() { ShapeDrawer::DrawShape(collider_shape); }
