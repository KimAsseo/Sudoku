#ifndef MOVES_LIST_H_
#define MOVES_LIST_H_

#include "cell_change_move.h"
#include "Cell.h"
#include "moves_list.h"

/*this structure is the element of the doubly linked list of moves.*/
typedef struct move{
/*	move_type is CELLMOVE or BOARDMOVE.*/
	int move_type;
/*	head_indicator = HEAS for list's head, CURR otherwise.*/
	int head_indicator;
/*	if move_type is CELLMOVE, this is the cell's value prior to the move.*/
	int orig_cell_val;
/*	if move_type is CELLMOVE, this is the cell's value after the move.*/
	int new_cell_val;
/*	if move_type is CELLMOVE, this is the cell's row coordinate.*/
	int set_row;
/*	if move_type is CELLMOVE, this is the cell's col coordinate.*/
	int set_col;
/*	if move_type is BOARDMOVE, this is the board's status prior to the move.*/
	Cell **saved_board;

	struct move *next;
	struct move *prev;
	struct cell_change *cell_change_head;
	int move_undone;
} move;

#endif /* MOVES_LIST_H_ */
