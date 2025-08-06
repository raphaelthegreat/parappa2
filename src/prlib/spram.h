#ifndef PRLIB_SPRAM_H
#define PRLIB_SPRAM_H

#include "common.h"

#include <nalib/namatrix.h>

#include <libdma.h>

struct PrSPRAM_DATA {
    PR_PADDING(unk0, 0xe0);
    NaMATRIX<float, 4, 4> unkE0;
    PR_PADDING(unk120, 0x180);
    sceDmaTag unk2A0;
};

#endif /* PRLIB_SPRAM_H */
