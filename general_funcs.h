/*
 * general_funcs:
 * general function used all across the modules.
 *  - getting/setting game and cell fields
 *  - calculating indices for blocks
 *  - general board functions
 *
 */

#ifndef GENERAL_FUNCS_H_
#define GENERAL_FUNCS_H_

#include "Game.h"

/*returns len of board - n*m*/
int get_len(Game *game);

/*copies CURRENT to TEMP*/
void copy_curr_to_temp(Game *game);
/*copies TEMP to CURRENT and updates num_of_filled*/
void copy_temp_to_curr(Game *game);

/*calculates the num of block cell[row][col] is in, starting from 0 */
int calc_block(int row, int col, int n, int m);

/*returns the indexes of the left upper corner of the block cell[row][col] is in*/
int row_index_of_block(int row, int m);
int col_index_of_block(int col, int n);

/*returns the indexes of the left upper corner the block is in*/
int block_to_row(int block, int n);
int block_to_col(int block, int n, int m);

/**
 * Returns TRUE if the num has no identical neighbors in its block, row, or column.
 * FALSE otherwise.
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
int valid_num(Game *game, int num, int row, int col, int board);

/* get - returns TRUE if the field in CURR[row][col] is TRUE, FALSE otherwise
 * set - sets num in CURR[row][col] in field*/
int get_cell_field(Game *game, int row, int col, int field);
void set_cell_field(Game *game, int row, int col, int field, int num);

/*gets or sets the game fields - num_of_filled, game_mode, and mark_errors*/
int get_game_field(Game *game, int field);
void set_game_field(Game *game, int field, int val);

/*updates num_of_filled field with +1 or -1*/
void update_num_of_filled(Game *game, int plus_or_minus);
void update_num_of_filled_for_moves(Game *game, int before_change, int after_change);

#endif /* GENERAL_FUNCS_H_ */
