/*general functions related to boards*/
#include "general_funcs.h"
#include <math.h>
#include "settings.h"

int valid_block(Game *game, int num, int row, int col, int board);
int valid_row(Game *game, int num, int row, int col, int board);
int valid_column(Game *game, int num, int row, int col, int board);

/*returns len of board - n*m*/
int get_len(Game *game){
	int n = game->n;
	int m = game->m;
	return n*m;
}

void copy_curr_to_temp(Game *game){
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int row, col, num;

	for (row = 0; row < len; row++){
		for (col = 0; col < len; col++){
			num = get_cell_field(game, row, col, VAL);
			set_cell_field(game, row, col, TEMP, num);
		}
	}
}
void copy_temp_to_curr(Game *game){
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int row, col, temp_num, curr_num, filled = 0;

	for (row = 0; row < len; row++){
		for (col = 0; col < len; col++){
			temp_num = get_cell_field(game, row, col, TEMP);
			curr_num = get_cell_field(game, row, col, VAL);
			if (curr_num == 0 && temp_num != 0){
				set_cell_field(game, row, col, VAL, temp_num);
				filled++;
			}
		}
	}
	update_num_of_filled(game, filled);
}


/*calculates the num of block cell[row][col] is in, starting from 0 */
int calc_block(int row, int col, int n, int m){
	int block;
	int upper_left_row = row_index_of_block(row, n);
	int upper_left_col = col_index_of_block(col, m);
/*	block = (upper_left_row/n)*m + (upper_left_col/m);*/
	block = upper_left_row + upper_left_col/m;
	return block;
}

/*returns the indexes of the left upper corner of the block cell[row][col] is in*/
int row_index_of_block(int row, int n){
	return floor(row/n)*n;
}
int col_index_of_block(int col, int m){
	return floor(col/m)*m;
}

/*returns the indexes of the left upper corner the block is in*/
int block_to_row(int block, int n){
	return floor(block/n)*n;
}
int block_to_col(int block, int n, int m){
	return (block%n)*m;
}

/*Returns TRUE if the num has no identical neighbors in its block.
 *FALSE otherwise.
 *
 * @param
 * game - The game
 * num - The number to check
 * row - The row's index on the board
 * col - The column's index on the board
 * board - Marks which board is being checked - current game or solution
 *
 * @return
 * TRUE if valid
 * FALSE otherwise
 */
int valid_block(Game *game, int num, int row, int col, int board){
	int n = game->n;
	int m = game->m;
	int k,l;
	/*calculate the coordinates of the upper left cell in the block.*/
	int row_index = row_index_of_block(row, n);
	int column_index = col_index_of_block(col, m);

	for(k = row_index; k<row_index+(n); k++){
		for(l = column_index; l<column_index+(m); l++){
			if (!((k==row) && (l == col))){
				if (board == TEMP){
					if(get_cell_field(game, k, l, TEMP) == num){
						return FALSE;
					}
				}
				else{
					if(get_cell_field(game, k, l, VAL) == num){
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}

/*Returns TRUE if the num has no identical neighbors in its row.
 *FALSE otherwise.
 *
 * @param
 * game - The game
 * num - The number to check
 * row - The row's index on the board
 * board - Marks which board is being checked - current game or solution
 *
 * @return
 * TRUE if valid
 * FALSE otherwise
 */
int valid_row(Game *game, int num, int row, int col, int board){
	int len = get_len(game);
	int k;
	for(k=0; k<len; k++){
		if (k != col){
			if (board == TEMP){
				if(get_cell_field(game, row, k, TEMP) == num){
					return FALSE;
				}
			}
			else{
				if (get_cell_field(game, row, k, VAL) == num){
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/*Returns TRUE if the num has no identical neighbors in its column.
 *FALSE otherwise.
 *
 * @param
 * game - The game
 * num - The number to check
 * row - The row's index on the board
 * col - The column's index on the board
 * board - Marks which board is being checked - current game or solution
 *
 * @return
 * TRUE if valid
 * FALSE otherwise
 */
int valid_column(Game *game, int num, int row, int col, int board){
	int len = get_len(game);
	int k;
	for(k=0; k<len;k++){
		if (k != row){
			if (board == TEMP){
				if(get_cell_field(game, k, col, TEMP) == num){
					return FALSE;
				}
			}
			else{
				if (get_cell_field(game, k, col, VAL) == num){
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/*checks valid row, col and block*/
int valid_num(Game *game, int num, int row, int col, int board){
	 if(!valid_row(game, num, row, col, board)|| !valid_column(game, num, row, col, board) ||
			!valid_block(game, num, row, col, board)){
		 return FALSE;
	}
	return TRUE;
}


int get_cell_field(Game *game, int row, int col, int field){

	switch (field){
	case FIXED:
		return game->curr_board[row][col].fixed;
	case VAL:
		return game->curr_board[row][col].val;
	case ERRONEOUS:
		return game->curr_board[row][col].error;
	case CHOSEN:
		return game->curr_board[row][col].chosen;
	case TEMP:
		return game->curr_board[row][col].temp;
	}
	return 0;
}

void set_cell_field(Game *game, int row, int col, int field, int num){

	switch (field){
	case FIXED:
		game->curr_board[row][col].fixed = num;
		break;
	case VAL:
		game->curr_board[row][col].val = num;
		break;
	case ERRONEOUS:
		game->curr_board[row][col].error = num;
		break;
	case CHOSEN:
		game->curr_board[row][col].chosen = num;
		break;
	case TEMP:
		game->curr_board[row][col].temp = num;
	}
}

int get_game_field(Game *game, int field){

	switch(field){
		case NUM_OF_FILLED:
			return game->num_of_filled;
		case GAME_MODE:
			return game->game_mode;
		case MARK_ERRORS_FIELD:
			return game->mark_errors;
	}
	return 0;
}

void set_game_field(Game *game, int field, int val){

	switch(field){
		case NUM_OF_FILLED:
			game->num_of_filled = val;
			break;
		case GAME_MODE:
			game->game_mode = val;
			break;
		case MARK_ERRORS_FIELD:
			game->mark_errors = val;
			break;
	}
}

void update_num_of_filled(Game *game, int add){
	int num = get_game_field(game, NUM_OF_FILLED);
	set_game_field(game, NUM_OF_FILLED, num + add);
}

/* updating num_of_filled field according to the change made
 * if the num before change was 0 and now in [1, len] - num++
 * if it was in [1, len] and now 0 - num--*/
void update_num_of_filled_for_moves(Game *game, int before_change, int after_change){
	if ((before_change == 0) && (after_change != 0)){
		update_num_of_filled(game, 1);
	}
	if ((before_change != 0) && (after_change == 0)){
		update_num_of_filled(game, -1);
	}
}
