#include <stdio.h>
#include <stdlib.h>
#include "cell_change_move.h"
#include "SPBufferset.h"
#include "Game.h"
#include "Errors.h"

/* cell_change summary:
 * linked list structure for board moves
 * this list is contained in moves_list as a node.
 */

void init(cell_change** head){
	*head = NULL;
}

cell_change* add(cell_change* cell_node, int row, int col, int orig_val, int new_val){
	cell_change* temp = (cell_change*)malloc(sizeof(cell_change));
	if(temp == NULL){
		failure_error("malloc", "add");
		return cell_node;
	}
	temp->row = row;
	temp->col = col;
	temp->orig_cell_val = orig_val;
	temp->new_cell_val = new_val;
	temp->next = cell_node;
	return temp;
}

/*NOT IN USE*/
void remove_node(cell_change* head){
	cell_change* temp = (cell_change*)malloc(sizeof(cell_change));
	if(temp == NULL){
		failure_error("malloc", "remove_node");
		return;
	}
	temp = head->next;
	head->next = head->next->next;
	free(temp);
}

void free_list(cell_change* head){
	cell_change* tmpPtr = head;
	cell_change *followPtr;
	while(tmpPtr != NULL){
		followPtr = tmpPtr;
		tmpPtr = tmpPtr->next;
		free(followPtr);
	}
	return;
}
