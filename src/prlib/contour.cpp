#include "model.h"

void PrModelObject::SaveContour() {
    SpmFileHeader* spm = mSpmImage;
    if (spm->unk70 == 0 || this->unk84 == 0) {
        return;
    }

    u_int node_num = spm->node_num;
    for (int i = 0; i < node_num; i++) {
        SpmNode* node = spm->nodes[i];
        if (node->flags & 0x40) {
            SpmComplexNode* complex = reinterpret_cast<SpmComplexNode*>(node);
            complex->SaveContour(this);
        }
    }

    mFlags |= 2;
}

INCLUDE_ASM("asm/nonmatchings/prlib/contour", SaveContour__14SpmComplexNodeP13PrModelObject);

void PrModelObject::ResetContour() {
    mFlags &= ~2;
}

INCLUDE_ASM("asm/nonmatchings/prlib/contour", RenderContour__14SpmComplexNodeP13PrModelObject);
