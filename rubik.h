#ifndef RUBIK_H
# define RUBIK_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "queue.h"

typedef struct s_state {
    struct s_state *parent;
    char move;
    char edges[12];
    char edge_orientations[12];
    char corners[8];
    char corner_orientations[8];
} t_state;

void move_state(t_state *cube, int f, int move);

void print_cube(t_state *cube);

void solve(t_state *root);

#endif