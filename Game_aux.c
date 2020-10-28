#include<stdio.h>
#include<stdlib.h>
#include "SPBufferset.h"
#include "Game.h"
#include "Game_aux.h"
#include "settings.h"
#include "stack.h"
#include "general_funcs.h"
#include <math.h>
#include <string.h>
#include "moves_list.h"
#include "parser.h"

/*FOR PRINT_BOARD*/
void print_cell(Game *game, int i, int j);
void series_of_dashes(int n);

/*FOR LOAD*/
int read_line(char *buff, FILE *fp);
int legal_integer(char* token);
int fixed_integer(char* token);
int get_row(int index, int len);
int get_col(int index, int len);

/*FOR SET*/
int mark_row_errors(Game *game, int row, int col, int val, int len, int old_val);
int mark_col_errors(Game *game, int row, int col, int val, int len, int old_val);
int mark_block_errors(Game *game, int row, int col, int val, int old_val);

void print_autofill_message(int row, int col, int num);
int get_nth_val_of_arr(int *arr, int len, int n);

/*creates board with block size n*m*/
int create_board(Game *game, int n, int m){

	int len = n*m;
	int i,j;
	Cell **Board = (Cell**)malloc(len*sizeof(Cell*));
	if (Board == NULL){
		failure_error("malloc", "create_board");
		return EXIT;
	}
	for(i = 0; i< len;i++){
		Board[i] = (Cell*)malloc(len*sizeof(Cell));
		if (Board[i] == NULL){
			failure_error("malloc", "create_board");
			return EXIT;
		}
	}

	/*initializes val and fixed as 0*/
	for(i = 0; i<len; i++){
		for(j=0; j<len;j++){
			Board[i][j].val = 0;
			Board[i][j].fixed = 0;
			Board[i][j].chosen = 0;
			Board[i][j].error = 0;
			Board[i][j].temp = 0;
		}
	}
	/*connects pointers*/
	game->curr_board = Board;
	game->num_of_filled = 0;
	return 0;
}

/*START OF PRINT FUNCTIONS*/
void print_board(Game *game){
	int n = game->n;
	int m = game->m;
	int chars_in_row = 4*n*m+n+1;
	int i,j;
	int i_index, j_index;
	int num_of_rows = (n+1)*m+1;
	int num_of_cols = (m+1)*n+1;

	for (i=0; i< num_of_rows; i++){
		if (i % (n+1) == 0){
			series_of_dashes(chars_in_row);
		}
		else{
			for (j=0; j<num_of_cols ; j++){
				if (j % (m+1) == 0){
					printf("|");
				}
				else{
					i_index = i - floor(i/(n+1))-1;
					j_index = j - floor(j/(m+1))-1;
					print_cell(game, i_index, j_index);
				}
			}
			printf("\n");
		}
	}
}

void print_cell(Game *game, int i, int j){
	int val = get_cell_field(game, i, j, VAL);
	int fixed = get_cell_field(game, i, j, FIXED);
	int error = get_cell_field(game, i, j, ERRONEOUS);

	printf(" ");
	if (val == 0){
		printf("  ");
	}
	else{
		printf("%2d", val);
	}
	if (fixed){
		printf(".");
	}
	else if(error){
		if (game->mark_errors == 1){
			printf("*");
		}
		else{
			printf(" ");
		}
	}
	else{
		printf(" ");
	}
}

void series_of_dashes(int n){
	int i;
	for (i=0; i<n; i++){
		printf("-");
	}
	printf("\n");
}
/*END OF PRINT FUNCTIONS*/

/* loads board from file
 * returns TRUE if successful, FALSE otherwise, or ERROR.*/
int load_game(Game *game, char* file_path){
	int counter = 0;
	char* token;
	int row, col;
	int n = 0, m = 0, i, len = 0, num;
	int num_of_filled = 0;
	int max_num = 0;
	char line[BUFFER_SIZE];
	Cell** Board = NULL;
	FILE *file = fopen(file_path, "r");

	if(file == NULL){
		error_file_pointer();
		return FALSE;
	}
	while(read_line(line,file)){
		if(strcmp(line, "")== 0){
			continue;
		}
		token = strtok(line, DELIM);
		while((token != NULL) && (counter < 2)){
			counter++;
			if((num = legal_integer(token)) <0){
				printf("error: n,m not correctly supplied.\n");
				return FALSE;
			}
			else{
				/*our n, m are switched from the instructions*/
				if(counter == 1){
					n = num;
				}
				if(counter == 2){
					m = num;
					len = n*m;
					max_num = len*len;
					Board = (Cell**)malloc(len*sizeof(Cell*));
					if (Board == NULL){
						failure_error("malloc", "load_game");
						return ERROR;
					}
					for(i = 0; i<len; i++){
						Board[i] = (Cell*)malloc(len*sizeof(Cell));
						if (Board[i] == NULL){
							failure_error("malloc", "load_game");
							return ERROR;
						}
					}
				}
			}
			token = strtok(NULL,DELIM);
		}
		while(token != NULL){
			counter ++;
			if (counter <= max_num + 2){
				row = get_row(counter-2, len);
				col = get_col(counter-2, len);
				if((num = legal_integer(token)) >= 0){
					Board[row][col].val = num;
					Board[row][col].fixed = 0;
					if (num != 0){
						num_of_filled++;
					}
				}
				else if((num = fixed_integer(token))){
					Board[row][col].val = num;
					Board[row][col].fixed = 1;
					if (num != 0){
						num_of_filled++;
					}
				}
				else{
					printf("Error: illegal board for loading.\n");
					return FALSE;
				}
				token = strtok(NULL,DELIM);
			}
			else{
				counter = max_num + 6;
				break;
			}
		}
	}
	counter = counter - 2;
	if(counter != max_num){
		printf("Error: invalid number of elements\n");
		/*DESTORY BOARD!!!!*/
		for (i=0; i<len; i++){
			if (Board[i]){
				free(Board[i]);
			}
		}
		return FALSE;
	}
	/*destroy previous game*/
	if (game->curr_board != NULL){
		destroy_boards(game);
		destroy_moves_list_starting_from(game, game->head, HEAD);
	}
	/*connect new game*/
	game->curr_board = Board;
	game->n = n;
	game->m = m;
	update_num_of_filled(game, num_of_filled);
	update_mark_for_errors_board(game);
	return TRUE;
}

int read_line(char *buff, FILE *fp){
	char *tmp;
	buff[0] = '\0';
	buff[BUFFER_SIZE - 1] = '\0';  /* mark end of buffer */

	if (fgets(buff, BUFFER_SIZE, fp) == NULL) {
		*buff = '\0';                   /* EOF */
		return FALSE;
	}
	else {
		/* remove newline */
		if ((tmp = strrchr(buff, '\n')) != NULL) {
			*tmp = '\0';
		}
	}
	return TRUE;
}

/*returns the positive integer value, or -1 if it's not an integer.*/
int legal_integer(char* token){
	int num;
	num = atoi(token);
	if((num == 0 && token[0] != '0') || (strlen(token) != get_atoi_length(num)) || num<0){
		return -1;
	}
	return num;
}

size_t get_atoi_length(int atoi_val){
	size_t counter = 0;
	if(atoi_val == 0){
		return 1;
	}
	while(atoi_val > 0){
		atoi_val = atoi_val/10;
		counter++;
	}
	return counter;
}

/*returns the positive integer value, or -1 if it's not an integer.*/
int fixed_integer(char* token){
	int len;
	int num;
	if((len = strlen(token)) < 2){
		return -1;
	}
	else{
		token[strlen(token)-1] = '\0';
		if((num = legal_integer(token))){
			return num;
		}
		else{
			return -1;
		}
	}
}

/*calculate row by index, for example index in range 1-81.*/
int get_row(int index, int len){
	if(index%len == 0){
		return (index/len) -1;
	}
	else{
		return index/len;
	}
}

/*calculate row by index, for example index in range 1-81.*/
int get_col(int index, int len){
	if((index%len) == 0){
		return (len-1);
	}else{
		return (index%len)-1;
	}
}

/*START OF SET FUNCTIONS*/

/*Returns TRUE if CURRENT[x][y] == z, for set_command.
 * Checks if the value already in CURRENT[x][y] is z.
 *
 * @param
 * game - The game
 * x_index, y_index - Index for row, col in CURRENT respectively
 * z - The number the player wishes to set
 * @return
 * TRUE if z == CURRENT[x][y],
 * FALSE otherwise*/
int compare_z_to_num(Game *game, int x, int y, int z){
	int val = get_cell_field(game, x, y, VAL);
	if (val == z){
		return TRUE;
	}
	return FALSE;
}

/*Sets num in CURRENT[x][y], updates num_of_filled.
 * If num is 0 - num_of_filled will be lowered by 1.
 * Otherwise - raised by 1.
 *
 * @param
 * val - Number to set
 * x,y - index for row, column respectively in CURRENT.
 * game - The game*/
void set_value(Game *game, int x, int y, int old_val, int val){
	set_cell_field(game, x, y, VAL, val);
	update_num_of_filled_for_moves(game, old_val, val);
}

/*Check if the game is over.
 * if all cells are filled after a set move, validates the board.
 * if valid - game over and go to INITMODE
 * else - keep playing*/
int check_for_game_over(Game *game){
	int valid;
	int len = get_len(game);
	int max = len*len;
	int num_of_cells_filled = get_game_field(game, NUM_OF_FILLED);
	if (num_of_cells_filled == max){
		print_board(game);
		check_for_game_over_message();
		valid = validate(game, SET);
		if (valid == TRUE){
			game_over_message();
			set_game_field(game, GAME_MODE, INITMODE);
			return TRUE;
		}
		else if (valid == FALSE){
			game_not_over_yet_message();
			return FALSE;
		}
		else{
			return ERROR;
		}
	}
	return 0;
}
/*END OF SET FUNCTIONS*/

/* START OF MARK ERRORS FUNCTIONS*/

/* goes over the entire board and updates all errors
 * for loading a game in solve or edit*/
void update_mark_for_errors_board(Game *game){
	int row, col, val;
	int len = get_len(game);
	for (row = 0; row < len; row++){
		for (col = 0; col < len; col++){
			val = get_cell_field(game, row, col, VAL);
			update_mark_for_errors_cell(game, row, col, 0, val);
		}
	}
}

/*marks all errors regarding the val in cell[row][col]
 * updates error field for all the relevant cells in row, col, block*/
void update_mark_for_errors_cell(Game *game, int row, int col, int old_val, int val){

	int len = get_len(game);
	int r = mark_row_errors(game, row, col, val, len, old_val);
	int c = mark_col_errors(game, row, col, val, len, old_val);
	int b = mark_block_errors(game, row, col, val, old_val);

	if (r || c || b){
		set_cell_field(game, row, col, ERRONEOUS, TRUE);
	}
	else{
		set_cell_field(game, row, col, ERRONEOUS, FALSE);
	}
}

int mark_row_errors(Game *game, int row, int col, int val, int len, int old_val){
	int i, op_val;
	int err = FALSE;
	for (i = 0; i < len; i++){
		if (i != col){
			op_val = get_cell_field(game, row, i, VAL);
			if (op_val != 0){
				/*checking for fixing a cell*/
				if (op_val == old_val){
					if (valid_num(game, op_val, row, i, CURRENT) == TRUE){
						set_cell_field(game, row, i, ERRONEOUS, FALSE);
					}
				}
				/*new error*/
				else if (op_val == val){
					set_cell_field(game, row, i, ERRONEOUS, TRUE);
					err = TRUE;
				}
			}
		}

	}
	return err;
}
int mark_col_errors(Game *game, int row, int col, int val, int len, int old_val){
	int i, op_val;
	int err = FALSE;
	for (i = 0; i < len; i++){
		if (i != row){
			op_val = get_cell_field(game, i, col, VAL);
			if (op_val != 0){
				/*checking for fixing a cell*/
				if (op_val == old_val){
					if (valid_num(game, op_val, i, col, CURRENT) == TRUE){
						set_cell_field(game, i, col, ERRONEOUS, FALSE);
					}
				}
				/*new error*/
				else if (op_val == val){
					set_cell_field(game, i, col, ERRONEOUS, TRUE);
					err = TRUE;
				}
			}
		}

	}
	return err;
}

int mark_block_errors(Game *game, int row, int col, int val, int old_val){
	int i, j, op_val;
	int err = FALSE;
	int n = game->n;
	int m = game->m;
	int row_index = row_index_of_block(row, n);
	int col_index = col_index_of_block(col, m);

	for (i = row_index; i < (row_index + n); i++){
		for (j = col_index; j < (col_index + m); j++){
			if (!(i == row && j == col)){
				op_val = get_cell_field(game, i, j, VAL);
				if (op_val != 0){
					/*checking for fixing a cell*/
					if (op_val == old_val){
						if (valid_num(game, op_val, i, j, CURRENT) == TRUE){
							set_cell_field(game, i, j, ERRONEOUS, FALSE);
						}
					}
					/*new error*/
					else if (op_val == val){
						set_cell_field(game, i, j, ERRONEOUS, TRUE);
						err = TRUE;
					}
				}
			}
		}
	}
	return err;
}
/* END OF MARK ERRORS FUNCTIONS*/


/*START OF AUTOFILL FUNCTIONS*/

/*this function is used by the function autofill.*/
/*if there is only one legal value, the cell is updated to
 * that value. otherwise, the cell is kept at 0. */
/*RETURN VALUE IS THE CALCULATED LEGAL VALUE.*/
int update_to_legal_val(Game *game, int row, int col, int* digits_array, int command){
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int i;
	int counter = 0;

	counter = update_digits_array(game, row, col, digits_array, command);

	if(counter < len-1){
		set_cell_field(game, row, col, TEMP, 0);
		return 0;
	}
	else{
		for(i = 0; i<len; i++){
			if(digits_array[i] == 0){
				set_cell_field(game, row, col, TEMP, i+1);
				if (command == AUTOFILL){
					print_autofill_message(row, col, i+1);
				}
				return i+1;
			}
		}
	}
	return 0;
}

/*updates valid nums in digits_array for cell[row][col] s.t. 1 - number is invalid, 0 - valid
 * used in AUTOFILL and GENERATE*/
int update_digits_array(Game *game,int row, int col, int *digits_array, int command){
	int i, j, val;
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int counter = 0;
	int row_index = row_index_of_block(row, n);
	int column_index = col_index_of_block(col, m);

	for(i = row_index; i<(row_index+n); i++){
		for(j = column_index; j<(column_index+m); j++){
			if (command == AUTOFILL){
				val = get_cell_field(game, i, j, VAL);
			}
			else{
				val = get_cell_field(game, i, j, TEMP);
			}
			if(val != 0){
				digits_array[val-1] = 1;
			}
		}
	}
	for(i=0; i<len; i++){
		if (command == AUTOFILL){
			val = get_cell_field(game, row, i, VAL);
		}
		else{
			val = get_cell_field(game, row, i, TEMP);
		}
		if(val != 0){
			digits_array[val-1] = 1;
		}
	}
	for(j=0; j<len; j++){
		if (command == AUTOFILL){
			val = get_cell_field(game, j, col, VAL);
		}
		else{
			val = get_cell_field(game, j, col, TEMP);
		}
		if(val != 0){
			digits_array[val-1] = 1;
		}
	}
	for(i=0; i<len; i++){
		if(digits_array[i] == 1){
			counter++;
		}
	}
	return counter;
}

void zero_all_digits_array(int *digits_array, int len){
	int i;
	for(i = 0; i<len; i++){
		digits_array[i] = 0;
	}
}

void print_autofill_message(int row, int col, int num){
	printf("cell (%d,%d) was filled with %d\n", row+1, col+1, num);
}
/*END OF AUTOFILL FUNCTIONS*/

/*START OF GENERATE FUNCTIONS*/
int generate_fill_x(Game *game, int x){
	int row, col, val, chosen;
	int len = get_len(game);
	int counter = 0;
	int num_of_filled = get_game_field(game, NUM_OF_FILLED);
	int size = len*len;
	int *digits_array;
	int num_of_valid_digits, num_of_invalid_digits;

	if((size-num_of_filled)< x){
		error_not_enough_empty_cells_for_generate();
		return FALSE;
	}
	while (counter < x){
		row = rand() % len;
		col = rand() % len;
		val = get_cell_field(game, row, col, TEMP);
		chosen = get_cell_field(game, row, col, CHOSEN);
		if ((val == 0) && (chosen == 0)){
			digits_array = calloc(len, sizeof(int));
			if (!digits_array){
				failure_error("calloc", "generate_fill_x");
				return ERROR;
			}
			num_of_invalid_digits = update_digits_array(game, row, col, digits_array, GENERATE);
			num_of_valid_digits = len - num_of_invalid_digits;
			if (num_of_valid_digits == 0){
				free(digits_array);
				return FALSE;
			}
			val = rand() % num_of_valid_digits;
			val = get_nth_val_of_arr(digits_array, len, val);

			set_cell_field(game, row, col, TEMP, val);
			counter++;
			free(digits_array);
		}
	}
	return TRUE;
}

/*returns the n'th zero index in arr
 * index is in range [1, len(arr)]*/
int get_nth_val_of_arr(int *arr, int len, int n){
	int i;
	int count = 0;

	for (i=0; i<len; i++){
		if (arr[i] == 0){
			if (count == n){
				return i+1;
			}
			count++;
		}
	}
	return -1;
}

/*randomly chooses y cells to copy back to board from temp.
 * updates relevant changes to BOARDMOVE*/
void generate_empty_y(Game *game, int y){
	int len = get_len(game);
	int i, j, temp_val, curr_val;
	int num = 0;
	cell_change *cell_change_head = NULL;
	while (num < y){
		j = rand()%len;
		i = rand()%len;
		if (get_cell_field(game, i, j, CHOSEN) == FALSE){
			temp_val = get_cell_field(game, i, j, TEMP);
			curr_val = get_cell_field(game, i, j, VAL);

			if (temp_val != curr_val){
				set_cell_field(game, i, j, VAL, temp_val);
				cell_change_head = add(cell_change_head, i,j, curr_val, temp_val);
			}
			set_cell_field(game, i, j, CHOSEN, TRUE);
			num++;
		}
	}
	/*clears all non chosen cells and updates board move*/
	for (i=0; i<len; i++){
		for (j=0; j<len; j++){
			if (get_cell_field(game, i, j, CHOSEN) == FALSE){
				curr_val = get_cell_field(game, i, j, VAL);
				if (curr_val != 0){
					set_cell_field(game, i, j, VAL, 0);
					cell_change_head = add(cell_change_head, i,j, curr_val, 0);
				}
			}
		}
	}
	append(game, &(game->head), 0, 0, 0, BOARDMOVE, &cell_change_head, 0);
}

/*clears the specified field from all cells*/
void clear_board_field(Game *game, int field){
	int len = get_len(game);
	int row, col;
	for (row=0; row<len; row++){
		for (col=0; col<len; col++){
			set_cell_field(game, row, col, field, 0);
		}
	}
}

/*END OF GENERATE FUNCTIONS*/


/*this command is only available in EDIT or SOLVE mode*/
/*COMPARE this function's format to the executable from Moodle!*/
int save(Game *game, char* file_path){
	/*check regarding FULL OR RELATIVE path to the file.*/
	int n = game->n;
	int m = game->m;
	int len = n*m;
	FILE *fptr;
	int row,col, mode;
	int val_int;
	mode = get_game_field(game, GAME_MODE);
	fptr = fopen(file_path, "w");

	if(fptr == NULL){
		error_file_pointer();
		return ERROR;
	}

	if(mode == EDITMODE){
		/*erroneous boards or boards without a solution cannot be saved in EDIT mode.*/
		val_int = validate(game, SAVE);
		if (val_int == FALSE){
			return FALSE;
		}
		if( val_int == ERROR){
			return ERROR;
		}
	}
	/*our n, m are switched from the instructions*/
	fprintf(fptr, "%d %d\n", n, m);
	for(row = 0;row<len; row++){
		for(col = 0; col<len; col++){
			fprintf(fptr, "%d", get_cell_field(game, row, col, VAL));
			/*in edit mode, all cell containing values are marked as fixed in the saved file.*/
			if((mode == EDITMODE) && (get_cell_field(game, row, col, VAL) != 0)){
				fprintf(fptr, ".");
			}
			else if(get_cell_field(game, row, col, FIXED)){
				fprintf(fptr, ".");
			}
			fprintf(fptr, " ");
		}
		fprintf(fptr, "\n");
	}
	fclose(fptr);
	printf("The board was successfully saved to %s\n", file_path);
	return TRUE;
}

/*title printed to user, should appear ONLY AT START OF ENTIRE PROGRAM*/
void print_title(){
	printf("***SUDOKU PROGRAM***\n");
	return;
}

/*FOR NUM_SOLUTIONS*/
unsigned long exhaustive_backtracking(Game *game, int *error){
	/*delete the next line, and delete the parameter error.*/
	int len = get_len(game);
	int row = 0, col = 0;
	int val = 1;
	int cell_val = 0;
	unsigned long num_of_solutions = 0;
	stack_node *top = NULL;
	int backwards = FALSE;

	copy_curr_to_temp(game);
	do {
/*		cell_val = get_cell_field(game, row, col, TEMP);*/
		cell_val = get_cell_field(game, row, col, VAL);

		if (val > len) {
			if (cell_val == 0){
				set_cell_field(game, row, col, TEMP, 0);
			}
			/*can't find legal value for cell -> fold*/
			top = (stack_node*)pop(top, &row, &col, &val);
			backwards = TRUE;
			val++;
			continue;
		}

		if (cell_val == 0) {
			/*check val for cell*/
			if (valid_num(game, val, row, col, TEMP) == FALSE) {
				val++;
				continue;
			}
		}
		else {
			/*cell has a value*/
			if (backwards == TRUE) {
				top = (stack_node*)pop(top, &row, &col, &val);
				val++;
				continue;
			}
			val = cell_val;
		}

		set_cell_field(game, row, col, TEMP, val);

		if ((col == len -1) && (row == len -1)){
			if (cell_val == 0){
				set_cell_field(game, row, col, TEMP, 0);
			}
			top = (stack_node*)pop(top, &row, &col, &val);

			num_of_solutions++;
			backwards = TRUE;
			val++;
		}
		else if (col == len-1){
			top = (stack_node*)push(top, row, col, val);

			if (top == NULL){
				/*push error*/
				failure_error("push", "exhaustive_backtracking");
				*error = ERROR;
				return 0;
			}
			backwards = FALSE;
			col = 0;
			row++;
			val = 1;
		}
		else {
			top = (stack_node*)push(top, row, col, val);
			if (top == NULL) {
				/*push error*/
				failure_error("push", "exhaustive_backtracking");
				*error = ERROR;
				return 0;
			}
			backwards = FALSE;
			col++;
			val = 1;
		}
	} while (top != NULL );

	return num_of_solutions;
}

int board_has_errors(Game *game){
	int len = get_len(game);
	int row, col;
	for (row = 0; row < len; row++){
		for (col = 0; col < len; col++){
			if (get_cell_field(game, row, col, ERRONEOUS) == TRUE){
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*START OF LINKED LIST FUNCTIONS*/

/*this function adds a new move to the list.*/
/*all moves of type set/autofill/generate/guess are added.*/
void append(Game *game, struct move** head_ref, int set_val, int row, int col, int move_type, cell_change** change_head, int new_val)
{
	move* new_move;
	move* last;

	printf("start append\n");
    new_move = (move*)malloc(sizeof(move));
    last = *head_ref; /* used in step 5*/
    new_move->move_undone = 0;
    /*data is inserted to the move structure according to the type of move,
     * either SETMOVE or BOARDMOVE.*/
    if(move_type == SETMOVE){
    	new_move->move_type = SETMOVE;
        new_move->orig_cell_val = set_val;
        new_move->new_cell_val = new_val;
        new_move->set_row = row;
        new_move->set_col = col;
    }
    else if(move_type == BOARDMOVE){
    	new_move->move_type = BOARDMOVE;
    	new_move->cell_change_head = *change_head;
    }
    game->curr_move = new_move;

    /*the new move is added at the end of the list, so we make the next of it NULL.*/
    /*KILL NEXT MOVES STRUCTS!!!!!!!!!!!*/
    new_move->next = NULL;
    /*if the list is empty, make the appended node it's head.*/
    if (*head_ref == NULL) {
        new_move->prev = NULL;
        new_move->head_indicator = 1;
        *head_ref = new_move;
        return;
    }
    else{
    	new_move->head_indicator = 0;
    }
    /* if the list is not empty, traverse it until its last node.*/
    while (last->next != NULL){
        last = last->next;
    }
    /*change the list's last node's next to be the appended move.*/
    last->next = new_move;
    /*make the appended node's prev to be the last node.*/
    new_move->prev = last;
	return;
}

/*NOT IN USE*/
void print_cell_change_list(Game* game, cell_change* cell_change_head){
	cell_change* current = cell_change_head;
	int before_change, after_change;
	printf("START PRINT CELL CHANGE LIST:\n");
	while(current != NULL){
		before_change = current->orig_cell_val;
		after_change = game->curr_board[current->row][current->col].val;
		if (before_change != after_change){
			printf("cell (%d,%d) changed from %d to %d\n", current->row, current->col,
				before_change, after_change);
		}
		current = current->next;
	}
	printf("DONE PRINT CELL CHANGE LIST\n");
	return;
}

void redo_board_move(Game *game, int first_move_undone){
	cell_change* ptr;
	int row, col, before_change, after_change;
	if(!first_move_undone){
		ptr = game->curr_move->next->cell_change_head;
	}
	else{
		ptr = game->curr_move->cell_change_head;
	}
	while(ptr != NULL){
		row = ptr->row;
		col = ptr->col;
		before_change = get_cell_field(game, row, col, VAL);
		after_change = ptr->new_cell_val;
		printf("redo cell (%d,%d) from %d to %d\n",
				row+1, col+1, before_change, after_change);
		set_cell_field(game, row, col, VAL, ptr->new_cell_val);
		ptr = ptr->next;
		update_num_of_filled_for_moves(game, before_change, after_change);
	}
}

void undo_board_move(Game *game){
	int row, col, before_change, after_change;
	cell_change* ptr = game->curr_move->cell_change_head;
	while(ptr != NULL){
		row = ptr->row;
		col = ptr->col;
		before_change = game->curr_board[row][col].val;
		after_change = ptr->orig_cell_val;
		printf("undo cell (%d,%d) from %d back to %d\n",
				row+1, col+1, before_change, after_change);
		set_cell_field(game, row, col, VAL, after_change);
		ptr = ptr->next;
		update_num_of_filled_for_moves(game, before_change, after_change);
	}
}

/*this function deletes all move elements in the moves list, starting from
 * the given element. this is used:
 * 1) for deleting an entire list starting from head, when exiting.
 * 2) for deleting the redo part of the list, when the user makes a
 * set/autofill/generate/guess move.
 */
void destroy_moves_list_starting_from(Game* game, move *start_move, int head_or_curr){
	move *tmpPtr = start_move;
	move *followPtr;

	while(tmpPtr != NULL){
		followPtr = tmpPtr;
		tmpPtr = tmpPtr->next;
		if(followPtr->move_type == SETMOVE){
			free(followPtr);
		}
		else{
			destroy_cell_change_list(followPtr->cell_change_head);
			free(followPtr);
		}
	}
	if((head_or_curr == HEAD) || (start_move == game->head)){
		game->head = NULL;
	}
	else{
		start_move->prev->next = NULL;
	}
	return;
}

void destroy_cell_change_list(cell_change* head){
	cell_change* tmpPtr = head;
	cell_change* followPtr;
	while(tmpPtr != NULL){
		followPtr = tmpPtr;
		tmpPtr = tmpPtr->next;
		free(followPtr);
	}
}

void clear_redo_list(Game *game){
	if(game->curr_move != NULL){
		if(game->curr_move->next != NULL){
			destroy_moves_list_starting_from(game, game->curr_move->next, CURR);
		}
	}
}
