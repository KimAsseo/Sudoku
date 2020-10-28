#ifndef CELL_H_
#define CELL_H_

/**
 * Type used to store information on each cell on the board.
 * Contains whether the cell is fixed (can't be changed by the player) or erroneous,
 * its value and temporary value, and a chosen bit.
 */

typedef struct cellStruct{
	int fixed;
	int val;
	int error;
	int chosen;
	int temp;
} Cell;

#endif  /* CELL_H_ */
