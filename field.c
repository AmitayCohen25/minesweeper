#include "raylib.h"
#include "field.h"
#include <stdlib.h>
#include "settings.h"

bool checkIfAlreadyPlanted(Cell* plantedMines, int plantedCount, int c, int r) {
	for (int i = 0; i < plantedCount; i++) {
		if ((plantedMines[i].c == c) && (plantedMines[i].r == r)) {
			return true;
		}
	}
	return false;
}

bool checkIfDeclaredSafe(Cell* safeCells, int safeCellsCount, int c, int r) {
	for (int i = 0; i < safeCellsCount; i++) {
		if ((safeCells[i].c == c) && (safeCells[i].r == r)) {
			return true;
		}
	}
	return false;
}

Cell* plantMines(Field* field, Cell safe) {
	Cell* planted = (Cell*)malloc(field->mines * sizeof(Cell));
	if (!planted) exit(1);
	int plantedCount = 0;

	int safeCellsCount = 0;
	Cell safeCells[9] = { 0 };

	for (int dc = -1; dc <= 1; dc++) {
		for (int dr = -1; dr <= 1; dr++) {
			int c = safe.c + dc;
			int r = safe.r + dr;
			if (c < 0 || r < 0 || c >= field->cols || r >= field->rows) continue;
			safeCells[safeCellsCount++] = (Cell){ c, r };
		}
	}

	while (plantedCount < field->mines) {
		int c = GetRandomValue(0, field->cols - 1);
		int r = GetRandomValue(0, field->rows - 1);

		bool isAlreadyPlanted = checkIfAlreadyPlanted(planted, plantedCount, c, r);
		bool isSafe = checkIfDeclaredSafe(safeCells, safeCellsCount, c, r);
		if (isAlreadyPlanted || isSafe) {
			continue;
		}
		field->cells[r * field->cols + c] = MINE;
		planted[plantedCount].c = c;
		planted[plantedCount].r = r;
		plantedCount++;
	}
	return planted;
}

void initField(Field* field, Difficulty diff) {
	field->rows = difficulties[diff].fieldRows;
	field->cols = difficulties[diff].fieldCols;
	field->mines = difficulties[diff].mineCount;
	field->cells = (CellValue*)calloc(field->rows * field->cols, sizeof(CellValue));
	if (!field->cells) exit(1);
	field->revealedCells = (CellState*)calloc(field->rows * field->cols, sizeof(CellState));
	if (!field->revealedCells) exit(1);
	field->numOfFlagsPlaced = 0;
	field->numOfRevealedCells = 0;
}

void calculateCellValues(Field* field) {
	for (int c = 0; c < field->cols; c++) {
		for (int r = 0; r < field->rows; r++) {
			if (field->cells[r * field->cols + c] == MINE) continue;
			int count = 0;
			for (int dc = -1; dc <= 1; dc++) {
				for (int dr = -1; dr <= 1; dr++) {
					if (dc == 0 && dr == 0) continue;
					int nc = c + dc;
					int nr = r + dr;
					if (nc < 0 || nr < 0 || nc >= field->cols || nr >= field->rows) continue;
					if (field->cells[nr * field->cols + nc] == MINE) count++;
				}
			}
			field->cells[r * field->cols + c] = (CellValue)count;
		}
	}
}

Rectangle getFieldBorders(Field* field, int* retCellSize) {
	const int w = WINDOW_WIDTH;
	const int h = WINDOW_HEIGHT;

	Rectangle fieldBorder = { 20, 80, w - 40, h - 100 };
	int cellSize = min((fieldBorder.height) / field->rows, fieldBorder.width / field->cols);
	int fieldWidth = cellSize * field->cols;
	int fieldHeight = cellSize * field->rows;
	int fieldOriginX = fieldBorder.x + fieldBorder.width / 2 - fieldWidth / 2;
	int fieldOriginY = fieldBorder.y + fieldBorder.height / 2 - fieldHeight / 2;

	if (retCellSize) *retCellSize = cellSize;
	return (Rectangle) { fieldOriginX, fieldOriginY, fieldWidth, fieldHeight };
}

Cell calculateChosenCell(Field* field, Vector2 mouse) {
	int cellSize = 0;
	Rectangle fieldBorders = getFieldBorders(field, &cellSize);
	int c = ((int)mouse.x - (int)fieldBorders.x) / (cellSize);
	int r = ((int)mouse.y - (int)fieldBorders.y) / (cellSize);

	return (Cell) { c, r };
}

bool clickCell(Field* field, Cell cell, int button) {
	if (cell.c < 0 || cell.r < 0 ||
		cell.c >= field->cols || cell.r >= field->rows) return false;

	CellState* cellState = &(field->revealedCells[cell.r * field->cols + cell.c]);

	if (button == MOUSE_BUTTON_LEFT) {
		if (*cellState == FLAGGED) return false;
		else if (*cellState == OPENED) return chord(field, cell);

		if (field->cells[cell.r * field->cols + cell.c] == MINE) return true;
		revealCell(field, cell);
	}
	else if (button == MOUSE_BUTTON_RIGHT) {
		if (*cellState == OPENED) return false;

		if (*cellState == UNOPENED) {
			if (field->numOfFlagsPlaced == field->mines) return false;
			*cellState = FLAGGED;
			field->numOfFlagsPlaced++;
		}
		else if (*cellState == FLAGGED) {
			*cellState = UNOPENED;
			field->numOfFlagsPlaced--;
		}
	}
	return false;
}

void revealCell(Field* field, Cell cell) {
	if (cell.c < 0 || cell.r < 0 ||
		cell.c >= field->cols || cell.r >= field->rows) return;

	int idx = cell.r * field->cols + cell.c;

	CellState state = field->revealedCells[idx];

	if (state == FLAGGED || state == OPENED) return;

	field->revealedCells[idx] = OPENED;
	field->numOfRevealedCells++;

	if (field->cells[idx] != EMPTY) return;

	for (int dc = -1; dc <= 1; dc++) {
		for (int dr = -1; dr <= 1; dr++) {
			if (dc == 0 && dr == 0) continue;
			int c = cell.c + dc;
			int r = cell.r + dr;
			if (c < 0 || r < 0 || c >= field->cols || r >= field->rows) continue;
			revealCell(field, (Cell) { c, r });
		}
	}
}

void revealAllMines(Field* field) {
	for (int i = 0; i < field->mines; i++) {
		Cell mine = field->plantedMines[i];
		field->revealedCells[mine.r * field->cols + mine.c] = OPENED;
	}
}

bool chord(Field* field, Cell cell) {
	int surroundingFlags = 0;
	for (int dc = -1; dc <= 1; dc++) {
		for (int dr = -1; dr <= 1; dr++) {
			if (dc == 0 && dr == 0) continue;
			int c = cell.c + dc;
			int r = cell.r + dr;
			if (c < 0 || r < 0 || c >= field->cols || r >= field->rows) continue;
			if (field->revealedCells[r * field->cols + c] == FLAGGED) surroundingFlags++;
		}
	}
	bool hitMine = false;
	if (surroundingFlags == field->cells[cell.r * field->cols + cell.c]) {
		for (int dc = -1; dc <= 1; dc++) {
			for (int dr = -1; dr <= 1; dr++) {
				if ((dc == 0 && dr == 0)) continue;
				int c = cell.c + dc;
				int r = cell.r + dr;
				if (c < 0 || r < 0 || c >= field->cols || r >= field->rows) continue;
				if (field->cells[r * field->cols + c] == MINE &&
						field->revealedCells[r * field->cols + c] != FLAGGED) hitMine = true;
				if (field->revealedCells[r * field->cols + c] != FLAGGED) revealCell(field, (Cell) { c, r });
			}
		}
	}
	return hitMine;
}