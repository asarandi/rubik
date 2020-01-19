#include "hashtable.h"
#include "move.h"
#include "queue.h"
#include "stack.h"
#include "utils.h"

extern t_move moves[];
int allowed_moves[][18] = {

//   f,f',f2,  b,b',b2,  u,u',u2,  d,d',d2,  r,r',r2,  l,l',l2
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},    //all moves
        {0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},    //all except F,F',B,B'
        {0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},    //all except F,F',B,B',R,R',L,L'
        {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},    //only F2,B2,U2,D2,R2,L2
};

/*
 * search:
 *  G0 = <F,B,U,D,R,L>          <<-- all states
 *  G1 = <F2,B2,U,D,R,L>        <<-- orientation of edges (only F,F',B,B' change corner orientations)
 *  G2 = <F2,B2,U,D,R2,L2>      <<-- orientation of corners
 *  G3 = <F2,B2,U2,D2,R2,L2>
 *  G4 = goal
*/

void move_state(t_state *cube, int f, int move) {
    int i, j, k;

    int rotations[3][4] = {
            {1, 2, 3, 0},   // clockwise
            {3, 0, 1, 2},   // anti-clockwise
            {2, 3, 0, 1}    // twice
    };

    int moving_edges[6][4] = {

            {0, 9,  4, 8},   // F
            {2, 10, 6, 11},  // B
            {0, 1,  2, 3},   // U
            {4, 7,  6, 5},   // D
            {1, 8,  5, 10},  // R
            {3, 11, 7, 9},   // L
    };

    int moving_corners[6][4] = {
            {0, 3, 5, 4},   // F
            {2, 1, 7, 6},   // B
            {0, 1, 2, 3},   // U
            {4, 5, 6, 7},   // D
            {1, 0, 4, 7},   // R
            {3, 2, 6, 5},   // L
    };

    t_state clone;

    (void) memcpy(&clone, cube, sizeof(*cube));
    for (i = 0; i < 4; i++) {
        j = moving_edges[f][i];
        k = moving_edges[f][rotations[move][i]];
        cube->edges[j] = clone.edges[k];
        cube->edge_orientations[j] = clone.edge_orientations[k];
        if ((f < 2) && (move < 2))
            cube->edge_orientations[j] ^= 1;

        j = moving_corners[f][i];
        k = moving_corners[f][rotations[move][i]];
        cube->corners[j] = clone.corners[k];
        cube->corner_orientations[j] = clone.corner_orientations[k];
        if ((f != 2) && (f != 3) && (move < 2))
            cube->corner_orientations[j] = (cube->corner_orientations[j] + (2 - (i & 1))) % 3;
    }
    cube->move = (char) f * 3 + move;
}

size_t phase_one_hash(t_state *cube) {
    return fnv_hash(cube->edge_orientations, 12);
}

size_t phase_two_hash(t_state *cube) {
    int i;
    size_t hash;

    hash = fnv_hash(cube->corner_orientations, 8);
    for (i = 0; i < 12; i++)
        hash ^= (cube->edge_orientations[i] > 7) << i;
    return hash;
}

int is_solved(t_state *cube) {
    int i;

    for (i = 0; i < 12; i++) {
        if (cube->edges[i] != i)
            return 0;
        if (cube->edge_orientations[i])
            return 0;
        if (i > 7)
            continue;
        if (cube->corners[i] != i)
            return 0;
        if (cube->corner_orientations[i])
            return 0;
    }
    return 1;
}

t_state *clone(t_state *src) {
    return memcpy(calloc(1, sizeof(*src)), src, sizeof(*src));
}

t_state *solved_state() {
    int i;
    t_state *res;

    if (!(res = calloc(1, sizeof(*res))))
        return NULL;
    for (i = 0; i < 12; i++) {
        res->edges[i] = (char) i;
        res->edge_orientations[i] = 0;
        if (i > 7)
            continue;
        res->corners[i] = (char) i;
        res->corner_orientations[i] = 0;
    }
    return res;
}


t_state *bfs(t_state *root, const int *possible_moves, size_t (*hf)(), t_queue *steps) {
    t_state *node, *child, *solved, *ptr = NULL, *res = NULL;
    t_queue *q;
    t_stack *s = NULL;
    t_ht *ht;
    int i;

    q = queue_init();
    ht = ht_init(hf);
    node = clone(root);
    node->parent = NULL;
    node->direction = 0;
    node->move = -1;
    solved = solved_state();
    solved->direction = 1;
    solved->parent = NULL;
    solved->move = -1;
    queue_enqueue(q, node);
    queue_enqueue(q, solved);
    while (!queue_is_empty(q)) {
        node = queue_dequeue(q);
        if ((ptr = ht_find(ht, node))) {
            if (ptr->direction != node->direction) {
                if (node->direction)
                    res = node, node = ptr, ptr = res;
                s = stack_init();
                while (node->parent) {
                    stack_push(s, (void *) (intptr_t) node->move);
                    node = node->parent;
                }
                break;
            }
            free(node);
            continue;
        } else {
            ht_insert(ht, node);
        }
        for (i = 0; i < 18; i++) {
            if (!possible_moves[i])
                continue;
            child = clone(node);
            child->parent = node;
            child->move = (char) i;
            child->direction = node->direction;
            move_state(child, i / 3, i % 3);
            queue_enqueue(q, child);
        }
    }
    printf("steps: ");
    while (!stack_is_empty(s)) {
        i = (intptr_t) stack_pop(s);
        queue_enqueue(steps, (void *) (intptr_t) i);
        printf("%s ", moves[i].str);
    }
    while (ptr) {
        queue_enqueue(steps, (void *) (intptr_t) ptr->move);    //INVERSE
        printf("%s ", moves[(int) ptr->move].str);
        if (!ptr->parent)
            res = clone(ptr);
        ptr = ptr->parent;
    }
    printf("\n");
    if (s)
        stack_destroy(s);
    queue_destroy(q);
    ht_destroy_all(ht);
    return res;
}

void solve(t_state *cube) {
    t_state *phase_one, *phase_two;
    t_queue *steps;

    steps = queue_init();
    phase_one = bfs(cube, allowed_moves[0], &phase_one_hash, steps);
    phase_two = bfs(phase_one, allowed_moves[1], &phase_two_hash, steps);

    print_cube(phase_two);
    free(phase_two);
    free(phase_one);
}