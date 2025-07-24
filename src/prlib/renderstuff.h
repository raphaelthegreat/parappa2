#ifndef PRLIB_RENDERSTUFF_H
#define PRLIB_RENDERSTUFF_H

#include "common.h"

#include "prlib/dmaqueue.h"
#include "prlib/scene.h"
#include "prlib/prpriv.h"

#include <eetypes.h>
#include <eestruct.h>

struct StrUnk18 {
    float unk0;
    u_int unk4;
    sceDmaTag* unk8;
};

class PrRenderStuff {
public:
    PrRenderStuff();
    ~PrRenderStuff();

    void Initialize(sceGsZbuf zbuf);
    void Cleanup();

    u_int GetZbufBits(void) const;
    void ResetStatistics();

    void StartRender(PrSceneObject* scene);
    void WaitRender();

    void AllocateTransmitDmaArray(unsigned int arg0);
    void AppendTransmitDmaTag(const sceDmaTag* arg0, unsigned int arg1, float arg2);

    static int CompareFunction(const void* arg0, const void* arg1);
    void SortTransmitDmaArray();

    void MergeRender();

public:
    void AppendDmaTag(const sceDmaTag* tag);

private:
    PrDmaQueue mDmaQueue;
    u_int unk10;
    int unk14;
    StrUnk18* unk18;
    PrSceneObject* unk1C;
    sceGsZbuf unk20;
    int unk28;
    PrRENDERING_STATISTICS mStatistics;
};

#if 0
void PrRenderStuff::AppendDmaTag(const sceDmaTag* tag) {
    mDmaQueue.Append((void*)tag);
}
#endif

extern PrRenderStuff prRenderStuff;

#endif /* PRLIB_RENDERSTUFF_H */
