#ifndef PRLIB_CAMERA_H
#define PRLIB_CAMERA_H

#include <eetypes.h>

#include "prpriv.h"
#include "spadata.h"

#include <nalib/navector.h>

struct SpcFileHeader {
    u_int magic;
    u_short version;
    u_short flags;

    char unk8[0x6c];

    int* unk74;
    char unk78[0x10];
    SpaTrack<NaVECTOR<float, 4> >* unk88;
    SpaTrack<NaVECTOR<float, 4> >* unk8C;
    SpaTrack<float>* unk90;
    SpaTrack<float>* unk94;

    u_int depth_level;

    SpaTrack<float>* focal_len_track;
    SpaTrack<float>* defocus_len_track;

public:
    void Initialize();
    PrPERSPECTIVE_CAMERA* GetCamera(float time) const;

    void ChangePointer();

    template <typename T>
    T* CalculatePointer(T* offset) {
        if (!offset) {
            return NULL;
        }
        return reinterpret_cast<T*>(reinterpret_cast<int>(this) + reinterpret_cast<int>(offset));
    }
};

#endif /* PRLIB_CAMERA_H */
