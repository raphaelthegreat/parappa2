#include "common.h"

#include <eetypes.h>
#include <eestruct.h>

#include "camera.h"
#include "microprogram.h"
#include "model.h"

void SpmFileHeader::ChangePointer() {
    if (this->flags & 0x1) {
        return;
    }

    this->unk64 = this->CalculatePointer<int>(this->unk64);
    this->unk74 = this->CalculatePointer<SpmNode*>(this->unk74);

    for (int i = 0; i < this->unk68; i++) {
        this->unk74[i] = this->CalculatePointer<SpmNode>(this->unk74[i]);
    }

    /* Change node pointers starting from the first. */
    (*this->unk74)->ChangePointer(this, NULL);
    this->flags |= 0x1;
}

void SpmNode::ChangePointer(SpmFileHeader* arg0, SpmNode* arg1) {
    this->unk158 = arg0;
    this->unk164 = arg1;

    this->unk15C = arg0->CalculatePointer<SpmNode>(this->unk15C);
    this->unk160 = arg0->CalculatePointer<SpmNode>(this->unk160);

    this->unk16C[0] = arg0->CalculatePointer<PrVuNodeHeaderDmaPacket>(this->unk16C[0]);
    this->unk16C[1] = arg0->CalculatePointer<PrVuNodeHeaderDmaPacket>(this->unk16C[1]);

    this->unk17C = this->unk16C[0];
    if (this->unk16C[0] == NULL) {
        this->unk17C = this->unk16C[1];
    }

    for (u_int i = 0; i < 2; i++) {
        if (this->unk16C[i]) {
            this->unk16C[i]->unk194 = SCE_VIF1_SET_MSCAL(PrGetMicroProgramAddress(this->unk16C[i]->unk60), 0);
        }
    }

    if (this->unk154 & 0xff0) {
        this->unk198 = arg0->CalculatePointer<int>(this->unk198);
        if (this->unk154 & 0x10) {
            this->unk1B4 = arg0->CalculatePointer<int>(this->unk1B4);
            this->unk1B8 = arg0->CalculatePointer<int>(this->unk1B8);
        } else if (this->unk154 & 0x20) {
            this->unk1B4 = arg0->CalculatePointer<int>(this->unk1B4);
        }

        this->unk1A0 = arg0->CalculatePointer<int>(this->unk1A0);
        this->unk1A4 = arg0->CalculatePointer<PrVuNodeHeaderDmaPacket>(this->unk1A4);
        PrVuNodeHeaderDmaPacket* s0 = this->unk1A4;
        if (s0 != NULL) {
            s0->unk194 = SCE_VIF1_SET_MSCAL(PrGetMicroProgramAddress(eVumpContour), 0);
        }
    }

    SpmNode* s0 = this->unk15C;
    while (s0 != 0) {
        s0->ChangePointer(arg0, this);
        s0 = s0->unk160;
    }
}

INCLUDE_ASM("prlib/setpointer", ChangePointer__13SpaFileHeader);

INCLUDE_ASM("prlib/setpointer", ChangePointer__16SpaNodeAnimationP13SpaFileHeader);

void SpcFileHeader::ChangePointer() {
    if (this->flags & 0x1) {
        return;
    }

    this->unk74 = this->CalculatePointer<int>(this->unk74);
    
    this->unk88 = this->CalculatePointer<SpaTrack<NaVECTOR<float, 4> > >(this->unk88);
    if (this->unk88 != NULL) {
        this->unk88->ChangePointer();
    }

    this->unk8C = this->CalculatePointer<SpaTrack<NaVECTOR<float, 4> > >(this->unk8C);
    if (this->unk8C != NULL) {
        this->unk8C->ChangePointer();
    }

    this->unk90 = this->CalculatePointer<SpaTrack<float> >(this->unk90);
    if (this->unk90 != NULL) {
        this->unk90->ChangePointer();
    }

    this->unk94 = this->CalculatePointer<SpaTrack<float> >(this->unk94);
    if (this->unk94 != NULL) {
        this->unk94->ChangePointer();
    }

    if (this->flags & 0x8) {
        this->focal_len_track = this->CalculatePointer<SpaTrack<float> >(this->focal_len_track);
        if (this->focal_len_track != NULL) {
            this->focal_len_track->ChangePointer();
        }

        this->defocus_len_track = this->CalculatePointer<SpaTrack<float> >(this->defocus_len_track);
        if (this->defocus_len_track != NULL) {
            this->defocus_len_track->ChangePointer();
        }
    }

    this->flags |= 1;
}
