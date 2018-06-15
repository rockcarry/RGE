/* 包含头文件 */
#include "scanline.h"

/* 内部函数实现 */
/* ++ 256-colors palette map functions -- */
static void scanlinepalmapdst(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *palmap  = params->palmap;
    BYTE *dstbyte = (BYTE*)dst;
    while (w--) {
        *dstbyte = palmap[*dstbyte];
         dstbyte++;
    }
}

static void scanlinepalmapsrc(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *palmap  = params->palmap;
    BYTE *dstbyte = (BYTE*)dst;
    BYTE *srcbyte = (BYTE*)src;
    void *start   = params->start;
    void *end     = params->end;
    while (w--) {
        *dstbyte++ = palmap[*srcbyte++];
        if (srcbyte == end) srcbyte = start;
    }
}

static void scanlinenone(void *dst, void *src, int w, SCANLINEPARAMS *params) {}

PFNSCANLINE PFN_SCANLINE_PALMAP_DST = scanlinepalmapdst;
PFNSCANLINE PFN_SCANLINE_PALMAP_SRC = scanlinepalmapsrc;
PFNSCANLINE PFN_SCANLINE_NONE       = scanlinenone;
/* -- 256-colors palette map functions ++ */
