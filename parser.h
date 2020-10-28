/**
 * Parser summary:
 *
 * Used to interpret commands from the user.
 * Extracts the command word (1st word) and any parameters if needed.
 *
 * parse 		- Interprets commands from stdin
 */

#ifndef PARSER_2_H_
#define PARSER_2_H_

#include <stdio.h>
#include <stdlib.h>

/** Updates command[] according to command from user.
 * command received | command[0] :
 * 		exit		|	   -1
 * 		reset		|		1
 * 		set			|		2
 * 		hint		|		3
 * 		validate	|		4
 * 		INVALID 	|		5
 * 		NULL		|		6
 *		guess_hint	|		7
 *		save		|		8
 *		undo		|		9
 *		redo		|		10
 *		solve		|		11
 *		edit		|		12
 *		marks_errors|		13
 *		print_board	|		14
 *		guess		|		15
 *		generate	|		16
 *		num_solutions|		17
 *		autofill	|		18
 *
 * 		if the command receives extra parameters they are stored in command[1-3]
 * 		for GUESS - threshold is updated.
 * 		if the command or one of its parameters is invalid -
 * 		command[0] is set to INVALID*/

char* parse(Game *game, int *command, char *str, float* threshold);


#endif /* PARSER_2_H_ */
