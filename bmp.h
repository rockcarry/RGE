#ifndef __RGE_BMP_H__
#define __RGE_BMP_H__

/* 包含头文件 */
#include "stdefine.h"
#include "fio.h"

/* 类型定义 */
/* BMP 对象的类型定义 */
typedef struct {
    int    width;      /* 宽度 */
    int    height;     /* 高度 */
    int    cdepth;     /* 像素位深度 */
    int    stride;     /* 行宽字节数 */
    RECT   clipper;    /* 裁剪器 */
    BYTE  *pdata;      /* 指向图像数据 */
    BYTE  *ppal;       /* 指向色盘数据 */
    void  *pextra;     /* 指向附加数据 */

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

/* 函数声明 */
/* 创建与销毁 */
BOOL createbmp (BMP *pb);
void destroybmp(BMP *pb);

/* lock & unlock */
void lockbmp  (BMP *pb);
void unlockbmp(BMP *pb);

/* 调色板管理 */
void setbmppal(BMP *pb, int i, int n, BYTE *pal);
void getbmppal(BMP *pb, int i, int n, BYTE *pal);

/* 裁剪器 */
void setclipper(BMP *pb, int left, int top, int right, int bottom);

BOOL loadbmp(BMP *pb, char *file, FIO *fio);
BOOL savebmp(BMP *pb, char *file, FIO *fio);

#endif


