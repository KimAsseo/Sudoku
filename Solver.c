#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include <math.h>
#include "settings.h"
#include "Solver.h"
#include "general_funcs.h"
#include "Errors.h"
#include "Game_aux.h"
#include "Gurobi.h"
#include "moves_list.h"
#include "cell_change_move.h"
#include "Game.h"


/*SOLVE_WITH_GUROBI FUNCTIONS*/
int fill_matrices(Game *game, int **matrix, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat);
void make_var_cube(Game *game, int ***cube, int **matrix, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat);
int fill_all_obvious_cells(Game *game);
int calc_num_of_vars_from_cube(int ***cube, int len);
void free_arrays(int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat, int **matrix, double *solution_array, int ***cube, int len);
int gurobi_solution_to_command_execution(Game *game, int ***cube, double *solution_array, int command, int len, int x, int y, double threshold);

/*GUESS FUNCTIONS*/
int fill_board_for_guess(Game *game, int ***cube, double *solution_array, int len, float x);
int fill_cell_for_guess(Game *game,int row, int col, int ***cube, double *solution_array, int len, float x);
void build_cell_array(Game *game, int row, int col, int ***cube, double *solution_array, int len, double *scores, int cnt);
int set_cell_for_guess(Game *game, int row, int col, int cnt, double *scores, int len, float x);
double get_rand();

void guess_hint_print(Game *game, int x, int y, int ***cube, double *sol);
void hint_print(Game *game, int x, int y, int ***cube, double *sol);

/*FOR TESTING*/
void print_analysis_arrays_for_gurobi(Game *game, int **matrix, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat);
void find_cell_legal_values(Game *game,int row, int col, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat);
void print_legal_values(Game *game, int* legal_values);
void print_cube(int ***cube, int len);

/*solves board using linear programming via Gurobi
 *if validate / generate board -> ILP
 *if guess / guess_hint -> LP
 *@param:
 *  - game        the game
 *  - LP_or_ILP   signals whether LP or ILP will be used
 *  - command     command from the user, one of: VALIDATE, GUESS, HINT, GUESS_HINT
 *  - x, y 		  cell indices for HINT or GUESS_HINT
 *  - threshold	  threshold score for GUESS
 *@return:
 *  - ERROR 	error
 *	- 2			optimal solution
 *	- 0     	else, there is no solution
 *returns 0 if succeeded, ERROR otherwise*/
int solve_with_gurobi(Game *game, int LP_or_ILP, int command, int x, int y, float threshold){
/*	printf("starting call_Gurobi\n");*/
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int i, j, ex;
	int num_of_vars, fill;
	int fixed_cells, to_be_filled;
	double *solution_array;
	int optimstatus, count = 0;
	int** row_arrays_mat;
	int** col_arrays_mat;
	int** block_arrays_mat;
	int** matrix;
	int*** cube;

	/*REMEMBER TO FREE ALL OF THE FOLLOWING CALLOCS.*/
/*	generate matrix of arrays for analysis results.*/
	row_arrays_mat = (int**)calloc(len, sizeof(int*));
	if (row_arrays_mat == NULL){
		failure_error("calloc", "solve_with_gurobi");
		return ERROR;
	}
	for(i = 0; i < len; i++){
		row_arrays_mat[i] = (int*)calloc(len, sizeof(int));
		if (row_arrays_mat[i] == NULL){
			failure_error("calloc", "solve_with_gurobi");
			return ERROR;
		}
	}
	col_arrays_mat = (int**)calloc(len, sizeof(int*));
	if (col_arrays_mat == NULL){
		failure_error("calloc", "solve_with_gurobi");
		return ERROR;
	}
	for(i = 0; i < len; i++){
		col_arrays_mat[i] = (int*)calloc(len, sizeof(int));
		if (row_arrays_mat[i] == NULL){
			failure_error("calloc", "solve_with_gurobi");
			return ERROR;
		}
	}
/*	the board's blocks will be numbered from 0, starting from upper left block.*/
	block_arrays_mat = (int**)calloc(len, sizeof(int*));
	if (block_arrays_mat == NULL){
		failure_error("calloc", "solve_with_gurobi");
		return ERROR;
	}
	for(i = 0; i < len; i++){
		block_arrays_mat[i] = (int*)calloc(len, sizeof(int));
		if (block_arrays_mat[i] == NULL){
			failure_error("calloc", "solve_with_gurobi");
			return ERROR;
		}
	}
/*	generate matrix for analysis results and initialize values to 1.*/
	matrix =(int**)calloc(len, sizeof(int*));
	if (matrix == NULL){
		failure_error("calloc", "solve_with_gurobi");
		return ERROR;
	}
	for(i = 0; i < len; i++){
		matrix[i] = (int*)calloc(len, sizeof(int));
		if (matrix[i] == NULL){
			failure_error("calloc", "solve_with_gurobi");
			return ERROR;
		}
	}
/*	generate cube for the variables*/
	cube =(int***)calloc(len, sizeof(int**));
	if (cube == NULL){
		failure_error("calloc", "solve_with_gurobi");
		return ERROR;
	}
	for(i = 0; i < len; i++){
		cube[i] = (int**)calloc(len, sizeof(int*));
		if (cube[i] == NULL){
			failure_error("calloc", "solve_with_gurobi");
			return ERROR;
		}
		for (j=0; j<len; j++){
			cube[i][j] = (int*)calloc(len, sizeof(int));
			if (cube[i][j] == NULL){
				failure_error("calloc", "solve_with_gurobi");
				return ERROR;
			}
		}
	}

	copy_curr_to_temp(game);
	fill = fill_all_obvious_cells(game);
	if (fill == ERROR){
		return ERROR;
	}
	fixed_cells = fill_matrices(game, matrix, row_arrays_mat, col_arrays_mat, block_arrays_mat);
/*	print_analysis_arrays_for_gurobi(game, matrix, row_arrays_mat, col_arrays_mat, block_arrays_mat);*/

	make_var_cube(game, cube, matrix, row_arrays_mat, col_arrays_mat, block_arrays_mat);

	num_of_vars = calc_num_of_vars_from_cube(cube, len);
	solution_array = (double*)calloc(num_of_vars, sizeof(double));
	if (solution_array == NULL){
		failure_error("calloc", "solve_with_gurobi");
		return ERROR;
	}

	optimstatus = get_gurobi_solution(cube, row_arrays_mat, col_arrays_mat, block_arrays_mat, matrix, solution_array, num_of_vars, LP_or_ILP, n, m);

	if (optimstatus == -1) {
		return ERROR;
	}

	else if(optimstatus == GRB_OPTIMAL) {
		if (LP_or_ILP == LP){
			count = num_of_non_zero_cells_in_solution(solution_array, num_of_vars);
		}
		else{
			count = solution_array_to_board(game, solution_array, cube, len, num_of_vars);
		}
		to_be_filled = len*len - fixed_cells;
		if (count < to_be_filled) {
			optimstatus = 0;
		}
	}

	/*act according to command*/
	if (optimstatus == GRB_OPTIMAL) {
		ex = gurobi_solution_to_command_execution(game, cube, solution_array, command, len, x, y, threshold);
		if (ex == ERROR){
			return ERROR;
		}
	}

	free_arrays(row_arrays_mat, col_arrays_mat, block_arrays_mat, matrix, solution_array, cube, len);
	return optimstatus;
}

/*fills all matrices and returns num of fixed cells*/
int fill_matrices(Game *game, int **matrix, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat){
	int n = game->n;
	int m = game->m;
	int row, col, count = 0;
	int len = n*m;
	int num, block;

/*	iterate entire board once*/
	for(row = 0; row<len; row++){
		for(col=0; col<len; col++){
			num = get_cell_field(game, row, col, VAL);
			if(num != 0){
				matrix[row][col] = 1;
				row_arrays_mat[num-1][row] = 1;
				col_arrays_mat[num-1][col] = 1;
				block = calc_block(row, col, n, m);
				block_arrays_mat[num-1][block] = 1;
				count++;
			}
		}
	}
	return count;
}

/*fills the cube s.t. 0 - need a variable, 1 - else */
void make_var_cube(Game *game, int ***cube, int **matrix, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat){
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int row,col,num,block;

	for(row = 0; row<len; row++){
		for(col = 0; col<len; col++){
			for(num = 0; num<len; num++){
				block = calc_block(row, col, n, m);
/*				this cell is filled*/
				if (matrix[row][col] == 1){
					cube[row][col][num] = 1;
				}
/*					this num appears in the current row*/
				if(row_arrays_mat[num][row] == 1){
/*					no variables required for this num in this row.*/
					cube[row][col][num] = 1;
				}
/*					this num appears in the current col*/
				if(col_arrays_mat[num][col] == 1){
/*					no variables required for this num in this col.*/
					cube[row][col][num] = 1;
				}
/*					this num appears in the current block*/
				if(block_arrays_mat[num][block] == 1){
					cube[row][col][num] = 1;
				}
			}
		}
	}
/*	print_cube(cube, len);*/
}

/*crosses between cube and solution array and fills missing cells in TEMP.
 *counts num of cells it fills */
int solution_array_to_board(Game *game, double *solution_array, int ***cube, int len, int num_of_vars){
	int row, col, num;
	int i = 0;
	int count = 0;

	while (i<num_of_vars){
		for (row = 0; row<len; row++){
			for (col = 0; col<len; col++){
				for (num=0; num<len; num++){
					if (cube[row][col][num] == 0){
						if (solution_array[i] != 0){
							set_cell_field(game, row, col, TEMP, num+1);
							count++;
						}
						i++;
					}
				}
			}
		}
	}
	return count;
}

/* counts number of non zero scores in solution array*/
int num_of_non_zero_cells_in_solution(double *solution_array, int num_of_vars){
	int count = 0;
	int i = 0;
	for (i = 0; i<num_of_vars; i++){
		if (solution_array[i] != 0){
			count++;
		}
	}
	return count;
}


/* fills all obvious cells in TEMP board by itering autofill
 * obvious cell - cell with just one valid number for it*/
int fill_all_obvious_cells(Game *game){
	int count = 0;
	copy_curr_to_temp(game);
	count = autofill(game, 0);
	while (count>0){
		count = autofill(game, 0);
	}
	if (count == ERROR){
		return ERROR;
	}
	return 0;
}

/*calculates the number of 0 cells in cube
 * for each 0, we'll need a variable in gurobi*/
int calc_num_of_vars_from_cube(int ***cube, int len){
	int row, col, num, count = 0;

	for (row = 0; row<len; row++){
		for (col=0; col<len; col++){
			for (num = 0; num<len; num++){
				if (cube[row][col][num] == 0){
					count++;
				}
			}
		}
	}
	return count;
}

void free_arrays(int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat, int **matrix, double *solution_array, int ***cube, int len){
	int i, j;
	free(solution_array);
	for (i=len-1; i>=0; i--){
		for (j = len-1; j >=0; j--){
			free(cube[i][j]);
		}
		free(cube[i]);
		free(matrix[i]);
		free(block_arrays_mat[i]);
		free(col_arrays_mat[i]);
		free(row_arrays_mat[i]);
	}
	free(cube);
	free(matrix);
	free(block_arrays_mat);
	free(col_arrays_mat);
	free(row_arrays_mat);
}

/* uses solution from gurobi to execute the command
 * goes according to command (HINT, GUESS_HINT, GUESS),
 * and uses to relevant parameters for each-
 *  x, y - cell indices for HINT and GUESS_HINT
 *  threshold - score threshold for GUESS, all valid cells with score > threshold will be filled
 */
int gurobi_solution_to_command_execution(Game *game, int ***cube, double *solution_array, int command, int len, int x, int y, double threshold){
	int val;
	switch (command){
		case HINT:
			hint_print(game, x, y, cube, solution_array);
			break;
		case GUESS_HINT:
			guess_hint_print(game, x, y, cube, solution_array);
			break;
		case GUESS:
			val = fill_board_for_guess(game, cube, solution_array, len, threshold);
			if (val == ERROR){
				return ERROR;
			}
			break;
		default:
			break;
	}
	return TRUE;
}

/*for hint and guess*/
/*returns the score of cube[row][col][val] from Gurobi*/
int score_of_cell(int ***cube, double *sol, int row, int col, int val, int len){
	int index;
	double score;
	index = find_index_in_var(cube, row, col, val, len);
	score = sol[index];
	return score;
}

/*prints all legal values for cell(x, y)
 * and their score according to LP*/
void guess_hint_print(Game *game, int x, int y, int ***cube, double *sol){
	int len = get_len(game);
	int i;
	double score;
	printf("the legal values for cell (%d, %d) are:\n", x, y);
	printf("value  |  score\n");
	for (i=0; i<len; i++){
		if (cube[x-1][y-1][i] == 0){
			score = score_of_cell(cube, sol, x-1, y-1, i, len);
			if (score > 0.0){
				printf("  %d    |  %.2f\n", i+1, score);
			}
		}
	}
	printf("\n");
}

/*prints the hint for cell(x, y) according to ILP*/
/*check if the 1.0 works*/
void hint_print(Game *game, int x, int y, int ***cube, double *sol){
	int len = get_len(game);
	int i, index;
	double score;
	for (i=0; i<len; i++){
		if (cube[x-1][y-1][i] == 1){
			index = find_index_in_var(cube, x-1, y-1, i, len);
			score = sol[index];
			if (score == 1.0){
				print_hint(x, y, i+1);
			}
		}
	}
}

/*START OF GUESS FUNCTIONS*/

/*fills the board with all valid cells that got a score > threshold from gurobi*/
int fill_board_for_guess(Game *game, int ***cube, double *solution_array, int len, float x){
	int row, col, set;
	cell_change *cell_change_head = NULL;
	clear_redo_list(game);
	for (row = 0; row< len; row++){
		for (col=0; col<len; col++){
			set = fill_cell_for_guess(game, row, col, cube, solution_array, len, x);
			if (set == ERROR){
				return ERROR;
			}
			if (set != 0){
				cell_change_head = add(cell_change_head, row, col, 0, set);
			}
		}
	}
	append(game, &(game->head), 0, 0, 0, BOARDMOVE, &cell_change_head, 0);
	return TRUE;
}

/*fills one cell according to GUESS rules
 * returns the value the cell got*/
int fill_cell_for_guess(Game *game,int row, int col, int ***cube, double *solution_array, int len, float x){
	int cnt, val;
	int set = 0;
	double *scores = (double*)malloc(len*sizeof(double));
	if (!scores){
		failure_error("calloc", "fill_cells_for_guess");
		return ERROR;
	}

	/*make array for scores of [row][col]*/
	val = get_cell_field(game, row, col, VAL);
	if (val == 0){
		cnt = 0;
		/*building cell array*/
		build_cell_array(game, row, col, cube, solution_array, len, scores, cnt);
		set = set_cell_for_guess(game, row, col, cnt, scores, len, x);
	}
	free(scores);
	return set;
}

/*builds scores array for cell[row][col] according to gurobi*/
void build_cell_array(Game *game, int row, int col, int ***cube, double *solution_array, int len, double *scores, int cnt){
	int val;
	double score = 0.0;

	for (val = 0; val < len; val++){
		if (cube[row][col][val] == 0){
			if (valid_num(game, val+1, row, col, CURRENT) == TRUE){
				score = score_of_cell(cube, solution_array, row, col, val, len);
				scores[val] = score;
				if (score > 0.0){
					cnt++;
				}
			}
			else{
				scores[val] = 0.0;
			}
		}
		else{
			scores[val] = 0.0;
		}
	}
}

/* if only one value hold for the cell - fill it.
 * else - randomly choose one according to the score
 * returns the value the cell got*/
int set_cell_for_guess(Game *game, int row, int col, int cnt, double *scores, int len, float x){
	int i;
	int set = 0;
	int is_set = FALSE;
	double score, random;

	if (cnt == 1){
		for (i=0; i< len; i++){
			if (scores[i] != 0){
				set_value(game, row, col, 0, i+1);
				set = i+1;
				return set;
			}
		}
	}
	else{
		is_set = FALSE;
		while (is_set == FALSE){
			random = get_rand();
			for (i=0; i<len; i++){
				score = scores[i];
				if ((random < score) && (score != 0)){
					if (score >= x){
						set_value(game, row, col, 0, i+1);
						set = i+1;
					}
					return set;
				}
				random -= scores[i];
			}
/*			if there is no valid num for [row][col] this will break the loop*/
			is_set = TRUE;
		}
	}
	return set;
}

/*gets random number in [0,1]*/
double get_rand(){
	return (double)(rand()%100)/100;
}

/*
*used in Gurobi and Solver
 * returns the index in var array sent to gurobi that belongs to cube[row][col][num]*/
int find_index_in_var(int ***cube, int row, int col, int num, int len){
	int index = 0;
	int r = 0, c = 0, n = 0;

  for (r=0 ; r < len; r++){
		for (c=0 ; c < len; c++){
			for (n=0 ; n < len; n++){
				if (cube[r][c][n] == 0){
				  if (r==row && c==col && n==num){
					return index;
					}
					else{
					  index++;
				  }
				}
			}
		}
	}
	return index;
}

/*FOR TESTING*/

void print_analysis_arrays_for_gurobi(Game *game, int **matrix, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat){

	int n = game->n;
	int m = game->m;
	int len = n*m;
	int i,j;
	printf("matrix:\n");
	for(i = 0; i < len; i++){
		for(j = 0 ; j<len; j++){
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
	printf("******************\n");

	printf("row_arrays_mat: \n");
	for(i = 0; i<len;i++){
		printf("%d: ", i+1);
		for(j = 0;j<len; j++){
			printf("%d ", row_arrays_mat[i][j]);
		}
		printf("\n");
	}

	printf("col_arrays_mat: \n");
	for(i = 0; i<len;i++){
		printf("%d: ", i+1);
		for(j = 0;j<len; j++){
			printf("%d ", col_arrays_mat[i][j]);
		}
		printf("\n");
	}

	printf("block_arrays_mat: \n");
	for(i = 0; i<len;i++){
		printf("%d: ", i+1);
		for(j = 0;j<len; j++){
			printf("%d ", block_arrays_mat[i][j]);
		}
		printf("\n");
	}
}

/*at the end of this function, each cell in the array is 0 if the digits is legal, and 1 otherwise.*/
void find_cell_legal_values(Game *game,int row, int col, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat){
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int block;
	int* legal_values = (int*)calloc(len, sizeof(int));
	int i;

	printf("(row,col) = %d,%d\n",row,col);
	for(i = 0; i<len; i++){
		if(row_arrays_mat[i][row] == 1){
			legal_values[i] = 1;
		}
	}
/*	print_legal_values(game, legal_values);*/
	for(i = 0; i<len; i++){
		if(col_arrays_mat[i][col] == 1){
			legal_values[i] = 1;
		}
	}
/*	print_legal_values(game, legal_values);*/
	/*find the cell's block.*/
	block = calc_block(row, col, n, m);

	for(i = 0; i<len; i++){
		if(block_arrays_mat[i][block] == 1){
			legal_values[i] = 1;
		}
	}
	print_legal_values(game, legal_values);
}

/*a temp function for us to print cell_legal_values.*/
void print_legal_values(Game *game, int* legal_values){
	int n = game->n;
	int m = game->m;
	int len = n*m;
	int i;
	printf("legal_values: ");
	for(i=0; i<len; i++){
		printf("%d, ", legal_values[i]);
	}
	printf("\n");
}

/*prints cube*/
void print_cube(int ***cube, int len){
	int row, col, num;
	for(row = 0; row<len; row++){
		for(col = 0; col<len; col++){
			printf("(");
			for(num = 0; num<len; num++){
/*				printf("(%d,%d,%d) ", row,col,num+1);*/
				printf("%d ", cube[row][col][num]);
			}
			printf(")");
		}
		printf("\n");
	}
}
/*END OF TESTING*/
