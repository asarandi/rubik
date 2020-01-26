#ifndef SOLVER_H
# define SOLVER_H

# include "state.h"

void move_state(t_state *cube, int f, int move);

void solve(t_state *root);

t_state *solved_state();

#endif
