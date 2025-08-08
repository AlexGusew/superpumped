#include "tile.h"

using std::string;

Tile::Tile(const string &tile_id, const Vector2 &_translation)
    : GameObject(), id(tile_id) {
  translation = _translation;
}

void Tile::Draw(TextureDrawer &drawer) { drawer.Draw(id, translation); }
