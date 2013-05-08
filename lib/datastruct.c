#include <const.h>
#include <memory.h>
#include <page.h>

struct queue* new_queue()
{
    struct page_struct* q_page;
    struct queue* q;
    q_page = alloc_page(0, PAGE_DATASTRUCT);
    q = (struct queue*)page_to_address(q_page);
    q->head = 1;
    q->tail = 0;
    q->size = 0;
    q->flags = 0;
    q->wait = NULL;
    return q;
}

void enqueue(struct queue *q, char value)
{
    q->size++;
    if(q->size > (int)sizeof(q->value))
        return;
    if(q->tail == sizeof(q->value) - 1)
        q->tail = 0;
    else
        q->tail++;
    q->value[q->tail] = value;
}

char dequeue(struct queue *q)
{
    char tmp;
    if(!q->size)
        return 0;
    q->size--;
    tmp = q->value[q->head];
    if(q->head == sizeof(q->value) - 1)
        q->head = 0;
    else
        q->head++;
    return tmp;
}

struct stack* new_stack()
{
    struct page_struct *s_page;
    struct stack *s;
    s_page = alloc_page(0, PAGE_DATASTRUCT);
    s = (struct stack*)page_to_address(s_page);
    s->top = 0;
    s->flags = 0;
    s->wait = NULL;
    return s;
}

void push(struct stack *s, char value)
{
    if(s->top == sizeof(s->value))
        return;
    s->value[s->top] = value;
    s->top++;
}

char pop(struct stack *s)
{
    if(!s->top)
        return 0;
    s->top--;
    return s->value[s->top];
}

char peek(struct stack *s)
{
    if(!s->top)
        return 0;
    return s->value[s->top - 1];
}
