#ifndef RUBIK_H
# define RUBIK_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

void move_state(t_state *cube, int f, int move);

void print_cube(t_state *cube);

void solve(t_state *root);


/* hash table */
typedef struct s_hash_table {
    size_t capacity;
    size_t num_keys;
    void **entries;
} t_ht;

t_state *ht_find(t_ht *ht, t_state *cube);

t_ht *ht_init();

void ht_insert(t_ht *ht, t_state *cube);

void ht_destroy_all(t_ht *ht);


/* stack */
typedef struct s_stack {
    struct s_node *top;
} t_stack;

t_stack *stack_init();

int stack_is_empty(t_stack *s);

void stack_push(t_stack *s, void *data);

void *stack_pop(t_stack *s);

void stack_destroy(t_stack *s);


#endif