#include <lib.h>
#include <tty.h>
#include <sched.h>
#include <signal.h>

struct queue *kbq[TTY_NR];

#define LEFT_CTRL 1
#define LEFT_SHIFT 2
#define RIGHT_SHIFT 3
#define LEFT_ALT 4

#define SHIFT_BIT 0x1
#define CTRL_BIT 0x2
#define ALT_BIT 0x4

#define KB_ECHO 0x1

static char kb_map[] = {
    0,									/* 0 */
    27, '1', '2', '3', '4', '5', '6', '7',		/* 1-8 */
    '8', '9', '0', '-', '=', 8, 9, 'q',		/* 9-16 */
    'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',	/* 17-24 */
    'p', '[', ']', 10, LEFT_CTRL, 'a', 's', 'd',	/* 25-32 */
    'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',	/* 33-40 */
    '`', LEFT_SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', /* 41-48 */
    'n', 'm', ',', '.', '/', RIGHT_SHIFT, 0, LEFT_ALT,	/* 49-56 */
    ' ', 0, 0, 0, 0, 0, 0, 0,					/* 57-64 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 65-72 */
    0, '-', 0, 0, 0, '+', 0, 0,				/* 73-80 */
    0, 0, 0,								/* 81-83 */
    0, 0, 0, 0, 0,							/* 84-88 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 89-96 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 97-104 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 105-112 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 113-120 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 121-128 */
    0, '!', '@', '#', '$', '%', '^', '&',		/* 129-136 */
    '*', '(', ')', '_', '+', 0, 0, 'Q',		/* 137-144 */
    'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',	/* 145-152 */
    'P', '{', '}', 0, LEFT_CTRL + 0x80, 'A', 'S', 'D',		/* 153-160 */
    'F', 'G', 'H', 'J', 'K', 'L', ':', '"',	/* 161-168 */
    '~', LEFT_SHIFT + 0x80, '|', 'Z', 'X', 'C', 'V', 'B',		/* 169-176 */
    'N', 'M', '<', '>', '?', RIGHT_SHIFT + 0x80, 0, LEFT_ALT + 0x80,	/* 177-184 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 184-192 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 193-200 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 201-208 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 209-216 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 217-224 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 225-232 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 233-240 */
    0, 0, 0, 0, 0, 0, 0, 0,					/* 241-248 */
    0, 0, 0, 0, 0, 0, 0,						/* 248-255 */
};

void keyboard_initial()
{
    for(int i = 0; i < TTY_NR; i++)
        kbq[i] = new_queue();
}

void do_keyboard_intr(int code)
{
    static int state = 0;
    if((code > 127 && code != 0x2a + 0x80 && code != 0x36 + 0x80 && code != 0x1d + 0x80)
       || code == 0xe0 || code == 0xe1)
        return;
    unsigned char ch = kb_map[code];
    switch(ch) {
    default:
        break;
    case LEFT_ALT:
        return;
    case LEFT_CTRL:
        state |= CTRL_BIT;
        return;
    case LEFT_CTRL+0x80:
        state &= ~CTRL_BIT;
        return;
    case LEFT_SHIFT:
    case RIGHT_SHIFT:
        state |= SHIFT_BIT;
        return;
    case LEFT_SHIFT+0x80:
    case RIGHT_SHIFT+0x80:
        state &= ~SHIFT_BIT;
        return;
    }
    if(state & CTRL_BIT) {
        if(code >= 0x3b && code <= 0x40) {
            switch_tty(ttys[code - 0x3b]);
            return;
        } else if(code == 0x2e) {
            set_signal(cur_tty->cur_task->signal, SIG_KILL);
            return;
        }
    }
    if(state & SHIFT_BIT)
        ch = kb_map[code + 0x80];

    if(cur_tty->kbq->flags & KB_ECHO) {
        enqueue(cur_tty->kbq, ch);
        wake_up(&cur_tty->kbq->wait);
    }
}
