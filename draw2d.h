#ifndef __RGE_DRAW2D_H__
#define __RGE_DRAW2D_H__

/* 包含头文件 */
#include "scanl.h"

/* pen style */
enum {
    DS_CIRCLE, /* 圆点 */
    DS_SQAURE, /* 方点 */
};

/* line style */
enum {
    LS_SOLID  = 0xffffffffL,  /* 实线   */
    LS_DOTTED = 0xaaaaaaaaL,  /* 虚点线 */
    LS_DASHED = 0xf0f0f0f0L,  /* 虚划线 */
};

/* draw flags */
enum {
    DF_POLYGON_CLOSED   = (1 << 0),
    DF_ARC_CENTER_POINT = (1 << 1),
};

/* paint begin & end */
void* draw2d_init(BMP  *pb  );
void  draw2d_free(void *ctxt);
void  paint_begin(void *ctxt);
void  paint_done (void *ctxt);

void setdrawalpha(void *ctxt, int   alpha );
void setdrawcolor(void *ctxt, DWORD color );
void setdrawflags(void *ctxt, DWORD flags );
void setantialias(void *ctxt, int   antia );
void setlinestyle(void *ctxt, DWORD style );
void setlinewidth(void *ctxt, DWORD width );
void setfillstyle(void *ctxt, DWORD style );
void setfillcolor(void *ctxt, DWORD color );
void setfillbmp  (void *ctxt, BMP  *bmp   );
void setfillorgxy(void *ctxt, int x, int y);
void setmaskcolor(void *ctxt, DWORD color );
void setpalmaptab(void *ctxt, BYTE *palmap);

void  putpixel(void *ctxt, int x, int y, DWORD c);
DWORD getpixel(void *ctxt, int x, int y);

void line     (void *ctxt, int x1, int y1, int x2, int y2); // 线
void rectangle(void *ctxt, int x1, int y1, int x2, int y2); // 矩形
void roundrect(void *ctxt, int x1, int y1, int x2, int y2, int a, int b); // 圆角矩形
void circle   (void *ctxt, int xo, int yo, int r);          // 圆
void ellipse  (void *ctxt, int xo, int yo, int a, int b);   // 椭圆
BOOL arc      (void *ctxt, int xo, int yo, int a, int b, int s, int e);   // 圆弧
BOOL polygon  (void *ctxt, int *pp, int n);                 // 多边形
BOOL floodfill(void *ctxt, int x, int y);                   // 泛滥式填充
BOOL bezier   (void *ctxt, int *pp, int degree, int pd);    // 贝塞尔曲线

#endif


