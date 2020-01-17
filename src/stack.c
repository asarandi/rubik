#include "common.h"
#include "node.h"
#include "stack.h"

t_stack *stack_init() {
    t_stack *s;

    if (!(s = malloc(sizeof(*s)))) {
        fprintf(stderr, "%s: malloc() failed\n", __func__);
        return NULL;
    }
    s->top = NULL;
    return s;
}

int stack_is_empty(t_stack *s) {
    if (!s) {
        fprintf(stderr, "%s: no stack\n", __func__);
        return 1;
    }
    return (!s->top);
}

void stack_push(t_stack *s, void *data) {
    t_node *n;

    if (!s) {
        fprintf(stderr, "%s: no stack\n", __func__);
        return;
    }
    if (!(n = malloc(sizeof(*n)))) {
        fprintf(stderr, "%s: malloc() failed\n", __func__);
        return;
    }
    n->data = data;
    n->next = s->top;
    s->top = n;
}

void *stack_pop(t_stack *s) {
    void *res;
    t_node *n;

    if (stack_is_empty(s)) {
        return NULL;
    }
    n = s->top;
    s->top = n->next;
    res = n->data;
    free(n);
    return res;
}

void stack_destroy(t_stack *s) {
    t_node *n, *m;

    if (!s)
        return;
    n = s->top;
    while (n) {
        m = n->next;
        /* free(n->data) */
        free(n);
        n = m;
    }
    free(s);
}
