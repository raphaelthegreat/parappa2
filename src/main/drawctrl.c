#include "main/drawctrl.h"

#include "dbug/dbgmsg.h"
#include "dbug/vramsave.h"

#include "os/cmngifpk.h"
#include "os/mtc.h"
#include "os/syssub.h"
#include "os/system.h"
#include "os/tim2.h"

#include "main/cdctrl.h"
#include "main/sprite.h"
#include "main/p3str.h"

#include <prlib/prlib.h>

#include <libpad.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* sdata 399518 */ extern VCLR_PARA vclr_black_tmp; /* static */
/* data 185478 */ extern float bra_tap[10/*undef*/][2]; /* static */
/* data 1854c8 */ extern float bra_title[16/*undef*/][2]; /* static */
/* data 185548 */ extern float bra_ret[49/*undef*/]; /* static */
/* sdata 39951c */ extern int bthrow_ctrl_time; /* static */
// /* data 185610 */ extern MOZAIKU_STR mozaiku_str_poll_00[/*undef*/]; /* static */
// /* data 185720 */ extern MOZAIKU_STR mozaiku_str_poll_01[/*undef*/]; /* static */
// /* data 185a80 */ extern MOZAIKU_STR mozaiku_str_poll_02[/*undef*/]; /* static */
/* data 185ca0 */ extern MOZAIKU_POLL_STR mozaiku_poll_str[3/*undef*/]; /* static */
// /* sdata 399528 */ extern int mend_title_req; /* static */
// /* data 185cc8 */ extern MENTITLE_DAT mentitle_dat[/*undef*/]; /* static */
// /* data 185d08 */ extern MENTITLE_DAT mentitle_dat_dera[/*undef*/]; /* static */
/* sdata 39952c */ extern int ddbg_event_num; /* static */
/* sdata 399530 */ extern int ddbg_scene_num; /* static */
/* sdata 399534 */ extern int ddbg_go_event_scene; /* static */
/* sdata 399538 */ extern int ddbg_tap_num; /* static */
/* sdata 39953c */ extern int ddbg_bmp_frame; /* static */
/* data 186248 */ extern DRAW_DBG_STR draw_dbg_str[5/*undef*/]; /* static */
/* bss 1c6e030 */ extern DR_TAP_REQ dr_tap_req[16]; /* static */
/* sbss 399a4c */ extern int dr_tap_req_num; /* static */
// /* bss 1c6e0f0 */ extern int octst_time[8]; /* static */
// /* bss 1c6e110 */ extern int octst_timeLoad[8]; /* static */
/* bss 1c6e130 */ extern SCENECTRL scenectrl_outside[8]; /* static */
/* sbss 399a50 */ extern int scenectrl_outside_cnt; /* static */
/* sbss 399a54 */ extern int scenectrl_outside_read_cnt; /* static */
// /* sdata 399550 */ enum ANI_BLUMOVE_ENUM {
// 	BLMV_NONE = 0,
// 	BLMV_BLUR = 1,
// 	BLMV_MOVE = 2,
// 	BLMV_BLUR2 = 3,
// 	BLMV_MAX = 4
// };
/* bss 1c6e230 */ extern BTHROW_CTRL bthrow_ctrl[2]; /* static */
/* sbss 399a58 */ extern MOZAIKU_POLL_STR *mozaiku_poll_str_current_pp; /* static */
/* bss 1c6fe58 */ extern void *tmp_buf_adrs[16]; /* static */
/* sbss 399a5c */ extern int drawCurrentLine; /* static */
/* sbss 399a60 */ extern int drawCurrentTime; /* static */
/* sbss 399a64 */ extern int drawCurrentTimeOld; /* static */
/* sbss 399a68 */ extern EVENTREC *drawEventrec; /* static */
/* sbss 399a6c */ extern u_int useDispFlag; /* static */
/* sbss 399a70 */ extern u_int drDispFlag; /* static */
// /* sbss 399a74 */ extern float men_ctrl_ratio; /* static */
/* sbss 399a78 */ extern MEN_CTRL_ENUM men_ctrl_enum; /* static */
/* bss 1c6fe98 */ extern SCENECTRL *check_scenectrl[20]; /* static */
/* sbss 399a7c */ extern u_char ddbg_pause_f; /* static */

static void  UseGsRegSet(void);
static void  UseGsSetXyOffset(int ofs);
static SCENECTRL* getOutsideCtrlScene(int time);
static void* vs06BomAdr(OBJBTHROW_TYPE thtype, int time);
static void  MozaikuPollSceneInit(void);
static void  MozaikuPollSceneReq(int id);
static int   MozaikuPollScene(void *para_pp, int frame, int first_f, int useDisp, int drDisp);
static void  DrawObjCtrlInit(OBJCTRL *oc_pp);
static void  DrawObjStrInit(OBJSTR *os_pp);
static void  DrawObjTapStrInit(OBJTAPSTR *objtapstr_pp);
static void  DrawObjStrReq(SCENE_OBJDATA *scn_pp, int num, u_int time);
static void  DrawObjStrTapReq(SCENE_OBJDATA *scn_pp, int num, u_int time, u_char *prs_adr);
static void  DrawObjTapStrTapReq(SCENE_OBJDATA *scn_pp, int req_num, u_int time, u_char *prs_adr);
static void  DrawObjStrKill(SCENE_OBJDATA *scn_pp, int num);
static u_int GetSpfTimeCtrl(OBJDAT *objdat_pp, u_int frame);
static int   DrawObjStrDisp(SCENE_OBJDATA *scn_pp, int num, u_int time, int sw);
static int   DrawObjStrDispTap(SCENE_OBJDATA *scn_pp, int num);
static void  drawUseDrDispCheckInit(void);
static int   drawDispCheckSub(u_int drD, u_int *dat_pp);
static int   drawUseDispCheck(int useD);
static int   drawDrDispCheck(int drD);
static int   DrawScenectrlReq(SCENECTRL *scenectrl_pp, u_int time);
static void  MendererCtrlInit(void);
static void  MendererCtrl(void);
static float mendRatioTitleGet(int frame, int dera_f);
static void  MendererCtrlTitleDisp(int frame, int dera_f);
static void  DrawCtrlMain(void *x);
static void  DrawSceneStrInit(SCENESTR *scstr_pp);
static void  DrawSceneStrReset(SCENESTR *scstr_pp);
/*static*/ int   ddbg_event_sel(int pad);
/*static*/ void  ddbg_event_msg(char *buf);
/*static*/ int   ddbg_scene_sel(int pad);
/*static*/ void  ddbg_scene_msg(char *buf);
/*static*/ int   ddbg_event_check(int pad);
/*static*/ int   ddbg_scene_check(int pad);
/*static*/ int   ddbg_bmp_check(int pad);
static void  ddbg_event_sub(void);
static void  ddbg_event_sub_bmp(void);
static void  ddbg_tap_check(void);
static void  ddbg_scene_sub(void);
static void  DrawCtrlMainDebug(void *x);

static void UseGsRegSet(void) {
    sceGifPacket gifpk;
  
    CmnGifADPacketMake(&gifpk, NULL);
    CmnGifADPacketMakeTrans(&gifpk);
}

void UseGsSetXyOffset(int ofs) {
    sceGifPacket gifpk;
    int offsy;
  
    if (ofs != 0) {
        offsy = 0x7908;
    } else {
        offsy = 0x7900;
    }

    CmnGifADPacketMake(&gifpk, NULL);
    sceGifPkAddGsAD(&gifpk, SCE_GS_XYOFFSET_1, SCE_GS_SET_XYOFFSET(0x6c00, offsy));
    CmnGifADPacketMakeTrans(&gifpk);
}

void outsideDrawSceneClear(void) {
    scenectrl_outside_cnt = 0;
    scenectrl_outside_read_cnt = 0;
    WorkClear(scenectrl_outside, sizeof(scenectrl_outside));
}

int outsideDrawSceneReq(OVL_FUNC prg_pp, u_char pri, u_int useF, u_int drawF, void *param) {
    SCENECTRL *scenectrl_pp;

    if (scenectrl_outside_cnt >= PR_ARRAYSIZE(scenectrl_outside)) {
        printf("outside draw over!!\n");
        return 1;
    }

    scenectrl_pp = &scenectrl_outside[scenectrl_outside_cnt];

    scenectrl_pp->pri = pri;
    scenectrl_pp->condition = 0;

    scenectrl_pp->prg_pp = prg_pp;
    scenectrl_pp->useDisp = useF;
    scenectrl_pp->drDisp = drawF;
    scenectrl_pp->param_pp = param;

    scenectrl_outside_cnt++;
    return 0;
}

static SCENECTRL* getOutsideCtrlScene(int time) {
    SCENECTRL *scenectrl_pp;

    if (scenectrl_outside_read_cnt >= scenectrl_outside_cnt) {
        return NULL;
    }

    scenectrl_pp = &scenectrl_outside[scenectrl_outside_read_cnt];

    scenectrl_pp->start_flame = time;
    scenectrl_pp->end_flame = time + 1;

    scenectrl_outside_read_cnt++;
    return scenectrl_pp;
}

float* bra_tap_GetNext(PR_MODELHANDLE model) {
    int    i;
    float  tmp_f;
    float *ret;

    ret = NULL;
    tmp_f = PrGetContourBlurAlpha(model);

    if (tmp_f == -1.0f) {
        return NULL;
    }

    for (i = 0; i < PR_ARRAYSIZEU(bra_tap) - 1; i++) {
        if (bra_tap[i][0] == tmp_f) {
            ret = &bra_tap[i + 1][0];
            break;
        }
    }

    return ret;
}

float* bra_title_GetNext(PR_MODELHANDLE model) {
    int i;
    float tmp_f;
    float *ret;

    ret = NULL;
    tmp_f = PrGetContourBlurAlpha(model);

    if (tmp_f == -1.0f) {
        return NULL;
    }

    for (i = 0; i < PR_ARRAYSIZEU(bra_title) - 1; i++) {
        if (bra_title[i][0] == tmp_f) {
            ret = &bra_title[i + 1][0];
            break;
        }
    }

    return ret;
}

float* bra_ret_GetNext(PR_MODELHANDLE model) {
    int i;
    float tmp_f;
    float *ret;

    ret = NULL;
    tmp_f = PrGetTransactionBlendRatio(model);

    if (tmp_f == -1.0f) {
        return NULL;
    }

    for (i = 0; i < PR_ARRAYSIZEU(bra_ret) - 1; i++) {
        if (bra_ret[i] == tmp_f) {
            ret = &bra_ret[i + 1];
            break;
        }
    }

    return ret;
}

void XAnimationLinkOption(PR_MODELHANDLE model, PR_ANIMATIONHANDLE animation, int first, int blumove, float time) {
    PR_ANIMATIONHANDLE anim_tmp;
    float *tmp_dat;

    if (blumove == BLMV_MOVE) {
        first = FALSE;
    }

    anim_tmp = PrGetLinkedAnimation(model);
    if (anim_tmp != animation) {
        if (anim_tmp != NULL) {
            first = TRUE;
        } else {
            first = FALSE;
            PrLinkAnimation(model, animation);
            PrSetTransactionBlendRatio(model, -1.0f);
            PrSetContourBlurAlpha(model, -1.0f, -1.0f);
        }
    }

    if (first) {
        switch (blumove) {
        case BLMV_NONE:
            PrResetPosture(model);
            PrSetTransactionBlendRatio(model, -1.0f);
            PrResetContour(model);
            PrSetContourBlurAlpha(model, -1.0f, -1.0f);
            break;
        case BLMV_BLUR:
            PrResetPosture(model);
            PrSetTransactionBlendRatio(model, -1.0f);
            PrSaveContour(model);
            PrSetContourBlurAlpha(model, bra_tap[0][0], bra_tap[0][1]);
            break;
        case BLMV_BLUR2:
            PrResetPosture(model);
            PrSetTransactionBlendRatio(model, -1.0f);
            PrSaveContour(model);
            PrSetContourBlurAlpha(model, bra_title[0][0], bra_title[0][1]);
            break;
        case BLMV_MOVE:
            PrResetContour(model);
            PrSetContourBlurAlpha(model, -1.0f, -1.0f);
            PrSavePosture(model);
            PrSetTransactionBlendRatio(model, bra_ret[0]);
            break;
        }

        PrLinkAnimation(model, animation);
    } else {
        switch (blumove) {
        case BLMV_NONE:
            break;
        case BLMV_BLUR:
            tmp_dat = bra_tap_GetNext(model);
            if (tmp_dat == NULL) {
                PrResetContour(model);
                PrSetContourBlurAlpha(model, -1.0f, -1.0f);
            } else {
                PrSetContourBlurAlpha(model, tmp_dat[0], tmp_dat[1]);
            }
            break;
        case BLMV_BLUR2:
            tmp_dat = bra_title_GetNext(model);
            if (tmp_dat == NULL) {
                PrResetContour(model);
                PrSetContourBlurAlpha(model, -1.0f, -1.0f);
            } else {
                PrSetContourBlurAlpha(model, tmp_dat[0], tmp_dat[1]);
            }
            break;
        case BLMV_MOVE:
            tmp_dat = bra_ret_GetNext(model);
            if (tmp_dat == NULL) {
                PrResetPosture(model);
                PrSetTransactionBlendRatio(model, -1.0f);
            } else {
                PrSetTransactionBlendRatio(model, tmp_dat[0]);
            }
            break;
        }
    }

    PrAnimateModel(model, time + 0.0f);
    PrShowModel(model, NULL);
}

void XAnimationPositionLink(PR_MODELHANDLE model, PR_ANIMATIONHANDLE animation, float time) {
    PrLinkPositionAnimation(model, animation);
    PrAnimateModelPosition(model, time + 0.0f);
}

void XAnimationPositionUnLink(PR_MODELHANDLE model, PR_ANIMATIONHANDLE animation) {
    if (PrGetLinkedPositionAnimation(model) == animation) {
        PrUnlinkPositionAnimation(model);
    }
}

void BallThrowSetFrame(int frame) {
    bthrow_ctrl_time = frame;
}

void BallThrowInit(void) {
    WorkClear(bthrow_ctrl, sizeof(bthrow_ctrl));

    bthrow_ctrl[0].targetY = 112.0f;
    bthrow_ctrl[0].targetX = 120.0f;

    bthrow_ctrl[1].targetX = 520.0f;
    bthrow_ctrl[1].targetY = 112.0f;
}

void BallThrowInitDare(int dare) {
    WorkClear(&bthrow_ctrl[dare], sizeof(BTHROW_CTRL));
}

static void* vs06BomAdr(OBJBTHROW_TYPE thtype, int time) {
    u_short bomdat_tea[12] = {
        0x178, 0x177, 0x176, 0x175,
        0x174, 0x173, 0x172, 0x171,
        0x170, 0x16f, 0x16e, 0x16d,
    };
    u_short bomdat_pa[12] = {
        0x184, 0x183, 0x182, 0x181,
        0x180, 0x17f, 0x17e, 0x17d,
        0x17c, 0x17b, 0x17a, 0x179,
    };
    u_short *bomdat;

    if (global_data.play_step != PSTEP_VS || time >= 12) {
        return NULL;
    }

    if (thtype == OBJBTHROW_PARAPPA) {
        bomdat = bomdat_pa;
    } else {
        bomdat = bomdat_tea;
    }

    return GetIntAdrsCurrent(bomdat[time]);
}

void BallThrowReq(void *mdlh, OBJBTHROW_TYPE thtype, void *texpp, void *mdlhoming) {
    BTHROW_STR *bt_pp;

    if (bthrow_ctrl[thtype].bthrow_str_cnt >= PR_ARRAYSIZE(bthrow_ctrl[thtype].bthrow_str)) {
        return;
    }

    bt_pp = &bthrow_ctrl[thtype].bthrow_str[bthrow_ctrl[thtype].bthrow_str_cnt];

    bt_pp->use = 3;
    bt_pp->mdl_adr = mdlh;
    bt_pp->tim2_dat_pp = texpp;
    bt_pp->homingpp = mdlhoming;

    if (global_data.play_step == PSTEP_VS) {
        bt_pp->endTime = 32;
        if (bt_pp->endTime > bthrow_ctrl_time) {
            bt_pp->endTime = bthrow_ctrl_time;
        }
    } else {
        bt_pp->endTime = bthrow_ctrl_time;
    }

    bthrow_ctrl[thtype].frame = bthrow_ctrl_time;
    bthrow_ctrl[thtype].bthrow_str_cnt++;
}

void BallThrowTarget(void *mdlh, OBJBTHROW_TYPE thtype, int targetframe) {
    bthrow_ctrl[thtype].targ_mdl_adr = mdlh;
}

void BallThrowPoll(void) {
    int i, j;

    sceGifPacket gifP;

    TIM2INFO info;
    int w, h;
    int px, py;

    for (i = 0; i < PR_ARRAYSIZE(bthrow_ctrl); i++) {
        if (bthrow_ctrl[i].frame == 0) {
            BallThrowInitDare(i);
        } else {
            if (bthrow_ctrl[i].targ_mdl_adr != NULL) {
                float *pos_pp = PrGetModelScreenPosition(bthrow_ctrl[i].targ_mdl_adr);

                bthrow_ctrl[i].targetX = pos_pp[0];
                bthrow_ctrl[i].targetY = pos_pp[1] + -16.0f;
                bthrow_ctrl[i].targ_mdl_adr = NULL;
            }

            if (bthrow_ctrl[i].bthrow_str_cnt != 0) {
                BTHROW_STR *bts_pp = bthrow_ctrl[i].bthrow_str;
                
                CmnGifADPacketMake(&gifP, DrawGetFrameP(DNUM_DRAW));
                sceGifPkAddGsAD(&gifP, SCE_GS_TEXA, 0x8000008000);
                sceGifPkAddGsAD(&gifP, SCE_GS_ALPHA_1, 0x44);
                sceGifPkAddGsAD(&gifP, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
                
                for (j = 0; j < bthrow_ctrl[i].bthrow_str_cnt; j++, bts_pp++) {
                    if (bts_pp->use & 1) {
                        float *pos_pp = PrGetModelScreenPosition(bts_pp->homingpp);

                        bthrow_ctrl[i].targetX = pos_pp[0];
                        bthrow_ctrl[i].targetY = pos_pp[1] + -16.0f;

                        if (bts_pp->use & 2) {
                            float *pos_pp;

                            bts_pp->use &= 0xfd;
                            
                            pos_pp = PrGetModelScreenPosition(bts_pp->mdl_adr);
                            
                            bts_pp->xp = pos_pp[0];
                            bts_pp->yp = pos_pp[1] + -16.0f;
                            bts_pp->mdl_adr = NULL;

                            if (global_data.play_step == PSTEP_VS && bts_pp->endTime < 12) {
                                bts_pp->xp = bthrow_ctrl[i].targetX;
                                bts_pp->yp = bthrow_ctrl[i].targetY;
                            }
                        } else if (global_data.play_step == PSTEP_VS) {
                            if (bts_pp->endTime < 12) {
                                if (bts_pp->endTime == 0) {
                                    bts_pp->use = 0;
                                }
                                if (bts_pp->endTime == 11) {
                                    bts_pp->xp = bthrow_ctrl[i].targetX;
                                    bts_pp->yp = bthrow_ctrl[i].targetY;
                                }

                                bts_pp->tim2_dat_pp = vs06BomAdr(i, bts_pp->endTime);
                            } else if (bts_pp->endTime != 12) {
                                bts_pp->xp += (bthrow_ctrl[i].targetX - bts_pp->xp) / (bts_pp->endTime - 12);
                                bts_pp->yp += (bthrow_ctrl[i].targetY - bts_pp->yp) / (bts_pp->endTime - 12);
                            } else {
                                bts_pp->xp = bthrow_ctrl[i].targetX;
                                bts_pp->yp = bthrow_ctrl[i].targetY;
                            }
                        } else if (bts_pp->endTime == 0) {
                            bts_pp->use = 0;
                            bts_pp->xp = bthrow_ctrl[i].targetX;
                            bts_pp->yp = bthrow_ctrl[i].targetY;
                        } else  {
                            bts_pp->xp += (bthrow_ctrl[i].targetX - bts_pp->xp) / bts_pp->endTime;
                            bts_pp->yp += (bthrow_ctrl[i].targetY - bts_pp->yp) / bts_pp->endTime;
                        }

                        GetTim2Info(bts_pp->tim2_dat_pp, &info, 1);

                        w = info.picturH->ImageWidth * 16;
                        h = info.picturH->ImageHeight * 16;

                        sceGifPkAddGsAD(&gifP, SCE_GS_TEX0_1, info.picturH->GsTex0);
                        sceGifPkAddGsAD(&gifP, SCE_GS_PRIM, 0x156);
                        
                        px = ((int)(bts_pp->xp * 16.0f) - (w) + 27648);
                        py = ((int)(bts_pp->yp * 16.0f) - (h / 2) + 30976);

                        sceGifPkAddGsAD(&gifP, SCE_GS_UV, SCE_GS_SET_UV(0, 0));

                        px &= 0xffff;
                        py &= 0xffff;
                        
                        sceGifPkAddGsAD(&gifP, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(px, py, 0));

                        px += w * 2;
                        py += h;
                        
                        sceGifPkAddGsAD(&gifP, SCE_GS_UV, SCE_GS_SET_UV(w, h));
                        sceGifPkAddGsAD(&gifP, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(px, py, 0));

                        if (bts_pp->use != 0) {
                            bts_pp->endTime--;
                        }
                    }
                }

                CmnGifADPacketMakeTrans(&gifP);
                bthrow_ctrl[i].frame--;
            }
        }
    }
}

int BallThrowPollScene(void *para_pp, int frame, int first_f, int useDisp, int drDisp) {
    ChangeDrawArea(DrawGetDrawEnvP(drDisp));
    BallThrowPoll();
    return 0;
}

static void MozaikuPollSceneInit(void) {
    mozaiku_poll_str_current_pp = NULL;
}

static void MozaikuPollSceneReq(int id) {
    if (id >= PR_ARRAYSIZEU(mozaiku_poll_str)) {
        return;
    }

    mozaiku_poll_str_current_pp = &mozaiku_poll_str[id];
    mozaiku_poll_str_current_pp->mozaiku_str_frame = 0;
}

static int MozaikuPollScene(void *para_pp, int frame, int first_f, int useDisp, int drDisp) {
    if (mozaiku_poll_str_current_pp == NULL) {
        return 0;
    }

    if (mozaiku_poll_str_current_pp->mozaiku_str_frame >= mozaiku_poll_str_current_pp->mozaiku_str_cnt) {
        mozaiku_poll_str_current_pp->mozaiku_str_frame = 0;
        mozaiku_poll_str_current_pp = NULL;
        return 0;
    } else {
        ChangeDrawArea(DrawGetDrawEnvP(drDisp));

        DrawMozaikuDisp(&mozaiku_poll_str_current_pp->mozaiku_str_pp[mozaiku_poll_str_current_pp->mozaiku_str_frame], frame, first_f, useDisp, drDisp);
        mozaiku_poll_str_current_pp->mozaiku_str_frame++;
        return 0;
    }
}

void DrawTmpBuffInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(tmp_buf_adrs); i++) {
        tmp_buf_adrs[i] = NULL;
    }
}

void DrawTmpBuffQuit(void *adrs) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(tmp_buf_adrs); i++) {
        if (tmp_buf_adrs[i] != NULL) {
            if (adrs == NULL || adrs == tmp_buf_adrs[i]) {
                usrFree(tmp_buf_adrs[i]);
                tmp_buf_adrs[i] = NULL;
            }
        }
    }
}

void* DrawTmpBuffGetArea(int size) {
    int   i;
    void *ret = NULL;

    for (i = 0; i < PR_ARRAYSIZE(tmp_buf_adrs); i++) {
        if (tmp_buf_adrs[i] == NULL) {
            ret = usrMalloc(size);
            tmp_buf_adrs[i] = ret;
            break;
        }
    }

    return ret;
}

void DrawObjdatInit(int size, OBJDAT *od_pp, PR_SCENEHANDLE prf) {
    int    i;
    void  *tmp_adr;
    float  frame;

    for (i = 0; i < size; i++, od_pp++) {
        tmp_adr = GetIntAdrsCurrent(od_pp->objdat_num);

        switch (od_pp->objdat_type) {
        case ODAT_SPA:
            od_pp->handle = PrInitializeAnimation(tmp_adr);
           
            frame = PrGetAnimationEndFrame(od_pp->handle);
            od_pp->maxfr = frame + frame;
            break;
        case ODAT_SPM:
            od_pp->handle = PrInitializeModel(tmp_adr, prf);
            break;
        case ODAT_SPC:
            od_pp->handle = PrInitializeCamera(tmp_adr);

            frame = PrGetCameraEndFrame(od_pp->handle);
            od_pp->maxfr = frame + frame;
            break;
        case ODAT_SPF:
        case ODAT_TM2:
        case ODAT_CL2:
            od_pp->handle = tmp_adr;
            break;
        default:
            printf("Objdat Error!! type[%d]\n", od_pp->objdat_type);
            break;
        }
    }
}

// FIXME: Is this necessary?
const int drawctrl_rodata_padding[] = { 0, 0 };

void DrawObjdatReset(int size, OBJDAT *od_pp) {
    int i;

    for (i = 0; i < size; i++, od_pp++) {
        /* not assigned to any variables */
        GetIntAdrsCurrent(od_pp->objdat_num);

        switch (od_pp->objdat_type) {
        case ODAT_SPA:
            break;
        case ODAT_SPM:
            PrResetPosture(od_pp->handle);
            PrResetContour(od_pp->handle);
            PrHideModel(od_pp->handle);

            if (PrGetLinkedAnimation(od_pp->handle) != NULL) {
                PrAnimateModel(od_pp->handle, 0.0f);
                PrUnlinkAnimation(od_pp->handle);
            }
            if (PrGetLinkedPositionAnimation(od_pp->handle) != NULL) {
                PrUnlinkPositionAnimation(od_pp->handle);
                PrAnimateModelPosition(od_pp->handle, 0.0f);
            }
            break;
        case ODAT_SPC:
        case ODAT_SPF:
        case ODAT_TM2:
        case ODAT_CL2:
            break;
        default:
            printf("Objdat Error!! type[%d]\n", od_pp->objdat_type);
            break;
        }
    }
}

void DrawSceneInit(sceGsDrawEnv1 *draw_env, SCENE_OBJDATA *scene_pp, int useDisp) {
    int   i;
    u_int fbp_tmp;

    if (scene_pp->handle != NULL) {
        return;
    }

    if (useDisp != 0) {
        fbp_tmp = DrawGetFbpPos(useDisp);
    } else {
        fbp_tmp = -1;
    }

    scene_pp->handle = PrInitializeScene(draw_env, scene_pp->usrName, fbp_tmp);
    DrawObjdatInit(scene_pp->objdat_size, scene_pp->objdat_pp, scene_pp->handle);

    for (i = 0; i < PR_ARRAYSIZE(scene_pp->objactprg_ctrl.objactprg); i++) {
        scene_pp->objactprg_ctrl.objactprg[i] = NULL;
    }

    scene_pp->objactprg_ctrl.num = 0;
    PrPreprocessSceneModel(scene_pp->handle);
}

static void DrawObjCtrlInit(OBJCTRL *oc_pp) {
    oc_pp->PRflag = 0;
    oc_pp->PRdata = 0;
    oc_pp->PRtime = 0;
}

static void DrawObjStrInit(OBJSTR *os_pp) {
    int i;

    for (i = 0; i < os_pp->size; i++) {
        DrawObjCtrlInit(&os_pp->objctrl_pp[i]);
    }

    os_pp->PRflag     = 0;
    os_pp->PRdata     = 0;
    os_pp->PRtime     = 0;
    os_pp->PRpress    = NULL;
    os_pp->PRtimeOld  = -1;

    os_pp->current_pp = NULL;
    os_pp->loop_time  = 0;
    os_pp->loop_pp    = NULL;
}

static void DrawObjTapStrInit(OBJTAPSTR *objtapstr_pp) {
    objtapstr_pp->ovl_cnt_num = 0;
}

static void DrawObjStrReq(SCENE_OBJDATA *scn_pp, int num, u_int time) {
    if (scn_pp->objstr_size <= num) {
        return;
    }

    scn_pp->objstr_pp[num].PRflag    = TRUE;
    scn_pp->objstr_pp[num].PRtime    = time;
    scn_pp->objstr_pp[num].PRtimeOld = -1;
}

static void DrawObjStrTapReq(SCENE_OBJDATA *scn_pp, int num, u_int time, u_char *prs_adr) {
    if (scn_pp->tapstr_size <= num) {
        return;
    }

    scn_pp->tapstr_pp[num].PRflag    = TRUE;
    scn_pp->tapstr_pp[num].PRtime    = 0;
    scn_pp->tapstr_pp[num].PRpress   = prs_adr;
    scn_pp->tapstr_pp[num].PRtimeOld = -1;
}

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawObjTapStrTapReq);

void DrawObjStrTapQuit(SCENE_OBJDATA *scn_pp, int num, u_int time) {
    if (num < 0 || scn_pp->tapstr_size <= num) {
        return;
    }

    scn_pp->tapstr_pp[num].PRflag    = FALSE;
    scn_pp->tapstr_pp[num].PRtime    = 0;
    scn_pp->tapstr_pp[num].PRpress   = NULL;
    scn_pp->tapstr_pp[num].PRtimeOld = -1;
}

void DrawObjTapStrTapQuit(SCENE_OBJDATA *scn_pp, int req_num) {
    int     i;

    int     max       = scn_pp->objtapstr_pp[req_num].objtap_size;
    OBJTAP *objtap_pp = scn_pp->objtapstr_pp[req_num].objtap_pp;

    for (i = 0; i < max; i++, objtap_pp++) {
        if ((u_short)(objtap_pp->channel + 16) > 2) {
            DrawObjStrTapQuit(scn_pp, scn_pp->dr_tap_req[objtap_pp->channel].req_no, 0);
            scn_pp->dr_tap_req[objtap_pp->channel].req_no = -1;
        }
    }
}

static void DrawObjStrKill(SCENE_OBJDATA *scn_pp, int num) {
    if (scn_pp->objstr_size <= num) {
        return;
    }

    scn_pp->objstr_pp[num].PRflag    = FALSE;
    scn_pp->objstr_pp[num].PRtime    = 0;
    scn_pp->objstr_pp[num].PRpress   = NULL;
    scn_pp->objstr_pp[num].PRtimeOld = -1;
}

static u_int GetSpfTimeCtrl(OBJDAT *objdat_pp, u_int frame) {
    SPF_STR *spf_str_pp;
    u_int    max_cnt;
    u_int    ret;

    spf_str_pp = objdat_pp->handle;
    max_cnt = (spf_str_pp->maxFrame * 2) - 2;
    if (frame > max_cnt) {
        frame = max_cnt;
    }

    if (frame & 0x1) {
        ret = (spf_str_pp->frame[frame / 2] + spf_str_pp->frame[(frame / 2) + 1]) / 2;
    } else {
        ret = spf_str_pp->frame[frame / 2];
    }

    return ret;
}

void camOtherKill(OBJACTPRG *objactprg_pp, int objactprg_num, int oya_num) {
    int i;

    for (i = 0; i < objactprg_num; i++, objactprg_pp++) {
        if (i != oya_num) {
            if (objactprg_pp->job_type == OCTRL_CAM) {
                objactprg_pp->job_type = OCTRL_NON;
            }
        }
    }
}

void posAniOtherKill(OBJACTPRG *objactprg_pp, int objactprg_num, int ani_num, int mod_num) {
    int i;

    for (i = 0; i < objactprg_num; i++, objactprg_pp++) {
        if (i != ani_num) {
            if (objactprg_pp->job_type == OCTRL_ANIPOS && objactprg_pp->sub_num == mod_num) {
                objactprg_pp->job_type = OCTRL_NON;
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawObjStrDisp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawObjStrDispTap);

void DrawSceneReset(SCENE_OBJDATA *scene_pp) {
    int i;

    DrawObjdatReset(scene_pp->objdat_size, scene_pp->objdat_pp);

    for (i = 0; i < scene_pp->objstr_size; i++) {
        DrawObjStrInit(&scene_pp->objstr_pp[i]);
    }

    for (i = 0; i < scene_pp->tapstr_size; i++) {
        DrawObjStrInit(&scene_pp->tapstr_pp[i]);
    }

    for (i = 0; i < scene_pp->objtapstr_size; i++) {
        DrawObjTapStrInit(&scene_pp->objtapstr_pp[i]);
    }

    for (i = 0; i < PR_ARRAYSIZE(scene_pp->objactprg_ctrl.objactprg); i++) {
        if (scene_pp->objactprg_ctrl.objactprg[i] != NULL) {
            DrawTmpBuffQuit(scene_pp->objactprg_ctrl.objactprg[i]);
            scene_pp->objactprg_ctrl.objactprg[i] = NULL;
        }
    }

    for (i = 0; i < PR_ARRAYSIZE(scene_pp->dr_tap_req); i++) {
        scene_pp->dr_tap_req[i].req_no = -1;
    }
}

void DrawSceneFirstSet(SCENE_OBJDATA *scene_pp) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(scene_pp->objactprg_ctrl.objactprg); i++) {
        if (scene_pp->objactprg_ctrl.objactprg[i] == NULL) {
            scene_pp->objactprg_ctrl.objactprg[i] = DrawTmpBuffGetArea(scene_pp->objdat_size * sizeof(OBJACTPRG));
        }

        WorkClear(scene_pp->objactprg_ctrl.objactprg[i], scene_pp->objdat_size * sizeof(OBJACTPRG));
    }

    scene_pp->objactprg_ctrl.num = scene_pp->objdat_size;

    for (i = 0; i < PR_ARRAYSIZE(scene_pp->dr_tap_req); i++) {
        scene_pp->dr_tap_req[i].req_no = -1;
    }
}

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", Cl2MixTrans);

INCLUDE_RODATA("asm/nonmatchings/main/drawctrl", D_00393290);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawObjPrReq);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawObjStrTapTimeNext);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawObjTapCtrl);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawSceneObjData);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawDoubleDispIn);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawVramClear);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawMoveDispIn);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawAlphaBlendDisp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawMozaikuDisp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawFadeDisp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawPlphaIndex8Disp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawTim2DIsp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawNoodlesDisp);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawVramLocalCopy);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawVramLocalCopy2);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", drawUseDrDispCheckInit);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", drawDispCheckSub);

/* static */ int drawUseDispCheck(int useD) {
    return drawDispCheckSub(useD, &useDispFlag);
}

/* static */ int drawDrDispCheck(int drD) {
    return drawDispCheckSub(drD, &drDispFlag);
}

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", DrawScenectrlReq);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", MendererCtrlInit);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", MendererCtrlTitle);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", MendererCtrlTitleDera);

INCLUDE_RODATA("asm/nonmatchings/main/drawctrl", D_00393300);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", MendererCtrl);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", mendRatioTitleGet);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", MendererCtrlTitleDisp);

void MendererReq(MEN_CTRL_ENUM menum) {
    men_ctrl_enum = menum;
}

MEN_CTRL_ENUM GetMendererEnum(void) {
    return men_ctrl_enum;
}

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", MendererCtrlScene);

INCLUDE_ASM("asm/nonmatchings/main/drawctrl", sceneConditionCheck);

static void DrawCtrlMain(void *x) {
    int        i, j, lsjkl;
    int        check_cnt;
    SCENESTR  *scenestr_pp;
    SCENECTRL *scenectrl_pp;
    int        scene_req_flag;
    SCENECTRL *scenectrl_tmp;
    
    BallThrowInit();
    MozaikuPollSceneInit();

    while (1) {
        scene_req_flag = 0;

        if (GlobalMendererUseCheck()) {
            PrCreateMendererTexture();
        }

        if (drawCurrentLine >= drawEventrec->scenestr_size) {
            scenestr_pp  = NULL;
            scenectrl_pp = NULL;
        } else {
            scenestr_pp  = &drawEventrec->scenestr_pp[drawCurrentLine];
            scenectrl_pp = scenestr_pp->scenectrl_pp;
        }

        drawUseDrDispCheckInit();
        check_cnt = 0;

        if (global_data.play_step == PSTEP_XTR) {
            scenestr_pp  = NULL;
            scenectrl_pp = NULL;
            p3StrPoll(drawCurrentTime);
        }

        if (scenectrl_pp != NULL) {
            for (i = 0; i < PR_ARRAYSIZE(check_scenectrl); i++) {
                check_scenectrl[i] = NULL;
            }

            for (i = 0; i < scenestr_pp->scenectrl_num; i++, scenectrl_pp++) {
                if (scenectrl_pp->start_flame <= drawCurrentTime &&
                    scenectrl_pp->end_flame    > drawCurrentTime) {
                    if (sceneConditionCheck(scenectrl_pp->condition)) {
                        check_scenectrl[check_cnt] = scenectrl_pp;
                        check_cnt++;
                        continue;
                    }
                }

                DrawScenectrlReq(scenectrl_pp, INT_MAX);
            }
        }

        outsideDrawSceneReq(BallThrowPollScene, 200, DNUM_NON, DNUM_DRAW, NULL);
        outsideDrawSceneReq(MozaikuPollScene, 215, DNUM_DRAW, DNUM_DRAW, NULL);

        if (GlobalMendererUseCheck()) {
            outsideDrawSceneReq(MendererCtrlScene, 210, DNUM_NON, DNUM_DRAW, &scene_req_flag);
        }

        if (global_data.play_step == PSTEP_VS || global_data.play_step == PSTEP_HOOK) {
            outsideDrawSceneReq(DrawVramClear, 229, DNUM_NON, DNUM_ZBUFF, &vclr_black_tmp);
        }

        while ((scenectrl_tmp = getOutsideCtrlScene(drawCurrentTime)) != NULL) {
            check_scenectrl[check_cnt] = scenectrl_tmp;
            check_cnt++;
        }

        for (i = 0; i < check_cnt - 1; i++) {
            for (j = i + 1; j < check_cnt; j++) {
                if (check_scenectrl[i]->pri > check_scenectrl[j]->pri) {
                    SCENECTRL *tmp = check_scenectrl[i];
                    check_scenectrl[i] = check_scenectrl[j];
                    check_scenectrl[j] = tmp;
                }
            }
        }

        for (lsjkl = 0; lsjkl < check_cnt; lsjkl++) {
            float men_tmp = PrGetMendererRatio();

            if (check_scenectrl[lsjkl]->prg_pp == DrawSceneObjData) {
                if (check_scenectrl[lsjkl]->pri >= 211) {
                    PrSetMendererRatio(0.0f);
                }
                
                scene_req_flag = 1;
                DrawScenectrlReq(check_scenectrl[lsjkl], drawCurrentTime);
                PrSetMendererRatio(men_tmp);
            } else {
                if (!scene_req_flag) {
                    UsrPrSetScene();
                    scene_req_flag = 1;
                }

                DrawScenectrlReq(check_scenectrl[lsjkl], drawCurrentTime);
            }
        }

        dr_tap_req_num = 0;

        ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));
        UseGsRegSet();

        if (!scene_req_flag) {
            UsrPrSetScene();
        }

        outsideDrawSceneClear();
        MtcWait(1);
    }
}

static void DrawSceneStrInit(SCENESTR *scstr_pp) {
    int i;

    for (i = 0; i < scstr_pp->scenectrl_num; i++) {
        scstr_pp->scenectrl_pp[i].use_flag = 0;

        if (scstr_pp->scenectrl_pp[i].prg_pp != NULL) {
            /* 
             * Call the overlay function pointer (DrawSceneObjData)
             * to initialize the scenes and handles for each object 
             */
            scstr_pp->scenectrl_pp[i].prg_pp(scstr_pp->scenectrl_pp[i].param_pp, 0, -2, scstr_pp->scenectrl_pp[i].useDisp, scstr_pp->scenectrl_pp[i].drDisp);
        }
    }
}

static void DrawSceneStrReset(SCENESTR *scstr_pp) {
    int i;

    if (scstr_pp == NULL || scstr_pp->scenectrl_pp == NULL) {
        return;
    }

    for (i = 0; i < scstr_pp->scenectrl_num; i++) {
        if (scstr_pp->scenectrl_pp[i].use_flag != 0) {
            scstr_pp->scenectrl_pp[i].use_flag = 0;

            if (scstr_pp->scenectrl_pp[i].prg_pp != NULL) {
                /* 
                 * Call the overlay function pointer (DrawSceneObjData)
                 * to reset the objects 
                 */
                scstr_pp->scenectrl_pp[i].prg_pp(scstr_pp->scenectrl_pp[i].param_pp, 0, -1, 0, 0);
            }
        }
    }
}

void resetDrawSceneObjData(SCENESTR *scstr_pp) {
    int            i;
    SCENE_OBJDATA *sod_pp;

    for (i = 0; i < scstr_pp->scenectrl_num; i++) {
        if (scstr_pp->scenectrl_pp[i].prg_pp != NULL &&
            scstr_pp->scenectrl_pp[i].prg_pp == DrawSceneObjData) {
            sod_pp = scstr_pp->scenectrl_pp[i].param_pp;
            sod_pp->handle = NULL;
        }
    }
}

void DrawCtrlInit(EVENTREC *ev_pp, int ctrlTbl, void *dat_top) {
    int i;

    PrSetFrameRate(60.0f);
    DrawTmpBuffInit();

    for (i = 0; i < ev_pp->scenestr_size; i++) {
        resetDrawSceneObjData(&ev_pp->scenestr_pp[i]);
    }

    for (i = 0; i < ev_pp->scenestr_size; i++) {
        DrawSceneStrInit(&ev_pp->scenestr_pp[i]);
    }

    drawEventrec    = ev_pp;
    drawCurrentLine = ctrlTbl;

    drawCurrentTime    = 0;
    drawCurrentTimeOld = -1;
    dr_tap_req_num     = 0;

    outsideDrawSceneClear();
    MendererCtrlInit();
    
    MtcExec(DrawCtrlMain, MTC_TASK_DRAWCTRL);
}

void DrawCtrlQuit(void) {
    int       i;
    EVENTREC *ev_pp = drawEventrec;

    DrawTmpBuffQuit(NULL);

    for (i = 0; i < ev_pp->scenestr_size; i++) {
        resetDrawSceneObjData(&ev_pp->scenestr_pp[i]);
    }

    if (global_data.play_step == PSTEP_XTR) {
        p3StrQuitSd();
    }

    MendererCtrlInit();

    PrSetMendererRatio(0.0f);
    PrCleanupModel(NULL);
    PrCleanupAnimation(NULL);
    PrCleanupCamera(NULL);
    PrCleanupScene(NULL);

    MtcKill(MTC_TASK_DRAWCTRL);
}

void DrawCtrlTimeSet(int time) {
    if (drawCurrentTime == 0) {
        drawCurrentTime = time;
        return;
    }

    if (drawCurrentTime == time) {
        drawCurrentTime = time + 1;
        return;
    }

    drawCurrentTime = time;
}

void DrawCtrlTblChange(int ctrlTbl) {
    if (ctrlTbl == drawCurrentLine) {
        return;
    }

    if (drawCurrentLine >= drawEventrec->scenestr_size) {
        drawCurrentLine = ctrlTbl;
        return;
    }

    DrawSceneStrReset(&drawEventrec->scenestr_pp[drawCurrentLine]);
    drawCurrentLine = ctrlTbl;
}

int DrawTapReqTbl(int atap, PLAYER_INDEX pindx, u_char *prs_pp) {
    if (dr_tap_req_num >= 16) {
        printf("TAP REQ OVER ERROR!!\n");
        return 1;
    }

    dr_tap_req[dr_tap_req_num].tap_id = (u_int)(atap & 0xff00) >> 0x8;
    dr_tap_req[dr_tap_req_num].req_no = atap & 0xff;
    dr_tap_req[dr_tap_req_num].player_index = pindx;
    dr_tap_req[dr_tap_req_num].pad_prs_pp = prs_pp;
    dr_tap_req_num++;
    return 0;
}

/* static */ int ddbg_event_sel(int pad) {
    ddbg_event_num += pad;

    if (ddbg_event_num < 0) {
        ddbg_event_num = drawEventrec->scenestr_size - 1;
    }
    
    if (ddbg_event_num >= drawEventrec->scenestr_size) {
        ddbg_event_num = 0;
    }

    drawCurrentLine = ddbg_event_num;
    
    ddbg_scene_sel(0);
    return 0;
}

extern const char D_00399540[]; // .sdata - "#%02X"

/* static */ void ddbg_event_msg(char *buf) {
    sprintf(buf, D_00399540, ddbg_event_num);
}

/* static */ int ddbg_scene_sel(int pad) {
    SCENESTR *scenestr_pp = &drawEventrec->scenestr_pp[ddbg_event_num];

    ddbg_scene_num += pad;

    if (ddbg_scene_num < 0) {
        ddbg_scene_num = scenestr_pp->scenectrl_num - 1;
    }
    
    if (ddbg_scene_num >= scenestr_pp->scenectrl_num) {
        ddbg_scene_num = 0;
    }

    return 0;
}

/* static */ void ddbg_scene_msg(char *buf) {
    sprintf(buf, D_00399540, ddbg_scene_num);
}

/* static */ int ddbg_event_check(int pad) {
    ddbg_go_event_scene = 0;
    return 1;
}

/* static */ int ddbg_scene_check(int pad) {
    ddbg_go_event_scene = 1;
    return 1;
}

/* static */ int ddbg_bmp_check(int pad) {
    ddbg_go_event_scene = 2;
    return 1;
}

static void ddbg_event_sub(void) {
    int        i, j, lsjkl;
    int        check_cnt;
    SCENECTRL *scenectrl_pp;
    SCENESTR  *scenestr_pp;

    if (drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp == NULL) {
        return;
    }

    scenectrl_pp = drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp;
    scenestr_pp = &drawEventrec->scenestr_pp[drawCurrentLine];

    check_cnt = 0;
    drawUseDrDispCheckInit();

    for (i = 0; i < PR_ARRAYSIZE(check_scenectrl); i++) {
        check_scenectrl[i] = NULL;
    }

    for (i = 0; i < scenestr_pp->scenectrl_num; i++, scenectrl_pp++) {
        if (scenectrl_pp->start_flame <= drawCurrentTime &&
            scenectrl_pp->end_flame    > drawCurrentTime) {
            check_scenectrl[check_cnt] = scenectrl_pp;
            check_cnt++;
        } else {
            DrawScenectrlReq(scenectrl_pp, drawCurrentTime);
        }
    }

    for (i = 0; i < check_cnt - 1; i++) {
        for (j = i + 1; j < check_cnt; j++) {
            if (check_scenectrl[i]->pri > check_scenectrl[j]->pri) {
                SCENECTRL *tmp = check_scenectrl[i];
                check_scenectrl[i] = check_scenectrl[j];
                check_scenectrl[j] = tmp;
            }
        }
    }

    for (lsjkl = 0; lsjkl < check_cnt; lsjkl++) {
        DrawScenectrlReq(check_scenectrl[lsjkl], drawCurrentTime);
    }

    dr_tap_req_num = 0;

    ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));
    UseGsRegSet();
}

INCLUDE_RODATA("asm/nonmatchings/main/drawctrl", D_003933B0);

INCLUDE_RODATA("asm/nonmatchings/main/drawctrl", D_003933C0);

INCLUDE_RODATA("asm/nonmatchings/main/drawctrl", D_003933D0);

INCLUDE_RODATA("asm/nonmatchings/main/drawctrl", D_003933E0);

static void ddbg_event_sub_bmp(void) {
    int        i, j, lsjkl;
    int        check_cnt;
    SCENECTRL *scenectrl_pp;
    SCENESTR  *scenestr_pp;
    u_char    *dat1_pp, *dat2_pp;
    
    if (drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp == NULL) {
        return;
    }

    scenectrl_pp = drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp;
    scenestr_pp = &drawEventrec->scenestr_pp[drawCurrentLine];

    check_cnt = 0;
    drawUseDrDispCheckInit();

    drawCurrentTime = ddbg_bmp_frame * 2;

    for (i = 0; i < PR_ARRAYSIZE(check_scenectrl); i++) {
        check_scenectrl[i] = NULL;
    }

    for (i = 0; i < scenestr_pp->scenectrl_num; i++, scenectrl_pp++) {
        if (scenectrl_pp->start_flame <= drawCurrentTime &&
            scenectrl_pp->end_flame    > drawCurrentTime) {
            check_scenectrl[check_cnt] = scenectrl_pp;
            check_cnt++;
        } else {
            DrawScenectrlReq(scenectrl_pp, drawCurrentTime);
        }
    }

    for (i = 0; i < check_cnt - 1; i++) {
        for (j = i + 1; j < check_cnt; j++) {
            if (check_scenectrl[i]->pri > check_scenectrl[j]->pri) {
                SCENECTRL *tmp = check_scenectrl[i];
                check_scenectrl[i] = check_scenectrl[j];
                check_scenectrl[j] = tmp;
            }
        }
    }

    /* BUG: should use usrMalloc */
    dat1_pp = malloc(0x8C000); // ~573.4 KB
    dat2_pp = malloc(0x8C000);

    sceGsSetHalfOffset(&DBufDc.draw11,  2048, 2048, 0);
    sceGsSetHalfOffset2(&DBufDc.draw12, 2048, 2048, 0);
    sceGsSetHalfOffset(&DBufDc.draw01,  2048, 2048, 0);
    sceGsSetHalfOffset2(&DBufDc.draw02, 2048, 2048, 0);
    FlushCache(WRITEBACK_DCACHE);

    ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));

    for (lsjkl = 0; lsjkl < check_cnt; lsjkl++) {
        DrawScenectrlReq(check_scenectrl[lsjkl], drawCurrentTime);
    }

    VramTmpSave(dat1_pp, 640, 224, outbuf_idx ^ 1);

    PR_SCOPE
    VCLR_PARA vclr_para = {};
        
    DrawVramClear(&vclr_para, 0, 0, DNUM_NON, DNUM_DRAW);
    DrawVramClear(&vclr_para, 0, 0, DNUM_NON, DNUM_VRAM2);
    DrawVramClear(&vclr_para, 0, 0, DNUM_NON, DNUM_ZBUFF);
    PR_SCOPEEND

    sceGsSetHalfOffset(&DBufDc.draw11,  2048, 2048, 1);
    sceGsSetHalfOffset2(&DBufDc.draw12, 2048, 2048, 1);
    sceGsSetHalfOffset(&DBufDc.draw01,  2048, 2048, 1);
    sceGsSetHalfOffset2(&DBufDc.draw02, 2048, 2048, 1);
    FlushCache(WRITEBACK_DCACHE);

    ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));

    for (lsjkl = 0; lsjkl < check_cnt; lsjkl++) {
        DrawScenectrlReq(check_scenectrl[lsjkl], drawCurrentTime);
    }

    ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));

    VramTmpSave(dat2_pp, 640, 224, outbuf_idx ^ 1);

    PR_SCOPE
    char save_name[20];

    sprintf(save_name, "scr%04d.bmp", ddbg_bmp_frame);
    VramTmpSaveOutBMP(save_name, 640, 224, outbuf_idx ^ 1, dat1_pp, dat2_pp);
    PR_SCOPEEND

    usrFree(dat2_pp);
    usrFree(dat1_pp);
    
    dr_tap_req_num = 0;
    ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));
    UseGsRegSet();

    ddbg_bmp_frame++;
}

static void ddbg_tap_check(void) {
    SCENECTRL *scenectrl_pp;
    SCENE_OBJDATA *scene_objdata_pp;
    int tid;
    int req_num;
    u_short paddata;
    u_char *prs_adr;

    req_num = -1;
    prs_adr = NULL;

    if (drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp == NULL) {
        return;
    }

    scenectrl_pp = &drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp[ddbg_scene_num];
    if (scenectrl_pp->prg_pp == DrawSceneObjData) {
        scene_objdata_pp = scenectrl_pp->param_pp;
        if (scene_objdata_pp->tapstr_size != 0) {
            paddata = pad[0].one;
            tid = scene_objdata_pp->tap_id;
            if (paddata & SCE_PADLup) {
                req_num = ddbg_tap_num;
                prs_adr = &pad[0].press[2];
            } else if (paddata & SCE_PADLdown) {
                req_num = ddbg_tap_num;
                prs_adr = &pad[0].press[3];
            } else if (paddata & SCE_PADLright) {
                req_num = ddbg_tap_num;
                prs_adr = &pad[0].press[0];
                ddbg_tap_num++;
            } else if (paddata & SCE_PADLleft) {
                req_num = ddbg_tap_num;
                prs_adr = &pad[0].press[1];
                ddbg_tap_num--; 
            }

            if (ddbg_tap_num >= scene_objdata_pp->objtapstr_size) {
                ddbg_tap_num = 0;
            }
            if (ddbg_tap_num < 0) {
                ddbg_tap_num = scene_objdata_pp->objtapstr_size - 1;
            }
            if (req_num >= 0) {
                DrawTapReqTbl(req_num + (tid << 0x8), PINDEX_NONE, prs_adr);
            }
        }
    }
}

static void ddbg_scene_sub(void) {
    SCENECTRL *scenectrl_pp;

    if (drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp == NULL) {
        return;
    }

    scenectrl_pp = &drawEventrec->scenestr_pp[drawCurrentLine].scenectrl_pp[ddbg_scene_num];

    drawUseDrDispCheckInit();

    if (drawCurrentTime < scenectrl_pp->start_flame) {
        drawCurrentTime = scenectrl_pp->start_flame;
    }
    if (drawCurrentTime >= scenectrl_pp->end_flame) {
        drawCurrentTime = scenectrl_pp->end_flame - 1;
    }

    DrawScenectrlReq(scenectrl_pp, drawCurrentTime);

    dr_tap_req_num = 0;
    ChangeDrawArea(DrawGetDrawEnvP(DNUM_DRAW));
    UseGsRegSet();
}

void ddbg_tctrl_sub(void) {
    u_short paddata = pad[0].shot;
    u_short paddata_one = pad[0].one;

    if (paddata_one & SCE_PADRdown) {
        ddbg_pause_f ^= 1;
    }

    if (paddata_one & SCE_PADRup) {
        drawCurrentTime = 0;
    }

    if (!ddbg_pause_f) {
        drawCurrentTime += 1;
    } else {
        if (paddata & SCE_PADR1) {
            drawCurrentTime++;
        }
        if (paddata_one & SCE_PADR2) {
            drawCurrentTime += 600;
        }

        if (paddata & SCE_PADL1) {
            drawCurrentTime--;
        }
        if (paddata_one & SCE_PADL2) {
            drawCurrentTime -= 600;
        }

        if (paddata_one & SCE_PADRright) {
            drawCurrentTime++;
        }
        if (paddata_one & SCE_PADRleft) {
            drawCurrentTime--;
        }
    }

    if (drawCurrentTime < 0) {
        drawCurrentTime = 0;
    }
}

static void DrawCtrlMainDebug(void *x) {
    u_short paddata;
    int sel_pos;
    int i;
    char msg_buff[32];

    sel_pos = 0;

    while (1) {
        DbgMsgInit();

        while (1) {
            MtcWait(1);

            paddata = pad[0].one;
            if (paddata & SCE_PADLup) {
                sel_pos--;
            }
            if (paddata & SCE_PADLdown) {
                sel_pos++;
            }

            if (sel_pos < 0) {
                sel_pos = 4;
            }
            if (sel_pos > 4u) {
                sel_pos = 0;
            }

            if (paddata & SCE_PADRright) {
                if (draw_dbg_str[sel_pos].prg_pp(1)) {
                    ddbg_pause_f = 0;
                    break;
                }
            }
            if (paddata & SCE_PADRleft) {
                if (draw_dbg_str[sel_pos].prg_pp(-1)) {
                    ddbg_pause_f = 0;
                    break;
                }
            }

            DbgMsgClear();
            DbgMsgSetSize(0x20, 0x10);

            for (i = 0; i < PR_ARRAYSIZEU(draw_dbg_str); i++) {
                if (i == sel_pos) {
                    DbgMsgSetColor(128, 128, 0);
                } else {
                    DbgMsgSetColor(128, 128, 128);
                }

                DbgMsgPrint(draw_dbg_str[i].dbgmsg, 0x6ea, 0x79c + (i*0x14));
                if (draw_dbg_str[i].msg_pp != NULL) {
                    draw_dbg_str[i].msg_pp(msg_buff);
                    DbgMsgPrint(msg_buff, 0x866, 0x79c + (i*0x14));
                }
            }

            DbgMsgFlash();
        }

        drawCurrentTime = 0;
        ddbg_tap_num = 0;
        ddbg_bmp_frame = 0;

        while (1) {
            MtcWait(1);

            if (pad[0].one & SCE_PADselect) {
                break;
            }
            
            ddbg_tctrl_sub();
            if (ddbg_go_event_scene == 1) {
                ddbg_tap_check();
                ddbg_scene_sub();
            } else if (ddbg_go_event_scene == 0) {
                ddbg_event_sub();
            } else {
                ddbg_event_sub_bmp();
            }
        }

        DrawSceneStrReset(&drawEventrec->scenestr_pp[drawCurrentLine]);
    }
}

void DrawCtrlInitDebug(EVENTREC *ev_pp, int ctrlTbl, void *dat_top) {
    int i;

    PrSetFrameRate(60.0f);
    DrawTmpBuffInit();

    for (i = 0; i < ev_pp->scenestr_size; i++) {
        DrawSceneStrInit(&ev_pp->scenestr_pp[i]);
    }

    drawEventrec       = ev_pp;
    drawCurrentLine    = 0;
    drawCurrentTime    = 0;
    drawCurrentTimeOld = -1;

    dr_tap_req_num = 0;

    MtcExec(DrawCtrlMainDebug, MTC_TASK_DRAWCTRL);
}
