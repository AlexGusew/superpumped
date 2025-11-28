#include "config.h"
#include "constants.h"
#include "game_manager.h"
#include "game_over_screen.h"
#include "gizmos.h"
#include "lvl_editor.h"
#include "player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "target.h"
#include "terrain.h"
#include "texture_drawer.h"

#define NO_FONT_AWESOME
#include "rlImGui.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI |
                 FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 800, "Hello superpumped");
  SetTargetFPS(60);
  SearchAndSetResourceDir("resources");
  InitAudioDevice();
  GuiLoadStyle("./style_dark.rgs");

  rlImGuiSetup(true);

  TextureDrawer spritesheet("spritesheet.png",
                            {
                                {"wall", {0, 1}},
                                {"grass", {0, 2}},
                                {"water", {0, 3}},
                            },
                            32);
  float h = GetScreenHeight();
  float w = GetScreenWidth();

  Config& config = Config::Get();
  config.terrain = Terrain();
  config.player = Player();
  config.gameManager = GameManager();
  config.mainCamera = Camera2D();

  Camera2D& camera = config.mainCamera;
  Player& player = config.player;
  GameManager& gameManager = config.gameManager;
  Gizmos gizmos;
  GameOverScreen gameOverScreen;
  LvlEditor lvlEditor;

  lvlEditor.Init();
  config.terrain.load("terrain.csv");

  // config.terrain.generateTiles();

  camera.target =
      (Vector2){player.translation.x + 20.0f, player.translation.y + 20.0f};
  camera.offset = (Vector2){w / 2, h / 2};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  gameManager.Init(80.0f, 0.3f);

  while (!WindowShouldClose()) {
    config.w = w;
    config.h = h;

    if (IsKeyPressed(KEY_E)) {
      config.gameState = GameState::EDITOR;
      gameManager.Reset();
    }

    if (config.gameState == GameState::PLAY) {
      for (Enemy& enemy : config.enemies) {
        enemy.Update();
      };
      player.Update();
      gameManager.Update();
      for (Target& target : config.targets) {
        target.Update();
      };
      camera.target = (Vector2){player.translation.x, player.translation.y};

      BeginDrawing();
      BeginMode2D(camera);
      ClearBackground(BLACK);

      gizmos.Draw();
      for (Tile& tile : config.terrain.tiles) {
        tile.Draw(spritesheet);
      }
      for (Target& target : config.targets) {
        target.Draw();
      };
      for (Enemy& enemy : config.enemies) {
        enemy.Draw();
      };
      player.Draw();
      EndMode2D();

      DrawText(TextFormat("Score %.0f x %.0f", config.score, config.scoreMult),
               w / 2 - 100, 20, 20, YELLOW);
      gizmos.DrawUI();
      player.DrawUI();
      gameManager.DrawUI();

      EndDrawing();
    } else if (config.gameState == GameState::GAME_OVER) {
      gameOverScreen.Update();
      BeginDrawing();
      ClearBackground(BLACK);
      gameOverScreen.Draw();
      EndDrawing();
    } else if (config.gameState == GameState::EDITOR) {
      lvlEditor.Update();

      BeginDrawing();
      ClearBackground(BLACK);

      BeginMode2D(camera);
      gizmos.Draw();
      lvlEditor.Draw();
      EndMode2D();

      gizmos.DrawUI();
      lvlEditor.DrawUI();
      DrawFPS(16, 16);

      EndDrawing();
    }
  }
  gameManager.Destroy();
  rlImGuiShutdown();
  CloseWindow();

  return 0;
}
