#ifndef CELL_CHANGE_MOVE_H_
#define CELL_CHANGE_MOVE_H_

/* cell_change summary:
 * linked list structure for board moves
 * this list is contained in moves_list as a node.
 */

typedef struct cell_change{
	int orig_cell_val;
	int new_cell_val;
	int row;
	int col;
	struct cell_change* next;
	struct cell_change* prev;
} cell_change;

void init(cell_change** head);
cell_change* add(cell_change* cell_node, int row, int col, int orig_val, int new_val);

/*NOT IN USE*/
void remove_node(cell_change* head);
void free_list(cell_change* head);

#endif /* CELL_CHANGE_MOVE_H_ */
