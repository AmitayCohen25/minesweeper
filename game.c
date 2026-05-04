#include "game.h"
#include "raylib.h"
#include "field.h"
#include "settings.h"
#include "ui.h"
#include <stdlib.h>

void initGame(Game* game) {
	initUI(&game->ui);
	game->gameState = MAIN_MENU;
	game->field.cells = NULL;
	game->field.plantedMines = NULL;
	game->field.revealedCells = NULL;
}

void freeAll(Game* game) {
	if (game->field.cells) {
		free(game->field.cells);
		game->field.cells = NULL;
	}
	if (game->field.plantedMines) {
		free(game->field.plantedMines);
		game->field.plantedMines = NULL;
	}
	if (game->field.revealedCells) {
		free(game->field.revealedCells);
		game->field.revealedCells = NULL;
	}
}

void startGame(Game* game) {
	initField(&game->field, game->difficulty);
	game->ui.timer = 0;
	game->gameState = PLAYING;
}

void resetGame(Game* game) {
	freeAll(game);
	initField(&game->field, game->difficulty);
	game->ui.timer = 0;
	game->gameState = PLAYING;
}

void goToMainMenu(Game* game) {
	freeAll(game);
	game->gameState = MAIN_MENU;
}

void gameOver(Game* game) {
	revealAllMines(&game->field);
	game->gameState = GAME_OVER;
}

void gameWon(Game* game) {
	for (int i = 0; i < game->field.mines; i++) {
		Cell mine = game->field.plantedMines[i];
		game->field.revealedCells[mine.r * game->field.cols + mine.c] = FLAGGED;
	}
	game->field.numOfFlagsPlaced = game->field.mines;
	game->gameState = GAME_OVER;
}

void updateGameMainMenu(Game* game) {
	if (game->gameState != MAIN_MENU) return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mouse = GetMousePosition();
		for (int i = 0; i < MAIN_MENU_BUTTON_COUNT; i++) {
			Button* b = &(game->ui.mainMenu.buttons[i]);

			if (CheckCollisionPointRec(mouse, b->bounds)) {
				switch (b->id) {
				case DIFFICULTY_EASY:
					game->difficulty = EASY;
					break;
				case DIFFICULTY_MEDIUM:
					game->difficulty = MEDIUM;
					break;
				case DIFFICULTY_HARD:
					game->difficulty = HARD;
					break;
				}
				startGame(game);
			}
		}
	}
}

void updateGamePlaying(Game* game, float dt) {
	if (game->gameState != PLAYING) return;

	game->ui.timer += dt;
	
	bool left = IsMouseButtonPressed(MOUSE_BUTTON_LEFT), right = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

	if (left || right ) {
		Vector2 mouse = GetMousePosition();
		if (left && CheckCollisionPointRec(mouse, game->ui.playing.buttons[RESET_GAME].bounds)) {
			resetGame(game);
		}
		else if (left && CheckCollisionPointRec(mouse, game->ui.playing.buttons[PLAYING_MAIN_MENU].bounds)) {
			goToMainMenu(game);
		}
		else if (CheckCollisionPointRec(mouse, getFieldBorders(&game->field, NULL))) {
			Cell chosenCell = calculateChosenCell(&game->field, mouse);
			int button = (right ? MOUSE_BUTTON_RIGHT : MOUSE_BUTTON_LEFT);
			bool firstClick = (game->field.plantedMines == NULL ? true : false);
			if (firstClick) {
				game->field.plantedMines = plantMines(&game->field, chosenCell);
				calculateCellValues(&game->field);
			}
			bool cellWasMine = clickCell(&game->field, chosenCell, button);
			if (cellWasMine) gameOver(game);
		}
	}

	if (game->field.numOfRevealedCells == game->field.cols * game->field.rows - game->field.mines) gameWon(game);
}

void updateGameGameOver(Game* game) {
	if (game->gameState != GAME_OVER) return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mouse = GetMousePosition();
		if (CheckCollisionPointRec(mouse, game->ui.gameOver.buttons[RESTART_GAME].bounds)) {
			resetGame(game);
		}
		else if (CheckCollisionPointRec(mouse, game->ui.gameOver.buttons[GAME_OVER_MAIN_MENU].bounds)) {
			goToMainMenu(game);
		}
	}
}

void updateGameState(Game* game, float dt) {
	switch (game->gameState) {
	case MAIN_MENU:
		updateGameMainMenu(game);
		break;
	case PLAYING:
		updateGamePlaying(game, dt);
		break;
	case GAME_OVER:
		updateGameGameOver(game);
		break;
	}
}

void drawGameMainMenu(Game* game) {
	const int w = WINDOW_WIDTH;
	const int h = WINDOW_HEIGHT;

	ClearBackground(LIGHTGRAY);

	DrawCircle(w / 2, 150, 100, DARKGRAY); // mine
	DrawRectangleRounded((Rectangle) { w / 2 - 15, 15, 30, 270 }, 10, 10, DARKGRAY); // spike
	DrawRectangleRounded((Rectangle) { w / 2 - 135, 135, 270, 30 }, 10, 10, DARKGRAY); // spike

	const char* title = "Minesweeper";
	int titleFontSize = 80;
	int titleWidth = MeasureText(title, titleFontSize);
	DrawText(title, w / 2 - (titleWidth / 2), 280, titleFontSize, BLACK);

	const char* selDif = "Select Difficulty";
	int selDifFontSize = 40;
	int selDifWidth = MeasureText(selDif, selDifFontSize);
	DrawText(selDif, w / 2 - (selDifWidth / 2), 380, selDifFontSize, BLACK);

	for (int i = 0; i < MAIN_MENU_BUTTON_COUNT; i++) {
		Button b = game->ui.mainMenu.buttons[i];
		Color c = { 0 };
		if (b.id == DIFFICULTY_EASY) c = GREEN;
		else if (b.id == DIFFICULTY_MEDIUM) c = ORANGE;
		else if (b.id == DIFFICULTY_HARD) c = RED;
		DrawRectangleRec(b.bounds, c);
		if (CheckCollisionPointRec(GetMousePosition(), b.bounds)) {
			Color nc = ColorBrightness(c, 0.1), outc = ColorBrightness(c, -0.1);
			DrawRectangleRec(b.bounds, nc);
			DrawRectangleLinesEx(b.bounds, 5, outc);
		}
		int diffFontSize = 50;
		int diffWidth = MeasureText(b.text, diffFontSize);
		DrawText(b.text, b.bounds.x + b.bounds.width / 2 - diffWidth / 2, b.bounds.y + b.bounds.height / 2 - diffFontSize / 2, diffFontSize, BLACK);
	}
}

void drawGamePlaying(Game* game) {
	const int w = WINDOW_WIDTH;
	const int h = WINDOW_HEIGHT;

	ClearBackground(LIGHTGRAY);

	char minesText[3];
	sprintf_s(minesText, 3, "%d", game->field.mines - game->field.numOfFlagsPlaced);

	char timerText[10];
	int time = (int)game->ui.timer;
	sprintf_s(timerText, 10, "%02d:%02d", time / 60, time % 60);

	Rectangle resetButton = game->ui.playing.buttons[RESET_GAME].bounds;
	DrawRectangleRec(resetButton, GRAY);
	DrawCircle(resetButton.x + (resetButton.width / 2), resetButton.y + (resetButton.height / 2), resetButton.width / 2 - 5, YELLOW); // face
	DrawLineEx((Vector2) { resetButton.x + 17, resetButton.y + 15 }, (Vector2) { resetButton.x + 17, resetButton.y + 25 }, 3, DARKGRAY); // left eye
	DrawLineEx((Vector2) { resetButton.x + resetButton.width - 17, resetButton.y + 15 }, (Vector2) { resetButton.x + resetButton.width - 17, resetButton.y + 25 }, 3, DARKGRAY); // right eye
	DrawCircleSector((Vector2) { resetButton.x + (resetButton.width / 2), resetButton.y + (resetButton.height / 2) }, resetButton.width / 2 - 12, 20, 160, 10, DARKGRAY); // mouth
	DrawCircleSector((Vector2) { resetButton.x + (resetButton.width / 2), resetButton.y + (resetButton.height / 2) }, resetButton.width / 2 - 15, 20, 160, 10, YELLOW); // mouth

	Rectangle mainMenuButton = game->ui.playing.buttons[PLAYING_MAIN_MENU].bounds;
	DrawRectangleRec(mainMenuButton, GRAY);
	DrawRectangle(mainMenuButton.x+10, mainMenuButton.y + 23, mainMenuButton.width-20, mainMenuButton.height-30, DARKGRAY); // house
	DrawTriangle((Vector2) { mainMenuButton.x + 10, mainMenuButton.y + 23 }, // roof
		(Vector2) { mainMenuButton.x + 10 + mainMenuButton.width - 20, mainMenuButton.y + 23},
		(Vector2) {(mainMenuButton.x + 10 + mainMenuButton.x + 10 + mainMenuButton.width - 20) / 2, mainMenuButton.y+8}, DARKGRAY);
	DrawRectangle((mainMenuButton.x + 10 + mainMenuButton.x + 10 + mainMenuButton.width - 20) / 2 + 5, mainMenuButton.y + 11, 6, 20, DARKGRAY); // chimney

	int cellSize = 0;
	Rectangle fieldBorders = getFieldBorders(&game->field, &cellSize);

	DrawText(minesText, fieldBorders.x + 20, 15, 50, RED);
	DrawText(timerText, fieldBorders.x + fieldBorders.width - 20 - MeasureText(timerText, 50), 15, 50, RED);

	for (int i = 0; i < game->field.rows; i++) {
		for (int j = 0; j < game->field.cols; j++) {
			DrawRectangle(fieldBorders.x + j * cellSize, fieldBorders.y + i * cellSize, cellSize, cellSize, DARKGRAY);

			if (game->field.revealedCells[i * game->field.cols + j] == OPENED) {
				DrawRectangle(fieldBorders.x + j * cellSize, fieldBorders.y + i * cellSize, cellSize, cellSize, GRAY);
				char value[2];
				if (game->field.cells[i * game->field.cols + j] == MINE) {
					DrawRectangle(fieldBorders.x + j * cellSize, fieldBorders.y + i * cellSize, cellSize, cellSize, ColorTint(LIGHTGRAY, RED));
					DrawCircle(fieldBorders.x + j * cellSize + cellSize / 2, fieldBorders.y + i * cellSize + cellSize / 2, cellSize / 2 - cellSize / 5, BLACK);
				}
				else if (game->field.cells[i * game->field.cols + j]) {
					sprintf_s(value, 2, "%d", game->field.cells[i * game->field.cols + j]);
					Color c = { 0 };
					switch (game->field.cells[i * game->field.cols + j]) {
					case ONE:
						c = DARKBLUE;
						break;
					case TWO:
						c = DARKGREEN;
						break;
					case THREE:
						c = RED;
						break;
					case FOUR:
						c = ColorBrightness(DARKBLUE, -0.5);
						break;
					case FIVE:
						c = ColorBrightness(RED, -0.3);
						break;
					case SIX:
						c = SKYBLUE;
						break;
					case SEVEN:
						c = BLACK;
						break;
					default:
						c = ColorBrightness(GRAY, -0.5);
					}
					DrawText(value, fieldBorders.x + j * cellSize + cellSize/2 - MeasureText(value, cellSize)/2, fieldBorders.y + i * cellSize, cellSize, c);
				}
			}
			else if (game->field.revealedCells[i * game->field.cols + j] == FLAGGED) {
				DrawRectangle(fieldBorders.x + j * cellSize + cellSize / 3, fieldBorders.y + i * cellSize + cellSize - cellSize / 4, cellSize / 3, cellSize / 8, BLACK); // base
				DrawRectangle(fieldBorders.x + j * cellSize + cellSize / 2 - cellSize/20, fieldBorders.y + i * cellSize + cellSize / 4, cellSize/10, 9*cellSize / 16, LIGHTGRAY); // pole
				DrawRectangle(fieldBorders.x + j * cellSize + cellSize / 2 - cellSize / 4, fieldBorders.y + i * cellSize + cellSize / 4 + cellSize / 16, cellSize / 4, cellSize / 4, RED); // flag
			}
			DrawRectangleLinesEx((Rectangle) { fieldBorders.x + j * cellSize, fieldBorders.y + i * cellSize, cellSize, cellSize }, 1, BLACK);
		}
	}

}

void drawGameGameOver(Game* game) {
	drawGamePlaying(game);
	DrawText("Game Over", WINDOW_WIDTH / 2 - MeasureText("Game Over", 70) / 2, WINDOW_HEIGHT / 2 - 35, 70, ColorAlpha(GREEN, 0.8));
}

void drawGameState(Game* game) {
	switch (game->gameState) {
	case MAIN_MENU:
		drawGameMainMenu(game);
		break;
	case PLAYING:
		drawGamePlaying(game);
		break;
	case GAME_OVER:
		drawGameGameOver(game);
		break;
	}
}