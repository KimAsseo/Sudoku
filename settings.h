#ifndef SETTINGS_H_
#define SETTINGS_H_

/*#define settings for the program*/

#define TRUE 1
#define FALSE 0

#define ERROR -1
#define FLOATERROR -1.0

/*board settings*/
#define CURRENT 1

/*default sizes*/
#define N 3
#define M 3

/*Cell fields*/
#define FIXED 1
#define VAL 2
#define ERRONEOUS 3
#define CHOSEN 4
#define TEMP 5

/*Game fields*/
#define NUM_OF_FILLED 5
#define GAME_MODE 6
#define MARK_ERRORS_FIELD 7

/*game modes*/
#define INITMODE 100
#define EDITMODE 200
#define SOLVEMODE 300

/*move settings*/
#define EXIT - 1
#define RESET 1
#define SET 2
#define HINT 3
#define VALIDATE 4
#define INVALID 5
/*to be handled!!!*/
/*#define NULL 6*/
#define GUESS_HINT 7
#define SAVE 8
#define UNDO 9
#define REDO 10
#define SOLVE 11
#define EDIT 12
#define MARK_ERRORS 13
#define PRINT_BOARD 14
#define GUESS 15
#define GENERATE 16
#define NUM_SOLUTIONS 17
#define AUTOFILL 18

#define BUFFER_SIZE 128
#define CHARMAX 256
#define DELIM " \t\r\n"

/*type of move*/
#define SETMOVE 0
#define BOARDMOVE 1

/* Gurobi Model status codes (after call to GRBoptimize())*/
#define GRB_LOADED          1
#define GRB_OPTIMAL         2
#define GRB_INFEASIBLE      3

/*defines for gurobi*/
#define LP 15
#define ILP 16

#define STRING "\\"

/*destroy list - from head or from current move.*/
#define HEAD 1
#define CURR 2

#endif /* SETTINGS_H_ */

