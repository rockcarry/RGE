#ifndef _PALETTE_H_
#define _PALETTE_H_

/* 包含头文件 */
#include "stdefine.h"

/* 函数声明 */
/* 创建 256 色标准调色板 */
void createstdpal(BYTE *pal);

/* 调色板魔术 */
void magicpal(BYTE *pal1, BYTE *pal2);

/* 左移右移调色板 */
void rightrotpal(BYTE *pal);
void leftrotpal (BYTE *pal);

/* 随机化和灰度化调色板 */
void randpal(BYTE *pal);
void graypal(BYTE *pal);

/* swap rgb order in palette */
enum
{
    SWAP_PAL_RGB,
    SWAP_PAL_RBG,
    SWAP_PAL_GRB,
    SWAP_PAL_GBR,
    SWAP_PAL_BRG,
    SWAP_PAL_BGR,
};
void swappal(BYTE *pal, int order);

/* 查找调色板中最匹配的颜色 */
BYTE PALRGB(BYTE *pal, BYTE r, BYTE g, BYTE b);

#endif







