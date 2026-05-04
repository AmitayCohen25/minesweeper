#pragma once

typedef enum {
	EASY, MEDIUM, HARD, DIFFICULTY_SIZE
} Difficulty;

typedef struct {
	int fieldRows;
	int fieldCols;
	int mineCount;
} DifficultySettings;

extern const DifficultySettings difficulties[DIFFICULTY_SIZE];

extern const WINDOW_WIDTH, WINDOW_HEIGHT;