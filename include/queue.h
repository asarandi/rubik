#ifndef QUEUE_H
# define QUEUE_H

typedef struct s_queue {
    struct s_node *first;
    struct s_node *last;
} t_queue;

t_queue *queue_init();

int queue_is_empty(t_queue *q);

void queue_enqueue(t_queue *q, void *data);

void *queue_dequeue(t_queue *q);

void queue_destroy(t_queue *q);

#endif