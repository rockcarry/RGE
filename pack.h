#ifndef _RGE_PACK_H_
#define _RGE_PACK_H_

/* ����ͷ�ļ� */
#include "fiodrv.h"

/* ���Ͷ��� */
enum {
    TDT_START,     /* ��ʼ���� */
    TDT_DONE,      /* ��ɱ��� */
    TDT_FILE,      /* �ļ� */
    TDT_DIR_ENTER, /* ����Ŀ¼ */
    TDT_DIR_EXIT , /* �˳�Ŀ¼ */
};

typedef BOOL (*PFN_TRAVEL_FUNC)(char *name, char *path, int depth, int type, void *param);

/* �������� */
/* ����Ŀ¼ */
BOOL travel(char *path, int depth, PFN_TRAVEL_FUNC func, void *param);

/* ����ͽ�� */
BOOL dirdopack(char *dir, char *file);
BOOL dirunpack(char *dir, char *file);

/* ���ڷ��ʴ���ļ� */
extern FIODRV PACK_FIO_DRV;

#endif
