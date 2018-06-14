#ifndef __RGE_PCX_H__
#define __RGE_PCX_H__

/* 包含头文件 */
#include "bmp.h"
#include "fio.h"

/* note: now only surpport 256 color pcx images */

/* 类型定义 */
/* PCX 对象定义 */
typedef struct {
    WORD  width;     /* pcx 的宽度 */
    WORD  height;    /* pcx 的高度 */
    DWORD datasize;  /* pcx 的数据长度 */
    BYTE  encoding;  /* pcx 的编码方式 */
    BYTE *pdata;     /* 指向数据 */
    BYTE *ppal;      /* 指向调色板 */
} PCX;

/* 函数声明 */
BOOL createpcx (PCX *pcx);
void destroypcx(PCX *pcx);
BOOL loadpcx(PCX *pcx, char *file, FIO *fio);
BOOL savepcx(PCX *pcx, char *file, FIO *fio);
BOOL encodepcx(PCX *pcx, BMP *pb);
BOOL decodepcx(PCX *pcx, BMP *pb);

#endif





















