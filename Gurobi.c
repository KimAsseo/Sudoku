/*
 This example formulates and solves the following simple MIP model:

     maximize    x +   y + 2 z
     subject to  x + 2 y + 3 z <= 4
                 x +   y       >= 1
     x, y, z binary
*/

#include <stdlib.h>
#include <stdio.h>
#include "Solver.h"
#include "settings.h"
#include "Errors.h"
#include "Gurobi.h"
#include "gurobi_c.h"
#include "general_funcs.h"

#define BUFF_SIZE 256

void free_arrs(int *ind, double *val, double *obj, char *vtype, char *const_name, double *ub);
void print_sol(double *sol, int num_of_vars);

int get_gurobi_solution(int ***cube, int **row_arrays_mat, int **col_arrays_mat, int **block_arrays_mat, int **matrix, double *sol, int num_of_vars, int LP_or_ILP, int n, int m){

  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;

  int i = 0;
  int error = 0, optimstatus = 0;
  int row, col, num, block, row_start, col_start;
  int num_of_consts = 0;
  int len = n*m;
  int constraints = 0;
  /*double objval;*/

  int 		*ind = (int *) calloc(len, sizeof(int));
  double 	*val = (double *) calloc(len, sizeof(double));
  double  *obj = (double *) calloc(num_of_vars, sizeof(double));
  char 		*vtype = (char *) calloc((int)num_of_vars, sizeof(char));
  char 		*const_name = (char *) calloc(BUFF_SIZE, sizeof(char));
  double  *ub = (double *) calloc(num_of_vars, sizeof(double));

  if (!ind || !val || !vtype || !const_name || !ub) {
	  failure_error("calloc", "get_gurobi_solution");
	  return ERROR;
  }

/*   Create environment - log file is mip1.log*/
  error = GRBloadenv(&env, "mip1.log");
  if (error) {
	  printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }

  error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
/*
  #the above line should be used if we don't want Gurobi to print everything
  #to the console. we can print to the console in the project, whatever we want.
*/
  if (error) {
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

/*   Create an empty model named "mip1"*/
  if (LP_or_ILP == ILP){
      error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
      }
    else{
      for (i = 0; i< num_of_vars; i++){
        ub[i] = 1.0;
      }
      error = GRBnewmodel(env, &model, "mip1", 0, obj, NULL, ub, NULL, NULL);
    }
  if (error) {
	  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }


  /* Add obj func*/

    /*random weights for vals in LP*/
	if (LP_or_ILP == LP){
	  for (i=0; i<num_of_vars; i++){
		  obj[i] = rand()%(len*2) +1;
	  }
	}

/*   Add variables*/


/*   variable types - for x,y,z (cells 0,1,2 in "vtype")*/
  if (LP_or_ILP == ILP){
	  for(i = 0; i < num_of_vars; ++i) {
		  vtype[i] = GRB_BINARY;
	  }
  }
  else{
	  for(i = 0; i < num_of_vars; ++i) {
		  vtype[i] = GRB_CONTINUOUS;
	  }
  }


/*   add variables to model*/
  error = GRBaddvars(model, num_of_vars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
  if (error) {
	  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model); GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }

/*   Change objective sense to maximization*/
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) {
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model); GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }

/*   update the model - to integrate new variables*/
  error = GRBupdatemodel(model);
  if (error) {
	  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model); GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }


/*starting constraints*/


 /* first constraint: one of in num in row
   * for col in [0,len-1], sum(Xrow,col,num) = 1*/
	constraints = 0;
	for (num = 0; num < len; num++){
		for (row = 0; row < len; row++){
			if (row_arrays_mat[num][row] == 0){
				num_of_consts = 0;
				for (col = 0; col<len; col++){
					if (cube[row][col][num] == 0){
						ind[num_of_consts] = find_index_in_var(cube, row, col, num, len);
						val[num_of_consts] = 1;
						num_of_consts++;
					}
				}
/*		  add constraint to model*/
  			sprintf(const_name, "one_num_per_row n = %d, r = %d\n", num, row);
  			error = GRBaddconstr(model, num_of_consts, ind, val, GRB_EQUAL, 1.0, const_name);
  			if (error) {
  				printf("ERROR %d %s GRBaddconstr(): %s\n", error, const_name, GRBgeterrormsg(env));
  			    GRBfreemodel(model); GRBfreeenv(env);
  				free_arrs(ind, val, obj, vtype, const_name, ub);
  				return -1;
  			}
  			constraints++;
      }
		}
	}


  /* second constraint: one of in num in col
   * for row in [0,len-1], sum(Xrow,col,num) = 1*/

  	constraints = 0;
    num_of_consts = 0;
	for (num = 0; num < len; num++){
		for (col = 0; col < len; col++){
			if (col_arrays_mat[num][col] == 0){
				num_of_consts = 0;
				for (row = 0; row<len; row++){
					if (cube[row][col][num] == 0){
						ind[num_of_consts] = find_index_in_var(cube, row, col, num, len);
						val[num_of_consts] = 1;
						num_of_consts++;
					}
				}
/*		  add constraint to model*/
  			sprintf(const_name, "one_num_per_col n = %d, c = %d\n", num, col);
  			error = GRBaddconstr(model, num_of_consts, ind, val, GRB_EQUAL, 1.0, const_name);
  			if (error) {
  				printf("ERROR %d %s GRBaddconstr(): %s\n", error, const_name, GRBgeterrormsg(env));
  			    GRBfreemodel(model); GRBfreeenv(env);
  				free_arrs(ind, val, obj, vtype, const_name, ub);
  				return -1;
  			}
  			constraints++;
      }
		}
	}


/* third constraint: one num in each cell
 * for num in [0,len-1], sum(Xrow,col,num) = 1*/

	constraints = 0;
	for (row = 0; row < len; row++){
		for (col = 0; col < len; col++){
			if (matrix[row][col] == 0){
				num_of_consts = 0;
				for (num = 0; num<len; num++){
					if (cube[row][col][num] == 0){
						ind[num_of_consts] = find_index_in_var(cube, row, col, num, len);
						val[num_of_consts] = 1;
						num_of_consts++;
					}
				}

/*		    add constraint to model*/
  			sprintf(const_name, "one_num_per_cell r = %d, c = %d\n", row, col);
  			error = GRBaddconstr(model, num_of_consts, ind, val, GRB_EQUAL, 1.0, const_name);
  			if (error) {
  				printf("ERROR %d %s GRBaddconstr(): %s\n", error, const_name, GRBgeterrormsg(env));
  				GRBfreemodel(model); GRBfreeenv(env);
  				free_arrs(ind, val, obj, vtype, const_name, ub);
  				return -1;
  			}
  			constraints++;
        }
		}
	}

/* forth constraint: one num in each block
 * for num in [0,len-1], sum(Xrow,col,num) = 1*/

constraints = 0;
for (num = 0; num < len; num++){
	for (block = 0; block < len; block++){
		if (block_arrays_mat[num][block] == 0){
			num_of_consts = 0;
			row_start = block_to_row(block, n);
			col_start = block_to_col(block, n, m);
			for (row = row_start; row < row_start+n; row++){
				for (col = col_start; col < col_start+m; col++){
					if (cube[row][col][num] == 0){
						ind[num_of_consts] = find_index_in_var(cube, row, col, num, len);
						val[num_of_consts] = 1;
						num_of_consts++;
					}
				}
			}


/*		add constraint to model*/
		sprintf(const_name, "one_num_per_block n = %d, bl = %d\n", num, block);
		error = GRBaddconstr(model, num_of_consts, ind, val, GRB_EQUAL, 1.0, const_name);
		if (error) {
			printf("ERROR %d %s GRBaddconstr(): %s\n", error, const_name, GRBgeterrormsg(env));
			GRBfreemodel(model); GRBfreeenv(env);
			free_arrs(ind, val, obj, vtype, const_name, ub);
			return -1;
		}
		constraints++;
	    }
	}
}


/*   Optimize model - need to call this before calculation*/
  error = GRBoptimize(model);
  if (error) {
	  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model); GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }

/*   Write model to 'mip1.lp' - this is not necessary but very helpful*/
  error = GRBwrite(model, "mip1.lp");
  if (error) {
	  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model); GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }

/*   Get solution information*/
  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) {
	  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model); GRBfreeenv(env);
	  free_arrs(ind, val, obj, vtype, const_name, ub);
	  return -1;
  }

  /* get the objective -- the optimal result of the function */

  /*if (LP_or_ILP == LP){
	  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	  if (error) {
		  printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
		  return -1;
	  }
  }*/

	if (optimstatus == GRB_OPTIMAL) {
/*		 get the solution - the assignment to each variable*/
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, num_of_vars, sol);
		if (error) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
			GRBfreemodel(model); GRBfreeenv(env);
			free_arrs(ind, val, obj, vtype, const_name, ub);
			return -1;
		}
	}

/*   print results*/
/*  printf("\nOptimization complete\n");
  printf("optimstatus = %d\n", optimstatus);*/

/*  solution found*/
/*  if (optimstatus == GRB_OPTIMAL) {
	    printf("Optimal objective: %.4e\n", objval);
	    printf("Solution found\n");
  }*/
/*  no solution found*/
 /* else if (optimstatus == GRB_INF_OR_UNBD) {
    printf("Model is infeasible or unbounded\n");
  }*/
/*  error or calculation stopped*/
/*  else {
    printf("Optimization was stopped early\n");
  }*/

/*   IMPORTANT !!! - Free model and environment*/
  GRBfreemodel(model);
  GRBfreeenv(env);
  free_arrs(ind, val, obj, vtype, const_name, ub);
  return optimstatus;
}


void free_arrs(int *ind, double *val, double *obj, char *vtype, char *const_name, double *ub){
	free(ub);
	free(const_name);
	free(vtype);
	free(obj);
	free(val);
	free(ind);
}

void print_sol(double *sol, int num_of_vars){
	int i;
	printf("sol = ");
	for (i=0; i<num_of_vars; i++){
		printf("%f ", sol[i]);
	}
	printf("\n");
}
