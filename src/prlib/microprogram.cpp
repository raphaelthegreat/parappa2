#include "microprogram.h"

#include <libdma.h>

extern u_int vump_dmatag[]                  __attribute__((section(".vutext")));

extern u_int vump_top[]                     __attribute__((section(".vutext")));
extern u_int vump_normal[]                  __attribute__((section(".vutext")));
extern u_int vump_bothface[]                __attribute__((section(".vutext")));
extern u_int vump_contour[]                 __attribute__((section(".vutext")));
extern u_int vump_refmap[]                  __attribute__((section(".vutext")));
extern u_int vump_screen[]                  __attribute__((section(".vutext")));
extern u_int vump_antiline[]                __attribute__((section(".vutext")));
extern u_int vump_menderer_create_texture[] __attribute__((section(".vutext")));
extern u_int vump_menderer_draw_mesh[]      __attribute__((section(".vutext")));

static u_int MicroProgramEntryPoint[PR_MICRO_PROGRAM_NUM];

static u_int MendererCreateTextureEntryPoint;
static u_int MendererDrawMeshEntryPoint;

/* Divides address by 8 to be used on MSCAL */
#define VU_ADDR(x, base) (((int)x - (int)base) >> 3)

static void CalculateEntryPoint() {
    MicroProgramEntryPoint[PR_MICRO_PROGRAM_NORMAL]   = VU_ADDR(vump_normal, vump_top);
    MicroProgramEntryPoint[PR_MICRO_PROGRAM_BOTHFACE] = VU_ADDR(vump_bothface, vump_top);
    MicroProgramEntryPoint[PR_MICRO_PROGRAM_CONTOUR]  = VU_ADDR(vump_contour, vump_top);
    MicroProgramEntryPoint[PR_MICRO_PROGRAM_REFMAP]   = VU_ADDR(vump_refmap, vump_top);
    MicroProgramEntryPoint[PR_MICRO_PROGRAM_SCREEN]   = VU_ADDR(vump_screen, vump_top);
    MicroProgramEntryPoint[PR_MICRO_PROGRAM_ANTILINE] = VU_ADDR(vump_antiline, vump_top);
    
    MendererCreateTextureEntryPoint = VU_ADDR(vump_menderer_create_texture, vump_top);
    MendererDrawMeshEntryPoint      = VU_ADDR(vump_menderer_draw_mesh, vump_top);
}

void PrLoadMicroPrograms() {
    sceDmaChan* chan = sceDmaGetChan(SCE_DMA_VIF1);

    /*
     * TTE is required to transfer
     * the first MPG VIFcodes inside
     * of the DMAtag's upper 64 bits:
     *
     * NOP,
     * MPG 0x0, 0x100 (2048 bytes)
     */
    chan->chcr.TTE = 1;
    
    sceDmaSend(chan, vump_dmatag);
    CalculateEntryPoint();
}

u_int PrGetMicroProgramAddress(PrMICRO_PROGRAM_MODULE module) {
    return MicroProgramEntryPoint[module];
}

u_int PrGetInputVertexParameterNum(PrMICRO_PROGRAM_MODULE module) {
    static u_int ParameterTable[] = {
        3, /* NORMAL   */
        3, /* BOTHFACE */
        2, /* CONTOUR  */
        4, /* REFMAP   */
        3, /* SCREEN   */
        3, /* ANTILINE */
    };

    return ParameterTable[module];
}

u_int PrGetInputVertexParameterSet(PrMICRO_PROGRAM_MODULE module) {
    static u_int ParameterTable[] = {
        13, /* NORMAL   */
        13, /* BOTHFACE */
        5,  /* CONTOUR  */
        15, /* REFMAP   */
        13, /* SCREEN   */
        13, /* ANTILINE */
    };

    return ParameterTable[module];
}

u_int PrGetOutputVertexParameterNum(PrMICRO_PROGRAM_MODULE module) {
    static u_int ParameterTable[] = {
        3, /* NORMAL   */
        3, /* BOTHFACE */
        2, /* CONTOUR  */
        3, /* REFMAP   */
        3, /* SCREEN   */
        3, /* ANTILINE */
    };

    return ParameterTable[module];
}

u_int PrGetOutputVertexParameterSet(PrMICRO_PROGRAM_MODULE module) {
    static u_int ParameterTable[] = {
        13, /* NORMAL   */
        13, /* BOTHFACE */
        5,  /* CONTOUR  */
        13, /* REFMAP   */
        13, /* SCREEN   */
        13, /* ANTILINE */
    };

    return ParameterTable[module];
}

u_int PrGetMendererCreateTextureAddress() {
    return MendererCreateTextureEntryPoint;
}

u_int PrGetMendererDrawMeshAddress() {
    return MendererDrawMeshEntryPoint;
}
