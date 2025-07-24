#ifndef PRLIB_SCENE_H
#define PRLIB_SCENE_H

#include "common.h"

#include <eetypes.h>
#include <eestruct.h>

class PrSceneObject {
public:
    float GetFocalLength(void) const;

    void PreprocessModel();
    void ApplyDepthOfField();
    void PrepareScreenModelRender();

public:
    PR_PADDING(unk0, 0x50);
    sceGsFrame unk50;
    sceGsXyoffset unk58;
    PR_PADDING(unk60, 0x40);
    int unkA0;
};

#endif /* PRLIB_SCENE_H */
