/**
 * Game summary:
 *
 * Used to play a Sudoku game.
 * Initializes a new game and allows operations in the game.
 *
 * game_loop				- Initializes and runs game.
 * autofill					- Fills all obvious cells in the board
 * validate					- validates the board
 * destroy_boards			- frees the board
 *
 * after every cell changing in CURR board
 * - the board is checked for new/fixed errors
 *   and updates all the relevant cells' error field
 * - num_of_filled field is also updated
 * - moves_list is updated
 *
 **/

#ifndef GAME_H_
#define GAME_H_

#include "settings.h"
#include "Cell.h"
#include "moves_list.h"
#include "Game.h"

typedef struct game{
	int num_of_filled;
	Cell **curr_board;
	int n;
	int m;
	move *head;
	move *curr_move;
	int game_mode;
	int mark_errors;
} Game;

/* initializes a new game and runs it until an ERROR or EXIT*/
void game_loop(Game* game, int n, int m);

/*fills all cells that has a single valid number.
 * returns number of filled cell.*/
int autofill (Game *game, int command);

/* checks for errors.
 * if none, uses ILP with Gurobi to solve board
 * prints success/fail message.
 * returns TRUE if succeeded, FALSE otherwise,
 * or ERROR if Gurobi failed */
int validate(Game *game, int command);

/*destroys all the dynamically allocated memory of the boards and their cells.
 * used before exiting or when loading a new board*/
void destroy_boards(Game *game);


/*void game_mock_loop(Game *game, int n, int m);*/




#endif /* GAME_H_ */
