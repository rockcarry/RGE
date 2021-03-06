/* 包含头文件 */
#include "scanline.h"

/* 函数实现 */
void select_scanline_func(BMP *dstpb, BMP *srcpb, int style, PFNSCANLINE *pfn)
{
    int cdepthdst = dstpb ? dstpb->cdepth : 8;
    int cdepthsrc = srcpb ? srcpb->cdepth : 8;

    if (style & FS_NONE) {
        *pfn = PFN_SCANLINE_NONE;
    } else if (style & FS_SOLID) {
        *pfn = TABFN_SCANLINEBAR_SOLID[cdepthdst/8-1];
    } else if (style & FS_PATTERN) {
        *pfn = TABFN_SCANLINEBAR_PATTERN[cdepthdst/8-1];
    } else if (style & FS_ALPHA) {
        *pfn = TABFN_SCANLINEBAR_ALPHA[cdepthdst/8-1];
    } else if (cdepthdst == 8 && cdepthdst == cdepthsrc && (style & FS_256_PALMAPDST)) {
        *pfn = PFN_SCANLINE_PALMAP_DST;
    } else if (cdepthdst == 8 && cdepthdst == cdepthsrc && (style & FS_256_PALMAPSRC)) {
        *pfn = PFN_SCANLINE_PALMAP_SRC;
    } else if ((style & (FS_BMP_MASK|FS_BMP_ALPHA)) == (FS_BMP_MASK|FS_BMP_ALPHA)) {
        *pfn = TABFN_SCANLINEBMP_MASKALPHA[cdepthdst/8-1];
    } else if (style & FS_BMP_ALPHA) {
        *pfn = TABFN_SCANLINEBMP_ALPHA[cdepthdst/8-1];
    } else if (style & FS_BMP_MASK) {
        *pfn = TABFN_SCANLINEBMP_MASK[cdepthdst/8-1];
    } else {
        if (cdepthdst != cdepthsrc || (cdepthdst == 8 && (style & FS_256_COPYDATA) == 0)) {
            *pfn = TABFN_SCANLINEBMP_CONVERT[cdepthdst/8-1];
        } else {
            *pfn = TABFN_SCANLINEBMP_FAST[cdepthdst/8-1];
        }
    }
}
