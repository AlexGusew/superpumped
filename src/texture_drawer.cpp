#include "texture_drawer.h"

TextureDrawer::TextureDrawer(
    const std::string& path,
    const std::unordered_map<std::string, std::pair<int, int>>& _config,
    int _size)
    : config(_config), size(_size) {
  texture = LoadTexture(path.c_str());
}

TextureDrawer::~TextureDrawer() { Unload(); }

void TextureDrawer::Draw(const std::string& id, Vector2 position) {
  if (config.find(id) != config.end()) {
    int row = config[id].first;
    int col = config[id].second;
    DrawTextureRec(texture,
                   {static_cast<float>(size * col),
                    static_cast<float>(size * row), static_cast<float>(size),
                    static_cast<float>(size)},
                   position, WHITE);
  }
}

void TextureDrawer::Unload() {
  if (texture.id != 0) {
    UnloadTexture(texture);
    texture = {0};
  }
}
