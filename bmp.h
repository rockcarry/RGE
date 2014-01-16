#ifndef _BMP_H_
#define _BMP_H_

/* 包含头文件 */
#include "stdefine.h"

/* 类型定义 */
/* BMPDRV 类型定义 */
typedef struct
{
    BOOL (*createbmp )(void *pb);
    void (*destroybmp)(void *pb);
    void (*lock  )(void *pb);
    void (*unlock)(void *pb);
    void (*setpal)(void *pb, int i, int n, BYTE *pal);
    void (*getpal)(void *pb, int i, int n, BYTE *pal);
} BMPDRV;

/* BMP 对象的类型定义 */
typedef struct
{
    BMPDRV *pbmpdrv;    /* BMPDRV 驱动指针 */
    int     width;      /* 宽度 */
    int     height;     /* 高度 */
    int     cdepth;     /* 像素位深度 */
    int     stride;     /* 行宽字节数 */
    RECT    clipper;    /* 裁剪器 */
    BYTE   *pdata;      /* 指向图像数据 */
    BYTE   *ppal;       /* 指向色盘数据 */
    void   *pextra;     /* 指向附加数据 */
} BMP;

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

#endif


