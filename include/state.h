#ifndef STATE_H
# define STATE_H

typedef struct s_state {
    size_t hash;
    struct s_state *parent;
    char direction;
    char move;
    char edges[12];
    char edge_orientations[12];
    char corners[8];
    char corner_orientations[8];
} t_state;

#endif
