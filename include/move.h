#ifndef MOVE_H
# define MOVE_H

typedef struct s_move {
    int sdlk;
    int kmod;
    char *str;
    int dir;
} t_move;

#endif
