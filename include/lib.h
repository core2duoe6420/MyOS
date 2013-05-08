#ifndef __LIB_H
#define __LIB_H

#include <const.h>

extern int printk(const char *fmt, ...);

#ifndef __UINT32
#define __UINT32
typedef unsigned long uint32;
#endif
#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

#undef NULL
#define NULL ((void *)0)

struct queue {
    volatile int flags;
    int head;
    int tail;
    int size;
    struct task_struct *wait;
    char value[PAGE_SIZE - sizeof(int) * 5];
};
struct queue* new_queue();
void enqueue(struct queue* q, char value);
char dequeue(struct queue*q);

struct stack {
    volatile int flags;
    int top;
    struct task_struct *wait;
    char value[PAGE_SIZE - sizeof(int) * 3];
};
struct stack* new_stack();
void push(struct stack *s, char value);
char pop(struct stack *s);
char peek(struct stack *s);

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
