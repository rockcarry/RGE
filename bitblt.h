#ifndef __RGE_BITBLT_H__
#define __RGE_BITBLT_H__

/* 包含头文件 */
#include "scanline.h"

/* bitblt mask color
   0xffffffff - default mask color, first pixel color in source bitmap will used.
 */
#define putbmp(dstpb, x, y, srcpb, style, color, alpha, palmap)  bitblt(dstpb, x, y, srcpb, 0, 0, -1, -1, style, color, alpha, palmap)

/* 函数声明 */
void bitblt(BMP *dstpb, int dstx, int dsty,
            BMP *srcpb, int srcx, int srcy, int srcw, int srch,
            int style, int color, int alpha, BYTE *palmap);
            
#endif

