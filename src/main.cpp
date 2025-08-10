#include "config.h"
#include "gizmos.h"
#include "player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "target.h"
#include "terrain.h"
#include "texture_drawer.h"
#include <cstdlib>

int main() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 800, "Hello superpumped");
  SearchAndSetResourceDir("resources");

  TextureDrawer spritesheet("spritesheet.png",
                            {
                                {"wall", {0, 1}},
                                {"grass", {0, 2}},
                                {"water", {0, 3}},
                            },
                            32);
  Config::Get().terrain = Terrain();
  Player player;
  Gizmos gizmos;

  Config::Get().terrain.load("terrain.csv");
  // Config::Get().terrain.generate_tiles();
  float h = GetScreenHeight();
  float w = GetScreenWidth();

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      Config::Get().targets.clear();
      for (int i = 0; i < GetRandomValue(2, 5); i++) {
        Config::Get().targets.push_back(
            Target({static_cast<float>(GetRandomValue(100, w - 100)),
                    static_cast<float>(GetRandomValue(100, h - 100))},
                   (double)rand() * 4 / RAND_MAX));
      };
    }
    player.Update();
    gizmos.Update();

    BeginDrawing();
    ClearBackground(BLACK);
    gizmos.Draw();
    for (Target &target : Config::Get().targets) {
      target.Update();
    };
    for (Tile &tile : Config::Get().terrain.tiles) {
      tile.Draw(spritesheet);
    }
    for (Target &target : Config::Get().targets) {
      target.Draw();
    };
    player.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
