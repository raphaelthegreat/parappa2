#include "prlib/mfifo.h"

#include "prlib/dma.h"
#include "prlib/spram.h"

#include <eeregs.h>
#include <libdma.h>

#define PR_MFIFOSIZE (0x10000)
extern char mfifoBase[PR_MFIFOSIZE] __attribute__((section(".mfifo")));

static PrSPRAM_DATA* prSpramData = (PrSPRAM_DATA*)0x70000000;
static bool mfifoProcessing = false;

static inline u_int PrMfifoGetUnsentDataSize(u_int arg0, u_int arg1) {
    return (arg1 - arg0) & 0xfff0;
}

void PrInitializeMfifo() {
    *D_RBOR = (u_int)mfifoBase;
    *D_RBSR = PR_MFIFOSIZE - 0x10;
}

void PrCleanupMfifo() {
    PrStopMfifo();
}

#ifndef NON_MATCHING
INCLUDE_ASM("prlib/mfifo", PrStartMfifo__Fv);
#else
void PrStartMfifo() {
    if (mfifoProcessing) {
        return;
    }

    mfifoProcessing = true;
    PrWaitDmaFinish(SCE_DMA_fromSPR);
    PrWaitDmaFinish(SCE_DMA_GIF);

    ((tD_CTRL*)D_CTRL)->MFD = 3;
    ((tD_CTRL*)D_CTRL)->RELE = 0;
    ((tD_CTRL*)D_CTRL)->RCYC = 0;
    
    *D8_MADR = (u_int)mfifoBase;
    *D2_TADR = (u_int)mfifoBase;

    // ((tD_CHCR*)D2_CHCR)->MOD = 1;
    // ((tD_CHCR*)D2_CHCR)->TTE = 1;
    *D2_CHCR = 0x104;
}
#endif

void PrStopMfifo() {
    if (mfifoProcessing) {
        PrSendMfifo(&prSpramData->unk2A0);
        PrWaitDmaFinish(SCE_DMA_GIF);

        /*
         * Disable MFIFO drain and cycle stealing.
         */
        ((tD_CTRL*)D_CTRL)->MFD = 0;
        ((tD_CTRL*)D_CTRL)->RELE = 0;

        mfifoProcessing = false;
    }
}

void PrWaitMfifo() {
    if (mfifoProcessing) {
        while (*D8_CHCR & (1<<8));
        while (*D2_TADR != *D8_MADR);
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("prlib/mfifo", PrSendMfifo__FPC10_sceDmaTag);
#else
void PrSendMfifo(const sceDmaTag* tag) {
    u_int a0 = tag->qwc + 1;
    u_int a2 = a0 * 16;

    while ( *D8_CHCR & 0x100 );

    *D8_SADR = (u_int)tag & 0xfff0;
    *D8_QWC = a0;

    u_int v0;
    u_int v1;
    while (1) {
        v1 = *D2_TADR;
        v0 = *D8_MADR;
        if (v1 == v0 || a2 < PrMfifoGetUnsentDataSize(v0, v1)) {
            break;
        }
    }

    *D8_CHCR |= (1<<8);
}
#endif

u_int PrMfifoUnsentDataSize() {
    return PrMfifoGetUnsentDataSize(*D2_TADR, *D8_MADR);
}
