#ifndef _TEST_

/* 包含头文件 */
#include "font.h"
#include "fnps/fnps.h"

/* 外部函数声明 */
extern DWORD convertcolor(int cdepth, DWORD color);

/* 预定义字体 */
FONT FONT12 = {12, 12,  6, 12, "font/hzk1212.dat", "font/asc0612.dat"};
FONT FONT16 = {16, 16,  8, 14, "font/hzk1616.dat", "font/asc0814.dat"};
FONT FONT24 = {24, 24, 12, 21, "font/hzk2424.dat", "font/asc1221.dat"};

/* 内部函数实现 */
/* draw raster font */
static void drawrasterfont(void *ctxt, int x, int y, BYTE *data, int w, int h)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    PFNPIXEL pixel;
    DWORD    color;
    int      i, j;

    // invalid ctxt
    if (!ctxt || !data) return;

    if (pc->alpha) pixel = pc->pixelalpha;
    else           pixel = pc->pixelsolid;
    color = pc->textcolor;

    /* w 对齐到 8 */
    w = (w + 7) / 8 * 8;

    /* 绘制字模 */
    for (; h>0; h--,y++,x-=w)
    {
        for (i=0; i<w/8; i++,data++)
        {
            for (j=7; j>=0; j--,x++)
            {
                if (*data & (1 << j))
                {
                    if (  x + j >= pc->dstbmp->clipper.left
                       && x + j <= pc->dstbmp->clipper.right
                       && y + i >= pc->dstbmp->clipper.top
                       && y + i <= pc->dstbmp->clipper.bottom)
                    {
                        pixel(pc->dstbmp, x, y, color);
                    }
                }
            }
        }
    }
}

/* 输出一个汉字的函数 */
static void _draw_hz(DRAWCONTEXT *pc, char hz[2], int x, int y)
{
    FONT *pf = (FONT*)pc->textfont;
    int  ch0 = (BYTE)hz[0] - 0xA0;
    int  ch1 = (BYTE)hz[1] - 0xA0;

    long offset = pf->_hzk_buf_size * ((ch0 - 1L) * 94 + (ch1 - 1L));
    fseek(pf->_hzk_fp, offset, SEEK_SET);
    fread(pf->_font_buf, pf->_hzk_buf_size, 1, pf->_hzk_fp);
    drawrasterfont(pc, x, y, pf->_font_buf, pf->hzk_width, pf->hzk_height);
}

/* 输出一个英文字符的函数 */
static void _draw_asc(DRAWCONTEXT *pc, char asc, int x, int y)
{
    FONT *pf     = (FONT*)pc->textfont;
    long  offset = (long)asc * pf->_asc_buf_size;

    fseek(pf->_asc_fp, offset, SEEK_SET);
    fread(pf->_font_buf, pf->_asc_buf_size, 1, pf->_asc_fp);
    drawrasterfont(pc, x, y, pf->_font_buf, pf->asc_width, pf->asc_height);
}

BOOL loadfont(FONT *pf)
{
    int bufsize;

    if (!pf) return FALSE;

    /* 计算字体所需要的缓冲区大小 */
    pf->_hzk_buf_size = (pf->hzk_width + 7) / 8 * pf->hzk_height;
    pf->_asc_buf_size = (pf->asc_width + 7) / 8 * pf->asc_height;
    bufsize = max(pf->_hzk_buf_size, pf->_asc_buf_size);

    /* 打开中文字库文件 */
    pf->_hzk_fp = fopen(pf->hzk_file, "rb");
    if (!pf->_hzk_fp) return FALSE;

    /* 打开英文字库文件 */
    pf->_asc_fp = fopen(pf->asc_file, "rb");
    if (!pf->_asc_fp) return FALSE;

    /* 申请字体所需要的缓冲区 */
    pf->_font_buf = (BYTE*)malloc(bufsize);
    if (pf->_font_buf) return FALSE;

    return TRUE;
}

void freefont(FONT *pf)
{
    if (!pf) return;

    if (pf->_hzk_fp)
    {
        fclose(pf->_hzk_fp);
        pf->_hzk_fp = NULL;
    }

    if (pf->_asc_fp)
    {
        fclose(pf->_asc_fp);
        pf->_asc_fp = NULL;
    }

    if (pf->_font_buf)
    {
        free(pf->_font_buf);
        pf->_font_buf = NULL;
    }
}

void settextfont(void *ctxt, FONT *font)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    // invalid ctxt & brush
    if (!ctxt) return;

    pc->textfont = font;
}

void settextcolor(void *ctxt, DWORD color)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;

    // invalid ctxt & brush
    if (!ctxt) return;

    pc->textcolor = convertcolor(pc->dstbmp->cdepth, color);
}

/* TEXTBOX 相关函数 */
void resettextbox(TEXTBOX *ptb)
{
    if (!ptb) return;
    ptb->_cur_xpos = ptb->xpos;
    ptb->_cur_ypos = ptb->ypos;
}

int magictextbox(void *ctxt, TEXTBOX *ptb)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    FONT        *pf = (FONT*)pc->textfont;
    int fhw, fhh, faw, fah;

    if (!ctxt || !ptb) return 1;

    fhw = pf->hzk_width;
    fhh = pf->hzk_height;
    faw = pf->asc_width;
    fah = pf->asc_height;

    /* 对不同的文字进行分别处理 */
    switch (ptb->text[ptb->cursor])
    {
    case '\0':  /* 字符串结束符 */
        ptb->cursor = 0;
        return 1; /* text box end */

    case '\n':  /* 换行符 */
        ptb->_cur_xpos  = ptb->xpos;
        ptb->_cur_ypos += fhh + ptb->rowspace;
        ptb->cursor++;
        break;

    default:
        if (ptb->text[ptb->cursor] & 0x80)
        {   /* 中文字符 */
            if (!ptb->calrect) {
               _draw_hz(pc, &(ptb->text[ptb->cursor]),
                    ptb->_cur_xpos, ptb->_cur_ypos);
            }
            ptb->_cur_xpos += fhw;
            ptb->cursor++;
            ptb->cursor++;
        }
        else
        {   /* 英文字符 */
            if (!ptb->calrect) {
                _draw_asc(pc, ptb->text[ptb->cursor],
                    ptb->_cur_xpos, ptb->_cur_ypos + fhh - fah);
            }
            ptb->_cur_xpos += faw;
            ptb->cursor++;
        }
        ptb->_cur_xpos += ptb->colspace;
        break;
    }

    if (  ptb->_cur_xpos + fhw > ptb->xpos + ptb->width
       && ptb->text[ptb->cursor] != '\n')
    {
        ptb->_cur_xpos  = ptb->xpos;
        ptb->_cur_ypos += fhh + ptb->rowspace;
    }

    if (ptb->_cur_ypos + fhh > ptb->ypos + ptb->height)
    {
        ptb->_cur_xpos = ptb->xpos;
        ptb->_cur_ypos = ptb->ypos;
        return -1; /* text box full */
    }

    return 0;
}

int outtextbox(void *ctxt, TEXTBOX *ptb)
{
    int retv;
    do { retv = magictextbox(ctxt, ptb); } while (!retv);
    return retv;
}

/* 文本输出函数 */
int printtext(void *ctxt, char *txt)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    TEXTBOX      tb = {0};

    if (!ctxt || !txt) return 1;

    tb.width     = pc->dstbmp->width;
    tb.height    = pc->dstbmp->height;
    tb.rowspace  = 1;
    tb.colspace  = 1;
    tb.text      = txt;
    tb.cursor    = 0;
    return outtextbox(ctxt, &tb);
}

int outtextxy(void *ctxt, char *txt, int x, int y)
{
    DRAWCONTEXT *pc = (DRAWCONTEXT*)ctxt;
    TEXTBOX      tb = {0};

    if (!ctxt || !txt) return 1;

    tb.width     = pc->dstbmp->width;
    tb.height    = pc->dstbmp->height;
    tb.rowspace  = 1;
    tb.colspace  = 1;
    tb.text      = txt;
    tb.cursor    = 0;
    tb._cur_xpos = x;
    tb._cur_ypos = y;
    return outtextbox(ctxt, &tb);
}


#else
/* 包含头文件 */
#include "win.h"
#include "font.h"

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    void *context;

    loadfont(&FONT16);

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 16;
    createbmp(&SCREEN);

    context = paint_begin(&SCREEN);
    settextfont (context, &FONT16);
    settextcolor(context, RGB(0, 255, 0));
    printtext(context, "Hello RGE! 汉字");
    paint_end(context);

    RGE_MSG_LOOP();
    destroybmp(&SCREEN);
    freefont(&FONT16);
    return 0;
}
#endif

