#include "prlib/renderstuff.h"

#include "prlib/gifreg.h"
#include "prlib/mfifo.h"
#include "prlib/microprogram.h"
#include "prlib/scene.h"

#include <eeregs.h>
#include <libgraph.h>

#include <stdlib.h>

PrRenderStuff::PrRenderStuff() : mDmaQueue(1200) {
    this->unk10 = 0;
    this->unk14 = 0;
    this->unk18 = NULL;

    AllocateTransmitDmaArray(600);

    this->unk1C = NULL;
}

PrRenderStuff::~PrRenderStuff() {
    Cleanup();
}

void PrRenderStuff::Initialize(sceGsZbuf zbuf) {
    this->unk20 = zbuf;
    this->unk1C = NULL;
    this->unk28 = 0;

    PrLoadMicroPrograms();

    PrInitializeDmaStripGifRegister(zbuf);
    PrInitializeMfifo();

    /*
     * Disable VIF DMAtag mismatch errors to
     * avoid HW bug where these are triggered
     * with valid packets, causing stalls.
     *
     * Though these are already guaranteed
     * to be set from the sceDevVif0Reset()
     * and sceGsResetPath() calls on
     * InitSystem() (os/system.c).
     */
    *VIF0_ERR |= (1<<1);
    *VIF1_ERR |= (1<<1);
}

void PrRenderStuff::Cleanup() {
    PrCleanupMfifo();

    delete this->unk18;

    this->unk10 = 0;
    this->unk14 = 0;
    this->unk18 = NULL;
    this->unk1C = NULL;
}

u_int PrRenderStuff::GetZbufBits(void) const {
    switch (this->unk20.PSM) {
    case 0:  /* PSMZ32 */
        return 32;
    case 1:  /* PSMZ24 */
        return 24;
    case 2:  /* PSMZ16 */
        return 16;
    case 10: /* PSMZ16S */
        return 16;
    default:
        break;
    }

    return 0;
}

void PrRenderStuff::ResetStatistics() {
    mStatistics.node_num = 0;

    mStatistics.opaque_context1_node_num = 0;
    mStatistics.transmit_context1_node_num = 0;
    mStatistics.opaque_context2_node_num = 0;
    mStatistics.transmit_context2_node_num = 0;
    
    mStatistics.render_time0 = 0;
    mStatistics.render_time1 = 0;
    mStatistics.render_time2 = 0;
    mStatistics.render_time3 = 0;
    mStatistics.render_time4 = 0;
    mStatistics.render_time5 = 0;
    mStatistics.render_time6 = 0;
    mStatistics.render_time7 = 0;
    mStatistics.render_time8 = 0;

    mStatistics.dynamic_append_transmit_node = false;
}

void PrRenderStuff::StartRender(PrSceneObject* scene) {
    mDmaQueue.Start();
    this->unk1C = scene;
}

void PrRenderStuff::WaitRender() {
    bool s1;

    mDmaQueue.Wait();
    s1 = false;
    PrStopMfifo();

    mStatistics.render_time6 = *T3_COUNT;

    const float f0 = this->unk1C->GetFocalLength();
    if (f0 != 0.0f) {
        sceGsSyncPath(0, 0);
        s1 = true;
        this->unk1C->ApplyDepthOfField();
        sceGsSyncPath(0, 0);
    }

    if (this->unk1C->unkA0) {
        s1 = false;
        this->unk1C->PrepareScreenModelRender();
        mDmaQueue.Wait();
    }

    if (!s1) {
        sceGsSyncPath(0, 0);
    }

    mStatistics.render_time7 = *T3_COUNT;

    this->unk1C = NULL;
}

void PrRenderStuff::AllocateTransmitDmaArray(unsigned int arg0) {
    if (this->unk14 >= arg0) {
        return;
    }

    int s0 = 600;
    while (s0 < arg0) {
        s0 = s0 * 2;
    }

    delete this->unk18;

    this->unk18 = new StrUnk18[s0];
    this->unk14 = s0;
}

void PrRenderStuff::AppendTransmitDmaTag(const sceDmaTag* arg0, unsigned int arg1, float arg2) {
    extern bool warned_tmp_renderstuff;

    if (this->unk10 >= this->unk14) {
        if (!warned_tmp_renderstuff) {
            warned_tmp_renderstuff = true;
            return;
        }
    } else {
        this->unk18[this->unk10].unk0 = arg2;
        this->unk18[this->unk10].unk4 = arg1;
        this->unk18[this->unk10].unk8 = (sceDmaTag*)arg0;
        this->unk10++;
    }
}

int PrRenderStuff::CompareFunction(const void* arg0, const void* arg1) {
    StrUnk18* a0 = (StrUnk18*)arg0;
    StrUnk18* a1 = (StrUnk18*)arg1;

    if (a0->unk4 != a1->unk4) {
        return (a0->unk4 >= a1->unk4) ? 1 : -1;
    }

    if (a0->unk0 == a1->unk0) {
        return 0;
    } else if (a1->unk0 < a0->unk0) {
        return 1;
    }

    return -1;
}

void PrRenderStuff::SortTransmitDmaArray() {
    if (this->unk10 > 1) {
        qsort(this->unk18, this->unk10, sizeof(StrUnk18), CompareFunction);
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("prlib/renderstuff", MergeRender__13PrRenderStuff);
#else
void PrRenderStuff::MergeRender() {
    bool s4 = false;

    for (int i = 0; i < this->unk10; i++) {
        if (!s4 && this->unk18[i].unk4 == -1) {
            s4 = true;
            PrDmaStripForSetGifRegister* strip = PrGetDmaStripGifRegister(eGifRegisterMode_Unk1);
            AppendDmaTag(&strip->mTag);
        }

        AppendDmaTag(this->unk18[i].unk8);
    }

    if (prCurrentStage == 19) {
        PrDmaStripForSetGifRegister* strip = PrGetDmaStripGifRegister(eGifRegisterMode_Unk1);
        AppendDmaTag(&strip->mTag);
    }
}
#endif

INCLUDE_ASM("prlib/renderstuff", _GLOBAL_$D$prRenderStuff);

INCLUDE_ASM("prlib/renderstuff", _GLOBAL_$I$prRenderStuff);

/* prlib/renderstuff.h */
INCLUDE_ASM("prlib/renderstuff", func_00141650);
