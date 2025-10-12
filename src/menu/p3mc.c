#include "menu/p3mc.h"

#include "main/cdctrl.h"

#include "menu/memc.h"
#include "menu/menufont.h"

#include <libcdvd.h>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* .sdata */
extern char *HedderID;
extern char *FooterID;
extern GETUSER_WORK *pUChkWork;

/* .bss */
extern P3MC_WORK P3MC_Work;

/* .sbss */
extern int FreeSizeFlg;
extern int portCheckFlg;
extern int NeedSize[2];
extern int UChkSize[2];

/* .bss */
extern char filePath[64];
extern MEMC_INFO mcmenu_info;
extern sceMcTblGetDir p3mcTblGetDir[8];

/* .sbss */
extern int isFileFlgCash;

/* .bss */
extern u_char McLogFileFlg[80];
extern u_char McReplayFileFlg[80];

static int      P3MC_GetIconSize(int mode);
static void*    P3MC_GetIconPtr(int mode, int stageNo);
static void    _P3MC_SetUserDirName(int mode, int fileNo);
static char*   _P3MC_GetFilePath(int mode, int fileNo);
static void    _P3MC_EUC2SJIS(char *des, char *src);
static void    _P3MC_ASC2SJIS(char *des, char *src);
static void    _P3MC_UserName_ASC2SJIS(char *des, char *src);
static void    _P3MC_SetBrowsInfo(int mode, int fileNo, char *name, int stageNo, int roundNo, int isVs, int ParaCol);
static int     _P3MC_mainfile_chk(int no, int data_csize, int mode, int *need);
static int     _P3MC_file_chk(char *name, int size, int *need);
static int     _P3MC_freesize_chk(void);
static int     _P3MC_GetSaveDataSize(int dsize);
static int     _P3MCStrCmpLen(char *str, char *id, int len);
static int     _P3MCStrNum(char *nstr, int len);
static int     _P3MC_MemcCheck(int mode, sceMcTblGetDir *pDirTable);
static void    _P3MC_AddUserBroken(P3MC_USRLST *pUser, int mode, int fno);
static int     _P3MC_loadCheck(P3MC_WORK *pw, int skip);
static int     _P3MC_SaveCheck(P3MC_WORK *pw);
static u_short _P3MC_proc(u_short prg);
static void    _P3MC_dataCheckFunc(P3MC_WORK *pw, P3MCDataCheckFunc funcp);
static int     _P3MC_CheckUserData(P3MC_WORK *pw);
static int     _P3MC_CheckUserDataHead(P3MC_WORK *pw);

static int P3MC_GetIconSize(int mode) {
    int isize;

    switch (mode) {
    case 2:
        isize = 0x1e360;
        break;
    case 1:
    default:
        isize = 0x1ccb0;
        break;
    }

    return isize;
}

INCLUDE_RODATA("asm/nonmatchings/menu/p3mc", D_00396180);
INCLUDE_RODATA("asm/nonmatchings/menu/p3mc", D_00396190);

static void* P3MC_GetIconPtr(int mode, int stageNo) {
    int fn;

    if (mode == 1) {
        switch (stageNo) {
        case 1:
            fn = 0x161;
            break;
        case 2:
            fn = 0x162;
            break;
        case 3:
            fn = 0x163;
            break;
        default:
            fn = 0x164;
            break;
        }
    } else {
        switch (stageNo) {
        case 1:
            fn = 0x159;
            break;
        case 2:
            fn = 0x15a;
            break;
        case 3:
            fn = 0x15b;
            break;
        case 4:
            fn = 0x15c;
            break;
        case 5:
            fn = 0x15d;
            break;
        case 6:
            fn = 0x15e;
            break;
        case 7:
            fn = 0x15f;
            break;
        case 8:
        default:
            fn = 0x160;
            break;
        }
    }

    return GetIntAdrsCurrent(fn);
}

static void _P3MC_SetUserDirName(int mode, int fileNo) {
    memc_setDirName(_P3MC_GetFilePath(mode, fileNo));
}

extern char D_00399840[]; /* "LOG???"  */
extern char D_00399848[]; /* "LOG%03d" */
extern char D_00399850[]; /* "REP???"  */
extern char D_00399858[]; /* "REP%03d" */
extern char D_00399860[]; /* "??????"  */

#ifndef NON_MATCHING /* Requires sdata split to match */
INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_GetFilePath);
#else
static char* _P3MC_GetFilePath(int mode, int fileNo)
{
    char *addName;

    strcpy(filePath, "BISCPS-15017"); /* D_003961C0 */
    addName = &filePath[12];

    switch (mode)
    {
    case 1:
        if (fileNo < 0)
            strcpy(addName, "REP???");
        else
            sprintf(addName, "REP%03d", fileNo);
        break;
    case 2:
        if (fileNo < 0)
            strcpy(addName, "LOG???");
        else
            sprintf(addName, "LOG%03d", fileNo);
        break;
    case 3:
    default:
        strcpy(addName, "??????");
        break;
    }

    return filePath;
}
#endif

static void _P3MC_EUC2SJIS(char *des, char *src) {
    u_char c1, c2;

    while ((c1 = *src++) != '\0') {
        c2 = *src++;
        if ((c1 % 2) == 0) {
            c2 -= 0x02;
        } else {
            c2 -= 0x61;
            if (c2 > 0x7e) {
                c2++;
            }
        }

        c1++;
        if (c1 < 0xe0) {
            c1 /= 2;
            c1 += 0x30;
        } else {
            c1 /= 2;
            c1 += 0x70;
        }

        *des++ = c1;
        *des++ = c2;
    }

    *des = '\0';
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_ASC2SJIS);

static void _P3MC_UserName_ASC2SJIS(char *des, char *src) {
    MenuFont_ASC2EUC(des, src);
    _P3MC_EUC2SJIS(des, des);
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_SetBrowsInfo);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_mainfile_chk);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_file_chk);

int P3MC_InitReady(void) {
    int re;

    portCheckFlg = 0;
    FreeSizeFlg = 0;

    memset(&P3MC_Work, 0, sizeof(P3MC_WORK));
    memset(&mcmenu_info, 0, sizeof(MEMC_INFO));
    memset(p3mcTblGetDir, 0, sizeof(p3mcTblGetDir));

    isFileFlgCash = 0;

    memset(McLogFileFlg, 0, sizeof(McLogFileFlg));
    memset(McReplayFileFlg, 0, sizeof(McReplayFileFlg));
    memc_init();

    mcmenu_info.dirfile = p3mcTblGetDir;
    mcmenu_info.dirfileMax = 8;
    memc_port_info(0, &mcmenu_info);

    re = memc_manager(0);
    if (re == 0x10 || re == 0x6 || re == 0x30) {
        P3MC_CheckChangeClear();
        return -1;
    }

    return 0;
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_GetSaveSize);

void P3MC_SetCheckSaveSize(int mode, int fsize, int csize) {
    int asize = _P3MC_GetSaveDataSize(csize);

    switch (mode) {
    case 1:
        NeedSize[0] = fsize;
        UChkSize[0] = asize;
        break;
    case 2:
        NeedSize[1] = fsize;
        UChkSize[1] = asize;
    }
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_freesize_chk);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_CheckChange);

void P3MC_CheckChangeClear(void) {
    memc_setChangeState(0);
}

void P3MC_CheckChangeSet(void) {
    memc_setChangeState(1);
}

int P3MC_CheckIsNewSave(int mode) {
    return FreeSizeFlg & mode;
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_GetSaveDataSize);

void P3MC_DeleteDataWork(MCRWDATA_HDL *phdl) {
    if (phdl == NULL) {
        return;
    }
    
    if (phdl->pMemTop != NULL) {
        free(phdl->pMemTop);
    }
    
    free(phdl);
}

#define USER_HDR(x) ((USER_HEADER*)x)

#if 1
INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_MakeDataWork);
#else
MCRWDATA_HDL* P3MC_MakeDataWork(/* s4 20 */ int dsize, /* s5 21 */ USER_DATA *puser)
{
    /* s3 19 */ MCRWDATA_HDL *phdl;
    /* s2 18 */ u_char *pdata;
    /* s1 17 */ int asize;
    /* s0 16 */ int dsize0;

    phdl = malloc(sizeof(MCRWDATA_HDL));

    if (phdl != NULL)
    {
        dsize0 = ((dsize + 15) >> 4) * 16;

        pdata = memalign(16, dsize0 + 0x2694);
        memset(pdata, 0, dsize0 + 0x2694);

        phdl->pMemTop  = pdata; // 1045
        phdl->rwsize   = dsize0 + sizeof(USER_HEADER); // 1046
        phdl->datasize = dsize0 + sizeof(USER_HEADER); // 1047
        phdl->srcsize  = dsize; // 1048

        phdl->pHead = (USER_HEADER*)pdata; // 1050
        phdl->pData = pdata + sizeof(USER_HEADER); // 1051
        phdl->pFoot = USER_HDR(pdata + sizeof(USER_HEADER))->footer[dsize0]; // 1052

        if (puser != NULL)
        {
            USER_HDR(pdata)->user = *puser;
        }

        return phdl;
    }
    else
    {
        P3MC_DeleteDataWork(NULL);
        return NULL;
    }
}
#endif

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MCStrCmpLen);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MCStrNum);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_MemcCheck);

int P3MC_GetUserStart(int mode, P3MC_USRLST *pUsrLst, int bFirst) {
    GETUSER_WORK *pWork;

    if (pUChkWork != NULL) {
        free(pUChkWork);
    }
    
    pWork = memalign(16, sizeof(*pWork));
    pUChkWork = pWork;

    portCheckFlg = 0;

    if (pWork != NULL) {
        pWork->bFirst = bFirst;
        pWork->curState = 0;

        if (mode & 1) {
            pWork->curMode = 1;
        } else if (mode & 2) {
            pWork->curMode = 2;
        } else {
            printf("P3MC_GetUser Error Mode is unknown!\n");
            pWork->curMode = 1;
        }

        pWork->curFno = 0;
        pWork->curUserMode = mode;
        pWork->pUserLst = pUsrLst;
    }

    return -1;
}

void P3MC_GetUserEnd(void) {
    if (pUChkWork != NULL) {
        free(pUChkWork);
    }

    pUChkWork = NULL;
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_GetUserCheck);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_AddUser);

INCLUDE_RODATA("asm/nonmatchings/menu/p3mc", D_00396420);

INCLUDE_RODATA("asm/nonmatchings/menu/p3mc", D_00396448);

INCLUDE_RODATA("asm/nonmatchings/menu/p3mc", D_00396470);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_AddUserBroken);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_SortUser);

int P3MC_CheckBrokenUser(P3MC_USRLST *pUser, int mode) {
    int         nmuser;
    USER_DATA **pmuser;

    int i;
    int nBrk = 0;

    if (mode & 1) {
        pmuser = pUser->plog_user;
        nmuser = pUser->nLogGet;

        for (i = 0; i < nmuser; i++, pmuser++) {
            if ((*pmuser)->flg == 2) {
                nBrk++;
            }
        }
    }

    if (mode & 2) {
        pmuser = pUser->prep_user;
        nmuser = pUser->nRepGet;

        for (i = 0; i < nmuser; i++, pmuser++) {
            if ((*pmuser)->flg == 2) {
                nBrk++;
            }
        }
    }

    return nBrk;
}

void P3MC_OpeningCheckStart(void) {
    GETUSER_WORK *pWork;

    if (pUChkWork != NULL) {
        free(pUChkWork);
    }

    pWork = memalign(16, sizeof(*pWork));
    pUChkWork = pWork;
    
    P3MC_CheckChangeSet();
    portCheckFlg = 0;

    if (pWork != NULL) {
        pWork->curState = 0;
    }
}

void P3MC_OpeningCheckEnd(void) {
    if (pUChkWork != NULL) {
        free(pUChkWork);
    }

    pUChkWork = NULL;
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_OpeningCheck);

#if 1
INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_LoadUser);
#else
int P3MC_LoadUser(/* s5 21 */ int mode, /* s3 19 */ int fileNo, /* s1 17 */ MCRWDATA_HDL *pdhdl, /* s4 20 */ int flg)
{
    P3MC_WORK *pw = &P3MC_Work;

    memset(pdhdl->pMemTop, 0, pdhdl->rwsize);

    _P3MC_SetUserDirName(mode, fileNo);
    _P3MC_dataCheckFunc(pw, _P3MC_CheckUserData);

    pw->prgflag    = flg; // 2114

    pw->dstat      = 0; // 2115
    pw->dhdl       = pdhdl; // 2121
    
    pw->data_mode  = mode; // 2117
    pw->data_no    = fileNo; // 2118
    pw->data_stage = 0; // 2119

    pw->prg = 0x1000; // 2122
    return 0;
}
#endif

int P3MC_LoadCheck(void) {
    int        re;
    P3MC_WORK *pw = &P3MC_Work;

    re = _P3MC_loadCheck(pw, 0);
    if (re < 0) {
        if (pw->dstat == 0) {
            return -1;
        } else {
            return -2;
        }
    }

    if (re != 0) {
        if (re == 0xb) {
            re = 4;
        }

        memset(pw->dhdl->pMemTop, 0, pw->dhdl->rwsize);
    }

    return re;
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_loadCheck);

void P3MC_SetUserWorkTime(USER_DATA *puser) {
    int        err;
    sceCdCLOCK clock;

    err = sceCdReadClock(&clock);
    puser->date_pad = rand() % 200;

    if (err != 0 && clock.stat == 0) {
        puser->date_second = clock.second;
        puser->date_minute = clock.minute;
        puser->date_hour   = clock.hour;

        puser->date_day    = clock.day;
        puser->date_month  = clock.month;
        puser->date_year   = clock.year + 0x2000;
    } else {
        puser->date_second = 0;
        puser->date_minute = 0;
        puser->date_hour   = 12;

        puser->date_day    = 1;
        puser->date_month  = 1;
        puser->date_year   = 0x2000;
    }
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", P3MC_SaveUser);

int P3MC_SaveCheck(void) {
    int        re;
    P3MC_WORK *pw = &P3MC_Work;

    re = _P3MC_SaveCheck(pw);
    if (re >= 0) {
        return re;
    }

    if (pw->dstat == 1) {
        return -2;
    }
    if (pw->dstat != 2) {
        return -1;
    }
    return -3;
}

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_SaveCheck);

INCLUDE_ASM("asm/nonmatchings/menu/p3mc", _P3MC_proc);

void _P3MC_dataCheckFunc(P3MC_WORK *pw, P3MCDataCheckFunc funcp) {
    pw->data_cfunc = funcp;
}

static int _P3MC_CheckUserData(P3MC_WORK *pw) {
    USER_FOOTER *pfoot = pw->dhdl->pFoot;

    if (_P3MC_CheckUserDataHead(pw)) {
        return 1;
    } else {
        return (strcmp(FooterID, pfoot->footer) != 0);
    }
}

/* There are no traces of such use of inlines on the
 * symbols, yet the function only matches this way. */
static inline int _P3MC_CheckHead(USER_HEADER *hed, P3MC_WORK *pw) {
    /* Checks done on a single line according to symbols */
    if (hed->user.fileNo != pw->data_no ||
        hed->user.mode != pw->data_mode || 
        hed->user.mode != pw->data_mode ||
        (pw->data_stage > 0 && hed->user.stageNo != pw->data_stage) ||
        (hed->user.name[0] == 0 && hed->user.name1[0] == 0)) {
        return 1;
    }
    return 0;
}

/* Without use of inlines you can have a decent
 * match with a minor difference on a likely branch:
 * https://decomp.me/scratch/QD9p8 */
static int _P3MC_CheckUserDataHead(P3MC_WORK *pw) {
    USER_HEADER *hed = (USER_HEADER*)pw->dhdl->pMemTop;

    if (strcmp(hed->header, HedderID) != 0 || strcmp(hed->footer, FooterID) != 0) {
        return 1;
    }

    if (hed->user.flg == 0) {
        return 0;
    }

    return _P3MC_CheckHead(hed, pw);
}
