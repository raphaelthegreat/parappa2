#ifndef PRLIB_SPRAM_H
#define PRLIB_SPRAM_H

#include "common.h"

#include <libdma.h>

struct PrSPRAM_DATA {
    PR_PADDING(unk0, 0x2a0);
    sceDmaTag unk2A0;
};

#endif /* PRLIB_SPRAM_H */
