#include "ui.h"
#include "settings.h"

void initUI(UI* ui) {
	const int w = WINDOW_WIDTH;
	const int h = WINDOW_HEIGHT;
	ui->mainMenu.buttons[DIFFICULTY_EASY] = (Button){
		.id = DIFFICULTY_EASY,
		.bounds = (Rectangle) {50, 450, 200, 100},
		.text = "Easy"
	};
	ui->mainMenu.buttons[DIFFICULTY_MEDIUM] = (Button){
		.id = DIFFICULTY_MEDIUM,
		.bounds = (Rectangle) {300, 450, 200, 100},
		.text = "Medium"
	};
	ui->mainMenu.buttons[DIFFICULTY_HARD] = (Button){
		.id = DIFFICULTY_HARD,
		.bounds = (Rectangle) {550, 450, 200, 100},
		.text = "Hard"
	};
	ui->playing.buttons[RESET_GAME] = (Button){
		.id = RESET_GAME,
		.bounds = (Rectangle) {w / 2 + 30 - 25, 15, 50, 50}
	};
	ui->playing.buttons[PLAYING_MAIN_MENU] = (Button){
		.id = PLAYING_MAIN_MENU,
		.bounds = (Rectangle) {w / 2 - 30 - 25, 15, 50, 50}
	};
	ui->gameOver.buttons[RESTART_GAME] = (Button){
		.id = RESTART_GAME,
		.bounds = (Rectangle) {w / 2 + 30 - 25, 15, 50, 50}
	};
	ui->gameOver.buttons[GAME_OVER_MAIN_MENU] = (Button){
		.id = GAME_OVER_MAIN_MENU,
		.bounds = (Rectangle) {w / 2 - 30 - 25, 15, 50, 50}
	};
}