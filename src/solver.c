#include <stdint.h>
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
        {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},    //only F2,B2,U2,D2,R2,L2 moves
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
    //for (i=hash=0; i<8; i++)
//        hash = (hash << 3) | cube->corner_orientations[i];

    for (i = 0; i < 12; i++)
        hash = (hash << 1) | (cube->edges[i] > 7);
    return hash;
}

size_t phase_three_hash(t_state *cube) {
    int i, j;
    size_t hash;

    for (i = 0; i < 12; i++)
        hash = (hash << 2) | (cube->edge_orientations[i] < 8 ? cube->edge_orientations[i] & 1 : 2);
    for (i = hash = 0; i < 8; i++)
        hash = (hash << 3) | (cube->corners[i] & 5);
    for (hash <<= 1, i = 0; i < 8; i++) {
        for (j = i + 1; j < 8; j++)
            hash ^= cube->corners[i] > cube->corners[j];
        hash <<= 1;
    }
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

size_t cube_hash(t_state *cube) {
    return fnv_hash(cube->edges, 40);
//    size_t a, b, c, d;
//    a = fnv_hash(cube->edges, 12);
//    b = fnv_hash(cube->edge_orientations, 12);
//    c = fnv_hash(cube->corners, 8);
//    d = fnv_hash(cube->corner_orientations, 8);
//    return a ^ b ^ c ^ d;
}

void bfs(t_state *root, const int *possible_moves, size_t (*hf)(), t_queue *steps) {
    t_state *node, *child, *solved, *ptr = NULL, *res = NULL;
    t_queue *q;
    t_stack *s = NULL;
    t_ht *config;
    int i, max_depth;

    q = queue_init();
    config = ht_init(hf);
  //  all = ht_init(cube_hash);

    node = clone(root);
    node->parent = NULL;
    node->direction = 0;
    node->move = -1;
    node->depth = 0;

    solved = solved_state();
    solved->direction = 1;
    solved->parent = NULL;
    solved->move = -1;
    solved->depth = 0;

    queue_enqueue(q, node);
    queue_enqueue(q, solved);
//    ht_insert(all, node);
  //  ht_insert(all, solved);

    max_depth = 0;
    printf("\n");

    while (!queue_is_empty(q)) {
        node = queue_dequeue(q);
        if (node->depth > max_depth)
        {
            max_depth = node->depth;
            printf("new depth = %d, num nodes = %lu\n", max_depth, config->num_keys);
        }
        if ((ptr = ht_find(config, node))) {
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
        }
        ht_insert(config, node);
        for (i = 0; i < 18; i++) {
            if (!possible_moves[i])
                continue;
            child = clone(node);
            child->parent = node;
            child->move = (char) i;
            child->direction = node->direction;
            child->depth = node->depth + 1;
            move_state(child, i / 3, i % 3);
            queue_enqueue(q, child);
//            ht_insert(all, child);
        }
    }
    printf("steps: ");
    while (!stack_is_empty(s)) {
        i = (intptr_t) stack_pop(s);
        queue_enqueue(steps, (void *) (intptr_t) i);
        move_state(root, i / 3, i % 3);
        printf("%s ", moves[i].str);
    }
    while ((ptr) && (ptr->parent)) {
        i = ptr->move;
        if ((i % 3) != 2)
            i = i - (i % 3) + ((i % 3) ^ 1);
        queue_enqueue(steps, (void *) (intptr_t) i);    //inverse
        move_state(root, i / 3, i % 3);
        printf("%s ", moves[i].str);
        ptr = ptr->parent;
    }
    printf("\n");
    if (s)
        stack_destroy(s);
    queue_destroy(q);
    ht_destroy_all(config);
    //ht_destroy_all(all);
}

void solve(t_state *input) {
    t_state *root;
    t_queue *steps;

    root = clone(input);
    steps = queue_init();

    printf("before steps\n");
    print_cube(root);

    bfs(root, allowed_moves[0], &phase_one_hash, steps);
    printf("after phase one\n");
    print_cube(root);

    bfs(root, allowed_moves[1], &phase_two_hash, steps);
    printf("after phase two\n");
    print_cube(root);

    bfs(root, allowed_moves[2], &phase_three_hash, steps);
    printf("after phase three\n");
    print_cube(root);

    bfs(root, allowed_moves[3], &cube_hash, steps);
    printf("after phase four\n");
    print_cube(root);

    free(root);
    queue_destroy(steps);
}
