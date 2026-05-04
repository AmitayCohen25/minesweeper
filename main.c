#include "raylib.h" // v5.5
#include "raymath.h"
#include <stdio.h>
#include <time.h>
#include "settings.h"
#include "field.h"
#include "ui.h"
#include "game.h"

#if defined(_WIN32)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

// |--------------------------|
// | Made This In 02-05/04/26 |
// |--------------------------|

int main() {
	SetRandomSeed(time(NULL));
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");
	SetTargetFPS(60);

	Game game;
 	initGame(&game);

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		updateGameState(&game, dt);
		
		BeginDrawing();

		drawGameState(&game);

		EndDrawing();
	}

	CloseWindow();

	freeAll(&game);

	return 0;
}