#ifndef __RGE_PIXEL_H__
#define __RGE_PIXEL_H__

// 包含头文件
#include "bmp.h"

// 类型定义
typedef void (*PFNPIXEL)(BMP *pb, int x, int y, DWORD c);  // 像素点函数指针类型

// 全部变量声明
extern PFNPIXEL TABFN_PIXEL_SOLID[];
extern PFNPIXEL TABFN_PIXEL_ALPHA[];

#endif

