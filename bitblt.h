#ifndef _BITBLT_H_
#define _BITBLT_H_

/* 包含头文件 */
#include "bmp.h"

/* bitblt mask color
   0xffffffff - default mask color, first pixel color in source bitmap will used.
 */

/* 常量定义 */
#define bitbltmask(dstpb, dstx, dsty, srcpb, srcx, srcy, srcw, srch, maskc) \
        bitbltmaskalpha(dstpb, dstx, dsty, srcpb, srcx, srcy, srcw, srch, maskc, 256)

#define putbmp(dstpb, x, y, srcpb)               bitbltcopy(dstpb, x, y, srcpb, 0, 0, -1, -1)
#define putbmpcopy(dstpb, x, y, srcpb)           bitbltcopy(dstpb, x, y, srcpb, 0, 0, -1, -1)
#define putbmpalpha(dstpb, x, y, srcpb, alpha)   bitbltalpha(dstpb, x, y, srcpb, 0, 0, -1, -1, alpha)
#define putbmpmask(dstpb, x, y, srcpb, maskc)    bitbltmask(dstpb, x, y, srcpb, 0, 0, -1, -1, maskc)
#define putbmppalmap(dstpb, x, y, srcpb, palmap) bitbltpalmap(dstpb, x, y, srcpb, 0, 0, -1, -1, palmap)
#define putbmpstretch(dstpb, x, y, w, h, srcpb)  bitbltstretch(dstpb, x, y, w, h, srcpb, 0, 0, -1, -1)

/* 函数声明 */
void bitbltcopy     (BMP *dstpb, int dstx, int dsty,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch);
void bitbltalpha    (BMP *dstpb, int dstx, int dsty,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch, int alpha);
void bitbltmaskalpha(BMP *dstpb, int dstx, int dsty,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch, DWORD maskc, int alpha);
void bitbltpalmap   (BMP *dstpb, int dstx, int dsty,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch, BYTE *palmap);
void bitbltstretch  (BMP *dstpb, int dstx, int dsty, int dstw, int dsth,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch);

void setblttype(void *ctxt, int type);
void bitblt    (void *ctxt, int dstx, int dsty, int dstw, int dsth,
                BMP  *srcpb,   int srcx, int srcy, int srcw, int srch);

#endif

