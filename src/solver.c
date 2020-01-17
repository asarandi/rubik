#include "queue.h"
#include "hashtable.h"
#include "utils.h"

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

int is_phase_one(t_state *cube) {
    int i;

    for (i = 0; i < 12; i++) {
        if (cube->edge_orientations[i])
            return 0;
    }
    return 1;
}

int is_phase_two(t_state *cube) {
    int i;

    for (i = 0; i < 8; i++) {
        if (cube->corner_orientations[i])
            return 0;
    }
    return 1;
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

void solve(t_state *root) {
    t_state *node, *child;
    t_queue *q;
    t_ht *ht;
    int i;

    q = queue_init();
    ht = ht_init();
    node = clone(root);
    node->parent = NULL;
    queue_enqueue(q, node);
    ht_insert(ht, node);
    while (!queue_is_empty(q)) {
        node = queue_dequeue(q);
        if (is_phase_one(node)) {
            printf("found!\n");
            while (node) {
                print_cube(node);
                printf("\n");
                node = node->parent;
            }
            break;
        }
        for (i = 0; i < 18; i++) {
            if (!allowed_moves[0][i])
                continue;
            child = clone(node);
            child->parent = node;
            child->move = (char) i;
            move_state(child, i / 3, i % 3);
            if (ht_find(ht, child)) {
                free(child);
                continue;
            }
            ht_insert(ht, child);
            queue_enqueue(q, child);
        }
    }
    queue_destroy(q);
    ht_destroy_all(ht);
}
