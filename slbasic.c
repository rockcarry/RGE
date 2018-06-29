/* 包含头文件 */
#include <string.h>
#include "scanline.h"

/* 内部函数实现 */
static void scanline_8bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DO_USE_VAR(src);
    memset(dst, (BYTE)params->fillc, w);
}

static void scanline16bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD *dstword = (WORD*)dst;
    DO_USE_VAR(src);
    while (w--) {
        *dstword++ = (WORD)params->fillc;
    }
}

static void scanline24bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *dstbyte = (BYTE*)dst;
    DO_USE_VAR(src);
    while (w--) {
        *dstbyte++ = (BYTE)(params->fillc >> 0 );
        *dstbyte++ = (BYTE)(params->fillc >> 8 );
        *dstbyte++ = (BYTE)(params->fillc >> 16);
    }
}

static void scanline32bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
    DO_USE_VAR(src);
    while (w--) {
        *dstdword++ = params->fillc;
    }
}

PFNSCANLINE TABFN_SCANLINEBAR_SOLID[] =
{
    scanline_8bitsolid,
    scanline16bitsolid,
    scanline24bitsolid,
    scanline32bitsolid,
};

static void scanline_8bitpattern(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *dstbyte = (BYTE*)dst;
    DWORD patline = params->pattern[(params->fillsrcy + params->filldsty) % 32];
    DO_USE_VAR(src);
    while (w--) {
        if (patline & (1 << ((params->fillsrcx + params->filldstx++) % 32))) {
            *dstbyte = (BYTE)params->fillc;
        }
        dstbyte++;
    }
}

static void scanline16bitpattern(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD *dstword = (WORD*)dst;
    DWORD patline = params->pattern[(params->fillsrcy + params->filldsty) % 32];
    DO_USE_VAR(src);
    while (w--) {
        if (patline & (1 << ((params->fillsrcx + params->filldstx++) % 32))) {
            *dstword = (WORD)params->fillc;
        }
        dstword++;
    }
}

static void scanline24bitpattern(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *dstbyte = (BYTE*)dst;
    DWORD patline = params->pattern[(params->fillsrcy + params->filldsty) % 32];
    DO_USE_VAR(src);
    while (w--) {
        if (patline & (1 << ((params->fillsrcx + params->filldstx++) % 32))) {
            dstbyte[0] = (BYTE)(params->fillc >> 0 );
            dstbyte[1] = (BYTE)(params->fillc >> 8 );
            dstbyte[2] = (BYTE)(params->fillc >> 16);
        }
        dstbyte += 3;
    }
}

static void scanline32bitpattern(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
    DWORD  patline  = params->pattern[(params->fillsrcy + params->filldsty) % 32];
    DO_USE_VAR(src);
    while (w--) {
        if (patline & (1 << ((params->fillsrcx + params->filldstx++) % 32))) {
            *dstdword = params->fillc;
        }
        dstdword++;
    }
}

PFNSCANLINE TABFN_SCANLINEBAR_PATTERN[] =
{
    scanline_8bitpattern,
    scanline16bitpattern,
    scanline24bitpattern,
    scanline32bitpattern,
};

static void scanline_8bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *pbyte = (BYTE*)dst;
    DWORD alpha = (params->alpha >> 5);
    DWORD fc    = (params->fillc | (params->fillc << 9)) & 0x38E3;
    DWORD bc, rc;
    DO_USE_VAR(src);
    while (w--) {
        bc = (*pbyte | (*pbyte << 9)) & 0x38E3;
        rc = (bc + alpha * (fc - bc) / 8) & 0x38E3;
        *pbyte++ = (BYTE)(rc | (rc >> 9));
    }
}

static void scanline16bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD *pword = (WORD*)dst;
    DWORD alpha = (params->alpha >> 3);
    DWORD fc    = (params->fillc | (params->fillc << 16)) & 0x07E0F81FL;
    DWORD bc, rc;
    DO_USE_VAR(src);
    while (w--) {
        bc = (*pword | (*pword << 16)) & 0x07E0F81FL;
        rc = (bc + alpha * (fc - bc) / 32) & 0x07E0F81FL;
        *pword++ = (WORD)(rc | (rc >> 16));
    }
}

static void scanline24bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *pbyte = (BYTE*)dst;
    DWORD alpha =  params->alpha;
    DWORD r     = (params->fillc >> 16) & 0xff;
    DWORD g     = (params->fillc >> 8 ) & 0xff;
    DWORD b     = (params->fillc >> 0 ) & 0xff;
    DO_USE_VAR(src);
    while (w--) {
        pbyte[0]= (BYTE)(pbyte[0] + alpha * (b - pbyte[0]) / 256);
        pbyte[1]= (BYTE)(pbyte[1] + alpha * (g - pbyte[1]) / 256);
        pbyte[2]= (BYTE)(pbyte[2] + alpha * (r - pbyte[2]) / 256);
        pbyte  += 3;
    }
}

static void scanline32bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *pdword = (DWORD*)dst;
    DWORD  alpha  = params->alpha;
    DWORD  fc     = params->fillc & 0xff00ffL;
    DWORD  fg     = params->fillc & 0x00ff00L;
    DWORD  bc, bg, rc, rg;
    DO_USE_VAR(src);
    while (w--) {
        bc = (*pdword & 0xff00ffL);
        bg = (*pdword & 0x00ff00L);
        rc = (bc + alpha * (fc - bc) / 256) & 0xff00ffL;
        rg = (bg + alpha * (fg - bg) / 256) & 0x00ff00L;
        *pdword++ = rc | rg;
    }
}

PFNSCANLINE TABFN_SCANLINEBAR_ALPHA[] =
{
    scanline_8bitalpha,
    scanline16bitalpha,
    scanline24bitalpha,
    scanline32bitalpha,
};

/*++ TABFN_SCANLINEBMP_FAST */
static void scanline__8bitbmp_fast(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *start = params->start;
    BYTE *end   = params->end;
    if ((BYTE*)src + w < end) {
        memcpy(dst, src, w * 1);
    } else {
        BYTE *dstbyte = (BYTE*)dst;
        BYTE *srcbyte = (BYTE*)src;
        while (w--) {
            *dstbyte++ = *srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
    }
}

static void scanline_16bitbmp_fast(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD *start = params->start;
    WORD *end   = params->end;
    if ((WORD*)src + w < end) {
        memcpy(dst, src, w * 2);
    } else {
        WORD *dstword = (WORD*)dst;
        WORD *srcword = (WORD*)src;
        while (w--) {
            *dstword++ = *srcword++;
            if (srcword == end) srcword = start;
        }
    }
}

static void scanline_24bitbmp_fast(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *start = params->start;
    BYTE *end   = params->end;
    if ((BYTE*)src + w * 3 < end) {
        memcpy(dst, src, w * 3);
    } else {
        BYTE *dstbyte = (BYTE*)dst;
        BYTE *srcbyte = (BYTE*)src;
        while (w--) {
            *dstbyte++ = *srcbyte++;
            *dstbyte++ = *srcbyte++;
            *dstbyte++ = *srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
    }
}

static void scanline_32bitbmp_fast(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *start = params->start;
    DWORD *end   = params->end;
    if ((DWORD*)src + w < end) {
        memcpy(dst, src, w * 4);
    } else {
        DWORD *dstdword = (DWORD*)dst;
        DWORD *srcdword = (DWORD*)src;
        while (w--) {
            *dstdword++ = *srcdword++;
            if (srcdword == end) srcdword = start;
        }
    }
}

PFNSCANLINE TABFN_SCANLINEBMP_FAST[] =
{
    scanline__8bitbmp_fast,
    scanline_16bitbmp_fast,
    scanline_24bitbmp_fast,
    scanline_32bitbmp_fast,
};
/*-- TABFN_SCANLINEBMP_FAST */
