#ifndef __RGE_DRAWCTXT_H__
#define __RGE_DRAWCTXT_H__

// 包含头文件
#include "pixel.h"
#include "scanline.h"

/* 常量定义 */
#define DRAW2D_STACK_SIZE  4096

/* draw context */
typedef struct {
    BMP  *dstbmp;      /* dest drawing bmp */

    int   antialias;   /* anti alias */
    int   drawalpha;   /* alpha value */
    DWORD drawcolor;   /* draw color */
    DWORD drawflags;   /* draw flags */

    int   linewidth;   /* line width */
    DWORD linestyle;   /* line style */

    /* ++ for fillpolygon or bitblt ++ */
    int   fillorgx;    /* fill org x */
    int   fillorgy;    /* fill org y */
    DWORD maskcolor;   /* mask color */
    SCANLINEPARAMS fillparams;
    /* -- for fillpolygon or bitblt -- */
    
    void *textfont;    /* pointer to font */
    DWORD textcolor;   /* font color */

    PFNPIXEL    pixelalpha;
    PFNPIXEL    pixelsolid;
    PFNSCANLINE scanlinebarsolid;
    PFNSCANLINE scanlinebaralpha;
    PFNSCANLINE scanlinebmpfast;
    PFNSCANLINE scanlinebmpconvert;
    PFNSCANLINE scanlinebmpmask;
    PFNSCANLINE scanlinebmpalpha;
    PFNSCANLINE scanlinebmpmaskalpha;
    PFNSCANLINE scanlinepalmapdst;
    PFNSCANLINE scanlinepalmapsrc;

    PFNPIXEL    pixel;
    PFNSCANLINE scanline;
} DRAWCONTEXT;

void select_scanline_color(BMP *dstpb, BMP *srcpb, int style, PFNSCANLINE *pfn, DWORD *color);

#endif
