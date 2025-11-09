#ifndef PRLIB_SPRAM_H
#define PRLIB_SPRAM_H

#include "common.h"

#include <nalib/namatrix.h>

#include <libdma.h>

class PrSPRAM_DATA {
public:
    PR_PADDING(unk0, 0xe0);
    NaMATRIX<float, 4, 4> unkE0;
    PR_PADDING(unk120, 0x180);
    sceDmaTag mEndDmaTag;
};

#endif /* PRLIB_SPRAM_H */
