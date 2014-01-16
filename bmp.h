#ifndef _BMP_H_
#define _BMP_H_

/* ����ͷ�ļ� */
#include "stdefine.h"

/* ���Ͷ��� */
/* BMPDRV ���Ͷ��� */
typedef struct
{
    BOOL (*createbmp )(void *pb);
    void (*destroybmp)(void *pb);
    void (*lock  )(void *pb);
    void (*unlock)(void *pb);
    void (*setpal)(void *pb, int i, int n, BYTE *pal);
    void (*getpal)(void *pb, int i, int n, BYTE *pal);
} BMPDRV;

/* BMP ��������Ͷ��� */
typedef struct
{
    BMPDRV *pbmpdrv;    /* BMPDRV ����ָ�� */
    int     width;      /* ��� */
    int     height;     /* �߶� */
    int     cdepth;     /* ����λ��� */
    int     stride;     /* �п��ֽ��� */
    RECT    clipper;    /* �ü��� */
    BYTE   *pdata;      /* ָ��ͼ������ */
    BYTE   *ppal;       /* ָ��ɫ������ */
    void   *pextra;     /* ָ�򸽼����� */
} BMP;

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

#endif


