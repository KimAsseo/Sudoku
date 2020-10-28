#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPBufferset.h"
#include "Game.h"
#include "Errors.h"
#include "Game_aux.h"
#include "parser.h"
#include "settings.h"
#include "general_funcs.h"

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
 * 		*/

/*CHANGE FILE_PATH.*/

void parse_set(Game *game, int* command, int mode);
void parse_hint_or_generate(Game* game, int* command, int cmd_num, char* cmd_name, int mode);
void parse_guess_hint(Game *game, int* command, int mode);
void parse_save(int* command, char** file_path, int mode);
void parse_solve(int* command, char** file_path);
void parse_mark_errors(int* command, int mode);
void parse_zero_params(int* command, int cmd_num, char* cmd_name, int mode);
void parse_edit(int* command, char** file_path, int* file_provided);
float parse_guess(int* command, int mode);
float check_if_legal_float(char* string);
int calc_factor(int b_len);

char* parse(Game *game, int *command, char *str, float* threshold){
	char* file_path;
	char* token;
	int file_provided = 0;
	int mode = get_game_field(game, GAME_MODE);
	SP_BUFF_SET();

	if(fgets(str,CHARMAX, stdin) == NULL){
		printf("command[0] = EXIT\n");
		command[0] = EXIT;
	}
	/*check if input is too long*/
	else if(str[strlen(str) - 1] != '\n'){
		error_input_too_long();
		command[0] = INVALID;
	}
	else{
		/*the first token in str.*/
		token = strtok(str,DELIM);

		if(token == NULL){
			command[0] = 6;
		}
		else if(strcmp(token, "reset") == 0){
			parse_zero_params(command, RESET, token, mode);
			return STRING;
		}
		else if(strcmp(token, "exit") == 0){
			parse_zero_params(command, EXIT, token, mode);
			return STRING;
		}
		else if(strcmp(token, "validate") == 0){
			parse_zero_params(command, VALIDATE, token, mode);
			return STRING;
		}
		else if(strcmp(token, "set") == 0){
			parse_set(game, command, mode);
			return STRING;
		}

		/*I reversed the order of x and y from targil 3.*/
		else if((strcmp(token,"hint") == 0)){
			parse_hint_or_generate(game, command, HINT, token, mode);
			return STRING;
		}
		else if(strcmp(token, "guess_hint") == 0){
			parse_guess_hint(game, command, mode);
		}
		else if(strcmp(token, "save") == 0){
			parse_save(command, &file_path, mode);
			return file_path;
		}
		else if(strcmp(token, "undo") == 0){
			parse_zero_params(command, UNDO, token, mode);
		}
		else if(strcmp(token, "redo") == 0){
			parse_zero_params(command, REDO, token, mode);
		}
		/*IN SOLVE METHOD< REMEMBER TO CHECK VALID FILE PATH.*/
		else if(strcmp(token, "solve") == 0){
			/*add handling of parse_solve.*/
			parse_solve(command, &file_path);
			if (command[0] == SOLVE){
				printf("file path in parse: %s\n", file_path);
				return file_path;
			}
			else{
				return STRING;
			}
		}
		else if(strcmp(token, "edit") == 0){
			parse_edit(command, &file_path, &file_provided);
			if(file_provided){
				return file_path;
			}
			else{
				return STRING;
			}
	}
		else if(strcmp(token, "mark_errors") == 0){
			parse_mark_errors(command, mode);
		}
		else if(strcmp(token, "print_board") == 0){
			parse_zero_params(command, PRINT_BOARD, token, mode);
		}
		else if(strcmp(token, "guess") == 0){
			*threshold = parse_guess(command, mode);
		}
		else if(strcmp(token, "generate") == 0){
			parse_hint_or_generate(game, command, GENERATE, token, mode);
		}
		else if(strcmp(token, "num_solutions") == 0){
			parse_zero_params(command, NUM_SOLUTIONS, token, mode);
		}
		else if(strcmp(token, "autofill") == 0){
			parse_zero_params(command, AUTOFILL, token, mode);
		}
		else{
			error_unrecognized_command();
			command[0] = INVALID;
		}
	}
	return STRING;
}

void parse_set(Game *game, int* command, int mode){
	char* strtok_val;
	char* x_strtok_val;
	char* y_strtok_val;
	char* z_strtok_val;
	int x,y,z;
	char x_tok, y_tok, z_tok;
	int len = get_len(game);
	int invalid_bit = 0;
	int counter = 0;

	if((mode != EDITMODE) && (mode != SOLVEMODE)){
		error_allowed_only_edit_and_solve("set");
		command[0] = INVALID;
		return;
	}
	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			x_strtok_val = strtok_val;
			x = atoi(strtok_val);
			x_tok = x_strtok_val[0];
		}
		else if(counter == 2){
			y_strtok_val = strtok_val;
			y = atoi(strtok_val);
			y_tok = y_strtok_val[0];
		}
		else if(counter == 3){
			z_strtok_val = strtok_val;
			z = atoi(strtok_val);
			z_tok = z_strtok_val[0];
		}
	}
	if(counter < 3){
		error_too_little_params("set", 3);
		invalid_bit = 1;
	}
	else if (counter > 3){
		error_too_many_params("set", 3);
		invalid_bit = 1;
	}
	else{
		if((x==0 && x_tok != '0') ||(strlen(x_strtok_val) != get_atoi_length(x))){
		error_not_an_int("set", "first");
		invalid_bit = 1;
		}
		else{
			if(x <= 0 || x> len){
				error_wrong_range("set", "first", len);
				invalid_bit = 1;
			}
		}
		if((y== 0 && y_tok != '0') || (strlen(y_strtok_val) != get_atoi_length(y))){
			error_not_an_int("set", "second");
			invalid_bit = 1;
		}
		else{
			if(y<= 0 || y> len){
				error_wrong_range("set", "second", len);
				invalid_bit = 1;
			}
		}
		if((z == 0 && z_tok != '0')|| (strlen(z_strtok_val) != get_atoi_length(z))){
			error_not_an_int("set", "third");
			invalid_bit = 1;
		}
		else{
			if(z< 0 || z > len){
				error_wrong_range("set", "third", len);
				invalid_bit = 1;
			}
		}
	}

/*	if all the parameters are valid.*/
	if(!invalid_bit){
		command[0] = SET;
		command[1] = x;
		command[2] = y;
		command[3] = z;
	}
/*	else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

/*void parse_zero_params(Game *game, int* command, int cmd_num, char* cmd_name){*/
void parse_hint_or_generate(Game* game, int* command, int cmd_num, char* cmd_name, int mode){
	char* strtok_val;
	char* x_strtok_val = "";
	char* y_strtok_val = "";
	int x = 0,y = 0;
	char x_tok, y_tok;
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int invalid_bit = 0;
	int counter = 0;

	if(cmd_num == HINT){
		if(mode != SOLVEMODE){
			error_allowed_only_solve(cmd_name);
			command[0] = INVALID;
			return;
		}
	}
	else if(cmd_num == GENERATE){
		if(mode != EDITMODE){
			error_allowed_only_edit(cmd_name);
			command[0] = INVALID;
			return;
		}
	}
	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			x_strtok_val = strtok_val;
			x = atoi(strtok_val);
			x_tok = x_strtok_val[0];
		}
		else if(counter == 2){
			y_strtok_val = strtok_val;
			y = atoi(strtok_val);
			y_tok = y_strtok_val[0];
		}
	}
	if(counter < 2){
		error_too_little_params(cmd_name, 2);
		invalid_bit = 1;
		command[0] = INVALID;
		return;
	}
	else if (counter > 2){
		error_too_many_params(cmd_name, 2);
		invalid_bit = 1;
		command[0] = INVALID;
		return;
	}
	else{
		if((x==0 && x_tok != '0') ||(strlen(x_strtok_val) != get_atoi_length(x))){
			error_not_an_int(cmd_name, "first");
			invalid_bit = 1;
			command[0] = INVALID;
			return;
		}
		else{
			if(cmd_num == HINT){
				if(x <= 0 || x> len){
					error_wrong_range("hint", "first", len);
					invalid_bit = 1;
					command[0] = INVALID;
					return;
				}
			}
			else if(cmd_num == GENERATE){
				if(x < 0 || x > (len*len - game->num_of_filled)){
					error_x_in_generate(len, game->num_of_filled);
					command[0] = INVALID;
					return;
				}
			}
		}
		if((y== 0 && y_tok != '0') || (strlen(y_strtok_val) != get_atoi_length(y))){
			error_not_an_int(cmd_name, "second");
			invalid_bit = 1;
			command[0] = INVALID;
			return;
		}
		else{
			if(cmd_num == HINT){
				if(y<= 0 || y> len){
					error_wrong_range(cmd_name, "second", len);
					invalid_bit = 1;
					command[0] = INVALID;
					return;
				}
			}
			else if(cmd_num == GENERATE){
				if(y > (len*len)){
					error_wrong_range(cmd_name, "second", len);
					command[0] = INVALID;
					return;
				}
			}
		}
	}

	/*if all the parameters are valid.*/
	if(!invalid_bit){
		command[0] = cmd_num;
		command[1] = x;
		command[2] = y;
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

void parse_guess_hint(Game *game, int* command, int mode){
	char* strtok_val;
	char* x_strtok_val;
	char* y_strtok_val;
	int x,y;
	char x_tok, y_tok;
	int len = get_len(game);
	int invalid_bit = 0;
	int counter = 0;

	if(mode != SOLVEMODE){
		error_allowed_only_solve("guess_hint");
		command[0] = INVALID;
		return;
	}

	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			x_strtok_val = strtok_val;
			x = atoi(strtok_val);
			x_tok = x_strtok_val[0];
		}
		else if(counter == 2){
			y_strtok_val = strtok_val;
			y = atoi(strtok_val);
			y_tok = y_strtok_val[0];
		}
	}
	if(counter < 2){
		error_too_little_params("guess_hint", 2);
		invalid_bit = 1;
	}
	else if (counter > 2){
		error_too_many_params("guess_hint", 2);
		invalid_bit = 1;
	}
	else{
		if((x==0 && x_tok != '0') ||(strlen(x_strtok_val) != get_atoi_length(x))){
		error_not_an_int("guess_hint", "first");
		invalid_bit = 1;
		}
		else{
			if(x <= 0 || x> len){
				error_wrong_range("guess_hint", "first", len);
				invalid_bit = 1;
			}
		}
		if((y== 0 && y_tok != '0') || (strlen(y_strtok_val) != get_atoi_length(y))){
			error_not_an_int("guess_hint", "second");
			invalid_bit = 1;
		}
		else{
			if(y<= 0 || y> len){
				error_wrong_range("guess_hint", "second", len);
				invalid_bit = 1;
			}
		}
	}

	/*if all the parameters are valid.*/
	if(!invalid_bit){
		command[0] = GUESS_HINT;
		command[1] = x;
		command[2] = y;
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

void parse_save(int* command, char** file_path, int mode){
	int invalid_bit = 0;
	int counter = 0;
	char* strtok_val;

	if(mode == INITMODE){
		error_allowed_only_edit_and_solve("save");
		command[0] = INVALID;
		return;
	}

	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			*file_path = strtok_val;
		}
	}
	if(counter < 1){
		error_too_little_params("save", 1);
		invalid_bit = 1;
	}
	else if (counter > 1){
		error_too_many_params("save", 1);
		invalid_bit = 1;
	}
	/*if all the parameters are valid.*/
	if(!invalid_bit){
		command[0] = SAVE;
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

void parse_solve(int* command, char** file_path){
	char* strtok_val;
	int invalid_bit = 0;
	int counter = 0;
	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			*file_path = strtok_val;
		}
	}
	if(counter < 1){
		error_too_little_params("solve", 1);
		invalid_bit = 1;
	}
	else if (counter > 1){
		error_too_many_params("solve", 1);
		invalid_bit = 1;
	}
	/*if all the parameters are valid.*/
	if(!invalid_bit){
		command[0] = SOLVE;
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

void parse_edit(int* command, char** file_path, int* file_provided){
	int path_provided = 0;
	int invalid_bit = 0;
	int counter = 0;
	char* strtok_val;
/*	int i, len;*/
	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			*file_path = strtok_val;
			path_provided = 1;
		}
	}
	if (counter >= 2){
		error_edit_too_many_params();
		invalid_bit = 1;
		command[0] = INVALID;
		return;
	}

	/*if all the parameters are valid.*/
	if(!invalid_bit){
		command[0] = EDIT;
		/*indicator whether or not the path was provided.*/
		if(path_provided){
			command[1] = 1;
			*file_provided = 1;
		}
		else{
			printf("no file provided\n");
			command[1] = 0;
			*file_provided = 0;
		}
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

void parse_mark_errors(int* command, int mode){
	char* strtok_val;
	char* x_strtok_val;
	int x;
	char x_tok;
	int invalid_bit = 0;
	int counter = 0;

	if(mode != SOLVEMODE){
		error_allowed_only_solve("mark_errors");
		invalid_bit = 1;
	}
	else{
		while((strtok_val = strtok(NULL,DELIM)) != NULL){
			counter++;
			if(counter == 1){
				x_strtok_val = strtok_val;
				x = atoi(strtok_val);
				x_tok = x_strtok_val[0];
			}
		}
		if(counter < 1){
			error_too_little_params("mark_errors", 1);
			invalid_bit = 1;
		}
		else if (counter > 1){
			error_too_many_params("mark_errors", 1);
			invalid_bit = 1;
		}
		else{
			if((x==0 && x_tok != '0') ||(strlen(x_strtok_val) != get_atoi_length(x))){
				error_mark_errors_range();
				invalid_bit = 1;
			}
			else if((x!= 0) && (x!= 1)){
				error_mark_errors_range();
				invalid_bit = 1;
			}
		}
	}
	/*if all the parameters are valid.*/
	if(!invalid_bit){
		printf("valid\n");
		command[0] = MARK_ERRORS;
		command[1] = x;
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
}

void parse_zero_params(int* command, int cmd_num, char* cmd_name, int mode){
	/*checking mode permissions for the specific function called.*/
	if(cmd_num == AUTOFILL){
		if(mode != SOLVEMODE){
			error_allowed_only_solve(cmd_name);
			command[0] = INVALID;
			return;
		}
	}
	else if(cmd_num == NUM_SOLUTIONS || cmd_num == PRINT_BOARD || cmd_num == REDO
			|| cmd_num == UNDO || cmd_num == VALIDATE || cmd_num == RESET){
		if(mode == INITMODE){
			error_allowed_only_edit_and_solve(cmd_name);
			command[0] = INVALID;
			return;
		}
	}
	if((strtok(NULL, DELIM) != NULL)){
		error_too_many_parameters(cmd_name, 0);
		command[0] = INVALID;
		return;
	}
	else{
		command[0] = cmd_num;
	}
}

/*returns the legal float value if parameters were passed correctly; returns -1.0 otherwise.*/
float parse_guess(int* command, int mode){
	int invalid_bit = 0;
	int counter = 0;
	float pos_threshold, threshold = 0.0;
	char* strtok_val;

	if (mode != SOLVEMODE){
		error_allowed_only_solve("guess");
		invalid_bit = 1;
	}

	while((strtok_val = strtok(NULL,DELIM)) != NULL){
		counter++;
		if(counter == 1){
			/*HANDLE FLOAT TESTS.*/
			pos_threshold = check_if_legal_float(strtok_val);
			if (pos_threshold != (float)(ERROR)){
				threshold = pos_threshold;
			}
			else{
				invalid_bit = 1;
			}
		}
	}
	if(counter < 1){
		error_too_little_params("guess", 1);
		invalid_bit = 1;
	}
	else if (counter > 1){
		error_too_many_params("guess", 1);
		invalid_bit = 1;
	}
	if(!invalid_bit){
		command[0] = GUESS;
		return threshold;
	}
	/*else - the command is invalid.*/
	else{
		command[0] = INVALID;
	}
	return (float)(0);
}

/*checks if the input for the Guess function is a valid float in range [0,1];*/
float check_if_legal_float(char* string){
	int a, a_len, i, b, b_len;
	float factor, ret_val;
	if(strcmp(string, "0") == 0){
		return (float)0;
	}
	if(strcmp(string, "1") == 0){
		return (float)1;
	}
	a = atoi(string);
	if((a==0 && string[0] != '0') || (a != 0 && a != 1)){
		printf("error here\n");
		error_not_a_legal_float();
		return (float)(ERROR);
	}
	else{
		a_len = get_atoi_length(a);
		if(string[a_len] != '.'){
			error_not_a_legal_float();
			return (float)(ERROR);
		}
		else{
			for(i = 0; i < a_len+1; i++){
				string++;
			}
			b = atoi(string);
			if(b==0 && string[0] != '0'){
				error_not_a_legal_float();
				return (float)(ERROR);
			}
			else{
				if(a == 1){
					if(b != 0){
						error_not_a_legal_float();
						return (float)(ERROR);
					}
					else{
						return (float)1;
					}
				}
				else{
					b_len = get_atoi_length(b);
					factor = calc_factor(b_len);
					ret_val = b/factor;
					return ret_val;
				}
			}
		}
	}
	return (float)(ERROR);
}

/*calculates factor needed for check_if_legal_float function.*/
int calc_factor(int b_len){
	int ret_val = 10;
	int i;
	for(i = 0; i<b_len-1; i++){
		ret_val = ret_val*10;
	}
	return ret_val;
}
