#ifndef __RGE_GIF_H__
#define __RGE_GIF_H__

/* 包含头文件 */
#include "bmp.h"
#include "fio.h"

enum {
    GIF_FRAME_TYPE_UNKNOWN, /* unknow */
    GIF_FRAME_TYPE_IMAGE,   /* 图像帧 */
    GIF_FRAME_TYPE_CTRL,    /* 控制帧 */
    GIF_FRAME_TYPE_TEXT,    /* 文本帧 */
    GIF_FRAME_TYPE_END,     /* 结束帧 */
};

enum {
    GIF_DISPOSAL_NONE,      /* 没有指定要做任何处理 */
    GIF_DISPOSAL_NOACT,     /* 不处理，图形留在原处 */
    GIF_DISPOSAL_BKGND,     /* 显示图形的区域必须要恢复成背景颜色 */
    GIF_DISPOSAL_PRE,       /* 恢复成以前显示的图形 */
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















