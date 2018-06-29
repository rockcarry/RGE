/* 包含头文件 */
#include "scanline.h"

/* 内部函数实现 */
/* ++ TABFN_SCANLINEBMP_CONVERT */
static void scanline__8bitbmp_convert(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE*)dst;
    BYTE  *srcbyte = (BYTE*)src;
    WORD  *srcword = (WORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color      = *srcbyte++;
            *dstbyte++ = RGB332(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color      = *srcword++;
            *dstbyte++ = (BYTE)(((color & 0x0018) >> 3) | ((color & 0x0700) >> 6) | ((color & 0xE000) >> 8));
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            *dstbyte++ = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
            srcbyte   += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            *dstbyte++ = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
            srcbyte   += 4;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

static void scanline_16bitbmp_convert(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD  *dstword = (WORD*)dst;
    BYTE  *srcbyte = (BYTE*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color      = *srcbyte++;
            *dstword++ = RGB565(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 24:
        while (w--) {
            *dstword++ = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
            srcbyte   += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            *dstword++ = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
            srcbyte   += 4;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

static void scanline_24bitbmp_convert(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE*)dst;
    BYTE  *srcbyte = (BYTE*)src;
    WORD  *srcword = (WORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color      = *srcbyte++;
            *dstbyte++ = pal[(size_t)color * 4 + 0];
            *dstbyte++ = pal[(size_t)color * 4 + 1];
            *dstbyte++ = pal[(size_t)color * 4 + 2];
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
             color     = *srcword++;
            *dstbyte++ = (BYTE)(((color & 0x001f) << 3) | ((color & 0x001c) >> 2));
            *dstbyte++ = (BYTE)(((color & 0x07e0) >> 3) | ((color & 0x0600) >> 9));
            *dstbyte++ = (BYTE)(((color & 0xf800) >> 8) | ((color & 0xe000) >>13));
            if (srcword == end) srcword = start;
        }
        break;
    case 32:
        while (w--) {
            *dstbyte++ = *srcbyte++;
            *dstbyte++ = *srcbyte++;
            *dstbyte++ = *srcbyte++;
            srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

static void scanline_32bitbmp_convert(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
    BYTE  *srcbyte  = (BYTE *)src;
    WORD  *srcword  = (WORD *)src;
    void  *start    = params->start;
    void  *end      = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color     = *srcbyte++;
            *dstdword++= RGB888(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color      = *srcword++;
            *dstdword++= ((color & 0x001f) << 3) | ((color & 0x001c) >> 2)
                       | ((color & 0x07e0) << 5) | ((color & 0x0600) >> 1)
                       | ((color & 0xf800) << 8) | ((color & 0xe000) << 3);
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            *dstdword++ = RGB888(srcbyte[2], srcbyte[1], srcbyte[0]);
             srcbyte   += 3;
             if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

PFNSCANLINE TABFN_SCANLINEBMP_CONVERT[] =
{
    scanline__8bitbmp_convert,
    scanline_16bitbmp_convert,
    scanline_24bitbmp_convert,
    scanline_32bitbmp_convert,
};
/* -- TABFN_SCANLINEBMP_CONVERT */

/* ++ TABFN_SCANLINEBMP_MASK */
static void scanline__8bitbmp_mask(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    DWORD *srcdword= (DWORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                if (params->style & FS_256_COPYDATA) {
                    *dstbyte = (BYTE)color;
                } else {
                    *dstbyte = RGB332(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
                }
            }
            dstbyte++;
            srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            if (color != params->maskc) {
                *dstbyte = (BYTE)(((color & 0x0018) >> 3) | ((color & 0x0700) >> 6) | ((color & 0xE000) >> 8));
            }
            dstbyte++;
            srcword++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB888(srcbyte[2], srcbyte[1], srcbyte[0]);
            if (color != params->maskc) {
                *dstbyte = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
            }
            dstbyte += 1;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = *srcdword;
            if (color != params->maskc) {
                *dstbyte = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
            }
            dstbyte  += 1;
            srcbyte  += 4;
            srcdword += 1;
            if (srcbyte == end) { srcbyte = start; srcdword = start; }
        }
        break;
    }
}

static void scanline_16bitbmp_mask(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD  *dstword = (WORD *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    DWORD *srcdword= (DWORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                *dstword = RGB565(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
            }
            dstword++;
            srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            if (color != params->maskc) {
                *dstword = (WORD)color;
            }
            dstword++;
            srcword++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB888(srcbyte[2], srcbyte[1], srcbyte[0]);
            if (color != params->maskc) {
                *dstword = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
            }
            dstword += 1;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = *srcdword;
            if (color != params->maskc) {
                *dstword = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
            }
            dstword  += 1;
            srcbyte  += 4;
            srcdword += 1;
            if (srcbyte == end) { srcbyte = start; srcdword = start; }
        }
        break;
    }
}

static void scanline_24bitbmp_mask(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    DWORD *srcdword= (DWORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                dstbyte[0] = pal[(size_t)color * 4 + 0];
                dstbyte[1] = pal[(size_t)color * 4 + 1];
                dstbyte[2] = pal[(size_t)color * 4 + 2];
            }
            dstbyte += 3;
            srcbyte += 1;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            if (color != params->maskc) {
                dstbyte[0] = (BYTE)(((color & 0x001f) << 3) | ((color & 0x001c) >> 2));
                dstbyte[1] = (BYTE)(((color & 0x07e0) >> 3) | ((color & 0x0600) >> 9));
                dstbyte[2] = (BYTE)(((color & 0xf800) >> 8) | ((color & 0xe000) >>13));
            }
            dstbyte += 3;
            srcword += 1;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB888(srcbyte[2], srcbyte[1], srcbyte[0]);
            if (color != params->maskc) {
                dstbyte[0] = srcbyte[0];
                dstbyte[1] = srcbyte[1];
                dstbyte[2] = srcbyte[2];
            }
            dstbyte += 3;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = *srcdword;
            if (color != params->maskc) {
                dstbyte[0] = srcbyte[0];
                dstbyte[1] = srcbyte[1];
                dstbyte[2] = srcbyte[2];
            }
            dstbyte += 3;
            srcbyte += 4;
            srcdword+= 1;
            if (srcbyte == end) { srcbyte = start; srcdword = start; }
        }
        break;
    }
}

static void scanline_32bitbmp_mask(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
    BYTE  *srcbyte  = (BYTE *)src;
    WORD  *srcword  = (WORD *)src;
    DWORD *srcdword = (DWORD*)src;
    void  *start    = params->start;
    void  *end      = params->end;
    DWORD  color;
    BYTE  *pal;

    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                *dstdword = RGB888(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
            }
            dstdword++;
            srcbyte ++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            if (color != params->maskc) {
                *dstdword = ((color & 0x001f) << 3) | ((color & 0x001c) >> 2)
                          | ((color & 0x07e0) << 5) | ((color & 0x0600) >> 1)
                          | ((color & 0xf800) << 8) | ((color & 0xe000) << 3);
            }
            dstdword++;
            srcword ++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB888(srcbyte[2], srcbyte[1], srcbyte[0]);
            if (color != params->maskc) {
                *dstdword = color;
            }
            dstdword += 1;
            srcbyte  += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = *srcdword;
            if (color != params->maskc) {
                *dstdword = color;
            }
            dstdword++;
            srcdword++;
            if (srcdword == end) srcdword = start;
        }
        break;
    }
}

PFNSCANLINE TABFN_SCANLINEBMP_MASK[] =
{
    scanline__8bitbmp_mask,
    scanline_16bitbmp_mask,
    scanline_24bitbmp_mask,
    scanline_32bitbmp_mask,
};
/* -- TABFN_SCANLINEBMP_MASK */
