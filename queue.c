#include "queue.h"

t_queue *queue_init() {
    t_queue *q;

    if (!(q = malloc(sizeof(*q)))) {
        fprintf(stderr, "%s: malloc() failed\n", __func__);
        return NULL;
    }
    q->first = q->last = NULL;
    return q;
}

int queue_is_empty(t_queue *q) {
    return ((!q->first) || (!q->last));
}

void queue_enqueue(t_queue *q, int value) {
    t_node *n;

    if (!q) {
        fprintf(stderr, "%s: no queue\n", __func__);
        return;
    }

    if (!(n = malloc(sizeof(*n)))) {
        fprintf(stderr, "%s: malloc() failed\n", __func__);
        return;
    }
    n->value = value;
    n->next = NULL;
    if (!q->first)
        q->first = n;
    if (q->last)
        q->last->next = n;
    q->last = n;
}

int queue_dequeue(t_queue *q) {
    int res;
    t_node *n;

    if ((!q) || (queue_is_empty(q))) {
        fprintf(stderr, "%s: no queue or empty queue\n", __func__);
        return -1;
    }
    n = q->first;
    q->first = n->next;
    if (!n->next)
        q->last = NULL;
    res = n->value;
    free(n);
    return res;
}

void queue_destroy(t_queue *q) {
    t_node *n, *m;

    if (!q)
        return;
    n = q->first;
    while (n) {
        m = n->next;
        free(n);
        n = m;
    }
    free(q);
}
