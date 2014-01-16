#ifndef _BMPFILE_H_
#define _BMPFILE_H_

/* 包含头文件 */
#include "bmp.h"
#include "fiodrv.h"

/* 函数声明 */
BOOL loadbmp(BMP *pb, char *file, FIODRV *fdrv);
BOOL savebmp(BMP *pb, char *file, FIODRV *fdrv);

#endif















