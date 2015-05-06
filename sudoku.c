/*
 * Sudoku solver
 *
   gcc -Wall -std=c11 -O3 -osudoku sudoku.c
 *
 * Author: Martin Uecker <uecker@eecs.berkeley.edu>
 */

#include <stdbool.h>
#include <stdio.h>

// #define ALL

static void print_board(const int board[9][9])
{
	static const char* line = "+-------+-------+-------+";

	for (int i = 0; i < 9; i++) {

		if (0 == i % 3)
			puts(line);

		for (int j = 0; j < 9; j++) {

			if (0 == j % 3)
				printf("| ");

			printf("%d ", board[i][j]);
		}

		puts("|");
	}

	puts(line);
}

static int block(int i, int j)
{
	return (i / 3) * 3 + (j / 3);
}

static bool sudoku_r(int board[9][9], int rows[9], int cols[9], int blks[9], int i, int j)
{
	goto in;

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
in:			if (0 == board[i][j])
				goto out;
#ifdef ALL
	puts("--");
	print_board(board);
	return false;
#else
	return true;
#endif

out:
	for (int n = 1; n <= 9; n++) {
	
		int k = block(i, j);

		if ((rows[i] | cols[j] | blks[k]) & (1 << n))
			continue;

		rows[i] |= (1 << n);
		cols[j] |= (1 << n);
		blks[k] |= (1 << n);
	
		board[i][j] = n;

		if (sudoku_r(board, rows, cols, blks, i, j))
			return true;

		rows[i] &= ~(1 << n);
		cols[j] &= ~(1 << n);
		blks[k] &= ~(1 << n);

		board[i][j] = 0;
	}

	return false;
}

bool sudoku(int board[9][9])
{
	int rows[9] = { 0 };
	int cols[9] = { 0 };
	int blks[9] = { 0 };

	for (int i = 0; i < 9; i++) {

		for (int j = 0; j < 9; j++) {

			int k = block(i, j);

			if (0 != board[i][j]) {

				rows[i] |= (1 << board[i][j]);
				cols[j] |= (1 << board[i][j]);
				blks[k] |= (1 << board[i][j]);
			}
		}
	}

	return sudoku_r(board, rows, cols, blks, 0, 0);
}

int main(void)
{
	int board[9][9] = {

		{ 5, 3, 0,  0, 7, 0,  0, 0, 0 },
		{ 6, 0, 0,  1, 9, 5,  0, 0, 0 },
		{ 0, 9, 8,  0, 0, 0,  0, 6, 0 },

		{ 8, 0, 0,  0, 6, 0,  0, 0, 3 },
		{ 4, 0, 0,  8, 0, 3,  0, 0, 1 },
		{ 7, 0, 0,  0, 2, 0,  0, 0, 6 },

		{ 0, 6, 0,  0, 0, 0,  2, 8, 0 },
		{ 0, 0, 0,  4, 1, 9,  0, 0, 5 },
		{ 0, 0, 0,  0, 8, 0,  0, 7, 9 },
	};

	sudoku(board);
#ifndef ALL
	print_board(board);
#endif
	return 0;	
}

