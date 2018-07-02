#ifndef __RGE_BMP_H__
#define __RGE_BMP_H__

/* ����ͷ�ļ� */
#include "stdefine.h"
#include "fio.h"

/* ���Ͷ��� */
/* BMP ��������Ͷ��� */
typedef struct {
    int    width;      /* ��� */
    int    height;     /* �߶� */
    int    cdepth;     /* ����λ��� */
    int    stride;     /* �п��ֽ��� */
    RECT   clipper;    /* �ü��� */
    BYTE  *pdata;      /* ָ��ͼ������ */
    BYTE  *ppal;       /* ָ��ɫ������ */
    void  *pextra;     /* ָ�򸽼����� */

    BOOL (*createbmp )(void *pb);
    void (*destroybmp)(void *pb);
    void (*lock  )(void *pb);
    void (*unlock)(void *pb);
    void (*setpal)(void *pb, int i, int n, BYTE *pal);
    void (*getpal)(void *pb, int i, int n, BYTE *pal);
} BMP;

#define RGB332(r, g, b)  ( ((DWORD)((r) & 0xE0) <<  0) | ((DWORD)((g) & 0xE0) >> 3) | ((DWORD)((b) & 0xC0) >> 6) )
#define RGB565(r, g, b)  ( ((DWORD)((r) & 0xF8) <<  8) | ((DWORD)((g) & 0xFC) << 3) | ((DWORD)((b) & 0xF8) >> 3) )
#define RGB888(r, g, b)  ( ((DWORD)((r) & 0xFF) << 16) | ((DWORD)((g) & 0xFF) << 8) | ((DWORD)((b) & 0xFF) >> 0) )
#define ARGB(a, r, g, b) ( ((DWORD)((a) & 0xFF) << 24) | ((DWORD)((r) & 0xFF) << 16) | ((DWORD)((g) & 0xFF) << 8) | ((DWORD)((b) & 0xFF) >> 0) )
DWORD COLOR_CONVERT(int cdepth, DWORD color, BOOL flag);

/* �������� */
/* ���������� */
BOOL createbmp (BMP *pb);
void destroybmp(BMP *pb);

/* lock & unlock */
void lockbmp  (BMP *pb);
void unlockbmp(BMP *pb);

/* ��ɫ����� */
void setbmppal(BMP *pb, int i, int n, BYTE *pal);
void getbmppal(BMP *pb, int i, int n, BYTE *pal);

/* �ü��� */
void setclipper(BMP *pb, int left, int top, int right, int bottom);

BOOL loadbmp(BMP *pb, char *file, FIO *fio);
BOOL savebmp(BMP *pb, char *file, FIO *fio);

#endif


