#include "main/mbar.h"

#include "main/cmnfile.h"
#include "os/syssub.h"
#include "os/tim2.h"

#include <stdio.h>

/* data 186288 */ extern TIM2_DAT tim2spr_tbl_tmp1[]; /* static, tim2spr_tbl */
/* data 186a68 */ extern u_int tmpColor[];
/* data 186aa8 */ extern NIKO_CHAN_STR niko_chan_str_hook[]; /* static */
/* data 186af8 */ extern NIKO_CHAN_STR niko_chan_str_vs[]; /* static */
/* sdata 399558 */ extern NIKO_CHAN_STR *niko_chan_str_pp; /* static */
/* sdata 39955c */ extern int niko_chan_str_cnt; /* static */
/* sdata 399560 */ extern int hook_use_flag; /* static */
/* data 186b28 */ extern MBHOOK_STR mbhook_str[2]; /* static */
/* data 186b38 */ extern u_int hook_fr_dat[];
/* sdata 399564 */ extern int exam_disp_cursor_timer; /* static */
/* sdata 399568 */ extern int scoreTentouFlag; /* static */
/* data 186b78 */ extern u_char scr_tenmetu_col[4][3]; /* static */
/* sdata 39956c */ extern int otehonAniCnt; /* static */
/* sdata 399570 */ extern int othon_frame; /* static */
/* sdata 399574 */ extern int vs_mouse_disp_flag; /* static */
/* data 186b88 */ extern METCOL_STR metcol_str[3]; /* static */
/* data 186bb0 */ extern MBA_CHAR_DATA mba_char_data[]; /* static */
// /* sdata 399578 */ static int mbar_pos_y_ofs;
// /* data 186d78 */ static u_char colp[][3];
// /* data 186d90 */ static void (*marSetPrgTbl[])(/* parameters unknown */);
// /* data 186da0 */ static GUIMAP guimap[];
// /* data 186ee0 */ static int guimap_single[];
// /* data 186ef0 */ static int guimap_vs[];
// /* data 186f00 */ static int guimap_sr[];
// /* data 186f08 */ static int guimap_hk[];
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

void examCharSet(EX_CHAR_DISP *ecd_pp, sceGifPacket *gifpk_pp) {
    int wl, hl;
    int xp, yp;

    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRMODECONT, 1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX0_1, ecd_pp->GsTex0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX1_1, ecd_pp->GsTex1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PABE, SCE_GS_SET_PABE(PR_REGS(ecd_pp).pabe));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_FBA_1, SCE_GS_SET_FBA(PR_REGS(ecd_pp).fba));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_COLCLAMP, 1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(ecd_pp->kido[0], ecd_pp->kido[1], ecd_pp->kido[2], 128, 0x3f800000));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, ecd_pp->alpha, 0, 1, 0, 0));

    wl = ecd_pp->w * ecd_pp->scalex * 16.0f;
    hl = ecd_pp->h * ecd_pp->scaley * 16.0f;
    xp = ecd_pp->x - (int)(ecd_pp->cx * ecd_pp->scalex);
    yp = ecd_pp->y - (int)(ecd_pp->cy * ecd_pp->scaley);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(ecd_pp->u << 4, ecd_pp->v << 4));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(xp & 0xffff, yp & 0xffff, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(((ecd_pp->u + ecd_pp->w) << 4) + 8, ((ecd_pp->v + ecd_pp->h) << 4) + 8));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((xp + wl) & 0xffff, (yp + hl) & 0xffff, 1));
}

static void clrColorBuffer(int id) {
    static sceGsLoadImage tp;
    TIM2_DAT *tim2_dat_pp;
    u_char *tr_adr;
    u_int cpsm, cbp;

    tim2_dat_pp = &tim2spr_tbl_tmp1[id];
    tr_adr = (u_char*)&tmpColor;
    cpsm = (int)(PR_TEX0(tim2_dat_pp).CPSM << 32) >> 32;
    cbp = (int)(PR_TEX0(tim2_dat_pp).CBP << 32) >> 32;
    sceGsSetDefLoadImage(&tp, cbp, 1, cpsm, 0, 0, 8, 2);
    FlushCache(0);
    sceGsExecLoadImage(&tp, (u_long128*)tr_adr);
    sceGsSyncPath(0, 0);
}

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

void MbarHookUseInit(void) {
    int i;

    hook_use_flag = TRUE;

    for (i = 0; i < PR_ARRAYSIZE(mbhook_str); i++) {
        mbhook_str[i].timer = 0;
        Tim2Trans(cmnfGetFileAdrs(mbhook_str[i].moto));
    }
}

void MbarHookUnUse(void) {
    hook_use_flag = FALSE;
}

void MbarHookUseOK(void) {
    mbhook_str[1].timer = 1;
}

void MbarHookUseNG(void) {
    mbhook_str[0].timer = 1;
}

static void MbarHookPoll(void) {
	int i;
	u_char *saki_pp;
	u_char *moto_pp;

    if (!hook_use_flag) {
        return;
    }

    for (i = 0; i < 2; i++) {
        if (mbhook_str[i].timer != 0) {
            mbhook_str[i].timer += 1;
            moto_pp = cmnfGetFileAdrs(mbhook_str[i].moto);
            saki_pp = cmnfGetFileAdrs(mbhook_str[i].saki);
            if (mbhook_str[i].timer >= 0x10u) {
                Tim2Trans(moto_pp);
                mbhook_str[i].timer = 0;
            } else {
                Cl2MixTrans(hook_fr_dat[mbhook_str[i].timer], 128, saki_pp, moto_pp);
            }
        }
    }
}

void vsAnimationInit(void) {
    int i;

    for (i = 0; i < 4; i++) {
        vsScoreMove[i] = 0;
        vsScoreAni[i] = 0;
    }

    WorkClear(vs_scr_ctrl, sizeof(vs_scr_ctrl));
}

void vsAnimationReq(int ply, long int scrMoto, long int scrSaki, VS_MV_TYPE vt) {
    VS_SCR_CTRL *vsc_pp;

    vsc_pp = &vs_scr_ctrl[ply];
    vsc_pp->motoScr = scrMoto;
    vsc_pp->sakiScr = scrSaki;
    vsc_pp->animation_time = 0x3c;
    vsc_pp->vt = vt;
}

void vsAnimationReset(int ply, long int scr) {
    VS_SCR_CTRL *vsc_pp;

    vsc_pp = &vs_scr_ctrl[ply];
    vsc_pp->motoScr = scr;
    vsc_pp->sakiScr = scr;
    vsc_pp->animation_time = 0;
    vsc_pp->vt = VSMT_NONE;
}

static int vsScr2Move(long scr) {
    if (scr > 500) {
        scr = 500;
    }

    return scr * 2;
}

static void vsAnimationPoll(void) {
    int i, score;

    for (i = 0; i < 4; i++) {
        if (vs_scr_ctrl[i].animation_time != 0) {
            vsScoreAni[i] = 60 - vs_scr_ctrl[i].animation_time;
            score = ((vs_scr_ctrl[i].sakiScr - vs_scr_ctrl[i].motoScr) * vsScoreAni[i]) / 60;
            score += vs_scr_ctrl[i].motoScr;
            vsScoreMove[i] = score;
            vsScoreMove[i] = vsScr2Move(vsScoreMove[i]);
            if (vs_scr_ctrl[i].vt == VSMT_UP) {
                vsScoreAni[i] += 62;
            }
            if (vs_scr_ctrl[i].vt == VSMT_DW) {
                vsScoreAni[i] += 2;
            }
            vs_scr_ctrl[i].animation_time -= 1;
            if (vs_scr_ctrl[i].animation_time == 0) {
                vs_scr_ctrl[i].motoScr = vs_scr_ctrl[i].sakiScr;
            }
        } else {
            vsScoreAni[i] = 0;
            vsScoreMove[i] = vsScr2Move(vs_scr_ctrl[i].sakiScr);
        }
    }
}

static void metColorInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(metcol_str); i++) {
        Tim2Trans(cmnfGetFileAdrs(metcol_str[i].df_num));
    }
}

// TODO
static void metColorSet(EXAM_TYPE exam_type, float per) {
	u_char *moto_pp;
	u_char *saki_pp;
	int sakiper;

    if (per == 0.0f) {
        return Tim2Trans(cmnfGetFileAdrs(metcol_str[exam_type].df_num));
    }
    if (per == -1.0f) {
        return Tim2Trans(cmnfGetFileAdrs(metcol_str[exam_type].ng_num));
    }
    if (per == 1.0f) {
        return Tim2Trans(cmnfGetFileAdrs(metcol_str[exam_type].ok_num));
    }
    sakiper = metcol_str[exam_type].df_num;
    moto_pp = cmnfGetFileAdrs(sakiper);
    if (per < 0.0f) {
        per = -per;
        if (1.0f < per) {
            per = 1.0f;
        }
        sakiper = metcol_str[exam_type].ng_num;
    } else {
        if (1.0f < per) {
            per = 1.0f;
        }
        sakiper = metcol_str[exam_type].ok_num;
    }
    saki_pp = cmnfGetFileAdrs(sakiper);
    Cl2MixTrans(per * 128.0f, 128, saki_pp, moto_pp);
}

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

void ExamDispPlySet(GLOBAL_PLY *ply, int pos) {
    exam_global_ply[pos] = ply;
}

INCLUDE_ASM("asm/nonmatchings/main/mbar", ExamDispReq);

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

INCLUDE_ASM("asm/nonmatchings/main/mbar", ExamDispOn);

INCLUDE_ASM("asm/nonmatchings/main/mbar", hex2dec_mbar_tmp);

INCLUDE_ASM("asm/nonmatchings/main/mbar", hex2decPlMi);

INCLUDE_ASM("asm/nonmatchings/main/mbar", examNumDisp);

INCLUDE_ASM("asm/nonmatchings/main/mbar", examScoreSet);

INCLUDE_ASM("asm/nonmatchings/main/mbar", examLevelDisp);

INCLUDE_ASM("asm/nonmatchings/main/mbar", ExamDispSet);

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

void MbarReset(void) {
    WorkClear(mbar_req_str, sizeof(mbar_req_str));
}

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393450);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393458);

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

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarCl1CharSet);
#else
static void MbarCl1CharSet(/* a0 4 */ int col_num, /* a1 5 */ int moto_num) {
    /* a0 4 */ MBA_CHAR_DATA *mbcd_col;
    /* a1 5 */ MBA_CHAR_DATA *mbcd_mot;
    /* v0 2 */ sceGsTex0 ColGsTex0;

    mbcd_col = &mba_char_data[col_num];
    mbcd_mot = &mba_char_data[moto_num];
    
    ColGsTex0 = *(sceGsTex0*)&mbcd_col->tim2_dat_pp->GsTex0;
    *mbcd_col->tim2_dat_pp = *mbcd_mot->tim2_dat_pp;

    PR_TEX0(mbcd_col->tim2_dat_pp).CBP = ColGsTex0.CBP;
}
#endif

static void MbarCharSetSub(void) {
    int i;

    for (i = 0; i < 7; i++) {
        MbarCl1CharSet(i + 15, i + 1);
    }
}

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGifInit);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGifTrans);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarCharSet);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarCharSet2);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarWindowSet);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGetDispPosX);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGetDispPosY);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGetTimeArea);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGetTimeArea2);

int MbarGetStartTime(MBAR_REQ_STR *mr_pp) {
    return ((mr_pp->current_time + mr_pp->tapset_pp->taptimeStart - 24) / 96) * 96;
}

int MbarGetEndTime(MBAR_REQ_STR *mr_pp) {
    return (mr_pp->current_time + mr_pp->tapset_pp->taptimeEnd);
}

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarGetStartTap);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarSclRotMake);

void MbarGuideLightMake(MBARR_CHR *mbarr_pp, int mbtime) {
    u_char col = 128;

    if (mbtime >= 0 && mbtime < 144) {
        col = (144 - mbtime) * 128 / 144 + 128u;
    }

    mbarr_pp->r = mbarr_pp->g = mbarr_pp->b = col;
}

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarFlashMake);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarBackSet);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393488);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_003934A0);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarOthSet);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarCurSet);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarTapSubt);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarPosOffsetSet);

INCLUDE_ASM("asm/nonmatchings/main/mbar", mbar_othon_frame_set);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDisp);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispScene);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispSceneDraw);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispSceneVsDraw);

void MbarDispSceneVsDrawInit(void) {
    vs_mouse_disp_flag = 0;
}

INCLUDE_ASM("asm/nonmatchings/main/mbar", guidisp_init_pr);

INCLUDE_ASM("asm/nonmatchings/main/mbar", guidisp_draw_quit);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispGuiScene);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispGuiSceneMbarArea);

TIM2_DAT* lessonTim2InfoGet(void) {
    return &tim2spr_tbl_tmp1[52];
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", lessonCl2InfoGet);
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

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDemoCharDisp);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_003934E8);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393500);
