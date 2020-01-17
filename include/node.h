#ifndef NODE_H
# define NODE_H

typedef struct s_node {
    void *data;
    struct s_node *next;
} t_node;

#endif
