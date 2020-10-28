#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "Game.h"
#include "Game_aux.h"
#include "SPBufferset.h"
#include "parser.h"
#include "Errors.h"
#include "cell_change_move.h"
#include "settings.h"
#include "general_funcs.h"
#include "Solver.h"

#define MAX_NUM_OF_TRIES 1000

int initialize_new_game(Game *game, int n,int m);
int run_game(Game *game);
int run_command(Game *game, int* command, char* parse_str, float threshold);
void print_board_for_certain_commands(Game *game, int *command);

int solve(Game *game, char* file_path);
int edit(Game *game, char* file_path, int load_board);
void mark_errors_command(Game *game, int i);
int set_command(Game *game, int row, int col, int val);

/*FUNCTIONS USING GUROBI*/
int validate(Game *game, int command);
int hint_commands(Game *game, int x, int y, int command);
int guess_command(Game *game, float x);

int generate(Game *game, int x, int y);
int num_of_solutions(Game *game);

/*MOVES LIST FUNCTIONS*/
int undo(Game* game, int print_bit);
int redo(Game* game);
int reset(Game *game);

void exit_game(Game *game);
void destroy_boards(Game *game);


/*update this board from targil 3. I deleted intialization of num of
 * fixed cells for now.*/
int initialize_new_game(Game *game, int n,int m){
	game->n = n;
	game->m = m;
	game->head = NULL;
	game->curr_move = NULL;
	game->game_mode = INITMODE;
	game->mark_errors = 1;
	/*check for malloc errors while creating boards*/
	if (create_board(game, n,m) == EXIT){
		return EXIT;
	}
	return 0;
}

/* initializes a new game and runs it until an ERROR or EXIT*/
void game_loop(Game* game, int n, int m){
	int game_val;
	initialize_new_game(game, n, m);
	game_val = run_game(game);
	if(game_val == ERROR){
		return;
	}
	return;
}

/*Receives commands from the user and runs them until ERROR or EXIT.
 * Prints the board after relevant commands*/
int run_game(Game *game){
	int command[4] = {0, 0, 0, 0};
	char str[CHARMAX];
	char* parse_str;
	float threshold;
	int ret_val;
	while(command[0] != EXIT){
		printf("\nPlease enter a new command:\n");
		parse_str = parse(game, command, str, &threshold);
		ret_val = run_command(game, command, parse_str, threshold);
		if (ret_val == ERROR){
			return ERROR;
		}
		if (ret_val != FALSE){
			print_board_for_certain_commands(game, command);
		}
	}
	return EXIT;
}

/*runs a single command.
 * @param
 *  - game - the game
 *  - command - command array for command name and necessary parameters
 *  - parse_str - command string from the user
 *  - threshold - for guess function
 *  @return ret_val
 *  - TRUE if succeeded, FALSE if not
 *  - ERROR if an ERROR occurred
 *  - or a number of filled cells for autofill */
int run_command(Game *game, int* command, char* parse_str, float threshold){
	int x,y,z;
	int ret_val = 0;
	switch(command[0]){
		case SOLVE:
			ret_val = solve(game, parse_str);
			break;
		case EDIT:
			if(strcmp(parse_str, STRING) == 0){
				ret_val = edit(game, NULL, FALSE);
			}else{
				ret_val = edit(game, parse_str, TRUE);
			}
			break;
		case MARK_ERRORS:
			mark_errors_command(game, command[1]);
			break;
		case PRINT_BOARD:
			print_board(game);
			break;
		case SET:
			x = command[1];
			y = command[2];
			z = command[3];
			ret_val = set_command(game,x,y,z);
			break;
		case VALIDATE:
			ret_val = validate(game, VALIDATE);
			break;
		case GUESS:
			ret_val = guess_command(game, threshold);
			break;
		case GENERATE:
			x = command[1];
			y = command[2];
			ret_val = generate(game, x, y);
			break;
		case UNDO:
			ret_val = undo(game, TRUE);
			break;
		case REDO:
			ret_val = redo(game);
			break;
		case SAVE:
			ret_val = save(game, parse_str);
			break;
		case HINT:
			x = command[1];
			y = command[2];
			ret_val = hint_commands(game, x, y, HINT);
			break;
		case GUESS_HINT:
			x = command[1];
			y = command[2];
			ret_val = hint_commands(game, x, y, GUESS_HINT);
			break;
		case NUM_SOLUTIONS:
			ret_val = num_of_solutions(game);
			break;
		case AUTOFILL:
			ret_val = autofill(game, AUTOFILL);
			break;
		case RESET:
			ret_val = reset(game);
			break;
		case EXIT:
			exit_game(game);
			break;
		case INVALID:
			/*do nothing*/
			break;
		default:
			/*HANDLE/CHANGE.*/
			break;
	}
	/*HANDLE/CHANGE*/
	return ret_val;
}

/*prints the board only after one of these commands worked successfully*/
void print_board_for_certain_commands(Game *game, int *command){
	if (command[0] == EDIT || command[0] == SOLVE){
		print_board(game);
		return;
	}
	if (command[0] == AUTOFILL || command[0] == GENERATE || command[0] == GUESS){
		print_board(game);
		return;
	}
	if (command[0] == UNDO || command[0] == REDO || command[0] == RESET){
		print_board(game);
		return;
	}
	return;
}

/*loads board from file_path, and changes mode to Solve
 * redo list is cleared*/
int solve(Game *game, char* file_path){
	int error_stat;

	error_stat = load_game(game, file_path);
	/*if load_game failed for some reason.*/
	if(error_stat == FALSE){
		return FALSE;
	}
	if(error_stat == ERROR){
		return ERROR;
	}
	set_game_field(game, GAME_MODE, SOLVEMODE);
	return TRUE;
}

/*loads board from file_path, and changes mode to Edit
 * if file_path not provided (!load_board), an empty 9x9 board is created.
 * mark error setting is set to 1
 * redo list is cleared*/
int edit(Game *game, char* file_path, int load_board){
	int error_stat;
	/*if no file name was supplied by the user,
	 * create a new 9X9 empty board). */
	/*KILL PREVIOUS GAME*/
	if(!load_board){
		if (game->curr_board != NULL){
			destroy_boards(game);
			destroy_moves_list_starting_from(game, game->head, HEAD);
		}
		initialize_new_game(game, N,M);
	}
	else{
		/*if load_game failed for some reason.*/
		error_stat = load_game(game, file_path);
		if(error_stat == FALSE){
			return FALSE;
		}
		if(error_stat == ERROR){
			return ERROR;
		}
	}
	set_game_field(game, GAME_MODE, EDITMODE);
	mark_errors_command(game, 1);
	return TRUE;
}

/*sets mark_error field to i.
 * i can be 0 or 1*/
void mark_errors_command(Game *game, int i){
	set_game_field(game, MARK_ERRORS_FIELD, i);
}

/* sets val in cell(row, col)
 * doesn't update cell if it's fixed
 * allows erroneous input and marks all the errors it causes
 * if mark_errors == 1 or we're in edit mode the errors will  be shown
 * if solve mode - if the last cell was filled checks to see if the game is over
 * if successful - game mode is set to init
 * otherwise the game continues
 * @param:
 *  - game       the game
 *  - row, col   cell indices received from user
 *  - val		 number to be set in cell[row][col] */
int set_command(Game *game, int row, int col, int val){
	int mode, fixed, game_over;
	int x = row-1;
	int y = col-1;
	int old_val = get_cell_field(game, x, y, VAL);

	mode = get_game_field(game, GAME_MODE);
	fixed = get_cell_field(game, x, y, FIXED);

	/*can't update fixed cells in solve mode*/
	if ((fixed == TRUE) && (mode == SOLVEMODE)){
		error_fixed_cell_in_solve_mode();
		return FALSE;
	}

	/*no need to check or set if z already in cell*/
	if (compare_z_to_num(game, x, y, val) == FALSE){
		set_value(game, x, y, old_val, val);
		update_mark_for_errors_cell(game, x, y, old_val, val);

	}
	clear_redo_list(game);
	append(game, &(game->head), old_val, x, y, SETMOVE, NULL, val);

	print_board(game);
	/*check if board is filled and is a valid solution*/
	if (mode == SOLVEMODE){
		game_over = check_for_game_over(game);
		if (game_over == ERROR){
			return ERROR;
		}
	}
	return TRUE;
}

/* checks for errors.
 * if none, uses ILP with Gurobi to solve board
 * prints success/fail message.
 * returns TRUE if succeeded, FALSE otherwise,
 * or ERROR if Gurobi failed */
int validate(Game *game, int command){
	int solution;

	if(board_has_errors(game) == TRUE){
		if (command == VALIDATE){
			error_board_is_erroneous("validate");
		}
		else if (command == SAVE){
			error_erroneous_board_in_editmode();
		}
		return FALSE;
	}
	solution = solve_with_gurobi(game, ILP, VALIDATE, 0, 0, 0.0);
	switch (solution){
	case GRB_OPTIMAL:
		if (command == VALIDATE){
			validate_success_message();
		}
		else if(command == SAVE){
			saving_board_in_edit_mode();
		}
		return TRUE;
	case ERROR:
		failure_error("solve_with_gurobi", "validate");
		return ERROR;
	default:
		if (command == VALIDATE){
			validate_fail_message();
		}
		else if (command == SAVE){
			validate_fail_in_edit_mode();
		}
		return FALSE;
	}
}

/*uses LP to fill all cells with score > x
 * returns FALSE if board has errors or is unsolvable
 * TRUE if successful, or ERROR*/
int guess_command(Game *game, float x){
	int solution;

	if(board_has_errors(game) == TRUE){
		error_board_is_erroneous("guess");
		return FALSE;
	}
	solution = solve_with_gurobi(game, LP, GUESS, 0, 0, x);
	/*handles solution*/
	switch (solution){
	case GRB_OPTIMAL:
		printf("guess was successful\n");
		return TRUE;
	case ERROR:
		failure_error("solve_with_gurobi", "guess");
		return ERROR;
	default:
		error_unsolvable_board();
		return FALSE;
	}
}

/*generates board from x cells and after solving it clearing all but y random cells*/
/*works on TEMP, ignores non-zero cells in CURR
 * fills in TEMP using ILP
 * empties all but y empty, unrelated to pre exsisting cells
 * returns FALSE if board has errors or is unsolvable*/
int generate(Game *game, int x, int y){
	int i, gen_val, solution;

	if (board_has_errors(game) == TRUE){
		error_board_is_erroneous("generate");
		return FALSE;
	}
	for (i = 0; i < MAX_NUM_OF_TRIES; i++){
		copy_curr_to_temp(game);
		gen_val = generate_fill_x(game, x);
		if (gen_val == ERROR){
			return ERROR;
		}
		else if(gen_val != TRUE){
			continue;
		}
		solution = solve_with_gurobi(game, ILP, GENERATE, 0, 0, 0.0);
		if(solution == ERROR){
			return ERROR;
		}
		else if (solution != GRB_OPTIMAL ){
			continue;
		}
		clear_redo_list(game);
		clear_board_field(game, CHOSEN);
		generate_empty_y(game, y);
		clear_board_field(game, CHOSEN);
		return TRUE;
	}
	error_generate_fail();
	return FALSE;
}

/*used for HINT or GUESS_HINT commands
 * @param
 *  - game - The game
 *  - x, y - cell indexes from user
 *  - command - HINT or GUESS_HINT
 *  if HINT prints a hint according to ILP solution
 *  if GUESS_HINT prints all legal values of cell and their scores
 *  @returns TRUE if successful, FALSE if doesn't, ERROR if gurobi had an ERROR*/
int hint_commands(Game *game, int x, int y, int command){
	int solution, val;
	/*check for errors, fixed cell, or a filled cell*/
	if(board_has_errors(game) == TRUE){
		error_board_is_erroneous("hint");
		return FALSE;
	}
	if (get_cell_field(game, x-1, y-1, FIXED) == TRUE){
		error_fixed_cell(x, y);
		return FALSE;
	}
	if (get_cell_field(game, x-1, y-1, VAL) != 0){
		error_contains_a_val(x, y);
		return FALSE;
	}
	/*calls gurobi with ILP for HINT or LP for GUESS_HINT*/
	if (command == HINT){
		solution = solve_with_gurobi(game, ILP, HINT, x, y, 0.0);
	}
	else{
		solution = solve_with_gurobi(game, LP, GUESS_HINT, x, y, 0.0);
	}
	/*handles solution*/
	switch (solution){
	case GRB_OPTIMAL:
		if (command == HINT){
			val = get_cell_field(game, x-1, y-1, TEMP);
			print_hint(x, y, val);
		}
		return TRUE;
	case ERROR:
		if (command == HINT){
			failure_error("solve_with_gurobi", "hint");
		}
		else{
			failure_error("solve_with_gurobi", "guess_hint");
		}
		return ERROR;
	default:
		error_unsolvable_board();
		return FALSE;
	}
}

/* prints number of solutions
 * check if the board has known errors - TO BE WRITTEN
 * uses backtracking.
 *  @param: game - the game
 *  @return: TRUE, FALSE or ERROR*/
int num_of_solutions(Game *game){
	unsigned long num_of_sols = 0;
	int error = 0;

	if (board_has_errors(game) == TRUE){
		error_board_is_erroneous("num_of_solutions");
		return FALSE;
	}
	num_of_sols = exhaustive_backtracking(game, &error);
	if (error != ERROR){
		printf("number of solutions is: %lu\n", num_of_sols);
		return TRUE;
	}
	return ERROR;
}

/*autofills all cells that has a single valid number.
 * returns number of filled cells.
 * is used by the autofill command and by SOLVER to solve the board.
 * the command dictates whether CURR board or TEMP board are filled*/
int autofill (Game *game, int command){
	int len = get_len(game);
	int legal_val;
	int i,j;
	int orig_val;
	int num_of_filled = 0;
	cell_change *cell_change_head = NULL;

	int* digits_array = (int*)calloc(len, sizeof(int));
	if (digits_array == NULL){
		failure_error("malloc", "autofill");
		return ERROR;
	}
	if (command == AUTOFILL){
		copy_curr_to_temp(game);
	}

	orig_val = 0;
	for(i=0 ;i<len ;i++){
		for(j=0 ; j<len; j++){
			if(get_cell_field(game, i, j, TEMP) == 0){
				zero_all_digits_array(digits_array, len);
				legal_val = update_to_legal_val(game, i, j, digits_array, command);
				if(legal_val != 0){
					if (command == AUTOFILL){
						cell_change_head = add(cell_change_head, i,j, orig_val, legal_val);
					}
					num_of_filled++;
				}
			}
		}
	}
	if (command == AUTOFILL){
		clear_redo_list(game);
		append(game, &(game->head), 0, 0, 0, BOARDMOVE, &cell_change_head, 0);

		copy_temp_to_curr(game);
	}
	free(digits_array);
	return num_of_filled;
}


/*MOVES LIST FUNCTIONS*/

/*this function undoes the last move from the moves list
 * undoes either a set move - set, or board move - generate, autofill, guess.
 * prints the changes made to the user if printbit == 1*/
int undo(Game* game, int print_bit){
	int row, col, before_change, after_change;
/*	if the list is empty.*/
	if((game->head == NULL) || (game->curr_move == NULL)){
		error_no_moves_to_undo();
		return FALSE;
	}
/*	if the list is not empty, but its first move has already been undone.*/
	if((game->curr_move == game->head)&& (game->curr_move->move_undone == 1)){
		error_no_moves_to_undo();
		return FALSE;
	}
	else{
/*		if move type is SETMOVE.*/
		game->curr_move->move_undone = 1;
	}
	if(game->curr_move->move_type == SETMOVE){
		row = game->curr_move->set_row;
		col = game->curr_move->set_col;
		before_change = get_cell_field(game, row, col, VAL);
		after_change = game->curr_move->orig_cell_val;
		if(print_bit){
			printf("set move undone: cell (%d, %d) was changed from %d back to %d\n", row+1,
				col+1, before_change, after_change);
		}
		set_cell_field(game, row, col, VAL, after_change);
		if(game->curr_move != game->head){
			game->curr_move = game->curr_move->prev;
		}
		update_mark_for_errors_cell(game, row, col, before_change, after_change);
		update_num_of_filled_for_moves(game, before_change, after_change);
	}
/*	    else - move type is BOARDMOVE.*/
	else{
		undo_board_move(game);
		if(game->curr_move != game->head){
		game->curr_move = game->curr_move->prev;
		}
	}
	return TRUE;
}

/*this function redoes the last move from the moves list
 * redoes either a set move - set, or board move - generate, autofill, guess.
 * prints the changes made to the user*/
int redo(Game* game){
/*	CHECK IF THERE ARE NO MOVES TO REDO.*/
/*	if the move is a SETMOVE.*/
	int first_move_undone;
	int row, col, before_change, after_change;
	if(game->curr_move-> next == NULL && (game->curr_move->head_indicator == 0 ||
			game->curr_move->move_undone == 0)){
		printf("Error: no more moves to redo in the current moves list\n");
		return FALSE;
	}
	if((game->curr_move == game->head)&&(game->curr_move->move_undone == 1)){
		first_move_undone = 1;
		if(game->curr_move->move_type == SETMOVE){
			row = game->curr_move->set_row;
			col = game->curr_move->set_col;
			before_change = get_cell_field(game, row, col, VAL);
			after_change = game->curr_move->new_cell_val;

	    	set_cell_field(game, row, col, VAL, after_change);
	    	update_mark_for_errors_cell(game, row, col, before_change, after_change);
			update_num_of_filled_for_moves(game, before_change, after_change);
			printf("set move redone: cell (%d, %d) was changed from %d back to %d\n", row+1,
								col+1, before_change, after_change);
			game->curr_move->move_undone = 0;
		}else{
			redo_board_move(game, first_move_undone);
		}
	}
/*	if the move is a BOARDMOVE.*/
	else{
		first_move_undone = 0;
		if(game->curr_move->next->move_type == SETMOVE){
			row = game->curr_move->next->set_row;
			col = game->curr_move->next->set_col;
			before_change = game->curr_board[row][col].val;
			after_change = game->curr_move->next->new_cell_val;

			update_mark_for_errors_cell(game, row, col, before_change, after_change);
			update_num_of_filled_for_moves(game, before_change, after_change);
			printf("set move redone: cell (%d, %d) was changed from %d back to %d\n", row+1,
					col+1, before_change, after_change);
			set_cell_field(game, row, col, VAL, after_change);
			game->curr_move = game->curr_move->next;
		}
		else{
			redo_board_move(game, first_move_undone);
			game->curr_move = game->curr_move->next;
		}
	}
	return TRUE;
}

/*this function reverts the board to its original loaded state.
 * the moves list is not cleared, but its pointer is moved to
 * the head of the list.
 */
int reset(Game *game){
	move *ptr = game->curr_move;
	while(ptr != game-> head){
		undo(game, FALSE);
		ptr = ptr->prev;
	}
	undo(game, FALSE);
	return TRUE;
}

/* destroys all the dynamically allocated memory used by the game*/
void exit_game(Game *game){
	printf("EXITS_GAME\n");
	/*step 1 - free all boards of this game.*/
	destroy_boards(game);
	/*step 2 - free all mallocs of moves list.*/
	destroy_moves_list_starting_from(game, game->head, HEAD);
}

/*used in step 1 of exit - destroys all the dynamically allocated
 * memory of the boards and their cells. */
void destroy_boards(Game *game){
	int len = get_len(game);
	int i;

	for (i=len-1; i>=0; i--){
		if (game->curr_board[i]){
			free (game->curr_board[i]);
		}
	}
	free (game->curr_board);
	return;
}
