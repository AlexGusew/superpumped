#include "config.h"
#include "constants.h"
#include "game_manager.h"
#include "game_over_screen.h"
#include "gizmos.h"
#include "player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "target.h"
#include "terrain.h"
#include "texture_drawer.h"
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 800, "Hello superpumped");
  SearchAndSetResourceDir("resources");
  InitAudioDevice();
  GuiLoadStyle("./style_dark.rgs");

  TextureDrawer spritesheet("spritesheet.png",
                            {
                                {"wall", {0, 1}},
                                {"grass", {0, 2}},
                                {"water", {0, 3}},
                            },
                            32);
  Config::Get().terrain = Terrain();
  Config::Get().player = Player();
  Config::Get().gameManager = GameManager();
  Player &player = Config::Get().player;
  GameManager &gameManager = Config::Get().gameManager;
  Gizmos gizmos;
  GameOverScreen gameOverScreen;

  Config::Get().terrain.load("terrain.csv");

  // Config::Get().terrain.generateTiles();

  float h = GetScreenHeight();
  float w = GetScreenWidth();
  Camera2D camera = {0};
  camera.target =
      (Vector2){player.translation.x + 20.0f, player.translation.y + 20.0f};
  camera.offset = (Vector2){w / 2, h / 2};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  gameManager.Init();

  while (!WindowShouldClose()) {
    std::cout << (int)Config::Get().gameState;
    if (Config::Get().gameState == GameState::PLAY) {
      for (Enemy &enemy : Config::Get().enemies) {
        enemy.Update();
      };
      player.Update();
      gizmos.Update();
      gameManager.Update();

      camera.target = (Vector2){player.translation.x, player.translation.y};
      BeginDrawing();
      BeginMode2D(camera);
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
      for (Enemy &enemy : Config::Get().enemies) {
        enemy.Draw();
      };
      player.Draw();
      EndMode2D();

      DrawText(TextFormat("Score %.0f x %.0f", Config::Get().score,
                          Config::Get().scoreMult),
               w / 2 - 100, 20, 20, YELLOW);
      gizmos.DrawUI();
      player.DrawUI();
      EndDrawing();
    } else if (Config::Get().gameState == GameState::GAME_OVER) {
      gameOverScreen.Update();
      BeginDrawing();
      ClearBackground(BLACK);
      gameOverScreen.Draw();
      EndDrawing();
    }
  }
  gameManager.Destroy();
  CloseWindow();

  return 0;
}
