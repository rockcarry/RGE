#ifndef __RGE_DRAWCTXT_H__
#define __RGE_DRAWCTXT_H__

// 包含头文件
#include "pixel.h"
#include "scanl.h"

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

    #define MAX_PPBUF_SIZE 4096
    int *ppbuf;
    int  ppcur;
} DRAWCONTEXT;

void select_scanline_color(BMP *dstpb, BMP *srcpb, int style, PFNSCANLINE *pfn, DWORD *color);

#endif
