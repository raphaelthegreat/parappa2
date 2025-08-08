#include "main/mbar.h"

#include "main/cmnfile.h"
#include "os/syssub.h"
#include "os/tim2.h"

#include <stdio.h>

/* data 186288 */ extern TIM2_DAT tim2spr_tbl_tmp1[0]; /* static, tim2spr_tbl */
// /* data 186a68 */ static u_int tmpColor[0];
/* data 186aa8 */ extern NIKO_CHAN_STR niko_chan_str_hook[]; /* static */
/* data 186af8 */ extern NIKO_CHAN_STR niko_chan_str_vs[]; /* static */
/* sdata 399558 */ extern NIKO_CHAN_STR *niko_chan_str_pp; /* static */
/* sdata 39955c */ extern int niko_chan_str_cnt; /* static */
// /* sdata 399560 */ static int hook_use_flag;
// /* data 186b28 */ static MBHOOK_STR mbhook_str[2];
// /* data 186b38 */ static u_int hook_fr_dat[0];
/* sdata 399564 */ extern int exam_disp_cursor_timer; /* static */
/* sdata 399568 */ extern int scoreTentouFlag; /* static */
/* data 186b78 */ extern u_char scr_tenmetu_col[4][3]; /* static */
/* sdata 39956c */ extern int otehonAniCnt; /* static */
/* sdata 399570 */ extern int othon_frame; /* static */
// /* sdata 399574 */ static int vs_mouse_disp_flag;
/* data 186b88 */ extern METCOL_STR metcol_str[3]; /* static */
// /* data 186bb0 */ static MBA_CHAR_DATA mba_char_data[0];
// /* sdata 399578 */ static int mbar_pos_y_ofs;
// /* data 186d78 */ static u_char colp[0][3];
// /* data 186d90 */ static void (*marSetPrgTbl[0])(/* parameters unknown */);
// /* data 186da0 */ static GUIMAP guimap[0];
// /* data 186ee0 */ static int guimap_single[0];
// /* data 186ef0 */ static int guimap_vs[0];
// /* data 186f00 */ static int guimap_sr[0];
// /* data 186f08 */ static int guimap_hk[0];
/* bss 1c70030 */ extern GLOBAL_PLY *exam_global_ply[4]; /* static */
// /* sbss 399a80 */ static GLOBAL_PLY *exam_global_ply_current;
/* bss 1c70040 */ extern int exam_global_ply_current_ply[4]; /* static */
/* bss 1c70050 */ extern int metFrameCnt[3]; /* static */
/* bss 1c70060 */ extern int metFrameCntLight[3]; /* static */
#if 0
/* sdata 399584 */ enum SCR_TENMETU_ENUM {
    SCR_TENMETU_NORMAL = 0,
    SCR_TENMETU_PL = 1,
    SCR_TENMETU_MI = 2,
    SCR_TENMETU_BLACK = 3,
    SCR_TENMETU_MAX = 4
};
#endif
/* bss 1c70070 */ extern u_char scr_tenmetu_col_dat[4][3]; /* static */
/* bss 1c70080 */ extern int conditionFramCnt[4]; /* static */
/* bss 1c70090 */ extern int vsScoreMove[4]; /* static */
/* bss 1c700a0 */ extern int vsScoreAni[4]; /* static */
/* bss 1c700b0 */ extern VS_SCR_CTRL vs_scr_ctrl[4]; /* static */
/* bss 1c70110 */ extern MBAR_REQ_STR mbar_req_str[5]; /* static */
/* sbss 399a84 */ extern int mbar_ctrl_time; /* static */
/* sbss 399a88 */ extern int mbar_ctrl_stage; /* static */
/* sbss 399a8c */ extern int mbar_ctrl_stage_selT; /* static */
// /* bss 1c701c8 */ static sceGifPacket mbar_gif;
// /* sbss 399a90 */ static PR_SCENEHANDLE guime_hdl;
// /* sbss 399a94 */ static PR_CAMERAHANDLE guime_camera_hdl;

static void   clrColorBuffer(int id);
static void   NikoReset(void);
static void   MbarNikoDisp(sceGifPacket *gifpk_pp);
static void   MbarHookPoll(void);
static int    vsScr2Move(long scr);
static void   vsAnimationPoll(void);
static void   metColorInit(void);
static void   metColorSet(EXAM_TYPE exam_type, float per);
static void   ExamDispOn(void);
static u_long hex2dec(u_long data);
static u_long hex2decPlMi(long data);
static void   examScoreSet(sceGifPacket *ex_gif_pp);
static void   examLevelDisp(sceGifPacket *ex_gif_pp);
static void   MbarCl1CharSet(int col_num, int moto_num);
static void   MbarCharSetSub(void);
static int    MbarGetDispPosX(int tick);
static int    MbarGetDispPosY(int tick);
static int    MbarGetTimeArea(MBAR_REQ_STR *mr_pp);
static int    MbarGetTimeArea2(MBAR_REQ_STR *mr_pp);
static int    MbarGetStartTime(MBAR_REQ_STR *mr_pp);
static int    MbarGetEndTime(MBAR_REQ_STR *mr_pp);
static int    MbarGetStartTap(MBAR_REQ_STR *mr_pp);
static void   MbarOthSet(MBAR_REQ_STR *mr_pp);
static void   MbarCurSet(MBAR_REQ_STR *mr_pp);
static int    MbarTapSubt(MBAR_REQ_STR *mr_pp);
static void   MbarPosOffsetSet(MBAR_REQ_STR *mr_pp);
static void   mbar_othon_frame_set(MBAR_REQ_STR *mr_pp);
static void   guidisp_init_pr(void);
static void   guidisp_draw_quit(int drapP);

INCLUDE_ASM("main/mbar", examCharSet);

INCLUDE_ASM("main/mbar", clrColorBuffer);

void MbarMemberClear(int stg) {
    if (stg < 6) {
        clrColorBuffer(59);
    }
    if (stg < 4) {
        clrColorBuffer(58);
    }
    if (stg < 3) {
        clrColorBuffer(57);
    }
    if (stg < 2) {
        clrColorBuffer(56);
    }
    if (stg < 1) {
        clrColorBuffer(55);
    }
}

void examCharBasic(EX_CHAR_DISP *ecd_pp, TIM2_DAT *tim2_dat_pp) {
    ecd_pp->GsTex0 = tim2_dat_pp->GsTex0;
    ecd_pp->GsTex1 = tim2_dat_pp->GsTex1;
    ecd_pp->GsRegs = tim2_dat_pp->GsRegs;

    ecd_pp->w = tim2_dat_pp->w;
    ecd_pp->h = tim2_dat_pp->h;
    
    ecd_pp->u = 0;
    ecd_pp->v = 0;
    ecd_pp->cx = 0;
    ecd_pp->cy = 0;
    
    ecd_pp->scalex = 1.0f;
    ecd_pp->scaley = 1.0f;
    ecd_pp->alpha = 0.0f;

    ecd_pp->kido[0] = 128;
    ecd_pp->kido[1] = 128;
    ecd_pp->kido[2] = 128;
}

void examCharScaleSet(EX_CHAR_DISP *ecd_pp, float scx, float scy) {
    ecd_pp->scalex = ecd_pp->scalex * scx;
    ecd_pp->scaley = ecd_pp->scaley * scy;
}

void examCharCltSet(EX_CHAR_DISP *ecd_pp, TIM2_DAT *tim2_dat_pp) {
    ((sceGsTex0*)&ecd_pp->GsTex0)->CBP = ((sceGsTex0*)&tim2_dat_pp->GsTex0)->CBP;
}

void examCharPosSet(EX_CHAR_DISP *ecd_pp, int xp, int yp) {
    ecd_pp->x = (xp << 4) - GS_X_COORD(640);
    ecd_pp->y = (yp << 4) - GS_Y_COORD(224);
}

void examCharUVWHSet(EX_CHAR_DISP *ecd_pp, u_short u, u_short v, u_short w, u_short h) {
    ecd_pp->u = u;
    ecd_pp->v = v;
    ecd_pp->w = w;
    ecd_pp->h = h;
}

void examCharAlphaSet(EX_CHAR_DISP *ecd_pp, u_short on_off) {
    ecd_pp->alpha = on_off;
}

void examCharKidoSet(EX_CHAR_DISP *ecd_pp, u_char rc, u_char gc, u_char bc) {
    ecd_pp->kido[0] = rc;
    ecd_pp->kido[1] = gc;
    ecd_pp->kido[2] = bc;
}

static void NikoReset(void) {
    int i;

    if (niko_chan_str_pp != NULL) {
        for (i = 0; i < niko_chan_str_cnt; i++) {
            niko_chan_str_pp[i].niko_enum = NIKO_KAGE;
        }
    }
}

void MbarNikoHookUse(void) {
    niko_chan_str_cnt = 10;
    niko_chan_str_pp = niko_chan_str_hook;
    NikoReset();
}

void MbarNikoVsUse(void) {
    niko_chan_str_cnt = 6;
    niko_chan_str_pp = niko_chan_str_vs;
    NikoReset();
}

void MbarNikoUnUse(void) {
    niko_chan_str_pp = NULL;
    niko_chan_str_cnt = 0;
}

void MbarNikoSet(int num, int ofs) {
    int i;

    if (niko_chan_str_pp == NULL) {
        return;
    }

    for (i = 0; i < num / 2; i++) {
        if ((i + ofs) >= niko_chan_str_cnt) {
            printf("NIKO OVER!!\n");
            return;
        }
        niko_chan_str_pp[i + ofs].niko_enum = NIKO_MARU;
    }

    if (num & 0x1) {
        if (((num / 2) + ofs) >= niko_chan_str_cnt) {
            printf("NIKO OVER!!\n");
            return;
        }  
        niko_chan_str_pp[(num / 2) + ofs].niko_enum = NIKO_HALF;
    }
}

static void MbarNikoDisp(sceGifPacket *gifpk_pp) {
    int i;
    NIKO_CHAN_STR *niko_pp;
    TIM2_DAT *tim2_dat_pp;
    TIM2_DAT *tim2_dat2_pp;

    if (niko_chan_str_pp == NULL || niko_chan_str_cnt == 0) {
        return;
    }

    ChangeDrawAreaSetGifTag(DrawGetDrawEnvP(DNUM_DRAW), gifpk_pp);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PABE, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0));

    niko_pp = niko_chan_str_pp;
    for (i = 0; i < niko_chan_str_cnt; i++, niko_pp++) {
        tim2_dat_pp = NULL;
        tim2_dat2_pp = NULL;

        switch (niko_chan_str_pp[i].niko_enum) {
        case NIKO_KAGE:
            tim2_dat_pp = &tim2spr_tbl_tmp1[60];
            break;
        case NIKO_HALF:
            tim2_dat_pp = &tim2spr_tbl_tmp1[60];
            tim2_dat2_pp = tim2_dat_pp + 1;
            break;
        case NIKO_MARU:
            tim2_dat_pp = &tim2spr_tbl_tmp1[61];
            break;
        case NIKO_SKIP:
            tim2_dat_pp = &tim2spr_tbl_tmp1[62];
            break;
        }

        if (tim2_dat_pp != NULL) {
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX0_1, tim2_dat_pp->GsTex0);
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX1_1, tim2_dat_pp->GsTex1);
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 1, 0, 1, 0, 0));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(0, 0));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + 0x6c0) << 4, (niko_pp->yp + 0x790) << 4, 1));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(tim2_dat_pp->w << 4, tim2_dat_pp->h << 4));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + tim2_dat_pp->w + 0x6c0) << 4, (niko_pp->yp + (tim2_dat_pp->h / 2) + 0x790) << 4, 1));

            if (tim2_dat2_pp != NULL) {
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX0_1, tim2_dat2_pp->GsTex0);
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX1_1, tim2_dat2_pp->GsTex1);
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 1, 0, 1, 0, 0));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(0, 0));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + 0x6c0) << 4, (niko_pp->yp + 0x790) << 4, 1));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(tim2_dat2_pp->w << 4, (tim2_dat2_pp->h / 2) << 4));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + tim2_dat2_pp->w + 0x6c0) << 4, (niko_pp->yp + (tim2_dat2_pp->h / 4) + 0x790) << 4, 1));
            }
        }
    }
}

INCLUDE_ASM("main/mbar", MbarHookUseInit);

INCLUDE_ASM("main/mbar", MbarHookUnUse);

INCLUDE_ASM("main/mbar", MbarHookUseOK);

INCLUDE_ASM("main/mbar", MbarHookUseNG);

INCLUDE_ASM("main/mbar", MbarHookPoll);

void vsAnimationInit(void) {
    int i;

    for (i = 0; i < 4; i++) {
        vsScoreMove[i] = 0;
        vsScoreAni[i] = 0;
    }

    WorkClear(vs_scr_ctrl, sizeof(vs_scr_ctrl));
}

INCLUDE_ASM("main/mbar", vsAnimationReq);

INCLUDE_ASM("main/mbar", vsAnimationReset);

INCLUDE_ASM("main/mbar", vsScr2Move);

INCLUDE_ASM("main/mbar", vsAnimationPoll);

static void metColorInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(metcol_str); i++) {
        Tim2Trans(cmnfGetFileAdrs(metcol_str[i].df_num));
    }
}

INCLUDE_ASM("main/mbar", metColorSet);

void metFrameInit(void) {
    int i;

    for (i = 0; i < 3; i++) {
        metFrameCnt[i] = 0x8c;
        metFrameCntLight[i] = 0;
    }

    scoreTentouFlag = 0;

    for (i = 0; i < 4; i++) {
        scr_tenmetu_col_dat[i][0] = scr_tenmetu_col[0][0];
        scr_tenmetu_col_dat[i][1] = scr_tenmetu_col[0][1];
        scr_tenmetu_col_dat[i][2] = scr_tenmetu_col[0][2];
        exam_global_ply_current_ply[i] = 0;
    }
}

void conditionFrameInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(conditionFramCnt); i++) {
        conditionFramCnt[i] = 60;
    }
}

void ExamDispInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(exam_global_ply); i++) {
        exam_global_ply[i] = NULL;
    }

    exam_disp_cursor_timer = -1;

    metFrameInit();

    metColorInit();

    conditionFrameInit();

    otehonAniCnt = 0;
    othon_frame = 0;

    ExamDispReset();

    vsAnimationInit();
}

INCLUDE_ASM("main/mbar", ExamDispPlySet);

INCLUDE_ASM("main/mbar", ExamDispReq);

void ExamDispReset(void) {
    exam_disp_cursor_timer = -1;
    metFrameInit();
    metColorInit();
}

float examScore2Level(long score) {
    float ret_lvl = score / 140.0f;

    if (ret_lvl > 0) {
        ret_lvl += 0.2;
    } else {
        ret_lvl -= 0.2;
    }

    if (ret_lvl > 1.0) {
        ret_lvl = 1.0f;
    }

    if (ret_lvl < -1.0) {
        ret_lvl = -1.0f;
    }

    return ret_lvl;
}

INCLUDE_ASM("main/mbar", ExamDispOn);

INCLUDE_ASM("main/mbar", hex2dec_mbar_tmp);

INCLUDE_ASM("main/mbar", hex2decPlMi);

INCLUDE_ASM("main/mbar", examNumDisp);

INCLUDE_ASM("main/mbar", examScoreSet);

INCLUDE_ASM("main/mbar", examLevelDisp);

INCLUDE_ASM("main/mbar", ExamDispSet);

void ExamDispSubt(void) {
    /* Empty */
}

void MbarInit(int stg) {
    mbar_ctrl_stage = stg;

    if (stg == 6) {
        mbar_ctrl_stage_selT = 1;
    } else {
        mbar_ctrl_stage_selT = stg;
    }
    
    MbarCharSetSub();
}

INCLUDE_ASM("main/mbar", MbarReset);

INCLUDE_RODATA("main/mbar", D_00393450);

INCLUDE_RODATA("main/mbar", D_00393458);

void MbarReq(MBAR_REQ_ENUM mm_req, TAPSET *ts_pp, int curr_time, SCR_TAP_MEMORY *tm_pp, int tm_cnt, 
             int lang, int tapdat_size, TAPDAT *tapdat_pp, GUI_CURSOR_ENUM guic) {
    PLAYER_INDEX pidx;

    if (ts_pp == NULL) {
        printf("MbarReq   TAPSET adrs is NULL\n");
        return;
    }

    pidx = Pcode2Pindex(ts_pp->player_code);
    mbar_req_str[pidx].mbar_req_enum = mm_req;
    mbar_req_str[pidx].tapset_pp = ts_pp;
    mbar_req_str[pidx].current_time = curr_time;
    mbar_req_str[pidx].scr_tap_memory_pp = tm_pp;
    mbar_req_str[pidx].scr_tap_memory_cnt = tm_cnt;
    mbar_req_str[pidx].lang = lang;

    mbar_req_str[pidx].tapdat_size = tapdat_size;
    mbar_req_str[pidx].tapdat_pp = tapdat_pp;
    mbar_req_str[pidx].gui_cursor_enum = guic;
}

void MbarSetCtrlTime(int mctime) {
    mbar_ctrl_time = mctime;
}

INCLUDE_ASM("main/mbar", MbarCl1CharSet);

INCLUDE_ASM("main/mbar", MbarCharSetSub);

INCLUDE_ASM("main/mbar", MbarGifInit);

INCLUDE_ASM("main/mbar", MbarGifTrans);

INCLUDE_ASM("main/mbar", MbarCharSet);

INCLUDE_ASM("main/mbar", MbarCharSet2);

INCLUDE_ASM("main/mbar", MbarWindowSet);

INCLUDE_ASM("main/mbar", MbarGetDispPosX);

INCLUDE_ASM("main/mbar", MbarGetDispPosY);

INCLUDE_ASM("main/mbar", MbarGetTimeArea);

INCLUDE_ASM("main/mbar", MbarGetTimeArea2);

int MbarGetStartTime(MBAR_REQ_STR *mr_pp) {
    return ((mr_pp->current_time + mr_pp->tapset_pp->taptimeStart - 24) / 96) * 96;
}

int MbarGetEndTime(MBAR_REQ_STR *mr_pp) {
    return (mr_pp->current_time + mr_pp->tapset_pp->taptimeEnd);
}

INCLUDE_ASM("main/mbar", MbarGetStartTap);

INCLUDE_ASM("main/mbar", MbarSclRotMake);

void MbarGuideLightMake(MBARR_CHR *mbarr_pp, int mbtime) {
    u_char col = 128;

    if (mbtime < 144u) {
        col = (144 - mbtime) * 128 / 144 + 128u;
    }

    mbarr_pp->r = mbarr_pp->g = mbarr_pp->b = col;
}

INCLUDE_ASM("main/mbar", MbarFlashMake);

INCLUDE_ASM("main/mbar", MbarBackSet);

INCLUDE_RODATA("main/mbar", D_00393488);

INCLUDE_RODATA("main/mbar", D_003934A0);

INCLUDE_ASM("main/mbar", MbarOthSet);

INCLUDE_ASM("main/mbar", MbarCurSet);

INCLUDE_ASM("main/mbar", MbarTapSubt);

INCLUDE_ASM("main/mbar", MbarPosOffsetSet);

INCLUDE_ASM("main/mbar", mbar_othon_frame_set);

INCLUDE_ASM("main/mbar", MbarDisp);

INCLUDE_ASM("main/mbar", MbarDispScene);

INCLUDE_ASM("main/mbar", MbarDispSceneDraw);

INCLUDE_ASM("main/mbar", MbarDispSceneVsDraw);

INCLUDE_ASM("main/mbar", MbarDispSceneVsDrawInit);

INCLUDE_ASM("main/mbar", guidisp_init_pr);

INCLUDE_ASM("main/mbar", guidisp_draw_quit);

INCLUDE_ASM("main/mbar", MbarDispGuiScene);

INCLUDE_ASM("main/mbar", MbarDispGuiSceneMbarArea);

TIM2_DAT* lessonTim2InfoGet(void) {
    return &tim2spr_tbl_tmp1[52];
}

#ifndef NON_MATCHING
INCLUDE_ASM("main/mbar", lessonCl2InfoGet);
#else /* requires .rodata migration */
TIM2_DAT* lessonCl2InfoGet(SCRRJ_LESSON_ROUND_ENUM type) {
    u_short le_num[10] = {
        0x2d, /* SCRRJ_LR_LESSON_1 */
        0x2e, /* SCRRJ_LR_LESSON_2 */
        0x2f, /* SCRRJ_LR_LESSON_3 */
        0x30, /* SCRRJ_LR_LESSON_4 */
        0x31, /* SCRRJ_LR_LESSON_5 */

        0x2d, /* SCRRJ_LR_ROUND_1 */
        0x2e, /* SCRRJ_LR_ROUND_2 */
        0x2f, /* SCRRJ_LR_ROUND_3 */
        0x30, /* SCRRJ_LR_ROUND_4 */
        0x31, /* SCRRJ_LR_ROUND_5 */
    };

    return &tim2spr_tbl_tmp1[le_num[type]];
}
#endif

INCLUDE_ASM("main/mbar", MbarDemoCharDisp);

INCLUDE_RODATA("main/mbar", D_003934E8);

INCLUDE_RODATA("main/mbar", D_00393500);
