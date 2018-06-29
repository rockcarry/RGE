#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "drawctxt.h"
#include "draw2d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void* draw2d_init(BMP *pb)
{
    DRAWCONTEXT *pc;

    /* invalid pb */
    if (!pb) return NULL;

    /* allocate context */
    pc = (DRAWCONTEXT*)calloc(1, sizeof(DRAWCONTEXT));
    if (!pc) return NULL;

    /* init context */
    pc->dstbmp               = pb;
    pc->drawcolor            = RGB888(255, 255, 255);
    pc->linestyle            = LS_SOLID;
    pc->textcolor            = RGB888(255, 255, 255);
    pc->pixelsolid           = TABFN_PIXEL_SOLID          [pb->cdepth / 8 - 1];
    pc->pixelalpha           = TABFN_PIXEL_ALPHA          [pb->cdepth / 8 - 1];
    pc->scanlinebarsolid     = TABFN_SCANLINEBAR_SOLID    [pb->cdepth / 8 - 1];
    pc->scanlinebaralpha     = TABFN_SCANLINEBAR_ALPHA    [pb->cdepth / 8 - 1];
    pc->scanlinebmpfast      = TABFN_SCANLINEBMP_FAST     [pb->cdepth / 8 - 1];
    pc->scanlinebmpconvert   = TABFN_SCANLINEBMP_CONVERT  [pb->cdepth / 8 - 1];
    pc->scanlinebmpmask      = TABFN_SCANLINEBMP_MASK     [pb->cdepth / 8 - 1];
    pc->scanlinebmpalpha     = TABFN_SCANLINEBMP_ALPHA    [pb->cdepth / 8 - 1];
    pc->scanlinebmpmaskalpha = TABFN_SCANLINEBMP_MASKALPHA[pb->cdepth / 8 - 1];
    pc->scanlinepalmapdst    = PFN_SCANLINE_PALMAP_DST;
    pc->scanlinepalmapsrc    = PFN_SCANLINE_PALMAP_SRC;

    /* pixel & scanline */
    pc->pixel    = pc->pixelsolid;
    pc->scanline = pc->scanlinebarsolid;

    return pc;
}

void draw2d_free(void *ctxt)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    /* invalid ctxt */
    if (!ctxt) return;

    /* release context */
    if (pc) free(pc);
}

void paint_begin(void *ctxt)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    lockbmp(pc->dstbmp);
}

void paint_done(void *ctxt)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    unlockbmp(pc->dstbmp);
}

void setdrawalpha(void *ctxt, int alpha)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->drawalpha = pc->fillparams.alpha = alpha;
    if (alpha == 0) pc->pixel = pc->pixelsolid;
    else            pc->pixel = pc->pixelalpha;
    pc->drawcolor &= ~(0xff  << 24);
    pc->drawcolor |=  (alpha << 24);
}

void setdrawcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->drawcolor   = COLOR_CONVERT(pc->dstbmp->cdepth, color, TRUE);
    pc->drawcolor  |= ((DWORD)pc->drawalpha << 24);
}

void setdrawflags(void *ctxt, DWORD flags)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->drawflags = flags;
}

void setantialias(void *ctxt, int antia)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->antialias = antia;
}

void setlinestyle(void *ctxt, DWORD style)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->linestyle = style;
}

void setlinewidth(void *ctxt, int width)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->linewidth = width;
}

void setfillstyle(void *ctxt, int style)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    BMP         *dstpb = pc->dstbmp;
    BMP         *srcpb = pc->fillparams.srcbmp;

    pc->fillparams.style = style;
    select_scanline_func(dstpb, srcpb, style, &pc->scanline);
}

void setfillcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillparams.fillc = COLOR_CONVERT(pc->dstbmp->cdepth, color, TRUE);
}

void setfillpattern(void *ctxt, DWORD *pattern)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillparams.pattern = pattern;
}

void setfillbmp(void *ctxt, BMP *bmp)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillparams.srcbmp = bmp;
}

void setfillsrcxy(void *ctxt, int x, int y)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillparams.fillsrcx = x;
    pc->fillparams.fillsrcy = y;
}

void setmaskcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc     = (DRAWCONTEXT*)ctxt;
    BMP         *srcbmp = pc->fillparams.srcbmp;
    if (color == -1 && srcbmp) {
        switch (srcbmp->cdepth) {
        case 8 : color = *(BYTE *)srcbmp->pdata; break;
        case 16: color = *(WORD *)srcbmp->pdata; break;
        case 24: color = *(DWORD*)srcbmp->pdata; break;
        case 32: color = *(DWORD*)srcbmp->pdata; break;
        }
        if (srcbmp->cdepth == 24) color &= 0xffffffL;
    }
    pc->fillparams.maskc = color;
}

void setpalmaptab(void *ctxt, BYTE *palmap)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillparams.palmap = palmap;
}

void putpixel(void *ctxt, int x, int y, DWORD c)
{
    DRAWCONTEXT *pc     = (DRAWCONTEXT*)ctxt;
    PFNPIXEL     pixel  = pc->pixel;
    BMP         *dstbmp = pc->dstbmp;
    pixel(dstbmp, x, y, c);
}

DWORD getpixel(void *ctxt, int x, int y)
{
    DRAWCONTEXT *pc     = (DRAWCONTEXT*)ctxt;
    BMP         *dstbmp = pc->dstbmp;
    BYTE        r, g, b;
    switch (dstbmp->cdepth) {
    case 8:
        return ((BYTE*)(dstbmp->pdata + y * dstbmp->stride))[x];
    case 16:
        return ((WORD*)(dstbmp->pdata + y * dstbmp->stride))[x];
    case 24:
        b = ((BYTE*)(dstbmp->pdata + y * dstbmp->stride))[x + 0];
        g = ((BYTE*)(dstbmp->pdata + y * dstbmp->stride))[x + 1];
        r = ((BYTE*)(dstbmp->pdata + y * dstbmp->stride))[x + 2];
        return RGB888(r, g, b);
    case 32:
        return ((DWORD*)(dstbmp->pdata + y * dstbmp->stride))[x];
    default:
        return 0;
    }
}

static __inline void scanline(DRAWCONTEXT *ctxt, int y, int x1, int x2)
{
    BMP  *dstpb = ctxt->dstbmp;
    BMP  *srcpb = ctxt->fillparams.srcbmp;
    int   dstx  = x1;
    int   dsty  = y ;
    void *dstp  = (BYTE*)dstpb->pdata + dsty * dstpb->stride + dstx * (dstpb->cdepth / 8);
    int   srcx;
    int   srcy;
    void *srcp  = NULL;
    if (srcpb) {
        srcx = (x1 + ctxt->fillparams.fillsrcx) % srcpb->width;
        srcy = (y  + ctxt->fillparams.fillsrcy) % srcpb->height;
        ctxt->fillparams.start = (BYTE*)srcpb->pdata + srcy * srcpb->stride;
        ctxt->fillparams.end   = (BYTE*)ctxt->fillparams.start + (srcpb->cdepth / 8) * srcpb->width;
        srcp                   = (BYTE*)ctxt->fillparams.start + (srcpb->cdepth / 8) * srcx;
    }
    ctxt->fillparams.filldstx = x1;
    ctxt->fillparams.filldsty = y ;
    ctxt->scanline(dstp, srcp, x2 - x1 + 1, &ctxt->fillparams);
}

void line(void *ctxt, int x1, int y1, int x2, int y2)
{
    DRAWCONTEXT *pc     = (DRAWCONTEXT*)ctxt;
    PFNPIXEL     pixel  = pc->pixel;
    BMP         *dstbmp = pc->dstbmp;
    DWORD        dc     = pc->drawcolor;
    int x, y, dx, dy, e;

    /* 绘制图形 */
    dx = abs(x1 - x2);
    dy = abs(y1 - y2);
    e  = -dx;

    if (dy < dx) {
        if (x1 > x2) {
            x  = x1;
            x1 = x2;
            x2 = x;
            y  = y1;
            y1 = y2;
            y2 = y;
        }

        y = y1;
        x = x1;
        while (x <= x2) {
            pixel(dstbmp, x, y, dc);

            e += dy * 2;
            if (e >= 0) {
                if (y1 < y2) y++;
                else y--;
                e -= dx * 2;
            }
            x++;
        }
    } else {
        if (y1 > y2) {
            x  = x1;
            x1 = x2;
            x2 = x;
            y  = y1;
            y1 = y2;
            y2 = y;
        }

        y = y1;
        x = x1;
        while (y <= y2) {
            pixel(dstbmp, x, y, dc);

            e += dx * 2;
            if (e > 0) {
                if (x1 < x2) x++;
                else x--;
                e -= dy * 2;
            }
            y++;
        }
    }
}

void rectangle(void *ctxt, int x1, int y1, int x2, int y2)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    int          t;

    if (x1 > x2) {
        t  = x1;
        x1 = x2;
        x2 = t;
    }

    if (y1 > y2) {
        t  = y1;
        y1 = y2;
        y2 = t;
    }

    if (!(pc->fillparams.style & FS_NONE)) {
        for (t=y1; t<=y2; t++) {
            scanline(ctxt, t, x1, x2);
        }
    } else {
        line(ctxt, x1, y1, x2, y1);
        line(ctxt, x1, y2, x2, y2);
        line(ctxt, x1, y1, x1, y2);
        line(ctxt, x2, y1, x2, y2);
    }
}

void roundrect(void *ctxt, int x1, int y1, int x2, int y2, int a, int b)
{
    DRAWCONTEXT *pc     = (DRAWCONTEXT*)ctxt;
    PFNPIXEL     pixel  = pc->pixel;
    BMP         *dstbmp = pc->dstbmp;
    DWORD        dc     = pc->drawcolor;
    int  x, y, t;
    long aa, bb, d;
    BOOL flag = FALSE;

    if (x1 > x2) {
        t  = x1;
        x1 = x2;
        x2 = t;
    }

    if (y1 > y2) {
        t  = y1;
        y1 = y2;
        y2 = t;
    }

    if (!(pc->fillparams.style & FS_NONE)) {
        /* + draw filled rectangle */
        for (y=y1; y<y2; y++) scanline(ctxt, y, x1 - a, x2 + a);
        /* - draw filled rectangle */

        /* + draw filled ellipse */
        if (a < b) {
            flag = TRUE;
            t = a;
            a = b;
            b = t;
        }
        aa = (long)a * a;
        bb = (long)b * b;

        x = 0;
        y = b;
        d = 4 * bb + aa * (-b * 4 + 1);
        while (4 * bb * (x + 1) < aa * (4 * y - 2)) {
            if (flag) {
                scanline(ctxt, y2 + x, x1 - y, x2 + y);
                if (x) scanline(ctxt, y1 - x, x1 - y, x2 + y);
            }
            if (d < 0) {
                d += 4 * bb * (2 * x + 3);
                x++;
            } else {
                if (!flag) {
                    scanline(ctxt, y2 + y, x1 - x, x2 + x);
                    scanline(ctxt, y1 - y, x1 - x, x2 + x);
                }
                d += 4 * bb * (2 * x + 3) + 4 * aa * (-2 * y + 2);
                x++;
                y--;
            }
        }

        y = 0;
        x = a;
        d = 4 * aa + bb * (-a * 4 + 1);
        while (4 * bb * (x + 1) > aa * (4 * y - 2)) {
            if (!flag) {
                scanline(ctxt, y2 + y, x1 - x, x2 + x);
                if (y) scanline(ctxt, y1 - y, x1 - x, x2 + x);
            }
            if (d < 0) {
                d += 4 * aa * (2 * y + 3);
                y++;
            } else {
                if (flag) {
                    scanline(ctxt, y2 + x, x1 - y, x2 + y);
                    scanline(ctxt, y1 - x, x1 - y, x2 + y);
                }
                d += 4 * aa * (2 * y + 3) + 4 * bb * (-2 * x + 2);
                y++;
                x--;
            }
        }
        /* - draw filled ellipse */
    } else {
        /* + draw rectangle */
        line(ctxt, x1, y1 - b, x2, y1 - b);
        line(ctxt, x1, y2 + b, x2, y2 + b);
        line(ctxt, x1 - a, y1, x1 - a, y2);
        line(ctxt, x2 + a, y1, x2 + a, y2);
        /* - draw rectangle */

        /* + draw ellipse */
        if (a < b) {
            flag = TRUE;
            t = a;
            a = b;
            b = t;
        }
        aa = a * a;
        bb = b * b;

        x = 0;
        y = b;
        d = 4 * bb + aa * (-b * 4 + 1);
        while (4 * bb * (x + 1) < aa * (4 * y - 2)) {
            if (!flag) pixel(dstbmp, x2 + x, y2 + y, dc);
            else       pixel(dstbmp, x2 + y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x2 + x, y1 - y, dc);
            else       pixel(dstbmp, x2 + y, y1 - x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y2 + y, dc);
            else       pixel(dstbmp, x1 - y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y1 - y, dc);
            else       pixel(dstbmp, x1 - y, y1 - x, dc);
            if (d < 0) {
                d += 4 * bb * (2 * x + 3);
                x++;
            } else {
                d += 4 * bb * (2 * x + 3) + 4 * aa * (-2 * y + 2);
                x++;
                y--;
            }
        }

        y = 0;
        x = a;
        d = 4 * aa + bb * (-a * 4 + 1);
        while (4 * bb * (x + 1) > aa * (4 * y - 2)) {
            if (!flag) pixel(dstbmp, x2 + x, y2 + y, dc);
            else       pixel(dstbmp, x2 + y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x2 + x, y1 - y, dc);
            else       pixel(dstbmp, x2 + y, y1 - x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y2 + y, dc);
            else       pixel(dstbmp, x1 - y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y1 - y, dc);
            else       pixel(dstbmp, x1 - y, y1 - x, dc);
            if (d < 0) {
                d += 4 * aa * (2 * y + 3);
                y++;
            } else {
                d += 4 * aa * (2 * y + 3) + 4 * bb * (-2 * x + 2);
                y++;
                x--;
            }
        }
        /* - draw ellipse */
    }
}

void circle(void *ctxt, int xo, int yo, int r)
{
    DRAWCONTEXT *pc     = (DRAWCONTEXT*)ctxt;
    PFNPIXEL     pixel  = pc->pixel;
    BMP         *dstbmp = pc->dstbmp;
    DWORD        dc     = pc->drawcolor;
    int x, y, dx, dy, d;

    /* 绘制图形 */
    x  = 0;
    y  = r;
    dx = 3;
    dy = 2 - r * 2;
    d  = 1 - r;

    if (!(pc->fillparams.style & FS_NONE)) {
        /* 绘制水平方向直径扫描线 */
        scanline(ctxt, yo, xo-r, xo+r);

        /* 计算 x, y, dx, dy, d 等参数 */
        d += dx; dx += 2; x++;

        while (x <= y) {
            scanline(ctxt, yo-x, xo-y, xo+y);
            scanline(ctxt, yo+x, xo-y, xo+y);
            if (d < 0) {
                d  += dx;
                dx += 2;
                x++;
            } else {
                scanline(ctxt, yo-y, xo-x, xo+x);
                scanline(ctxt, yo+y, xo-x, xo+x);
                d  += dx + dy;
                dx += 2;
                dy += 2;
                x++;
                y--;
            }
        }
    } else {
        while (x <= y) {
            pixel(dstbmp, xo+x, yo+y, dc);
            pixel(dstbmp, xo-x, yo+y, dc);
            pixel(dstbmp, xo+x, yo-y, dc);
            pixel(dstbmp, xo-x, yo-y, dc);
            pixel(dstbmp, xo+y, yo+x, dc);
            pixel(dstbmp, xo-y, yo+x, dc);
            pixel(dstbmp, xo+y, yo-x, dc);
            pixel(dstbmp, xo-y, yo-x, dc);

            if (d < 0) {
                d  += dx;
                dx += 2;
                x++;
            } else {
                d  += dx + dy;
                dx += 2;
                dy += 2;
                x++;
                y--;
            }
        }
    }
}

void ellipse(void *ctxt, int xo, int yo, int a, int b)
{
    roundrect(ctxt, xo, yo, xo, yo, a, b);
}

BOOL arc(void *ctxt, int xo, int yo, int a, int b, int start, int end)
{
    DRAWCONTEXT *pc    = (DRAWCONTEXT*)ctxt;
    float        pd    = 1.0f;
    float        fstep = (float)pd / (a > b ? a : b);
    float        ft;
    int          ppsize= (int)(M_PI / 180 * (end - start) / fstep) + 2;
    int         *ppbuf = malloc(2 * ppsize * sizeof(int));
    int          ppcur = 0;
    BOOL         ret;

    if (!ppbuf) return FALSE;
    if (pc->drawflags & DF_ARC_CENTER_POINT) {
        ppbuf[ppcur++] = xo;
        ppbuf[ppcur++] = yo;
    }

    for (ft = M_PI / 180 * start; ft <= M_PI / 180 * end; ft += fstep) {
        ppbuf[ppcur++] = (int)(xo + a * cos(ft) + 0.5f);
        ppbuf[ppcur++] = (int)(yo + b * sin(ft) + 0.5f);
    }

    ret = polygon(ctxt, ppbuf, ppcur / 2);
    free(ppbuf);
    return ret;
}

typedef struct tagNODE {
    float curx;
    float dx;
    int   maxy;
    struct tagNODE *next;
} NODE, *PNODE;

BOOL polygon(void *ctxt, int *pp, int n)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    PNODE *NET = NULL;
    PNODE  AET = NULL;
    BOOL   ret = FALSE;
    BOOL   flag;
    NODE  *pnew;
    NODE  *ptemp;
    PNODE *ppnode;
    int    pgminy;
    int    pgmaxy;
    int    miny;
    int    i;

    /*++ 绘制非填充多边形 ++*/
    if (pc->fillparams.style & FS_NONE) {
        for (i=0; i<n-1; i++) {
            line(ctxt, pp[i*2], pp[i*2+1], pp[(i+1)*2], pp[(i+1)*2+1]);
        }
        if (pc->drawflags & DF_POLYGON_CLOSED) {
            line(ctxt, pp[0], pp[1], pp[(n-1)*2], pp[(n-1)*2+1]);
        }
        return TRUE;
    }
    /*-- 绘制非填充多边形 --*/

    /* 计算 pgminy 和 pgmaxy */
    pgminy = pp[0 * 2 + 1];
    pgmaxy = pp[0 * 2 + 1];
    for (i=0; i<n; i++) {
        if (pp[i * 2 + 1] < pgminy) pgminy = pp[i * 2 + 1];
        if (pp[i * 2 + 1] > pgmaxy) pgmaxy = pp[i * 2 + 1];
    }

    /* 在纵坐标方向对多边形进行平移变换 */
    for (i=0; i<n; i++) pp[i * 2 + 1] -= pgminy;

    /* 创建并初始化新边表 */
    NET = (PNODE*)malloc((pgmaxy - pgminy + 1) * sizeof(PNODE));
    if (!NET) goto error_handler;
    else memset(NET, 0, (pgmaxy - pgminy + 1) * sizeof(PNODE));

    /* 把多边形的各个边放入新边表中 */
    for (i=0; i<n; i++) {
        if (pp[i * 2 + 1] == pp[((i + 1) % n) * 2 + 1]) continue;

        /* 创建新结点 */
        pnew = (NODE*)malloc(sizeof(NODE));
        if (!pnew) goto error_handler;

        /* 计算并填充新结点 */
        pnew->dx  = (float)(pp[i * 2 + 0] - pp[((i + 1) % n) * 2 + 0]);
        pnew->dx /= (float)(pp[i * 2 + 1] - pp[((i + 1) % n) * 2 + 1]);
        if (pp[i * 2 + 1] > pp[((i + 1) % n) * 2 + 1]) {
            miny = pp[((i + 1) % n) * 2 + 1];
            pnew->curx = (float)pp[((i + 1) % n) * 2 + 0];
            pnew->maxy = pp[i * 2 + 1];
        } else {
            miny = pp[i * 2 + 1];
            pnew->curx = (float)pp[i * 2 + 0];
            pnew->maxy = pp[((i + 1) % n) * 2 + 1];
        }

        /* 加入新边表中 */
        ppnode = &NET[miny];
        while (*ppnode != NULL && (*ppnode)->dx > pnew->dx) ppnode = &((*ppnode)->next);
        pnew->next = *ppnode;
        *ppnode = pnew;
    }

    /* 处理经过多边形的每一条扫描线 */
    for (i=0; i<=pgmaxy-pgminy; i++) {
        /* 删除活边表中无用结点 */
        ppnode = &AET;
        while (*ppnode != NULL) {
            if ((*ppnode)->maxy <= i) {
                ptemp   = *ppnode;
                *ppnode = ptemp->next;
                free(ptemp);
            } else ppnode = &((*ppnode)->next);
        }

        /* 把新边表中的边结点插入活边表中 */
        while (NET[i] != NULL) {
            /* 计算插入位置 */
            ppnode = &AET;
            while (*ppnode != NULL && NET[i]->curx > (*ppnode)->curx) ppnode = &((*ppnode)->next);

            /* 插入结点 */
            ptemp  = NET[i];
            NET[i] = ptemp->next;
            ptemp->next = *ppnode;
            *ppnode = ptemp;
        }

        /* 绘制多边形填充的扫描线 */
        flag = TRUE;
        for (ptemp = AET; ptemp != NULL && ptemp->next != NULL; ptemp = ptemp->next) {
            if (flag) {
                scanline(ctxt, i + pgminy, (int)ptemp->curx, (int)ptemp->next->curx);
            }
            flag = ! flag;
        }

        /* 更新活边表中的结点 */
        ptemp = AET;
        while (ptemp != NULL) {
            ptemp->curx += ptemp->dx;
            ptemp = ptemp ->next;
        }
    }
    ret = TRUE;

error_handler:
    /* 释放内存 */
    for (i=0; i<=pgmaxy-pgminy; i++) {
        while (NET[i]) {
            ptemp  = NET[i];
            NET[i] = ptemp->next;
            free(ptemp);
        }
    }
    while (AET) {
        ptemp = AET;
        AET   = ptemp->next;
        free(ptemp);
    }
    free(NET);

    /* 在纵坐标方向对多边形进行平移变换 */
    for (i=0; i<n; i++) pp[i * 2 + 1] += pgminy;
    return ret;
}

BOOL floodfill(void *ctxt, int x, int y)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    BMP         *pb = pc->dstbmp;
    DWORD bkcolor;
    BOOL  flag;
    BOOL  done;
    int   rx, lx, tx, ty;
    int  *stack = malloc(DRAW2D_STACK_SIZE * sizeof(int));
    int   top   = 0;
    BOOL  ret   = FALSE;

    /* check */
    if (!stack) return FALSE;

    /* get background color */
    bkcolor = getpixel(ctxt, x, y);

    /* push seed pixel to stack */
    if (top >= DRAW2D_STACK_SIZE) goto error_handler;
    stack[top++] = x;
    if (top >= DRAW2D_STACK_SIZE) goto error_handler;
    stack[top++] = y;

    while (top > 0) {
        /* pop up a pixel from stack */
        ty = stack[--top];
        tx = stack[--top];

        /* find the right pixel */
        x  = tx;
        y  = ty;
        while (1) {
            if (getpixel(ctxt, x, y) != bkcolor || x >= pb->width) break;
            x++;
        }
        rx = x - 1;

        /* find the left pixel */
        x  = tx - 1;
        y  = ty;
        while (1) {
            if (getpixel(ctxt, x, y) != bkcolor || x < 0) break;
            x--;
        }
        lx = x + 1;

        /* fast scan line fill */
        scanline(ctxt, y, lx, rx);

        /* done flag used for recheck scan line*/
        done = FALSE;

check_scan_line:
        /* check upper and lower scanline */
        if (done) y = ty - 1;
        else y = ty + 1;
        if (y >= 0 && y < pb->height) {
            flag = FALSE;
            for (x=lx; x<=rx; x++) {
                if (!flag && getpixel(ctxt, x, y) == bkcolor) flag = TRUE;
                else if (flag && getpixel(ctxt, x, y) != bkcolor) {
                    /* push this pixel (x - 1, y) to stack */
                    if (top >= DRAW2D_STACK_SIZE) goto error_handler;
                    stack[top++] = x - 1;
                    if (top >= DRAW2D_STACK_SIZE) goto error_handler;
                    stack[top++] = y;
                    flag = FALSE;
                }
            }
            if (flag) {
                /* push this pixel (x - 1, y) to stack */
                if (top >= DRAW2D_STACK_SIZE) goto error_handler;
                stack[top++] = x - 1;
                if (top >= DRAW2D_STACK_SIZE) goto error_handler;
                stack[top++] = y;
            }
        }

        if (!done) {
            done = TRUE;
            goto check_scan_line;
        }
    }
    ret = TRUE;

error_handler:
    if (stack) free(stack);
    return ret;
}

BOOL bezier(void *ctxt, int *pp, int degree, int pd)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    int *oldpoint;
    int *newpoint;
    int *stack = malloc(DRAW2D_STACK_SIZE * sizeof(int));
    int  top   = 0;
    int  i, j;
    BOOL ret = FALSE;

    /* 参数检查 */
    if (!stack || degree <= 0 || pd <= 0) {
        goto error_handler;
    }

    /* init stack */
    stack = malloc(DRAW2D_STACK_SIZE * sizeof(int));
    top   = 0;
    degree--;

    /* draw two end points of poly-bezier */
    putpixel(ctxt, pp[0 * 2 + 0]     , pp[0 * 2 + 1]     , pc->drawcolor);
    putpixel(ctxt, pp[degree * 2 + 0], pp[degree * 2 + 1], pc->drawcolor);

    /* push the first polygon to draw2dstack */
    for (i=0; i<=degree; i++) {
        if (top >= DRAW2D_STACK_SIZE) goto error_handler;
        stack[top++] = pp[i * 2 + 0] << 4;
        if (top >= DRAW2D_STACK_SIZE) goto error_handler;
        stack[top++] = pp[i * 2 + 1] << 4;
    }

    while (top > 0) {
        newpoint = &(stack[top]);
        oldpoint = newpoint - (degree + 1) * 2;
        if (abs(oldpoint[0 * 2 + 0] - oldpoint[degree * 2 + 0]) +
            abs(oldpoint[0 * 2 + 1] - oldpoint[degree * 2 + 1]) <= (pd << 4))
        {
            top -= (degree + 1) * 2;  /* pop up the old polygon from draw2dstack */
        } else {
            /* push new polygon to draw2dstack */
            top += (degree + 1) * 2;
            if (top >= DRAW2D_STACK_SIZE) goto error_handler;

            newpoint[0 * 2 + 0] = oldpoint[0 * 2 + 0];
            newpoint[0 * 2 + 1] = oldpoint[0 * 2 + 1];
            for (i=1; i<=degree; i++) {
                for (j=0; j<=degree-i; j++) {
                    oldpoint[j * 2 + 0] = (oldpoint[j * 2 + 0] + oldpoint[(j + 1) * 2 + 0]) / 2;
                    oldpoint[j * 2 + 1] = (oldpoint[j * 2 + 1] + oldpoint[(j + 1) * 2 + 1]) / 2;
                }
                newpoint[i * 2 + 0] = oldpoint[0 * 2 + 0];
                newpoint[i * 2 + 1] = oldpoint[0 * 2 + 1];
            }
            putpixel(ctxt, oldpoint[0 * 2 + 0] >> 4, oldpoint[0 * 2 + 1] >> 4, pc->drawcolor);
        }
    }
    ret = TRUE;

error_handler:
    if (stack) free(stack);
    return ret;
}

#else
/* 包含头文件 */
#include "win.h"
#include "draw2d.h"

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    void *context;
    BMP   me = {0};
    int pp[] = {12, 32, 233, 333, 233, 333, 500, 400, 570, 123};
    DWORD pattern[32] = { 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
                          0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
                          0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
                          0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555 };

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 32;
    createbmp(&SCREEN);
    loadbmp(&me, "res\\boy8888.bmp", NULL);

    context = draw2d_init(&SCREEN);
    paint_begin(context);

    setfillstyle(context, FS_ALPHA);
    setfillcolor(context, RGB888(0, 0, 255));
    setdrawcolor(context, RGB888(255, 255, 255));
    setdrawalpha(context, 180);
    setdrawflags(context, DF_POLYGON_CLOSED | DF_ARC_CENTER_POINT);
    circle(context, 123, 168, 100);
    rectangle(context, 162, 69, 430, 203);
    roundrect(context, 122, 169, 330, 303, 60, 70);
    ellipse(context, 480, 320, 40, 30);
    arc(context, 510, 100, 60, 60, 36, 36 + 300);

    setfillstyle(context, FS_NONE);
    line(context, 80, 121, 582, 320);
    circle(context, 123, 168, 100);
    rectangle(context, 162, 69, 430, 203);
    roundrect(context, 122, 169, 330, 303, 60, 70);
    ellipse(context, 480, 320, 40, 30);
    arc(context, 510, 100, 60, 60, 36, 36 + 300);

    setfillstyle(context, FS_BMP_COPY);
    setfillbmp(context, &me);
    circle(context, 320, 240, 100);
    floodfill(context, 100, 100);

/*  setfillstyle(context, FS_ALPHA); */
    setfillcolor(context, RGB888(0, 255, 0));
    setfillstyle(context, FS_PATTERN);
    setfillpattern(context, pattern);
    polygon(context, pp, sizeof(pp) / sizeof(int) / 2);
    bezier (context, pp, sizeof(pp) / sizeof(int) / 2, 1);

    paint_done(context);
    draw2d_free(context);

    RGE_MSG_LOOP();
    destroybmp(&me);
    destroybmp(&SCREEN);
    return 0;
}
#endif

