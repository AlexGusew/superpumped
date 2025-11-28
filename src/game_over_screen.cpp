#include "game_over_screen.h"
#include "config.h"
#include "raygui.h"
#include "raylib.h"
#include <iostream>
#include <ostream>
#include <string>

void GameOverScreen::Draw() {
  Config& config = Config::Get();
  float w = GetScreenWidth();
  float h = GetScreenHeight();

  std::string label;

  if (config.win) {
    label = "You win";
  } else {
    label = "You lose";
  };

  GuiLabel({w / 2 - 100, h / 2 - 40, 100, 20}, label.c_str());
  GuiLabel({w / 2 - 100, h / 2 - 10, 100, 20},
           TextFormat("Score %.0f", config.score));
  if (config.score == config.lastHighScore) {
    GuiLabel({w / 2 + 20, h / 2 - 10, 100, 20}, "Hew High Score!");
  }

  if (GuiButton({w / 2 - 100, h / 2 - 10 + 30, 100, 20}, "Respawn")) {
    config.gameManager.Start();
  }
}

void GameOverScreen::Update() {
  if (IsKeyPressed(KEY_R)) {
    Config::Get().gameManager.Start();
  }
}
