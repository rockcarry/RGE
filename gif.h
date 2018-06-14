#ifndef __RGE_GIF_H__
#define __RGE_GIF_H__

/* ����ͷ�ļ� */
#include "bmp.h"
#include "fio.h"

enum {
    GIF_FRAME_TYPE_UNKNOWN, /* unknow */
    GIF_FRAME_TYPE_IMAGE,   /* ͼ��֡ */
    GIF_FRAME_TYPE_CTRL,    /* ����֡ */
    GIF_FRAME_TYPE_TEXT,    /* �ı�֡ */
    GIF_FRAME_TYPE_END,     /* ����֡ */
};

enum {
    GIF_DISPOSAL_NONE,      /* û��ָ��Ҫ���κδ��� */
    GIF_DISPOSAL_NOACT,     /* ������ͼ������ԭ�� */
    GIF_DISPOSAL_BKGND,     /* ��ʾͼ�ε��������Ҫ�ָ��ɱ�����ɫ */
    GIF_DISPOSAL_PRE,       /* �ָ�����ǰ��ʾ��ͼ�� */
};

typedef struct {
    int  disposal;
    int  inputflag;
    int  transflag;
    BYTE transcolor;
    int  delay;
} GIF_CTRL;

typedef struct {
    int  width;
    int  height;
    int  charw;
    int  charh;
    BYTE forecolor;
    BYTE backcolor;
    char text[256];
} GIF_TEXT;

/* decode */
void* gifdecodeinit (void *fp, FIO *fio);
void  gifdecodefree (void *ctxt);
BOOL  gifdecodeframe(void *ctxt, int *type, BMP *pb, int *xpos, int *ypos, GIF_CTRL *ctrl, GIF_TEXT *text);

/* encode */
void* gifencodeinit (void *fp, FIO *fio, int width, int height, BYTE bkcolor, BYTE *pal, int size);
void  gifencodefree (void *ctxt);
BOOL  gifencodeframe(void *ctxt, int type, BMP *pb, int xpos, int ypos, GIF_CTRL *ctrl, GIF_TEXT *text);
void  gifencodedone (void *ctxt);

void  gifgetimageinfo(void *ctxt, int *width, int *height, BYTE *bkcolor);
void  gifgetglobalpal(void *ctxt, BYTE *pal, int *size);
void  gifgetlocalpal (void *ctxt, int  *flag);
void  gifsetlocalpal (void *ctxt, int   flag);

#endif















