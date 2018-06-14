/* 包含头文件 */
#include "scanl.h"
#include "bitblt.h"

/* 内部函数实现 */
static void scanline_8bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    memset(dst, params->fillc, w);
}

static void scanline16bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD *dstword = (WORD*)dst;
    while (w--) {
        *dstword++ = (WORD)params->fillc;
    }
}

static void scanline24bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *dstbyte = (BYTE*)dst;
    while (w--) {
        *dstbyte++ = (BYTE)(params->fillc >> 0 );
        *dstbyte++ = (BYTE)(params->fillc >> 8 );
        *dstbyte++ = (BYTE)(params->fillc >> 16);
    }
}

static void scanline32bitsolid(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
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

static void scanline_8bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *pbyte = (BYTE*)dst;
    DWORD alpha = (params->alpha >> 5);
    DWORD fc    = (params->fillc | (params->fillc << 9)) & 0x38E3;
    DWORD bc, rc;
    while (w--) {
        bc = (*pbyte | (*pbyte << 9)) & 0x38E3;
        rc = (bc + alpha * (fc - bc) / 8) & 0x38E3;
        *pbyte++ = (BYTE)(rc | rc >> 9);
    }
}

static void scanline16bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD *pword = (WORD*)dst;
    DWORD alpha = (params->alpha >> 3);
    DWORD fc    = (params->fillc | (params->fillc << 16)) & 0x07E0F81F;
    DWORD bc, rc;
    while (w--) {
        bc = (*pword | (*pword << 16)) & 0x07E0F81F;
        rc = (bc + alpha * (fc - bc) / 32) & 0x07E0F81F;
        *pword++ = (WORD)(rc | rc >> 16);
    }
}

static void scanline24bitalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE *pbyte = (BYTE*)dst;
    DWORD alpha =  params->alpha;
    DWORD r     = (params->fillc >> 16) & 0xff;
    DWORD g     = (params->fillc >> 8 ) & 0xff;
    DWORD b     = (params->fillc >> 0 ) & 0xff;
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
    DWORD  fc     = params->fillc & 0xff00ff;
    DWORD  fg     = params->fillc & 0x00ff00;
    DWORD  bc, bg, rc, rg;
    while (w--) {
        bc = (*pdword & 0xff00ff);
        bg = (*pdword & 0x00ff00);
        rc = (bc + alpha * (fc - bc) / 256) & 0xff00ff;
        rg = (bg + alpha * (fg - bg) / 256) & 0x00ff00;
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

//++ TABFN_SCANLINE_MEMCPY
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
//-- TABFN_SCANLINE_MEMCPY

//++ TABFN_SCANLINE_CONVERT
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
            *dstbyte++ = RGB332(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
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
            *dstword++ = RGB565(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
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
            *dstbyte++ = pal[color * 4 + 0];
            *dstbyte++ = pal[color * 4 + 1];
            *dstbyte++ = pal[color * 4 + 2];
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
            *dstdword++= RGB888(pal[color*4+2], pal[color*4+1], pal[color*4+0]);
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
//-- TABFN_SCANLINE_CONVERT

//++ TABFN_SCANLINE_MASK
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
                    *dstbyte = RGB332(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
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
                *dstword = RGB565(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
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
                dstbyte[0] = pal[color * 4 + 0];
                dstbyte[1] = pal[color * 4 + 1];
                dstbyte[2] = pal[color * 4 + 2];
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
                *dstdword = RGB888(pal[color*4+2], pal[color*4+1], pal[color*4+0]);
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
//-- TABFN_SCANLINE_MASK

static __inline void draw_alpha_8bit(BYTE *pdst, int alpha, DWORD color)
{
    DWORD fc = (color | (color << 9)) & 0x38E3;
    DWORD bc = (*pdst | (*pdst << 9)) & 0x38E3;
    DWORD rc = (bc + alpha * (fc - bc) / 8) & 0x38E3;
    *pdst = (BYTE)(rc | rc >> 9);
}

static __inline void draw_alpha_16bit(WORD *pdst, int alpha, DWORD color)
{
    DWORD fc = (color | (color << 16)) & 0x07E0F81F;
    DWORD bc = (*pdst | (*pdst << 16)) & 0x07E0F81F;
    DWORD rc = (bc + alpha * (fc - bc) / 32) & 0x07E0F81F;
    *pdst = (WORD)(rc | rc >> 16);
}

static __inline void draw_alpha_24bit(BYTE *pdst, int alpha, BYTE r, BYTE g, BYTE b)
{
    pdst[0] = pdst[0] + alpha * (b - pdst[0]) / 256;
    pdst[1] = pdst[1] + alpha * (g - pdst[1]) / 256;
    pdst[2] = pdst[2] + alpha * (r - pdst[2]) / 256;
}

static __inline void draw_alpha_32bit(DWORD *pdst, int alpha, DWORD color)
{
    DWORD fc = color & 0xff00ff;
    DWORD fg = color & 0x00ff00;
    DWORD bc = (*pdst & 0xff00ff);
    DWORD bg = (*pdst & 0x00ff00);
    DWORD rc = (bc + alpha * (fc - bc) / 256) & 0xff00ff;
    DWORD rg = (bg + alpha * (fg - bg) / 256) & 0x00ff00;
    *pdst = rc | rg;
}

static void scanline__8bitbmp_alpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    void  *start   = params->start;
    void  *end     = params->end;
    int    alpha   = params->alpha >> 5;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            color = RGB332(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
            draw_alpha_8bit(dstbyte, alpha, color);
            dstbyte++;
            srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            color = ((color & 0x0018) >> 3) | ((color & 0x0700) >> 6) | ((color & 0xE000) >> 8);
            draw_alpha_8bit(dstbyte, alpha, color);
            dstbyte++;
            srcword++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
            draw_alpha_8bit(dstbyte, alpha, color);
            dstbyte += 1;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
            draw_alpha_8bit(dstbyte, alpha, color);
            dstbyte  += 1;
            srcbyte  += 4;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

static void scanline_16bitbmp_alpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD  *dstword = (WORD *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    void  *start   = params->start;
    void  *end     = params->end;
    int    alpha   = params->alpha >> 3;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            color = RGB565(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
            draw_alpha_16bit(dstword, alpha, color);
            dstword++;
            srcbyte++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            draw_alpha_16bit(dstword, alpha, color);
            dstword++;
            srcword++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
            draw_alpha_16bit(dstword, alpha, color);
            dstword += 1;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
            draw_alpha_16bit(dstword, alpha, color);
            dstword  += 1;
            srcbyte  += 4;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

static void scanline_24bitbmp_alpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    void  *start   = params->start;
    void  *end     = params->end;
    int    alpha   = params->alpha;
    BYTE   r, g, b;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            r = pal[color * 4 + 2];
            g = pal[color * 4 + 1];
            b = pal[color * 4 + 0];
            draw_alpha_24bit(dstbyte, alpha, r, g, b);
            dstbyte += 3;
            srcbyte += 1;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            r = (BYTE)(((color & 0xf800) >> 8) | ((color & 0xe000) >>13));
            g = (BYTE)(((color & 0x07e0) >> 3) | ((color & 0x0600) >> 9));
            b = (BYTE)(((color & 0x001f) << 3) | ((color & 0x001c) >> 2));
            draw_alpha_24bit(dstbyte, alpha, r, g, b);
            dstbyte += 3;
            srcword += 1;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            r = srcbyte[2];
            g = srcbyte[1];
            b = srcbyte[0];
            draw_alpha_24bit(dstbyte, alpha, r, g, b);
            dstbyte += 3;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            r = srcbyte[2];
            g = srcbyte[1];
            b = srcbyte[0];
            draw_alpha_24bit(dstbyte, alpha, r, g, b);
            dstbyte += 3;
            srcbyte += 4;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    }
}

static void scanline_32bitbmp_alpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
    BYTE  *srcbyte  = (BYTE *)src;
    WORD  *srcword  = (WORD *)src;
    DWORD *srcdword = (DWORD*)src;
    void  *start    = params->start;
    void  *end      = params->end;
    int    alpha    = params->alpha;
    DWORD  color;
    BYTE  *pal;

    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            color = RGB888(pal[color*4+2], pal[color*4+1], pal[color*4+0]);
            draw_alpha_32bit(dstdword, alpha, color);
            dstdword++;
            srcbyte ++;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            color = ((color & 0x001f) << 3) | ((color & 0x001c) >> 2)
                  | ((color & 0x07e0) << 5) | ((color & 0x0600) >> 1)
                  | ((color & 0xf800) << 8) | ((color & 0xe000) << 3);
            draw_alpha_32bit(dstdword, alpha, color);
            dstdword++;
            srcword ++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            color = RGB888(srcbyte[2], srcbyte[1], srcbyte[0]);
            draw_alpha_32bit(dstdword, alpha, color);
            dstdword += 1;
            srcbyte  += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            color = *srcdword;
            draw_alpha_32bit(dstdword, alpha, color);
            dstdword++;
            srcdword++;
            if (srcdword == end) srcdword = start;
        }
        break;
    }
}

PFNSCANLINE TABFN_SCANLINEBMP_ALPHA[] =
{
    scanline__8bitbmp_alpha,
    scanline_16bitbmp_alpha,
    scanline_24bitbmp_alpha,
    scanline_32bitbmp_alpha,
};

static void scanline__8bitbmp_maskalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    DWORD *srcdword= (DWORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    int    alpha   = params->alpha >> 5;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                color = RGB332(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
                draw_alpha_8bit(dstbyte, alpha, color);
            }
            dstbyte++;
            srcbyte++;
            if (dstbyte == end) dstbyte = start;
        }
        break;
    case 16:
        while (w--) {
            color = *srcword;
            if (color != params->maskc) {
                color = ((color & 0x0018) >> 3) | ((color & 0x0700) >> 6) | ((color & 0xE000) >> 8);
                draw_alpha_8bit(dstbyte, alpha, color);
            }
            dstbyte++;
            srcword++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            if (RGB888(srcbyte[2], srcbyte[1], srcbyte[0]) != params->maskc) {
                color = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
                draw_alpha_8bit(dstbyte, alpha, color);
            }
            dstbyte += 1;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            if (*srcdword != params->maskc) {
                color = RGB332(srcbyte[2], srcbyte[1], srcbyte[0]);
                draw_alpha_8bit(dstbyte, alpha, color);
            }
            dstbyte  += 1;
            srcbyte  += 4;
            srcdword += 1;
            if (srcbyte == end) { srcbyte = start; srcdword = start; }
        }
        break;
    }
}

static void scanline_16bitbmp_maskalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    WORD  *dstword = (WORD *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    DWORD *srcdword= (DWORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    int    alpha   = params->alpha >> 3;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                color = RGB565(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
                draw_alpha_16bit(dstword, alpha, color);
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
                draw_alpha_16bit(dstword, alpha, color);
            }
            dstword++;
            srcword++;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            if (RGB888(srcbyte[2], srcbyte[1], srcbyte[0]) != params->maskc) {
                color = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
                draw_alpha_16bit(dstword, alpha, color);
            }
            dstword += 1;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            if (*srcdword != params->maskc) {
                color = RGB565(srcbyte[2], srcbyte[1], srcbyte[0]);
                draw_alpha_16bit(dstword, alpha, color);
            }
            dstword  += 1;
            srcbyte  += 4;
            srcdword += 1;
            if (srcbyte == end) { srcbyte = start; srcdword = start; }
        }
        break;
    }
}

static void scanline_24bitbmp_maskalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    BYTE  *dstbyte = (BYTE *)dst;
    BYTE  *srcbyte = (BYTE *)src;
    WORD  *srcword = (WORD *)src;
    DWORD *srcdword= (DWORD*)src;
    void  *start   = params->start;
    void  *end     = params->end;
    int    alpha   = params->alpha;
    BYTE   r, g, b;
    DWORD  color;
    BYTE  *pal;
    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                r = pal[color * 4 + 2];
                g = pal[color * 4 + 1];
                b = pal[color * 4 + 0];
                draw_alpha_24bit(dstbyte, alpha, r, g, b);
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
                r = (BYTE)(((color & 0xf800) >> 8) | ((color & 0xe000) >>13));
                g = (BYTE)(((color & 0x07e0) >> 3) | ((color & 0x0600) >> 9));
                b = (BYTE)(((color & 0x001f) << 3) | ((color & 0x001c) >> 2));
                draw_alpha_24bit(dstbyte, alpha, r, g, b);
            }
            dstbyte += 3;
            srcword += 1;
            if (srcword == end) srcword = start;
        }
        break;
    case 24:
        while (w--) {
            r = srcbyte[2];
            g = srcbyte[1];
            b = srcbyte[0];
            if (RGB888(r, g, b) != params->maskc) {
                draw_alpha_24bit(dstbyte, alpha, r, g, b);
            }
            dstbyte += 3;
            srcbyte += 3;
            if (srcbyte == end) srcbyte = start;
        }
        break;
    case 32:
        while (w--) {
            if (*srcdword != params->maskc) {
                r = srcbyte[2];
                g = srcbyte[1];
                b = srcbyte[0];
                draw_alpha_24bit(dstbyte, alpha, r, g, b);
            }
            dstbyte += 3;
            srcbyte += 4;
            srcdword+= 1;
            if (srcbyte == end) { srcbyte = start; srcdword = start; }
        }
        break;
    }
}

static void scanline_32bitbmp_maskalpha(void *dst, void *src, int w, SCANLINEPARAMS *params)
{
    DWORD *dstdword = (DWORD*)dst;
    BYTE  *srcbyte  = (BYTE *)src;
    WORD  *srcword  = (WORD *)src;
    DWORD *srcdword = (DWORD*)src;
    void  *start    = params->start;
    void  *end      = params->end;
    int    alpha    = params->alpha;
    DWORD  color;
    BYTE  *pal;

    switch (params->srcbmp->cdepth) {
    case 8:
        pal = params->srcbmp->ppal;
        while (w--) {
            color = *srcbyte;
            if (color != params->maskc) {
                color = RGB888(pal[color*4+2], pal[color*4+1], pal[color*4+0]);
                draw_alpha_32bit(dstdword, alpha, color);
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
                color = ((color & 0x001f) << 3) | ((color & 0x001c) >> 2)
                      | ((color & 0x07e0) << 5) | ((color & 0x0600) >> 1)
                      | ((color & 0xf800) << 8) | ((color & 0xe000) << 3);
                draw_alpha_32bit(dstdword, alpha, color);
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
                draw_alpha_32bit(dstdword, alpha, color);
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
                draw_alpha_32bit(dstdword, alpha, color);
            }
            dstdword++;
            srcdword++;
            if (srcdword == end) srcdword = start;
        }
        break;
    }
}

PFNSCANLINE TABFN_SCANLINEBMP_MASKALPHA[] =
{
    scanline__8bitbmp_maskalpha,
    scanline_16bitbmp_maskalpha,
    scanline_24bitbmp_maskalpha,
    scanline_32bitbmp_maskalpha,
};

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
