/**
 * Solver summary:
 *
 * Used to generate a solved Sudoku board and check validity of moves made by the player.
 *
 * solve_with_gurobi					- Solves board using LP with Gurobi.
 * score_of_cell						- returns score of a variable from Gurobi.
 * solution_array_to_board				- fills TEMP with values according to Gurobi solution.
 * num_of_non_zero_cells_in_solution	- returns number of non zero scores in solution array.
 * find_index_in_var					- returns index of var array for cube[row][col][num].
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include "Game.h"

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
int solve_with_gurobi(Game *game, int LP_or_ILP, int command, int x, int y, float threshold);

/*for HINT and GUESS*/
/*returns the score cube[row][col][val] got from Gurobi
 * @param
 *  - cube      the variable cube
 *  - sol       solution array recieved from Gurobi
 *  - row, col  cell indices
 *  - val		value of cell[row][col]
 *  - len       length of board, n*m
 *  @return score for cell[row][col][val]*/
int score_of_cell(int ***cube, double *sol, int row, int col, int val, int len);

/*crosses between cube and solution array from gurobi and fills missing cells in TEMP.
 *counts num of cells it fills */
int solution_array_to_board(Game *game, double *solution_array, int ***cube, int len, int num_of_vars);

/* returns number of non zero scores in solution array*/
int num_of_non_zero_cells_in_solution(double *solution_array, int num_of_vars);

/**used in Gurobi and Solver
 * returns the index in var array sent to gurobi that belongs to cube[row][col][num]*/
int find_index_in_var(int ***cube, int row, int col, int num, int len);


#endif /* SOLVER_H_ */
