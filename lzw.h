#ifndef _LZW_H_
#define _LZW_H_

/* ����ͷ�ļ� */
#include "fiodrv.h"

/* ���Ͷ��� */
/* LZW ����������Ͷ��� */
typedef struct
{
    /* �������ȿ��ƺ��� */
    void (*ppc)(long total, long cur);

    /* ��������������� */
    /* for user */
    int LZW_ROOT_SIZE;
    int LZW_CODE_SIZE_OUT;
    int LZW_CODE_SIZE_MIN;
    int LZW_CODE_SIZE_MAX;

    /* �ڲ�ʹ�õı��� */
    int LZW_ROOT_NUM;
    int LZW_CLEAR_CODE;
    int LZW_END_CODE;
    int LZW_TAB_SIZE;

    /* �ڲ�ʹ�õı��� */
    void  *_lzw_str_tab;
    DWORD *_lzw_str_buf;
    int    _str_tab_pos;
} LZWCODEC, *PLZWCODEC;

/* �������� */
BOOL initlzwcodec (LZWCODEC *plc);
void resetlzwcodec(LZWCODEC *plc);
void closelzwcodec(LZWCODEC *plc);

BOOL lzwencode(LZWCODEC *plc, void *fpout, FIODRV *drvout, void *fpin, FIODRV *drvin);
BOOL lzwdecode(LZWCODEC *plc, void *fpout, FIODRV *drvout, void *fpin, FIODRV *drvin);

#endif


