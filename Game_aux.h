#ifndef GAME_AUX_H_
#define GAME_AUX_H_

/**
 * Game_aux summary:
 *
 * Auxiliary functions used by Game. Supports the following functions:
 *
 * create_board 				- Creates new board
 * print_board 					- Prints board
 * save 						- Saves board to file
 * load_game					- Loads game from file, for SAVE and EDIT
 * get_atoi_length				- Gets length of number
 * compare_z_to_num				- Compares z to num already in cell
 * set_value					- Sets value in cell
 * check_for_game_over			- Checks if the game is over
 * update_mark_for_errors_board	- Updates errors for all the board
 * update_mark_for_errors_cell  - Updates errors resulting in one cell change
 * print_title					- Prints game title
 * exhaustive_backtracking		- Counts number of solutions
 * board_has_errors				- Checks if the board has any marked errors
 *
 * MOVES LIST FUNCTIONS
 * append						- Appends node to moves list
 * redo_board_move				- Redo board move - autofill, guess, generate
 * undo_board_move				- Undo board move - autofill, guess, generate
 * destroy_moves_list_starting_from - Destroys moves list from specified node
 * destroy_cell_change_list		- Destroy cell change list
 * clear_redo_list				- Clears redo list
 */

/*creates board with block size n*m*/
int create_board(Game *game, int n, int m);

/*prints board*/
void print_board(Game *game);

/*saves game to file_path*/
int save(Game *game, char* file_path);

/*SOLVE AND EDIT FUNCTIONS*/
/*loads board from file_path*/
int load_game(Game *game, char* file_path);
/*returns the length of atoi_val*/
size_t get_atoi_length(int atoi_val);

/*SET FUNCTIONS */

/*Returns TRUE if CURRENT[x][y] == z, for set_command.
 * Checks if the value already in CURRENT[x][y] is z.
 *
 * @param
 * game - The game
 * x_index, y_index - Index for row, col in CURRENT respectively
 * z - The number the player wishes to set
 * @return
 * TRUE if z == CURRENT[x][y],
 * FALSE otherwise*/
int compare_z_to_num(Game *game, int x, int y, int z);

/*Sets num in CURRENT[x][y], updates num_of_filled.
 * If num is 0 - num_of_filled will be lowered by 1.
 * Otherwise - raised by 1.
 *
 * @param
 * val - Number to set
 * x,y - index for row, column respectively in CURRENT.
 * game - The game*/
void set_value(Game *game, int x, int y, int old_val, int val);

/*Check if the game is over.
 * if all cells are filled after a set move, validates the board.
 * if valid - game over and go to INITMODE
 * else - keep playing*/
int check_for_game_over(Game *game);

/*START OF AUTOFILL FUNCTIONS*/

/*zeros all cells in digits_array*/
void zero_all_digits_array(int *digits_array, int len);
/*if there is only one legal value, the cell is updated to that value. otherwise, the cell is kept at 0. */
/*returns legal value for cell[row][col]*/
int update_to_legal_val(Game *game, int row, int col, int* digits_array, int command);

/*updates valid nums for cell[row][col] s.t. 1 means the number is invalid, 0 - valid*/
int update_digits_array(Game *game,int row, int col, int *digits_array, int command);

/*END OF AUTOFILL FUNCTIONS*/

/*START OF GENERATE FUNCTIONS*/

/*randomly fills x empty cells in the board*/
int generate_fill_x(Game *game, int x);
/*randomly chooses y cells to copy back to board from temp.
 * updates relevant changes to BOARDMOVE*/
void generate_empty_y(Game *game, int y);
/*clears the specified field from all cells*/
void clear_board_field(Game *game, int field);
/*END OF GENERATE FUNCTIONS*/

/* START OF MARK ERRORS FUNCTIONS*/

/* goes over the entire board and updates all errors
 * for loading a game in solve or edit*/
void update_mark_for_errors_board(Game *game);
/*marks all errors regarding the val in cell[row][col]
 * updates error field for all the relevant cells in row, col, block*/
void update_mark_for_errors_cell(Game *game, int row, int col, int old_val, int val);
/* END OF MARK ERRORS FUNCTIONS*/

/* title printed to user, appears AT START OF ENTIRE PROGRAM*/
void print_title();

/* backtracking for num_of_solutions
 * counts number of solutions to board
 * updates error if an ERROR occurs*/
unsigned long exhaustive_backtracking(Game *game, int *error);

/* checks the board for any cell with the error field TRUE
 * returns TRUE if the board has errors, FALSE otherwise*/
int board_has_errors(Game *game);

/*MOVES LIST FUNCTIONS*/

/* adds a move to moves_list
 * for SETMOVE - saves the change made for cell[row][col]
 * for BOARDMOVE - adds a cell_change_move node with all the cell changes*/
void append(Game *game, struct move** head_ref, int set_val, int row, int col, int move_type, cell_change** change_head, int new_val);
void print_cell_change_list(Game* game, cell_change* cell_change_head);
/*undo or redo a board move - generate, autofill, guess*/
void redo_board_move(Game *game, int first_move_undone);
void undo_board_move(Game *game);

/*this function deletes all move elements in the moves list, starting from
 * the given element. this is used:
 * 1) for deleting an entire list starting from head, when exiting.
 * 2) for deleting the redo part of the list, when the user makes a
 * set/autofill/generate/guess move. */
void destroy_moves_list_starting_from(Game* game, move* start_move, int head_or_curr);
void destroy_cell_change_list(cell_change* head);

/*clears redo list before one of the board changing commands - set, generate, autofill, guess*/
void clear_redo_list(Game *game);

#endif /* GAME_AUX_H_ */

