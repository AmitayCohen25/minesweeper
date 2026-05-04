#pragma once
#include "raylib.h"

typedef enum {
	DIFFICULTY_EASY, DIFFICULTY_MEDIUM, DIFFICULTY_HARD, MAIN_MENU_BUTTON_COUNT
} MainMenuButtonID;

typedef enum {
	RESET_GAME, PLAYING_MAIN_MENU, PLAYING_BUTTON_COUNT
} PlayingButtonID;

typedef enum {
	RESTART_GAME, GAME_OVER_MAIN_MENU, GAME_OVER_BUTTON_COUNT
} GameOverButtonID;

typedef struct {
	Rectangle bounds;
	int id;
	char* text;
} Button;

typedef struct {
	Button buttons[MAIN_MENU_BUTTON_COUNT];
} MainMenuUI;

typedef struct {
	Button buttons[PLAYING_BUTTON_COUNT];
} PlayingUI;

typedef struct {
	Button buttons[GAME_OVER_BUTTON_COUNT];
} GameOverUI;

typedef struct {
	MainMenuUI mainMenu;
	PlayingUI playing;
	GameOverUI gameOver;
	float timer;
} UI;

void initUI(UI* ui);