#include "Errors.h"
#include<stdio.h>
#include<stdlib.h>
#include "settings.h"

void failure_error(char* calloc, char* function){
	printf("Error: %s has failed in %s\n", calloc, function);
	return;
}

/*START OF PARSER ERRORS.*/
void error_allowed_only_edit_and_solve(char* function){
	printf("Error: the command %s is only available in modes edit and solve\n", function);
	return;
}

void error_too_many_parameters(char* function, int num){
	printf("Error: the command %s was given too many parameters\n", function);
	printf("Please provide the function's name and  %d parameters:\n", num);
	return;
}

void error_allowed_only_solve(char* function){
	printf("Error: the command %s is only available in solve mode\n", function);
	return;
}

void error_wrong_range(char *function, char * index, int len){
	printf("Error: %s parameter of function %s is out of range [1,%d]\n", index, function, len);
	return;
}

void error_not_an_int(char *function, char * index){
	printf("Error: %s parameter of function %s is not a positive integer\n", index, function);
	return;
}

void error_too_little_params(char *function, int num){
	printf("Error: the function %s received too little parameters\n", function);
	printf("It should receive %d parameters\n", num);
	return;
}

void error_too_many_params(char *function, int num){
	printf("Error: the function %s received too many parameters\n", function);
	printf("It should receive %d parameters\n", num);
	return;
}

void error_mark_errors_range(){
	printf("Error: the parameter of function mark_errors should be 0 or 1\n");
	return;
}

void error_input_too_long(){
	printf("Error: input is too long\n");
	printf("No more than 256 characters should be provided in a single line\n");
	return;
}

void error_unrecognized_command(){
	printf("Error: your input is not recognized as a valid command for this program\n");
	return;
}

void error_edit_too_many_params(){
	printf("Error: the function edit received too many parameters\n");
	printf("It should receive 0 or 1 parameters.\n");
	return;
}

void error_edit_wrong_param_format(){
	printf("Error: parameter for edit function is incorrectly formatted.\n");
	printf("Parameter format should be [file path]\n");
	return;
}

void error_not_a_legal_float(){
	printf("Error: the input for this function should be a float in range [0,1]\n");
}

void error_x_in_generate(int len, int num_of_filled){
	int max = (len*len) - num_of_filled;
	printf("Error: the first parameter of function generate should be integer in range 1 to %d\n", max);
}

void error_allowed_only_edit(char* function){
	printf("Error: the command %s is only available in edit mode\n", function);
	return;
}

/*END OF PARSER ERRORS.*/


/*START OF GAME.C ERRORS.*/
void error_opening_file(char* file_path){
	printf("Error: the file in path %s could not be opened.\n", file_path);
	return;
}

void error_not_enough_empty_cells_for_generate(){
	printf("Error: not enough empty cells for generate\n");
	printf("Generate command was not completed./n");
}

/*is this in use?*/
void error_no_sqrt(){
	printf("error: could not load sudoku board from file\n");
	printf("the total number of integers does not have an integer square root\n");
	return;
}

void error_file_ints_wrong_range(){
	printf("Error: could not load sudoku board from file\n");
	printf("Integers must be at range 1 to sqrt of number of elements\n");
	return;
}

void error_generate_fail(){
	printf("Error: generate function failed\n");
}
/*END OF GAME.C ERRORS.*/

/*START OF MOVES_LIST.C ERRORS.*/
void error_no_moves_to_undo(){
	printf("Error: there are no moves to undo\n");
	return;
}
/*END OF MOVES_LIST.C ERRORS.*/


/*START OF GAME_AUX.C ERRORS.*/
void error_file_pointer(){
	printf("Error: file could not be opened.\n");
	return;
}

void error_save_unsolveable_board(){
	printf("Error: the board is erroneous or unsolvable.\n");
	printf("Therefore, it cannot be saved in EDIT mode.\n");
	return;
}
/*END OF GAME_AUX.C ERRORS.*/

void error_board_is_erroneous(char *function){
	printf("Error: the board is erroneous\n");
	printf("Therefore, the function %s can't be completed.\n", function);
	return;
}

/*SAVE MESSAGES*/
void error_erroneous_board_in_editmode(){
	printf("Error: save command failed\n");
	printf("Erroneous boards can't be saved in edit mode.\n");
	return;
}
void saving_board_in_edit_mode(){
	printf("Board is solvable\n");
	printf("Therefore it can be saved in edit mode\n");
}
void validate_fail_in_edit_mode(){
	printf("Error: save command failed\n");
	printf("Unsolvable boards can't be saved in edit mode.\n");
	return;
}

/*SET MESSAGES*/
void check_for_game_over_message(){
	printf("The last empty cell was just filled.\n"
			"Let's see if you did a good job and validate the board.\n");
}
void game_over_message(){
	printf("Congratulations! The puzzle was solved successfully\n");
}
void game_not_over_yet_message(){
	printf("Sorry, it seems you have a few mistakes.\n"
			"feel free to try and correct them.\n");
}
void error_fixed_cell_in_solve_mode(){
	printf("Error, fixed cells can't be updated in solve mode.\n");
}

/*VALIDATE MESSAGES*/
/*Error message for validate in Game*/
void validate_fail_message(){
	printf("Validation failed: board is unsolvable\n");
}
/*Message for validate in Game*/
void validate_success_message(){
	printf("Validation passed: board is solvable\n");
}

/*HINT MESSAGES*/
void error_fixed_cell(int x, int y){
	printf("Error: cell (%d, %d) is fixed\n", x, y);
	return;
}
void error_contains_a_val(int x, int y){
	printf("Error: cell (%d, %d) already contains a value\n", x, y);
	return;
}
void error_unsolvable_board(){
	printf("Error, the board is unsolvable\n");
}

void print_hint(int x, int y, int val){
	printf("The value of cell (%d, %d) is %d\n", x, y, val);
	return;
}

