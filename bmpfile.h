#ifndef _BMPFILE_H_
#define _BMPFILE_H_

/* ����ͷ�ļ� */
#include "bmp.h"
#include "fiodrv.h"

/* �������� */
BOOL loadbmp(BMP *pb, char *file, FIODRV *fdrv);
BOOL savebmp(BMP *pb, char *file, FIODRV *fdrv);

#endif















