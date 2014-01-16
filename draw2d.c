#ifndef _TEST_

/* 包含头文件 */
#include <math.h>
#include "draw2d.h"
#include "fnps/fnps.h"

static DWORD _getfillcolor(DRAWCONTEXT *pc)
{
    switch (pc->fillstyle)
    {
    case FILL_STYLE_BMP_COPY:
    case FILL_STYLE_BMP_MASK:
    case FILL_STYLE_PALMAP_BMP:
    case FILL_STYLE_PALMAP_SOLID:
        return (DWORD)pc;

    case FILL_STYLE_NONE:
    case FILL_STYLE_SOLID:
        return pc->fillcolor;

    default:
        return 0;
    }
}

void* paint_begin(BMP *pb)
{
    DRAWCONTEXT *pc;

    // invalid pb
    if (!pb) return NULL;

    // allocate context
    pc = (DRAWCONTEXT*)malloc(sizeof(DRAWCONTEXT));
    if (!pc) return NULL;

    // init context
    memset(pc, 0, sizeof(DRAWCONTEXT));
    pc->dstbmp               = pb;
    pc->drawcolor            = RGB(255, 255, 255);
    pc->linestyle            = LINE_STYLE_SOLID;
    pc->fillcolor            = RGB(255, 255, 255);
    pc->fillstyle            = FILL_STYLE_SOLID;
    pc->maskc                = (DWORD)-1;
    pc->textcolor            = RGB(255, 255, 255);
    pc->pixelsolid           = TABFN_PIXEL_SOLID          [pb->cdepth / 8];
    pc->pixelalpha           = TABFN_PIXEL_ALPHA          [pb->cdepth / 8];
    pc->scanlinesolid        = TABFN_SCANLINE_SOLID       [pb->cdepth / 8];
    pc->scanlinealpha        = TABFN_SCANLINE_ALPHA       [pb->cdepth / 8];
    pc->scanlinebmpfast      = TABFN_SCANLINEBMP_FAST     [pb->cdepth / 8];
    pc->scanlinebmpconvert   = TABFN_SCANLINEBMP_CONVERT  [pb->cdepth / 8];
    pc->scanlinebmpalpha     = TABFN_SCANLINEBMP_ALPHA    [pb->cdepth / 8];
    pc->scanlinebmpmaskalpha = TABFN_SCANLINEBMP_MASKALPHA[pb->cdepth / 8];
    pc->scanlinepalmapsolid  = PFN_SCANLINE_PALMAP_COLOR;
    pc->scanlinepalmapbmp    = PFN_SCANLINE_PALMAP_BMP;

    // pixel & scanline
    pc->pixel                = pc->pixelsolid;
    pc->scanline             = pc->scanlinesolid;

    // allocate ppbuf
    pc->ppbuf = malloc(MAX_PPBUF_SIZE * sizeof(int));

    // lock dest bmp
    lockbmp(pc->dstbmp);

    return pc;
}

void paint_end(void *ctxt)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    // invalid ctxt
    if (!ctxt) return;

    // free ppbuf
    free(pc->ppbuf);

    // unlock bmp
    unlockbmp(pc->dstbmp);

    // release context
    if (pc) free(pc);
}

DWORD convertcolor(int cdepth, DWORD color)
{
    int r, g, b;

    r = (color >> 16) & 0xff;
    g = (color >> 8 ) & 0xff;
    b = (color >> 0 ) & 0xff;

    switch (cdepth)
    {
    case 8 : return RGB332(r, g, b);
    case 16: return RGB565(r, g, b);
    case 24: return RGB888(r, g, b);
    case 32: return RGB888(r, g, b);
    default: return (DWORD)-1;
    }
}

void setdrawalpha(void *ctxt, int alpha)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    pc->alpha = alpha;
    if (alpha == 0) pc->pixel = pc->pixelsolid;
    else            pc->pixel = pc->pixelalpha;

    setdrawcolor(ctxt, pc->drawcolor);
    setfillcolor(ctxt, pc->fillcolor);
    setfillstyle(ctxt, pc->fillstyle);
}

void setdrawcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->drawcolor  = convertcolor(pc->dstbmp->cdepth, color);
    pc->drawcolor |= ((DWORD)pc->alpha << 24);
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

void setlinewidth(void *ctxt, DWORD width)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->linewidth = width;
}

void setlinestyle(void *ctxt, DWORD style)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->linestyle = style;
}

void setfillcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    pc->fillcolor  = convertcolor(pc->dstbmp->cdepth, color);
    pc->fillcolor |= ((DWORD)pc->alpha << 24);
}

void setfillbmp(void *ctxt, BMP *bmp)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillbmp = bmp;
    setfillstyle(ctxt, pc->fillstyle);
}

void setfillorgxy(void *ctxt, int x, int y)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->fillorgx = x;
    pc->fillorgy = y;
}

void setfillstyle(void *ctxt, DWORD style)
{
    DRAWCONTEXT *pc    = (DRAWCONTEXT*)ctxt;
    BMP         *dstpb = (BMP*)pc->dstbmp;
    BMP         *srcpb = (BMP*)pc->fillbmp;

    if (pc->alpha == 0) {
        switch (style)
        {
        case FILL_STYLE_SOLID:
            pc->scanline = pc->scanlinesolid;
            break;
        case FILL_STYLE_BMP_COPY:
            if (srcpb && dstpb->cdepth == srcpb->cdepth) {
                pc->scanline = pc->scanlinebmpfast;
            }
            else {
                pc->scanline = pc->scanlinebmpconvert;
            }
            break;
        }
    }
    else {
        switch (style)
        {
        case FILL_STYLE_SOLID:
            pc->scanline = pc->scanlinealpha;
            break;
        case FILL_STYLE_BMP_COPY:
            pc->scanline  = pc->scanlinebmpalpha;
            break;
        }
    }

    switch (style)
    {
    case FILL_STYLE_BMP_MASK:
        pc->scanline = pc->scanlinebmpmaskalpha;
        break;
    case FILL_STYLE_PALMAP_SOLID:
        pc->scanline = pc->scanlinepalmapsolid;
        break;
    case FILL_STYLE_PALMAP_BMP:
        pc->scanline = pc->scanlinepalmapbmp;
        break;
    }

    pc->fillstyle = style;
}

void setmaskcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->maskc = color; // no need to convert
}

void setpalmaptab(void *ctxt, BYTE *palmap)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    pc->palmap = palmap;
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
    switch (dstbmp->cdepth)
    {
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

    if (dy < dx)
    {
        if (x1 > x2)
        {
            x  = x1;
            x1 = x2;
            x2 = x;
            y  = y1;
            y1 = y2;
            y2 = y;
        }

        y = y1;
        x = x1;
        while (x <= x2)
        {
            pixel(dstbmp, x, y, dc);

            e += dy * 2;
            if (e >= 0)
            {
                if (y1 < y2) y++;
                else y--;
                e -= dx * 2;
            }
            x++;
        }
    }
    else
    {
        if (y1 > y2)
        {
            x  = x1;
            x1 = x2;
            x2 = x;
            y  = y1;
            y1 = y2;
            y2 = y;
        }

        y = y1;
        x = x1;
        while (y <= y2)
        {
            pixel(dstbmp, x, y, dc);

            e += dx * 2;
            if (e > 0)
            {
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
    PFNSCANLINE  scanline = pc->scanline;
    BMP         *dstbmp   = pc->dstbmp;
    DWORD        fc       = _getfillcolor(pc);
    int          t;

    if (x1 > x2)
    {
        t  = x1;
        x1 = x2;
        x2 = t;
    }

    if (y1 > y2)
    {
        t  = y1;
        y1 = y2;
        y2 = t;
    }

    if (pc->fillstyle)
    {
        for (t= y1; t<=y2; t++) {
            scanline(dstbmp, t, x1, x2, fc);
        }
    }
    else
    {
        line(ctxt, x1, y1, x2, y1);
        line(ctxt, x1, y2, x2, y2);
        line(ctxt, x1, y1, x1, y2);
        line(ctxt, x2, y1, x2, y2);
    }
}

void roundrect(void *ctxt, int x1, int y1, int x2, int y2, int a, int b)
{
    DRAWCONTEXT *pc       = (DRAWCONTEXT*)ctxt;
    PFNPIXEL     pixel    = pc->pixel;
    PFNSCANLINE  scanline = pc->scanline;
    BMP         *dstbmp   = pc->dstbmp;
    DWORD        dc       = pc->drawcolor;
    DWORD        fc       = _getfillcolor(pc);
    int  x, y, aa, bb, d, t;
    BOOL flag = FALSE;

    if (x1 > x2)
    {
        t  = x1;
        x1 = x2;
        x2 = t;
    }

    if (y1 > y2)
    {
        t  = y1;
        y1 = y2;
        y2 = t;
    }

    if (pc->fillstyle)
    {
        /* + draw filled rectangle */
        for (y=y1; y<y2; y++) scanline(dstbmp, y, x1 - a, x2 + a, fc);
        /* - draw filled rectangle */

        /* + draw filled ellipse */
        if (a < b)
        {
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
        while (4 * bb * (x + 1) < aa * (4 * y - 2))
        {
            if (flag)
            {
                scanline(dstbmp, y2 + x, x1 - y, x2 + y, fc);
                if (x) scanline(dstbmp, y1 - x, x1 - y, x2 + y, fc);
            }
            if (d < 0)
            {
                d += 4 * bb * (2 * x + 3);
                x++;
            }
            else
            {
                if (!flag)
                {
                    scanline(dstbmp, y2 + y, x1 - x, x2 + x, fc);
                    scanline(dstbmp, y1 - y, x1 - x, x2 + x, fc);
                }
                d += 4 * bb * (2 * x + 3) + 4 * aa * (-2 * y + 2);
                x++;
                y--;
            }
        }

        y = 0;
        x = a;
        d = 4 * aa + bb * (-a * 4 + 1);
        while (4 * bb * (x + 1) > aa * (4 * y - 2))
        {
            if (!flag)
            {
                scanline(dstbmp, y2 + y, x1 - x, x2 + x, fc);
                if (y) scanline(dstbmp, y1 - y, x1 - x, x2 + x, fc);
            }
            if (d < 0)
            {
                d += 4 * aa * (2 * y + 3);
                y++;
            }
            else
            {
                if (flag)
                {
                    scanline(dstbmp, y2 + x, x1 - y, x2 + y, fc);
                    scanline(dstbmp, y1 - x, x1 - y, x2 + y, fc);
                }
                d += 4 * aa * (2 * y + 3) + 4 * bb * (-2 * x + 2);
                y++;
                x--;
            }
        }
        /* - draw filled ellipse */
    }
    else
    {
        /* + draw rectangle */
        line(ctxt, x1, y1 - b, x2, y1 - b);
        line(ctxt, x1, y2 + b, x2, y2 + b);
        line(ctxt, x1 - a, y1, x1 - a, y2);
        line(ctxt, x2 + a, y1, x2 + a, y2);
        /* - draw rectangle */

        /* + draw ellipse */
        if (a < b)
        {
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
        while (4 * bb * (x + 1) < aa * (4 * y - 2))
        {
            if (!flag) pixel(dstbmp, x2 + x, y2 + y, dc);
            else       pixel(dstbmp, x2 + y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x2 + x, y1 - y, dc);
            else       pixel(dstbmp, x2 + y, y1 - x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y2 + y, dc);
            else       pixel(dstbmp, x1 - y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y1 - y, dc);
            else       pixel(dstbmp, x1 - y, y1 - x, dc);
            if (d < 0)
            {
                d += 4 * bb * (2 * x + 3);
                x++;
            }
            else
            {
                d += 4 * bb * (2 * x + 3) + 4 * aa * (-2 * y + 2);
                x++;
                y--;
            }
        }

        y = 0;
        x = a;
        d = 4 * aa + bb * (-a * 4 + 1);
        while (4 * bb * (x + 1) > aa * (4 * y - 2))
        {
            if (!flag) pixel(dstbmp, x2 + x, y2 + y, dc);
            else       pixel(dstbmp, x2 + y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x2 + x, y1 - y, dc);
            else       pixel(dstbmp, x2 + y, y1 - x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y2 + y, dc);
            else       pixel(dstbmp, x1 - y, y2 + x, dc);
            if (!flag) pixel(dstbmp, x1 - x, y1 - y, dc);
            else       pixel(dstbmp, x1 - y, y1 - x, dc);
            if (d < 0)
            {
                d += 4 * aa * (2 * y + 3);
                y++;
            }
            else
            {
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
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    PFNSCANLINE  scanline = pc->scanline;
    PFNPIXEL     pixel    = pc->pixel;
    BMP         *dstbmp   = pc->dstbmp;
    DWORD        dc       = pc->drawcolor;
    DWORD        fc       = _getfillcolor(pc);
    int x, y, dx, dy, d;

    /* 绘制图形 */
    x  = 0;
    y  = r;
    dx = 3;
    dy = 2 - r * 2;
    d  = 1 - r;

    if (pc->fillstyle)
    {
        /* 绘制水平方向直径扫描线 */
        scanline(dstbmp, yo, xo-r, xo+r, fc);

        /* 计算 x, y, dx, dy, d 等参数 */
        d += dx; dx += 2; x++;

        while (x <= y)
        {
            scanline(dstbmp, yo-x, xo-y, xo+y, fc);
            scanline(dstbmp, yo+x, xo-y, xo+y, fc);
            if (d < 0)
            {
                d  += dx;
                dx += 2;
                x++;
            }
            else
            {
                scanline(dstbmp, yo-y, xo-x, xo+x, fc);
                scanline(dstbmp, yo+y, xo-x, xo+x, fc);
                d  += dx + dy;
                dx += 2;
                dy += 2;
                x++;
                y--;
            }
        }
    }
    else {
        while (x <= y)
        {
            pixel(dstbmp, xo+x, yo+y, dc);
            pixel(dstbmp, xo-x, yo+y, dc);
            pixel(dstbmp, xo+x, yo-y, dc);
            pixel(dstbmp, xo-x, yo-y, dc);
            pixel(dstbmp, xo+y, yo+x, dc);
            pixel(dstbmp, xo-y, yo+x, dc);
            pixel(dstbmp, xo+y, yo-x, dc);
            pixel(dstbmp, xo-y, yo-x, dc);

            if (d < 0)
            {
                d  += dx;
                dx += 2;
                x++;
            }
            else
            {
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

void arc(void *ctxt, int xo, int yo, int a, int b, int start, int end)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    float        pd = 2.0f;
    float        fstep;
    float        ft;

    // check ppbuf valid
    if (!pc->ppbuf) return;

    pc->ppcur = 0;
    if (pc->drawflags & DRAW_FLAG_ARC_CENTER_POINT) {
        pc->ppbuf[pc->ppcur++] = xo;
        pc->ppbuf[pc->ppcur++] = yo;
    }

    fstep = (float)pd / (a > b ? a : b);
    for (ft = M_PI / 180 * start; ft <= M_PI / 180 * end; ft += fstep)
    {
        pc->ppbuf[pc->ppcur++] = xo + a * cos(ft) + 0.5f;
        if (pc->ppcur == MAX_PPBUF_SIZE) break;
        pc->ppbuf[pc->ppcur++] = yo + b * sin(ft) + 0.5f;
        if (pc->ppcur == MAX_PPBUF_SIZE) break;
    }
    polygon(ctxt, pc->ppbuf, pc->ppcur / 2);
}

typedef struct tagNODE
{
    float curx;
    float dx;
    int   maxy;
    struct tagNODE *next;
} NODE, *PNODE;

void polygon(void *ctxt, int *pp, int n)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    PFNSCANLINE  scanline = pc->scanline;
    BMP         *dstbmp   = pc->dstbmp;
    DWORD        fc       = _getfillcolor(pc);

    PNODE *NET = NULL;
    PNODE  AET = NULL;
    BOOL   flag;
    NODE  *pnew;
    NODE  *ptemp;
    PNODE *ppnode;
    int    pgminy;
    int    pgmaxy;
    int    miny;
    int    i;

    //++ 绘制非填充多边形 ++//
    if (pc->fillstyle == 0) {
        for (i=0; i<n-1; i++) {
            line(ctxt, pp[i*2], pp[i*2+1], pp[(i+1)*2], pp[(i+1)*2+1]);
        }
        if (pc->drawflags & DRAW_FLAG_POLYGON_CLOSED) {
            line(ctxt, pp[0], pp[1], pp[(n-1)*2], pp[(n-1)*2+1]);
        }
        return;
    }
    //-- 绘制非填充多边形 --//

    /* 计算 pgminy 和 pgmaxy */
    pgminy = pp[0 * 2 + 1];
    pgmaxy = pp[0 * 2 + 1];
    for (i=0; i<n; i++)
    {
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
    for (i=0; i<n; i++)
    {
        if (pp[i * 2 + 1] == pp[((i + 1) % n) * 2 + 1]) continue;

        /* 创建新结点 */
        pnew = (NODE*)malloc(sizeof(NODE));
        if (!pnew) goto error_handler;

        /* 计算并填充新结点 */
        pnew->dx  = (float)(pp[i * 2 + 0] - pp[((i + 1) % n) * 2 + 0]);
        pnew->dx /= (float)(pp[i * 2 + 1] - pp[((i + 1) % n) * 2 + 1]);
        if (pp[i * 2 + 1] > pp[((i + 1) % n) * 2 + 1])
        {
            miny = pp[((i + 1) % n) * 2 + 1];
            pnew->curx = (float)pp[((i + 1) % n) * 2 + 0];
            pnew->maxy = pp[i * 2 + 1];
        }
        else
        {
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
    for (i=0; i<=pgmaxy-pgminy; i++)
    {
        /* 删除活边表中无用结点 */
        ppnode = &AET;
        while (*ppnode != NULL)
        {
            if ((*ppnode)->maxy <= i)
            {
                ptemp   = *ppnode;
                *ppnode = ptemp->next;
                free(ptemp);
            }
            else ppnode = &((*ppnode)->next);
        }

        /* 把新边表中的边结点插入活边表中 */
        while (NET[i] != NULL)
        {
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
        for (ptemp = AET; ptemp != NULL && ptemp->next != NULL; ptemp = ptemp->next)
        {
            if (flag) {
                scanline(dstbmp, i + pgminy, (int)ptemp->curx, (int)ptemp->next->curx, fc);
            }
            flag = ! flag;
        }

        /* 更新活边表中的结点 */
        ptemp = AET;
        while (ptemp != NULL)
        {
            ptemp->curx += ptemp->dx;
            ptemp = ptemp ->next;
        }
    }

error_handler:
    /* 释放内存 */
    for (i=0; i<=pgmaxy-pgminy; i++)
    {
        while (NET[i])
        {
            ptemp  = NET[i];
            NET[i] = ptemp->next;
            free(ptemp);
        }
    }
    while (AET)
    {
        ptemp = AET;
        AET   = ptemp->next;
        free(ptemp);
    }
    free(NET);

    /* 在纵坐标方向对多边形进行平移变换 */
    for (i=0; i<n; i++) pp[i * 2 + 1] += pgminy;
}

void floodfill(void *ctxt, int x, int y)
{
    // todo..
}

void bezier(void *ctxt, int *pp, int degree)
{
    // todo..
}

#else
/* 包含头文件 */
#include "win.h"
#include "draw2d.h"
#include "bmpfile.h"

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    void *context;
    BMP   me = {0};

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 32;
    createbmp(&SCREEN);

    loadbmp(&me, "res\\me.bmp", NULL);

    context = paint_begin(&SCREEN);
    setfillstyle(context, FILL_STYLE_SOLID);
    setfillcolor(context, RGB(0, 0, 255));
    setdrawalpha(context, 180);
    circle(context, 123, 168, 100);
    rectangle(context, 162, 69, 430, 203);
    roundrect(context, 122, 169, 330, 303, 60, 70);
    ellipse(context, 480, 320, 40, 30);
    arc(context, 510, 100, 60, 60, 36, 36 + 300);

    setfillstyle(context, FILL_STYLE_NONE);
    setdrawcolor(context, RGB(255, 255, 255));
    setdrawalpha(context, 255);
    line(context, 80, 121, 582, 320);
    circle(context, 123, 168, 100);
    rectangle(context, 162, 69, 430, 203);
    roundrect(context, 122, 169, 330, 303, 60, 70);
    ellipse(context, 480, 320, 40, 30);
    setdrawflags(context, DRAW_FLAG_POLYGON_CLOSED);
    arc(context, 510, 100, 60, 60, 36, 36 + 300);

    setfillstyle(context, FILL_STYLE_BMP_COPY);
    setfillbmp(context, &me);
    circle(context, 320, 240, 80);

    paint_end(context);

    RGE_MSG_LOOP();
    destroybmp(&me);
    destroybmp(&SCREEN);
    return 0;
}
#endif

