#include "model.h"

void PrModelObject::SavePosture() {
    if (!(mSpmImage->mFlags & 0x40)) {
        return;
    }

    if (mRenderedOnce) {
        mActiveTransition = 1 - mActiveTransition;
        mFlags |= 4;
    }
}

void PrModelObject::ResetPosture() {
    mFlags &= ~4;
}

INCLUDE_ASM("asm/nonmatchings/prlib/transition", BlendTransitionMatrix__7SpmNodeP13PrModelObjectRt8NaMATRIX3Zfi4i4);

INCLUDE_ASM("asm/nonmatchings/prlib/transition", BlendTransactionWeight__12SpmShapeNodeP13PrModelObjectfUi);
