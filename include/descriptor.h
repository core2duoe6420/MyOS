#ifndef __DESCRIPTOR_H
#define __DESCRIPTOR_H

#include <lib.h>
#include <const.h>

struct desc_struct {
    uint32 limit_low: 16;
    uint32 base_low: 16;
    uint32 base_mid: 8;
    uint32 type: 4;
    uint32 s_flag: 1;
    uint32 dpl: 2;
    uint32 present: 1;
    uint32 limit_high: 4;
    uint32 avl: 1;
    uint32 reserved: 1;
    uint32 db: 1;
    uint32 granularity: 1;
    uint32 base_high: 8;
};

#endif
