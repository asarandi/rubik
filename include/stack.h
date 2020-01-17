#ifndef STACK_H
# define STACK_H

typedef struct s_stack {
    struct s_node *top;
} t_stack;

t_stack *stack_init();

int stack_is_empty(t_stack *s);

void stack_push(t_stack *s, void *data);

void *stack_pop(t_stack *s);

void stack_destroy(t_stack *s);

#endif
