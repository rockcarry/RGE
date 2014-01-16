#ifndef _PALUTIL_H_
#define _PALUTIL_H_

#include "stdefine.h"

/* 函数声明 */
/* 创建用于调色板转换的颜色映射表
   参数说明：
   srcpal - 指向源调色板，srcpal 的 [i * 4 + 3] 元素用于保存映射表 map
   dstpal - 指向目的调色板
   映射关系：
   目的调色板上的颜色 c 经过 map[c] 映射后即可得到该颜色在源调色板上的值
 */
void palcolormap(BYTE *map, BYTE *srcpal, BYTE *dstpal);

/* 创建用于快速透明混合的颜色映射表
   参数说明：
   pal   - 指向调色板的指针，如果为 NULL 则使用 STDPAL 调色板
   color - 用于 alpha 混合的前景色，32bit ARGB 格式
 */
void alphacolormap(BYTE *map, BYTE *pal, DWORD color);

#endif















