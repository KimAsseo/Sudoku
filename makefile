CC = gcc
OBJS = main.o Game_aux.o Game.o Errors.o general_funcs.o Solver.o Gurobi.o cell_change_move.o parser.o stack.o
 
EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Game.h SPBufferset.h Game_aux.h settings.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
	
Game_aux.o: Game.h Errors.h Game_aux.h SPBufferset.h settings.h general_funcs.h stack.h moves_list.h parser.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
	
Game.o: Cell.h Game.h Game_aux.h Errors.h SPBufferset.h settings.h general_funcs.h parser.h cell_change_move.h Solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c

Gurobi.o: Gurobi.h Solver.h settings.h Errors.h general_funcs.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
	
Errors.o: Errors.h settings.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
	
general_funcs.o: general_funcs.h Game.h settings.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
	
Solver.o: Game.h settings.h Solver.h general_funcs.h Errors.h Gurobi.h Game_aux.h moves_list.h cell_change_move.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c

cell_change_move.o: cell_change_move.h Errors.h SPBufferset.h Game.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c

parser.o: SPBufferset.h Game.h Errors.h Game_aux.h parser.h settings.h general_funcs.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
	
stack.o: Errors.h stack.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c

clean:
	rm -f *.o $(EXEC) $(OBJS)
