#ifndef PRLIB_MODEL_H
#define PRLIB_MODEL_H

#include "common.h"

#include "linkedlist.h"
#include "microprogram.h"
#include "objectset.h"

#include <nalib/navector.h>
#include <nalib/namatrix.h>

#include <eetypes.h>
#include <libdma.h>

#define SPM_MAGIC   (0x18df540a)
#define SPM_VERSION (5)

struct SpmFileHeader;
struct SpaFileHeader;
class PrModelObject;
class PrSceneObject;

struct PrVuNodeHeaderDmaPacket {
    sceDmaTag mTag;
    PR_PADDING(unk10, 0x50);
    PrMICRO_PROGRAM_MODULE unk60;
    PR_PADDING(unk64, 0x130);
    int unk194;
};

struct SpmNode {
public:
    PR_PADDING(unk0, 0x40);
    NaMATRIX<float, 4, 4> unk40;
    PR_PADDING(unk80, 0xc0);
    NaVECTOR<float, 4> unk140;
    int unk150;
    u_int unk154;
    SpmFileHeader* unk158;
    SpmNode* unk15C;
    SpmNode* unk160;
    SpmNode* unk164;
    PR_PADDING(unk168, 0x4);
    PrVuNodeHeaderDmaPacket* unk16C[2];
    PR_PADDING(unk174, 0x8);
    PrVuNodeHeaderDmaPacket* unk17C;
    PR_PADDING(unk180, 0x8);
    u_int unk188;
    PR_PADDING(unk18C, 0xc);
    int* unk198;
    PR_PADDING(unk19C, 0x4);
    int* unk1A0;
    PrVuNodeHeaderDmaPacket* unk1A4;
    PR_PADDING(unk1A8, 0xc);
    int* unk1B4;
    int* unk1B8;

public:
    void ChangePointer(SpmFileHeader* arg0, SpmNode* arg1);

    void ModifySimpleDmaPacket(PrVuNodeHeaderDmaPacket* packet);
    void RenderContext1Node(PrModelObject* model);
};

enum SpmFlags {
    eSpmIsScreenModel = 0x80,
};

struct SpmFileHeader {
public:
    u_int magic;
    u_short version;
    u_short flags;
    PR_PADDING(unk8, 0x28);
    NaVECTOR<float, 4> unk30;
    NaVECTOR<float, 4> unk40;
    PR_PADDING(unk50, 0x14);
    int* unk64;
    u_int unk68;
    PR_PADDING(unk6C, 0x8);
    SpmNode** unk74;
    int unk78;

public:
    void ChangePointer();

    template <typename T>
    T* CalculatePointer(T* offset) {
        if (!offset) {
            return NULL;
        }
        return reinterpret_cast<T*>(reinterpret_cast<int>(this) + reinterpret_cast<int>(offset));
    }
};

struct SpmComplexNode {
public:
    void RenderContour(PrModelObject* model);
};

class PrModelObject {
public:
    PrModelObject(SpmFileHeader* spm);
    ~PrModelObject();

    void Initialize();
    void LinkAnimation(SpaFileHeader* animation);
    void CleanupAnimation();
    void LinkPositionAnimation(SpaFileHeader* animation);
    void CleanupPositionAnimation();

    void UnionBoundaryBox(NaVECTOR<float, 4>* arg0, NaVECTOR<float, 4>* arg1);

    void GetPrimitivePosition(NaVECTOR<float, 4>* position);
    void GetScreenPosition(NaVECTOR<float, 4>* position);

    void CalculateCurrentMatrix();

    void RenderContext1Model();
    void RenderScreenModelNode();
    void RenderBackgroundScreenModel();
    void RenderContext2Model();

    void SavePosture();
    void ResetPosture();

    void SaveContour();
    void ResetContour();

public:
    PrLinkedList<PrModelObject> mList;
    PrObjectSet<PrModelObject>* mObjSet;
    PrSceneObject* mLinkedScene;
    NaMATRIX<float, 4, 4> unk10;
    PR_PADDING(unk50, 0x8);
    SpmFileHeader* mSpmImage;
    PR_PADDING(unk5C, 0x54);
};

#endif /* PRLIB_MODEL_H */
