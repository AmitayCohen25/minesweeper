#pragma once
#include "raylib.h"
#include "settings.h"

typedef enum {
	EMPTY, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, MINE
} CellValue;

typedef enum {
	UNOPENED, OPENED, FLAGGED
} CellState;

typedef struct {
	int c;
	int r;
} Cell;

typedef struct {
	int rows;
	int cols;
	int mines;
	CellValue* cells;
	Cell* plantedMines;
	CellState* revealedCells;
	int numOfFlagsPlaced;
	int numOfRevealedCells;
} Field;

bool checkIfAlreadyPlanted(Cell* plantedMines, int plantedCount, int c, int r);

bool checkIfDeclaredSafe(Cell* safeCells, int safeNeighborsCount, int c, int r);

Cell* plantMines(Field* field, Cell safe);

void initField(Field* field, Difficulty diff);

void calculateCellValues(Field* field);

Rectangle getFieldBorders(Field* field, int* retCellSize);

Cell calculateChosenCell(Field* field, Vector2 mouse);

bool clickCell(Field* field, Cell cell, int button);

void revealCell(Field* field, Cell cell);

void revealAllMines(Field* field);

bool chord(Field* field, Cell cell);