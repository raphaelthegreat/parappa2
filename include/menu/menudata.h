#ifndef MENUDATA_H
#define MENUDATA_H

#include "common.h"

#include "main/etc.h"
#include "main/scrctrl.h"

#include <eetypes.h>
#include <libmc.h>

typedef struct { // 0xc0
    /* 0x00 */ unsigned char Head[4];
    /* 0x04 */ unsigned short Reserv1;
    /* 0x06 */ unsigned short OffsLF;
    /* 0x08 */ unsigned int Reserv2;
    /* 0x0c */ unsigned int TransRate;
    /* 0x10 */ _iconVu0IVECTOR BgColor[4];
    /* 0x50 */ _iconVu0FVECTOR LightDir[3];
    /* 0x80 */ _iconVu0FVECTOR LightColor[3];
    /* 0xb0 */ _iconVu0FVECTOR Ambient;
} MenuMcIconSys;

typedef struct { // 0xc4
    /* 0x00 */ int fileId;
    /* 0x04 */ MenuMcIconSys iconSys;
} MenuMcIcon;

typedef enum {
    MENU_SPU_CHAN,
    MENU_SPU_CHAN1,
    MENU_SPU_CHAN2,
    MENU_SPU_MAX,
} MENU_SPU_ENUM;

typedef enum {
    MDISK_00,
    MDISK_01,
    MDISK_02,
    MDISK_03,
    MDISK_04,
    MDISK_05,
    MDISK_06,
    MDISK_07,
    MDISK_08,
    MDISK_09,
    MDISK_MAX,
} MENU_DISKSND_ENUM;

typedef struct { // 0x8
    /* 0x0 */ u_short bnkNo;
    /* 0x4 */ SNDTAP *pTap;
} VOICE_TBL;

typedef struct { // 0x40
    /* 0x00 */ int fno[4][4];
} MN_CRWCL1_TBL;

void* MenuDataGetIconSysHed(int mode, int stageNo, int *size);

void MenuDataSndInit(void);
void MenuDataSndReq(int chanId, int req);
void MenuDataSndStop(int chanId);
void MenuDataSndQuit(void);
void MenuDataSndSetVol(int chanId, int req, int vol0);
MENU_SPU_ENUM MenuDataSndTrans(int bdId, int hdId, MENU_SPU_ENUM trId);
int MenuDataSndTransCheck(void);
void MenuDataSndReqChan(int chanId, int req, MENU_SPU_ENUM trId);

void MenuDataSpuVolume(int vol);
void MenuDataDiskVolume(u_int vol);

void MenuDataDiskSndReq(MENU_DISKSND_ENUM sndId);
int MenuDataDiskSndReady(void);
void MenuDataDiskSndPlay(void);
void MenuDataDiskSndEnd(void);

void menuDiskSndDebug(void *x);

int MenuVoiceBankSet(int bnkNo);
void MenuVoicePlayVol(int chanId, int vsetIdx, int vol0);
void MenuVoicePlay(int chanId, int vsetIdx);
void MenuVoiceStop(int chanId);
void MenuVoiceSetVol(int chanId, int vsetIdx, int vol0);

void MenuMsgInit(void);
void MenuMsgPrintSub(int id, int xp, int yp, int flg);
void MenuMsgPrintMc(int id, int xp, int yp, int flg);
char* MenuMsgGetMessageMc(int id, int flg);
char* MenuMsgGetMessageSub(int id, int flg);

int MenuRoundTim2Trans(TAP_ROUND_ENUM round);
int MenuStageCl1Trans(int nStage, int nRound);
int MenuCoolCl1Trans(int nStage, int nPos, int nRound);

#endif /* MENUDATA_H */
