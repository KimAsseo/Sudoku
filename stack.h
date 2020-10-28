#ifndef STACK_H_
#define STACK_H_

/*
 * stack summary:
 *
 * contains stack implementation
 * in ech node:
 * x - for row
 * y - for col
 * value - for value in cell[row][col]
 *
 * includes the functions:
 * - push
 * - pop
 */

#include "Errors.h"

typedef struct node
{
    int x;
    int y;
    int value;
    struct node *next;
} stack_node;

stack_node *push(stack_node *top, int x, int y, int value);

stack_node *pop(stack_node *top, int *x, int *y, int *value);

#endif /* STACK_H_ */
