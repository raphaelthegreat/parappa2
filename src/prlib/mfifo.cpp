#include "mfifo.h"

#include "dma.h"
#include "spram.h"

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

void PrStartMfifo() {
    if (!mfifoProcessing) {
        mfifoProcessing = true;
        PrWaitDmaFinish(SCE_DMA_fromSPR);
        PrWaitDmaFinish(SCE_DMA_GIF);

        ((tD_CTRL*)D_CTRL)->MFD = 3;
        ((tD_CTRL*)D_CTRL)->RELE = 0;
        ((tD_CTRL*)D_CTRL)->RCYC = 0;
        
        *D8_MADR = (u_int)mfifoBase;
        *D2_TADR = (u_int)mfifoBase;

        /* MOD=1, TTE=1 */
        *(int*)D2_CHCR = 0x104;
    }
}

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
INCLUDE_ASM("asm/nonmatchings/prlib/mfifo", PrSendMfifo__FPC10_sceDmaTag);
#else
void PrSendMfifo(const sceDmaTag* tag) {
    u_int qwc = tag->qwc + 1;
    u_int size = qwc * 16;

    while ( *D8_CHCR & 0x100 );

    *D8_SADR = (u_int)tag & 0xfff0;
    *D8_QWC = qwc;

    while (1) {
        u_int v1 = *D2_TADR;
        u_int v0 = *D8_MADR;
        if (v1 == v0 || size < PrMfifoGetUnsentDataSize(v0, v1)) {
            break;
        }
    }

    *D8_CHCR |= (1<<8);
}
#endif

u_int PrMfifoUnsentDataSize() {
    return PrMfifoGetUnsentDataSize(*D2_TADR, *D8_MADR);
}
