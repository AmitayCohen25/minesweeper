#pragma once
#include "raylib.h"
#include "field.h"
#include "settings.h"
#include "ui.h"

typedef enum {
	MAIN_MENU, PLAYING, GAME_OVER
} GameState;

typedef struct {
	Field field;
	GameState gameState;
	UI ui;
	Difficulty difficulty;
} Game;

void initGame(Game* game);

void freeAll(Game* game);

void startGame(Game* game);

void resetGame(Game* game);

void goToMainMenu(Game* game);

void gameOver(Game* game);

void gameWon(Game* game);

void updateGameMainMenu(Game* game);

void updateGamePlaying(Game* game, float dt);

void updateGameGameOver(Game* game);

void updateGameState(Game* game, float dt);

void drawGameMainMenu(Game* game);

void drawGamePlaying(Game* game);

void drawGameGameOver(Game* game);

void drawGameState(Game* game);