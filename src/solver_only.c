#include <ctype.h>
#include <stdint.h>
#include "common.h"
#include "move.h"
#include "queue.h"
#include "solver.h"
#include "utils.h"
#include "SDL.h"

t_move moves[] = {
        {SDLK_f, KMOD_NONE,  "F",  1},
        {SDLK_f, KMOD_LCTRL, "F'", -1},
        {SDLK_f, KMOD_LALT,  "F2", 2},
        {SDLK_b, KMOD_NONE,  "B",  -1},
        {SDLK_b, KMOD_LCTRL, "B'", 1},
        {SDLK_b, KMOD_LALT,  "B2", -2},
        {SDLK_u, KMOD_NONE,  "U",  1},
        {SDLK_u, KMOD_LCTRL, "U'", -1},
        {SDLK_u, KMOD_LALT,  "U2", 2},
        {SDLK_d, KMOD_NONE,  "D",  -1},
        {SDLK_d, KMOD_LCTRL, "D'", 1},
        {SDLK_d, KMOD_LALT,  "D2", -2},
        {SDLK_r, KMOD_NONE,  "R",  1},
        {SDLK_r, KMOD_LCTRL, "R'", -1},
        {SDLK_r, KMOD_LALT,  "R2", 2},
        {SDLK_l, KMOD_NONE,  "L",  -1},
        {SDLK_l, KMOD_LCTRL, "L'", 1},
        {SDLK_l, KMOD_LALT,  "L2", -2}
};

void read_input(int argc, char **argv, t_queue *q) {
    int i, j;
    char *s, *delim = " \t\n\r\f\v";

    for (i = 1; i < argc; i++) {
        s = argv[i];
        for (j = 0; s[j]; j++)
            s[j] = (char) toupper(s[j]);
        s = strtok(s, delim);
        while (s) {
            for (j = 0; j < 18; j++) {
                if (!strcmp(s, moves[j].str)) {
                    queue_enqueue(q, (void *) (intptr_t) j);
                    break;
                }
            }
            s = strtok(NULL, delim);
        }
    }
}

int main(int ac, char **av) {
    t_state *cube;
    t_queue *q;
    int i;

    q = queue_init();
    read_input(ac, av, q);
    cube = solved_state();
    while (!queue_is_empty(q)) {
        i = (intptr_t) queue_dequeue(q);
        move_state(cube, i / 3, i % 3);
    }
    print_cube(cube);
    solve(cube);
    free(cube);
    queue_destroy(q);
    return (0);
}
