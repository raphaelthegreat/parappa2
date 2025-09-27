#include "wave.h"

extern int WV_SCREEN_W;
extern int WV_SCREEN_H;

static void CG_WaveInit(WAVE_STR* wstr) {
    wstr->currentAng = 0.0f;
}

static void CG_WaveInitEasy(WAVE_STR* wstr, short x, short y, short w, short h, WMODE_ENUM wmode) {
    wstr->wmode = wmode;
    wstr->x = x;
    wstr->y = y;

    if (wmode == WM_WSLICE) {
        wstr->linecnt = h - 1;

        wstr->sizeW = w;
        wstr->sizeH = 1;

        wstr->addW = 0;
        wstr->addH = 1;

        wstr->u = (x - 2048) + (WV_SCREEN_W / 2);
        wstr->v = y + ((WV_SCREEN_H / 2) - 2047);

        wstr->addU = 0;
        wstr->addV = 1;
    } else {
        wstr->linecnt = w - 1;

        wstr->sizeW = 1;
        wstr->sizeH = h;

        wstr->addW = 1;
        wstr->addH = 0;

        wstr->u = x + ((WV_SCREEN_W / 2) - 2047);
        wstr->v = (y - 2048) + (WV_SCREEN_H / 2);

        wstr->addU = 1;
        wstr->addV = 0;
    }

    wstr->currentAng = 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/prlib/wave", UG_WaveDisp__FP8WAVE_STRP10sceGsFrameP12sceGifPacket);

INCLUDE_ASM("asm/nonmatchings/prlib/wave", CG_WaveDisp__FP8WAVE_STRP10sceGsFrame);
extern void CG_WaveDisp(WAVE_STR* wstr, sceGsFrame* frame_pp);

INCLUDE_ASM("asm/nonmatchings/prlib/wave", WaveCtrlInit__FP8WAVE_STRss10WMODE_ENUM);

void WaveCtrlDisp(WAVE_STR* wstr, sceGsFrame* frame_pp) {
    CG_WaveDisp(wstr, frame_pp);
}

INCLUDE_ASM("asm/nonmatchings/prlib/wave", WaveCtrlUpdate__FP8WAVE_STRf);

INCLUDE_RODATA("asm/nonmatchings/prlib/wave", D_003969A0);
