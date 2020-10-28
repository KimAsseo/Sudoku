#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdio.h>
#include <stdlib.h>

/*Error massages and various messages used by the commands*/

void failure_error(char* calloc, char* function);

/*START OF PARSER ERRORS.*/
void error_allowed_only_edit_and_solve(char* function);
void error_too_many_parameters(char* function, int num);
void error_allowed_only_solve(char* function);
void error_wrong_range(char *function, char * index, int len);
void error_not_an_int(char *function, char * index);
void error_too_little_params(char *function, int num);
void error_too_many_params(char *function, int num);
void error_mark_errors_range();
void error_input_too_long();
void error_unrecognized_command();
void error_edit_too_many_params();
void error_edit_wrong_param_format();
void error_not_a_legal_float();
void error_x_in_generate(int len, int num_of_filled);
void error_allowed_only_edit(char* function);
/*END OF PARSER ERRORS.*/

/*START OF GAME.C ERRORS.*/
void error_opening_file(char* file_path);
void error_not_enough_empty_cells_for_generate();
void error_no_sqrt();
void error_file_ints_wrong_range();
void error_generate_fail();
void error_board_is_erroneous(char *function);
/*END OF GAME.C ERRORS.*/

/*START OF MOVES_LIST.C ERRORS.*/
void error_no_moves_to_undo();
/*END OF MOVES_LIST.C ERRORS.*/

/*START OF GAME_AUX.C ERRORS.*/
void error_file_pointer();
/*END OF GAME_AUX.C ERRORS.*/

/*SAVE MESSAGES*/
void error_erroneous_board_in_editmode();
void saving_board_in_edit_mode();
void validate_fail_in_edit_mode();
void error_save_unsolveable_board();

/*SET MESSAGES*/
void check_for_game_over_message();
void game_over_message();
void game_not_over_yet_message();
void error_fixed_cell_in_solve_mode();

/*VALIDATE MESSAGES*/
void validate_fail_message();
void validate_success_message();

/*HINT MESSAGES*/
void error_fixed_cell(int x, int y);
void error_contains_a_val(int x, int y);
void error_unsolvable_board();
void print_hint(int x, int y, int val);


#endif /* ERRORS_H_ */
