#ifndef _FONT_H_
#define _FONT_H_

/* ����ͷ�ļ� */
#include <stdio.h>
#include "stdefine.h"
#include "draw2d.h"

/* �������Ͷ��� */
typedef struct
{
    BYTE  hzk_width;     /* �����ֵĿ�� */
    BYTE  hzk_height;    /* �����ֵĸ߶� */
    BYTE  asc_width;     /* ����Ӣ���ֵĿ�� */
    BYTE  asc_height;    /* ����Ӣ���ֵĸ߶� */
    char *hzk_file;      /* ���ֿ��ļ��� */
    char *asc_file;      /* Ӣ���ֿ��ļ��� */
    FILE *_asc_fp;       /* Ӣ���ֿ���ļ�ָ�� */
    FILE *_hzk_fp;       /* ���ֿ���ļ�ָ�� */
    BYTE *_font_buf;     /* ���建���� */
    int   _hzk_buf_size; /* ������ģ��С */
    int   _asc_buf_size; /* Ӣ����ģ��С */
} FONT;

/* �ı������Ͷ��� */
typedef struct
{
    int   xpos;       /* �ı����λ�� x ���� */
    int   ypos;       /* �ı����λ�� y ���� */
    int   width;      /* �ı���Ŀ�� */
    int   height;     /* �ı���ĸ߶� */
    int   colspace;   /* �ı����м�� */
    int   rowspace;   /* �ı����м�� */
    char *text;       /* �ı�����ı� */
    int   calrect;    /* �����ı���� */
    int   cursor;     /* �ı��α� */
    int  _cur_xpos;   /* ��ǰ�� x ���� */
    int  _cur_ypos;   /* ��ǰ�� y ���� */
} TEXTBOX;

/* Ԥ�������� */
extern FONT FONT12;
extern FONT FONT16;
extern FONT FONT24;

BOOL loadfont(FONT *pf);
void freefont(FONT *pf);

void settextfont (void *ctxt, FONT *font );
void settextcolor(void *ctxt, DWORD color);

/* TEXTBOX ��غ��� */
void resettextbox(TEXTBOX *ptb);
int  magictextbox(void *ctxt, TEXTBOX *ptb);
int  outtextbox  (void *ctxt, TEXTBOX *ptb);

/* �ı�������� */
int  printtext(void *ctxt, char *txt);
int  outtextxy(void *ctxt, char *txt, int x, int y);

#endif

