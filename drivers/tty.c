#include <memory.h>
#include <string.h>
#include <io.h>
#include <tty.h>
#include <page.h>
#include <sched.h>

#define DISP_PTR ((char*)__va(0xb8000))
#define CLEAR_OFFSET (80*2)
#define CLEAR_END_OFFSET (1920*2)

struct tty_struct *cur_tty;
struct tty_struct *ttys[TTY_NR];

static int tty_initialized = 0;
static int screen_x = 0;
static int screen_y = 0;

extern struct queue *kbq[TTY_NR];
extern void keyboard_initial();

static void screen_up(char *ptr)
{
    for(int i = 0; i < 1920; i++)
        *(ptr + i * 2) = *(ptr + CLEAR_OFFSET + i * 2);
    for(int i = 0; i < CLEAR_OFFSET; i += 2)
        * (ptr + CLEAR_END_OFFSET + i) = 0;
}

static void _initial_display(char *ptr)
{
    for(int i = 0; i < 4000; i += 2) {
        *(ptr + i) = 0;
        *(ptr + i + 1) = 7;
    }
}


void initial_display()
{
    _initial_display(DISP_PTR);
}

void initial_tty()
{
    struct page_struct *page;

    keyboard_initial();
    for(int i = 0; i < TTY_NR; i++) {
        page = alloc_page(0, PAGE_TTY | PAGE_KERNEL);
        ttys[i] = (struct tty_struct *)page_to_address(page);
        ttys[i]->cur_x = 0;
        ttys[i]->cur_y = 0;
        ttys[i]->cur_task = NULL;
        ttys[i]->kbq = kbq[i];
        _initial_display(ttys[i]->map);
    }
    cur_tty = ttys[0];
    tty_initialized = 1;
}

static inline void refresh_cursor(int offset)
{
    outb(0x0f, 0x3d4);
    outb(offset & 0xff, 0x3d5);
    outb(0x0e, 0x3d4);
    outb(offset >> 8, 0x3d5);
}

static void _display(char *str, char *ptr, int *x, int *y)
{
    int offset;

    offset = ((*x) * 80 + (*y)) * 2;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == 0xA) {
            (*x)++;
            (*y) = 0;
            if((*x) == 25) {
                screen_up(ptr);
                (*x) = 24;
            }
            offset = ((*x) * 80 + (*y)) * 2;
            continue;
        } else if(str[i] == 0x8) {
            if((*y)) {
                (*y)--;
                offset -= 2;
                *(ptr + offset) = 0;
            }
            continue;
        } else {
            (*y)++;
            if((*y) == 80) {
                (*y) = 0;
                (*x)++;
            }
            if((*x) == 25) {
                screen_up(ptr);
                (*x) = 24;
                offset -= 160;
            }
            *(ptr + offset) = str[i];
            offset = ((*x) * 80 + (*y)) * 2;
        }
    }
    if((!tty_initialized) || cur_tty == current->tty)
        refresh_cursor(offset >> 1);
}


void display(char *str)
{
    if(tty_initialized)
        _display(str, current->tty->map, &current->tty->cur_x, &current->tty->cur_y);
    if((!tty_initialized) || current->tty == cur_tty)
        _display(str, DISP_PTR, &screen_x, &screen_y);
}

void switch_tty(struct tty_struct *tty)
{
    struct tty_struct *tmp;
    tmp = cur_tty;

    /* switch to the current tty? */
    if(tmp == tty)
        return;

    cli();
    /* save screen to current tty */
    memcpy(tmp->map, DISP_PTR, TTY_MAP_SIZE);
    tmp->cur_x = screen_x;
    tmp->cur_y = screen_y;
    /* restore required tty to screen */
    memcpy(DISP_PTR, tty->map, TTY_MAP_SIZE);
    screen_x = tty->cur_x;
    screen_y = tty->cur_y;

    cur_tty = tty;
    refresh_cursor(tty->cur_x * 80 + tty->cur_y);
    sti();
}

void gets(char *dest)
{
    char ch = 0;
    char buffer[256];
    int i = 0, j = 0;
    struct queue *kbq;

    kbq = current->tty->kbq;
    kbq->flags |= 0x1;
    do {
        /* add counter to give good response to interactive */
        current->counter = 200;
        sleep_on(&kbq->wait);

        cli();
        while(kbq->size) {
            ch = dequeue(kbq);
            printk("%c", ch);
            if(ch == 0x0a)
                break;
            if(ch != 0x8) {
                buffer[i++] = ch;
            } else {
                i--;
                if(i < 0)
                    i = 0;
            }
        }
        sti();
    } while(ch != 0xa);
    kbq->flags &= ~0x1;
    for(j = 0; j < i; j++)
        dest[j] = buffer[j];
    buffer[j] = 0;
    dest[j] = 0;
//	printk("%s\n",buffer);
}
