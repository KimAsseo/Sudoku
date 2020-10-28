#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "SPBufferset.h"
#include "Game_aux.h"
#include "settings.h"
#include <time.h>

/*
 * Sudoku Project
 *
 *
 * The game allows the following commands:
 * solve x					- Starts a puzzle in Solve mode with the name "x", in any mode
 * edit [x]					- Starts a puzzle in Edit mode, with the name "x" or no parameter to an empty 9x9 board, in any mode
 * mark_errors x			- Sets mark errors setting to x - 0/1 (default is 1), in Solve mode
 * print_board				- Prints the board, in Solve and Edit modes
 * set x y z				- Sets the value of cell (x, y) to z, in Solve and Edit modes
 * validate					- Validates the board using ILP, in Solve and Edit modes
 * guess x					- Guesses a solution to board using LP, with threshold x, in Solve mode
 * generate x y				- Generates a puzzle by filling x empty cells, running ILP, and clearing all but y cells, in Edit mode
 * undo						- Undo previous move done by the user, in Solve and Edit modes
 * redo						- Redo previous move done by the user, in Solve and Edit modes
 * save x					- Saves current game to file with path x, in Solve and Edit modes
 * hint x y					- Gives a hint by showing the solution to cell (x, y), in Solve mode
 * guess_hint x y			- Show a guess for cell (x, y), in Solve mode
 * num_solutions			- Prints the number of solutions for the board, in Solve and Edit modes
 * autofill					- Fills all cell which contain a single legal value, in Solve mode
 * reset					- Undo all moves, reverting the board to its original state, in Solve and Edit modes
 * exit						- Terminates the program
 * */



int main(){
	Game game_pointer;
	SP_BUFF_SET();
	srand(time(0));
	print_title();
	game_loop(&game_pointer, N,M);
	return EXIT;
}
