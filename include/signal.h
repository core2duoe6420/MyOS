#ifndef _SIGNAL_H
#define _SIGNAL_H

#define SIG_KILL 1

#define set_signal(mask,sig) (mask)|=(1<<((sig)-1))
#define clear_signal(mask,sig) (mask)&=(~(1<<((sig)-1)))

#endif