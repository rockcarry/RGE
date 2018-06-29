/* 包含头文件 */
#include "scanline.h"

/* 内部函数实现 */
static __inline void draw_alpha_8bit(BYTE *pdst, int alpha, DWORD color)
{
    DWORD fc = (color | (color << 9)) & 0x38E3;
    DWORD bc = (*pdst | (*pdst << 9)) & 0x38E3;
    DWORD rc = (bc + alpha * (fc - bc) / 8) & 0x38E3;
    *pdst = (BYTE)(rc | (rc >> 9));
}

static __inline void draw_alpha_16bit(WORD *pdst, int alpha, DWORD color)
{
    DWORD fc = (color | (color << 16)) & 0x07E0F81FL;
    DWORD bc = (*pdst | (*pdst << 16)) & 0x07E0F81FL;
    DWORD rc = (bc + alpha * (fc - bc) / 32) & 0x07E0F81FL;
    *pdst = (WORD)(rc | (rc >> 16));
}

static __inline void draw_alpha_24bit(BYTE *pdst, int alpha, BYTE r, BYTE g, BYTE b)
{
    pdst[0] = pdst[0] + alpha * (b - pdst[0]) / 256;
    pdst[1] = pdst[1] + alpha * (g - pdst[1]) / 256;
    pdst[2] = pdst[2] + alpha * (r - pdst[2]) / 256;
}

static __inline void draw_alpha_32bit(DWORD *pdst, int alpha, DWORD color)
{
    DWORD fc = color & 0xff00ffL;
    DWORD fg = color & 0x00ff00L;
    DWORD bc = (*pdst & 0xff00ffL);
    DWORD bg = (*pdst & 0x00ff00L);
    DWORD rc = (bc + alpha * (fc - bc) / 256) & 0xff00ffL;
    DWORD rg = (bg + alpha * (fg - bg) / 256) & 0x00ff00L;
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
            color = RGB332(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
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
            color = RGB565(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
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
            r = pal[(size_t)color * 4 + 2];
            g = pal[(size_t)color * 4 + 1];
            b = pal[(size_t)color * 4 + 0];
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
            color = RGB888(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
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
                color = RGB332(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
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
                color = RGB565(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
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
                r = pal[(size_t)color * 4 + 2];
                g = pal[(size_t)color * 4 + 1];
                b = pal[(size_t)color * 4 + 0];
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
                color = RGB888(pal[(size_t)color*4+2], pal[(size_t)color*4+1], pal[(size_t)color*4+0]);
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
