#ifndef __RGE_PIXEL_H__
#define __RGE_PIXEL_H__

// ����ͷ�ļ�
#include "bmp.h"

// ���Ͷ���
typedef void (*PFNPIXEL)(BMP *pb, int x, int y, DWORD c);  // ���ص㺯��ָ������

// ȫ����������
extern PFNPIXEL TABFN_PIXEL_SOLID[];
extern PFNPIXEL TABFN_PIXEL_ALPHA[];

#endif

