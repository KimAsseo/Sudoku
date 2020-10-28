#include <stdio.h>
#include "stack.h"
#include "Errors.h"

stack_node *push(stack_node *top, int x, int y, int value) {
    struct node *ptr = malloc(sizeof(struct node));
    if (ptr == NULL) {
    	failure_error("malloc", "push");
    	return NULL;
    }
    ptr->x = x;
    ptr->y = y;
    ptr->value = value;
    ptr->next = top;
    return ptr;
}

stack_node *pop(stack_node *top, int *x, int *y, int *value) {
	stack_node *temp;
	int pop_x = -1;
	int pop_y = -1;
	int pop_value = -1;

    if (top != NULL) {
    	temp = top;
		pop_x = top->x;
		pop_y = top->y;
		pop_value = top->value;
		top = top->next;
		free(temp);
    }

	*x = pop_x;
	*y = pop_y;
	*value = pop_value;
    return top;
}

