#ifndef _DRAW2D_H_
#define _DRAW2D_H_

/* 包含头文件 */
#include "bmp.h"

/* 常量定义 */
#define RGB332(r, g, b)  ( (((r) & 0xE0) <<  0) | (((g) & 0xE0) >> 3) | (((b) & 0xC0) >> 6) )
#define RGB555(r, g, b)  ( (((r) & 0xF8) <<  7) | (((g) & 0xF8) << 2) | (((b) & 0xF8) >> 3) )
#define RGB565(r, g, b)  ( (((r) & 0xF8) <<  8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3) )
#define RGB888(r, g, b)  ( ((DWORD)(0) << 24) | ((r) << 16) | ((g) << 8) | ((b) << 0) )
#define ARGB(a, r, g, b) ( ((DWORD)(a) << 24) | ((r) << 16) | ((g) << 8) | ((b) << 0) )
#define RGB(r, g, b)     ARGB(0xff, r, g, b)

/* pen style */
enum {
    DOT_STYLE_CIRCLE, /* 圆点 */
    DOT_STYLE_SQAURE, /* 方点 */
};

enum {
    LINE_STYLE_SOLID  = 0xffffffffL,  /* 实线   */
    LINE_STYLE_DOTTED = 0xaaaaaaaaL,  /* 虚点线 */
    LINE_STYLE_DASHED = 0xf0f0f0f0L,  /* 虚划线 */
};

enum {
    FILL_STYLE_NONE        ,  /* none fill brush */
    FILL_STYLE_SOLID       ,  /* solid fill brush */
    FILL_STYLE_BMP_COPY    ,  /* bitmap fill brush, copy directly */
    FILL_STYLE_BMP_MASK    ,  /* bitmap fill brush, with color key mask */
    FILL_STYLE_PALMAP_SOLID,  /* color fill with palmap color, only for 256-color */
    FILL_STYLE_PALMAP_BMP  ,  /* color fill with palmap bmp  , only for 256-color */
};

enum {
    BLT_TYPE_COPY          ,  /* copy bitblt */
    BLT_TYPE_MASK          ,  /* mask bitblt */
    BLT_TYPE_PALMAP        ,  /* palmap bitblt */
};

enum {
    DRAW_FLAG_POLYGON_CLOSED   = (1 << 0),
    DRAW_FLAG_ARC_CENTER_POINT = (1 << 1),
};

/* paint begin & end */
void* paint_begin(BMP  *pb  );
void  paint_end  (void *ctxt);

void setdrawalpha(void *ctxt, int   alpha );
void setdrawcolor(void *ctxt, DWORD color );
void setdrawflags(void *ctxt, DWORD flags );
void setantialias(void *ctxt, int   antia );
void setlinewidth(void *ctxt, DWORD width );
void setlinestyle(void *ctxt, DWORD style );
void setfillcolor(void *ctxt, DWORD color );
void setfillbmp  (void *ctxt, BMP  *bmp   );
void setfillorgxy(void *ctxt, int x, int y);
void setfillstyle(void *ctxt, DWORD style );
void setmaskcolor(void *ctxt, DWORD color );
void setpalmaptab(void *ctxt, BYTE *palmap);

void  putpixel(void *ctxt, int x, int y, DWORD c);
DWORD getpixel(void *ctxt, int x, int y);

void line     (void *ctxt, int x1, int y1, int x2, int y2); // 线
void rectangle(void *ctxt, int x1, int y1, int x2, int y2); // 矩形
void roundrect(void *ctxt, int x1, int y1, int x2, int y2, int a, int b); // 圆角矩形
void circle   (void *ctxt, int xo, int yo, int r);          // 圆
void ellipse  (void *ctxt, int xo, int yo, int a, int b);   // 椭圆
void arc      (void *ctxt, int xo, int yo, int a, int b, int s, int e);   // 圆弧
void polygon  (void *ctxt, int *pp, int n);                 // 多边形
void floodfill(void *ctxt, int x, int y);                   // 泛滥式填充 
void bezier   (void *ctxt, int *pp, int degree);            // 贝塞尔曲线

#endif


