#ifndef HASHTABLE_H
# define HASHTABLE_H

# include "common.h"
# include "state.h"

typedef struct s_hash_table {
    size_t capacity;
    size_t num_keys;
    void **entries;
} t_ht;

t_state *ht_find(t_ht *ht, t_state *cube);

t_ht *ht_init();

void ht_insert(t_ht *ht, t_state *cube);

void ht_destroy_all(t_ht *ht);

#endif
