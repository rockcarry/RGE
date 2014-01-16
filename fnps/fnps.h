#ifndef _FNPS_H_
#define _FNPS_H_

/* 包含头文件 */
#include "../bmp.h"
#include "../draw2d.h"

/* 类型定义 */
typedef void (*PFNPIXEL   )(BMP*, int, int, DWORD);       // 像素点函数指针类型
typedef void (*PFNSCANLINE)(BMP*, int, int, int, DWORD);  // 扫描线函数指针类型

/* draw context */
typedef struct
{
    BMP  *dstbmp;      /* dest drawing bmp */

    int   alpha;       /* alpha value */
    int   antialias;   /* anti alias */
    DWORD drawcolor;   /* draw color */
    DWORD drawflags;   /* draw flags */

    int   linewidth;   /* line width */
    DWORD linestyle;   /* line style */

    /* ++ for fillpolygon or bitblt ++ */
    DWORD fillcolor;   /* fill color */
    BMP  *fillbmp;     /* fill bmp */
    int   fillorgx;    /* fill org x */
    int   fillorgy;    /* fill org y */
    DWORD fillstyle;   /* fill style */
    DWORD maskc;       /* mask color */
    BYTE *palmap;      /* palmap table */
    /* -- for fillpolygon or bitblt -- */
    
    void *textfont;    /* pointer to font */
    DWORD textcolor;   /* font color */

    /* for bitblt */
    int   blttype;     /* bitblt type */

    PFNPIXEL    pixelalpha;
    PFNPIXEL    pixelsolid;
    PFNSCANLINE scanlinesolid;
    PFNSCANLINE scanlinealpha;
    PFNSCANLINE scanlinebmpfast;
    PFNSCANLINE scanlinebmpconvert;
    PFNSCANLINE scanlinebmpalpha;
    PFNSCANLINE scanlinebmpmaskalpha;
    PFNSCANLINE scanlinepalmapsolid;
    PFNSCANLINE scanlinepalmapbmp;

    PFNPIXEL    pixel;
    PFNSCANLINE scanline;

    #define MAX_PPBUF_SIZE 4096
    int *ppbuf;
    int  ppcur;
} DRAWCONTEXT;

/* 变量声明 */
extern PFNPIXEL TABFN_PIXEL_SOLID[];
extern PFNPIXEL TABFN_PIXEL_ALPHA[];
extern PFNSCANLINE TABFN_SCANLINE_SOLID[];
extern PFNSCANLINE TABFN_SCANLINE_ALPHA[];
extern PFNSCANLINE TABFN_SCANLINEBMP_FAST[];
extern PFNSCANLINE TABFN_SCANLINEBMP_CONVERT[];
extern PFNSCANLINE TABFN_SCANLINEBMP_ALPHA[];
extern PFNSCANLINE TABFN_SCANLINEBMP_MASKALPHA[];
extern PFNSCANLINE PFN_SCANLINE_PALMAP_COLOR;
extern PFNSCANLINE PFN_SCANLINE_PALMAP_BMP;

#endif










