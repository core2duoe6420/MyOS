#ifndef _TTY_H
#define _TTY_H

#define TTY_MAP_SIZE 4000
#define TTY_NR 6

#include <sched.h>
#include <lib.h>
struct tty_struct {
    char map[TTY_MAP_SIZE];
    int cur_x;
    int cur_y;
    struct task_struct *cur_task;
    struct queue *kbq;
};

extern struct tty_struct *cur_tty;
extern struct tty_struct *ttys[TTY_NR];

extern void switch_tty(struct tty_struct *tty);

#endif
