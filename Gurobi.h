/*
 * Gurobi.h
 *
 *  Created on: Mar 30, 2019
 *      Author: HP
 */

#ifndef GUROBI_H_
#define GUROBI_H_


/*solves the board using gurobi*/
int get_gurobi_solution(int ***cube, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat, int **matrix, double *sol, int num_of_vars, int LP_or_ILP, int n, int m);

#endif /* GUROBI_H_ */
