#include "config.h"
#include "gizmos.h"
#include "player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "terrain.h"
#include "texture_drawer.h"

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
  Config::Get().terrain.generate_tiles();

  while (!WindowShouldClose()) {
    player.Update();
    gizmos.Update();

    BeginDrawing();
    ClearBackground(BLACK);
    gizmos.Draw();
    for (Tile &tile : Config::Get().terrain.tiles) {
      tile.Draw(spritesheet);
    }
    player.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
