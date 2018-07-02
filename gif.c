#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "lzw.h"
#include "gif.h"

/* ++ gif fio context & driver ++ */
typedef struct {
    BYTE    bitbuf;
    int     bitflag;
    void   *basefp;
    FIO    *basefio;
    int     counter;
} GIFFIOCTXT;

static int _gif_fio_getc(void *fp)
{
    GIFFIOCTXT *context = (GIFFIOCTXT*)fp;
    if (context->counter == 0) {
        context->counter = (context->basefio->getc)(context->basefp);
        if (context->counter == EOF || context->counter == 0) return EOF;
    }
    context->counter--;
    return (context->basefio->getc)(context->basefp);
}

static int _gif_fio_putc(int c, void *fp)
{
    GIFFIOCTXT *context = (GIFFIOCTXT*)fp;
    if (context->counter == 0) {
        if (EOF == (context->basefio->putc)(255, context->basefp)) return EOF;
    }
    if (++context->counter == 255) context->counter = 0;
    return (context->basefio->putc)(c, context->basefp);
}

static int _gif_fio_flush(void *fp)
{
    GIFFIOCTXT *context = (GIFFIOCTXT*)fp;
    if (context->counter != 0) {
        if (EOF == (context->basefio->seek)(context->basefp, -context->counter-1, SEEK_CUR)) return EOF;
        if (EOF == (context->basefio->putc)(context->counter, context->basefp)) return EOF;
        if (EOF == (context->basefio->seek)(context->basefp,  context->counter,   SEEK_CUR)) return EOF;
    }
    if (EOF == (context->basefio->putc)(0, context->basefp)) return EOF;
    return (context->basefio->flush)(context->basefp);
}

static void gif_fio_init(GIFFIOCTXT *ctxt, FIO *fio, void *basefp, FIO *basefio) {
    ctxt->bitbuf  = 0;
    ctxt->bitflag = 0;
    ctxt->basefp  = basefp;
    ctxt->basefio = basefio;
    ctxt->counter = 0;
    memcpy(fio, basefio, sizeof(FIO));
    fio->getc  = _gif_fio_getc;
    fio->putc  = _gif_fio_putc;
    fio->flush = _gif_fio_flush;
}

static void gif_lzw_decode(LZWCODEC *lzw, int codesize, BYTE *buf, unsigned bufsize, void *fp, FIO *fio)
{
    void      *memfp;
    GIFFIOCTXT giffp;
    FIO        gifio;

    memfp = MEM_FIO.open((const char*)buf, (const char*)bufsize);
    gif_fio_init(&giffp, &gifio, fp, fio);

    lzw->LZW_ROOT_SIZE     = codesize;
    lzw->LZW_CODE_SIZE_OUT = 8;
    lzw->LZW_CODE_SIZE_MIN = codesize + 1;
    lzw->LZW_ROOT_NUM   =  1 << codesize;
    lzw->LZW_CLEAR_CODE =  1 << codesize;
    lzw->LZW_END_CODE   = (1 << codesize) + 1;
    lzwdecode(lzw, memfp, &MEM_FIO, &giffp, &gifio);
    MEM_FIO.close(memfp);
}

static void gif_lzw_encode(LZWCODEC *lzw, int codesize, void *fp, FIO *fio, BYTE *buf, unsigned bufsize)
{
    void      *memfp;
    GIFFIOCTXT giffp;
    FIO        gifio;

    memfp = MEM_FIO.open((const char*)buf, (const char*)bufsize);
    gif_fio_init(&giffp, &gifio, fp, fio);

    lzw->LZW_ROOT_SIZE     = codesize;
    lzw->LZW_CODE_SIZE_OUT = 8;
    lzw->LZW_CODE_SIZE_MIN = codesize + 1;
    lzw->LZW_ROOT_NUM   =  1 << codesize;
    lzw->LZW_CLEAR_CODE =  1 << codesize;
    lzw->LZW_END_CODE   = (1 << codesize) + 1;
    lzwencode(lzw, &giffp, &gifio, memfp, &MEM_FIO);
    gifio.flush(&giffp);
    MEM_FIO.close(memfp);
}
/* -- gif fio context & driver -- */


/* 常量定义 */
#define GIF_IMG_SPRT  ','
#define GIF_EXT_INTR  '!'
#define GIF_TEXT_EXT  0x01
#define GIF_CTRL_EXT  0xf9
#define GIF_APP_EXT   0xff
#define GIF_CMT_EXT   0xfe

/* 类型定义 */
/* + GIF file format structs */
/* GIF sub-block */
#pragma pack(1)
typedef struct {
    BYTE  block_size;
    BYTE *pblock_data;
} SUBBLOCK;

/* GIF file header */
typedef struct {
    BYTE signature[3];
    BYTE version[3];
} GIFHEADER;

/* GIF file logic screen descriptor */
typedef struct {
    WORD     logic_screen_width;
    WORD     logic_screen_height;
    unsigned global_color_table_size : 3;
    unsigned sort_flag               : 1;
    unsigned color_resolution        : 3;
    unsigned global_color_table_flag : 1;
    BYTE     background_color;
    BYTE     pixel_aspect_ratio;
} LOGICSCREENDESC;

/* GIF file image descriptor */
typedef struct {
    WORD     image_left_position;
    WORD     image_top_position;
    WORD     image_width;
    WORD     image_height;
    unsigned local_color_table_size : 3;
    unsigned reserved               : 2;
    unsigned sort_flag              : 1;
    unsigned interlace_flag         : 1;
    unsigned local_color_table_flag : 1;
} IMAGEDESCRIPTOR;

/* GIF file image data */
typedef struct {
    BYTE lzw_min_code_size;
    SUBBLOCK  *pimage_data;
} GIFIMAGEDATA;

/* GIF file graphic control extension */
typedef struct {
    unsigned transparent_color_flag : 1;
    unsigned user_input_flag        : 1;
    unsigned disposal_method        : 3;
    unsigned reserved               : 3;
    WORD     delay_time;
    BYTE     transparent_color;
} GRAPHCTRLEXT;

/* GIF file text extension */
typedef struct {
    WORD      text_grid_left_position;
    WORD      text_grid_top_position;
    WORD      text_grid_width;
    WORD      text_grid_height;
    BYTE      char_cell_width;
    BYTE      char_cell_height;
    BYTE      text_foreground_color;
    BYTE      text_background_color;
    SUBBLOCK *plain_text_data;
} PLAINTEXTEXT;

/* GIF file comment extenstion */
typedef struct {
    SUBBLOCK *pcomment_data;
} COMMENTEXT;

/* GIF file app extension */
typedef struct {
    BYTE      application_identifier[8];
    BYTE      authentication_code[3];
    SUBBLOCK *papplication_data;
} APPLICATIONEXT;

/* GIF file format */
typedef struct {
    GIFHEADER       gif_header;
    LOGICSCREENDESC logic_screen_desc;
    BYTE           *pglobal_color_table;
    BYTE far       *pdata;
    BYTE            trailer;
} GIFFILE;
#pragma pack()
/* - GIF file format structs */

/* GIF codec context */
typedef struct {
    int          width;          /* GIF 动画的宽度 */
    int          height;         /* GIF 动画的高度 */
    BYTE         bkcolor;        /* GIF 动画的背景色 */
    BYTE         gpaldata[1024]; /* GIF 动画的全局调色板 */
    int          gpalsize;       /* GIF 动画的全局调色板 */
    int          locpalflag;     /* 当前帧是否使用局部调色板 */
    void        *fp;             /* fp */
    FIO         *fio;            /* fio */
    LZWCODEC     lzwcodec;       /* LZW 编解码器 */
    long         dataoffset;     /* GIF 动画数据偏移地址 */
} GIFCONTEXT;

/* 内部函数实现 */
/* 反隔行扫描处理 */
static void gifdeinterlace(BMP *pb)
{
    int i;
    int srcaddr = 0;
    BMP bmp     = {0};
    bmp.width   = pb->width;
    bmp.height  = pb->height;
    bmp.cdepth  = pb->cdepth;
    if (!createbmp(&bmp)) return;
    bmp.stride  = pb->stride;

    for (i=0; i<bmp.height; i+=8) {
        memcpy(bmp.pdata + i * bmp.stride, pb->pdata + srcaddr, pb->stride);
        srcaddr += pb->stride;
    }

    for (i=4; i<bmp.height; i+=8) {
        memcpy(bmp.pdata + i * bmp.stride, pb->pdata + srcaddr, pb->stride);
        srcaddr += pb->stride;
    }

    for (i=2; i<bmp.height; i+=4) {
        memcpy(bmp.pdata + i * bmp.stride, pb->pdata + srcaddr, pb->stride);
        srcaddr += pb->stride;
    }

    for (i=1; i<bmp.height; i+=2) {
        memcpy(bmp.pdata + i * bmp.stride, pb->pdata + srcaddr, pb->stride);
        srcaddr += pb->stride;
    }

    memcpy(pb->pdata, bmp.pdata, bmp.height * bmp.stride);
    destroybmp(&bmp);
}


/* decode */
void* gifdecodeinit(void *fp, FIO *fio)
{
    GIFCONTEXT *context = NULL;
    BOOL        failed  = FALSE;

    GIFHEADER       gifheader;
    LOGICSCREENDESC logscrdesc;
    BYTE            palbuf[256*3];
    int             i;

    context = calloc(1, sizeof(GIFCONTEXT));
    if (!context) {
        failed = TRUE;
        goto done;
    }

    /* init fp and fio */
    context->fp  = fp;
    context->fio = fio;

    /* init lzw codec */
    context->lzwcodec.LZW_CODE_SIZE_MAX = 12;
    if (!initlzwcodec(&(context->lzwcodec))) {
        failed = TRUE;
        goto done;
    }

    /* read gif header */
    fio->read(&gifheader, sizeof(gifheader), 1, context->fp);

    /* check if it is valid gif file */
    if (  gifheader.signature[0] != 'G'
       || gifheader.signature[1] != 'I'
       || gifheader.signature[2] != 'F')
    {
        /* it is not gif file */
        log_printf("check gif signature failed !\r\n");
        return FALSE;
    }

    /* read gif logic screen descriptor */
    fio->read(&logscrdesc, sizeof(logscrdesc), 1, context->fp);

    /* init gif object */
    context->width   = logscrdesc.logic_screen_width;
    context->height  = logscrdesc.logic_screen_height;
    context->bkcolor = logscrdesc.background_color;

    /* load global color table */
    if (logscrdesc.global_color_table_flag == 1) {
        context->gpalsize = 1 << (logscrdesc.global_color_table_size + 1);
        fio->read(palbuf, context->gpalsize * 3, 1, context->fp);
        for (i=0; i<context->gpalsize; i++) {
            context->gpaldata[i * 4 + 2] = palbuf[i * 3 + 0];
            context->gpaldata[i * 4 + 1] = palbuf[i * 3 + 1];
            context->gpaldata[i * 4 + 0] = palbuf[i * 3 + 2];
        }
    }

    /* save the image data offset */
    context->dataoffset = fio->tell(context->fp);

done:
    if (failed) {
        log_printf("failed to init gif decoder !\r\n");
        gifdecodefree(context);
        return NULL;
    }
    return context;
}

void gifdecodefree(void *ctxt)
{
    GIFCONTEXT *context = (GIFCONTEXT*)ctxt;
    if (context) {
        /* close lzw codec */
        closelzwcodec(&(context->lzwcodec));

        /* free fp fo context */
        context->fio->close(context->fp);

        free(context);
    }
}

BOOL gifdecodeframe(void *ctxt, int *type, BMP *pb, int *xpos, int *ypos, GIF_CTRL *ctrl, GIF_TEXT *text)
{
    GIFCONTEXT     *context = (GIFCONTEXT*)ctxt;
    FIO            *fio     = context->fio;
    BYTE            curbyte;
    int             palsize;
    BYTE            codesize;
    IMAGEDESCRIPTOR imagedesc = {0};
    GRAPHCTRLEXT    gctrlext  = {0};
    PLAINTEXTEXT    gtextext  = {0};
    BYTE            pallocal [256*4];
    BYTE            palbuffer[256*3];
    int             i;

    *type   = GIF_FRAME_TYPE_UNKNOWN;
    curbyte = (fio->getc)(context->fp);
    switch (curbyte) {
    case GIF_IMG_SPRT:
        /* set type */
        *type = GIF_FRAME_TYPE_IMAGE;

        /* read image descriptor */
        fio->read(&imagedesc, sizeof(IMAGEDESCRIPTOR), 1, context->fp);

        /* get bmp's display position */
        *xpos = imagedesc.image_left_position;
        *ypos = imagedesc.image_top_position;

        /* create a bmp to load image data */
        if (   pb->width  != imagedesc.image_width
            || pb->height != imagedesc.image_height)
        {
            destroybmp(pb);
            pb->width  = imagedesc.image_width;
            pb->height = imagedesc.image_height;
            pb->cdepth = 8;
            if (!createbmp(pb)) return FALSE;
            else pb->stride = pb->width; /* not align to 4-bytes */
        }

        /* load image's palette data */
        context->locpalflag = imagedesc.local_color_table_flag;
        if (imagedesc.local_color_table_flag) {
            palsize = 1 << (imagedesc.local_color_table_size + 1);
            fio->read(palbuffer, palsize * 3, 1, context->fp);
            for (i=0; i<palsize; i++) {
                pallocal[i * 4 + 2] = palbuffer[i * 3 + 0];
                pallocal[i * 4 + 1] = palbuffer[i * 3 + 1];
                pallocal[i * 4 + 0] = palbuffer[i * 3 + 2];
            }
            setbmppal(pb, 0, palsize, pallocal);
        } else {
            setbmppal(pb, 0, context->gpalsize, context->gpaldata);
        }

        /* get image's lzw min code size */
        codesize = (fio->getc)(context->fp);
        gif_lzw_decode(&context->lzwcodec, codesize, pb->pdata, pb->stride * pb->height, context->fp, context->fio);

        /* deinterlace gif image */
        if (imagedesc.interlace_flag) gifdeinterlace(pb);
        break;

    case GIF_EXT_INTR:
        curbyte = (fio->getc)(context->fp);
        switch (curbyte) {
        case GIF_CTRL_EXT:
            /* load gif control extension */
            curbyte = (fio->getc)(context->fp);
            if (curbyte != sizeof(GRAPHCTRLEXT)) break;
            fio->read(&gctrlext, sizeof(GRAPHCTRLEXT), 1, context->fp);

            /* fill ctrl block */
            *type  = GIF_FRAME_TYPE_CTRL;
            ctrl->disposal   = gctrlext.disposal_method;
            ctrl->inputflag  = gctrlext.user_input_flag;
            ctrl->transflag  = gctrlext.transparent_color_flag;
            ctrl->transcolor = gctrlext.transparent_color;
            ctrl->delay      = gctrlext.delay_time * 10;

            curbyte = (fio->getc)(context->fp);
            if (curbyte != 0) {
                log_printf("parser GIF_CTRL_EXT failed, can't get block terminator !\r\n");
                break;
            }
            break;

        case GIF_TEXT_EXT:
            /* load gif text extension */
            curbyte = (fio->getc)(context->fp);
            if (curbyte != sizeof(PLAINTEXTEXT)) break;
            fio->read(&gtextext, sizeof(PLAINTEXTEXT), 1, context->fp);
            curbyte = (fio->getc)(context->fp);

            /* fill text block */
            *type = GIF_FRAME_TYPE_TEXT;
            *xpos = gtextext.text_grid_left_position;
            *ypos = gtextext.text_grid_top_position;
            text->width     = gtextext.text_grid_width;
            text->height    = gtextext.text_grid_height;
            text->charw     = gtextext.char_cell_width;
            text->charh     = gtextext.char_cell_height;
            text->forecolor = gtextext.text_foreground_color;
            text->backcolor = gtextext.text_background_color;

            /* ++ we only support 256 bytes text length ++ */
            curbyte = (fio->getc)(context->fp);
            if (curbyte == 0) break;
            fio->read(text->text, curbyte, 1, context->fp);

            for (;;) {
                curbyte = (fio->getc)(context->fp);
                if (curbyte == 0) break;
                fio->seek(context->fp, curbyte, SEEK_CUR);
            }
            /* -- we only support 256 bytes text length -- */
            break;

        /* ++ not support now ++ */
        case GIF_CMT_EXT:
        case GIF_APP_EXT:
/*          log_printf("unsupported gif ext_intr !\r\n"); */
            for (;;) {
                curbyte = (fio->getc)(context->fp);
                if (curbyte == 0) break;
                fio->seek(context->fp, curbyte, SEEK_CUR);
            }
            break;
        }
        break;
        /* -- not support now -- */

    case ';':
        fio->seek(context->fp, context->dataoffset, SEEK_SET);
        *type = GIF_FRAME_TYPE_END; /* type */
        break;
    }

    return TRUE;
}

/* encode */
void* gifencodeinit(void *fp, FIO *fio, int width, int height, BYTE bkcolor, BYTE *pal, int size)
{
    GIFCONTEXT *context = NULL;
    BOOL        failed  = FALSE;

    GIFHEADER       gifheader;
    LOGICSCREENDESC logscrdesc;
    BYTE            palbuf[256*3];
    int             i;

    context = calloc(1, sizeof(GIFCONTEXT));
    if (!context) {
        failed = TRUE;
        goto done;
    }

    /* init fp and fio */
    context->fp  = fp;
    context->fio = fio;

    /* init lzw codec */
    context->lzwcodec.LZW_CODE_SIZE_MAX = 12;
    if (!initlzwcodec(&(context->lzwcodec))) {
        failed = TRUE;
        goto done;
    }

    /* set gif global palette */
    if (pal) {
        memcpy(context->gpaldata, pal, size * 4);
    }
    context->gpalsize = size;

    /* width & height & bkcolor */
    context->width   = width;
    context->height  = height;
    context->bkcolor = bkcolor;

    /* fill gif file header */
    gifheader.signature[0] = 'G';
    gifheader.signature[1] = 'I';
    gifheader.signature[2] = 'F';
    gifheader.version[0]   = '8';
    gifheader.version[1]   = '9';
    gifheader.version[2]   = 'a';

    /* write gif header */
    fio->write(&gifheader, sizeof(gifheader), 1, context->fp);

    /* fill gif logic screen desc */
    logscrdesc.logic_screen_width  = context->width;
    logscrdesc.logic_screen_height = context->height;
    logscrdesc.background_color    = context->bkcolor;
    logscrdesc.color_resolution    = 7;
    logscrdesc.pixel_aspect_ratio  = 0;
    logscrdesc.sort_flag           = 0;
    if (context->gpalsize > 0) {
        logscrdesc.global_color_table_flag = 1;
        logscrdesc.global_color_table_size = 7;
    }

    /* write gif logic screen descriptor */
    fio->write(&logscrdesc, sizeof(logscrdesc), 1, context->fp);

    /* if there is global palette write it to file */
    if (context->gpalsize > 0) {
        for (i=0; i<context->gpalsize; i++) {
            palbuf[i * 3 + 0] = context->gpaldata[i * 4 + 2];
            palbuf[i * 3 + 1] = context->gpaldata[i * 4 + 1];
            palbuf[i * 3 + 2] = context->gpaldata[i * 4 + 0];
        }
        fio->write(palbuf, 256 * 3, 1, context->fp);
    }

done:
    if (failed) {
        log_printf("failed to init gif decoder !\r\n");
        gifdecodefree(context);
        return NULL;
    }
    return context;
}

void gifencodefree(void *ctxt)
{
    GIFCONTEXT *context = (GIFCONTEXT*)ctxt;
    if (context) {
        /* close lzw codec */
        closelzwcodec(&(context->lzwcodec));

        /* free fp fo context */
        context->fio->close(context->fp);

        free(context);
    }
}

BOOL gifencodeframe(void *ctxt, int type, BMP *pb, int xpos, int ypos, GIF_CTRL *ctrl, GIF_TEXT *text)
{
    GIFCONTEXT     *context    = (GIFCONTEXT*)ctxt;
    FIO            *fio        = context->fio;
    GRAPHCTRLEXT    gifctrlext = {0};
    IMAGEDESCRIPTOR imagedesc  = {0};
    BYTE            palbuf[256*3];
    int             codesize   = 8;
    int             i;

    switch (type) {
    case GIF_FRAME_TYPE_IMAGE:
        /* first write Image Separator to file */
        (fio->putc)(GIF_IMG_SPRT, context->fp);

        /* fill imagedesc and write it to file */
        imagedesc.image_left_position = xpos;
        imagedesc.image_top_position  = ypos;
        imagedesc.image_width         = pb->width;
        imagedesc.image_height        = pb->height;
        if (context->locpalflag) {
            imagedesc.local_color_table_flag = 1;
            imagedesc.local_color_table_size = 7;
        }
        fio->write(&imagedesc, sizeof(imagedesc), 1, context->fp);

        /* if there is local palette write it to file */
        if (context->locpalflag) {
            for (i=0; i<context->gpalsize; i++) {
                palbuf[i * 3 + 0] = pb->ppal[i * 4 + 2];
                palbuf[i * 3 + 1] = pb->ppal[i * 4 + 1];
                palbuf[i * 3 + 2] = pb->ppal[i * 4 + 0];
            }
            fio->write(palbuf, 256 * 3, 1, context->fp);
        }

        /* write lzw minimum code size to file */
        (fio->putc)(8, context->fp);
        gif_lzw_encode(&context->lzwcodec, codesize, context->fp, context->fio, pb->pdata, pb->stride * pb->height);
        break;

    case GIF_FRAME_TYPE_CTRL:
        /* write the Extension Introducer and Graphic Control Label to file */
        (fio->putc)(GIF_EXT_INTR, context->fp);
        (fio->putc)(GIF_CTRL_EXT, context->fp);

        /* write block size to file */
        (fio->putc)(sizeof(gifctrlext), context->fp);

        /* fill gifctrlext and write it to file */
        gifctrlext.disposal_method        = ctrl->disposal;
        gifctrlext.user_input_flag        = ctrl->inputflag;
        gifctrlext.transparent_color_flag = ctrl->transflag;
        gifctrlext.transparent_color      = ctrl->transcolor;
        gifctrlext.delay_time             = ctrl->delay / 10;
        fio->write(&gifctrlext, sizeof(gifctrlext), 1, context->fp);

        /* write block terminator to file */
        (fio->putc)(0, context->fp);
        break;

    case GIF_FRAME_TYPE_END:
        /* write the gif file Trailer */
        (fio->putc)(';', context->fp);
        break;

    case GIF_FRAME_TYPE_TEXT:
        /* todo... */
        DO_USE_VAR(text);
        break;
    }
    return TRUE;
}

void gifgetimageinfo(void *ctxt, int *width, int *height, BYTE *bkcolor)
{
    GIFCONTEXT *context = (GIFCONTEXT*)ctxt;
    *width   = context->width;
    *height  = context->height;
    *bkcolor = context->bkcolor;
}

void gifgetglobalpal(void *ctxt, BYTE *pal, int *size)
{
    GIFCONTEXT *context = (GIFCONTEXT*)ctxt;
    *size = context->gpalsize;
    memcpy(pal, context->gpaldata, *size * 4);
}

void gifgetlocalpal(void *ctxt, int *flag)
{
    GIFCONTEXT *context = (GIFCONTEXT*)ctxt;
    *flag = context->locpalflag;
}

void gifsetlocalpal(void *ctxt, int  flag)
{
    GIFCONTEXT *context = (GIFCONTEXT*)ctxt;
    context->locpalflag = flag;
}

#else
#include <stdlib.h>
#include <conio.h>
#include "screen.h"
#include "gif.h"
#include "bitblt.h"
#include "log.h"
#include "fftask.h"

static void    *g_gdctxt = NULL;
static void    *g_gectxt = NULL;
static BMP      g_mybmp  = {0};
static GIF_CTRL g_ctrl   = {0};
static GIF_TEXT g_text   = {0};

int main(void)
{
    void *fpdec = NULL;
    void *fpenc = NULL;
    int   width, height;
    BYTE  bkcolor;
    BYTE  globalpaldata[256*4];
    int   globalpalsize = 0;
    int   localpalflag  = 0;
    int   xpos, ypos;
    int   type;

    ffkernel_init();
    log_init("gif.log");

    fpdec = DEF_FIO.open("res/cs.gif", "rb");
    fpenc = DEF_FIO.open("output.gif", "wb");

    g_gdctxt = gifdecodeinit(fpdec , &DEF_FIO);
    gifgetimageinfo(g_gdctxt, &width, &height, &bkcolor);
    gifgetglobalpal(g_gdctxt, globalpaldata, &globalpalsize);

    g_gectxt = gifencodeinit(fpenc, &DEF_FIO, width, height, bkcolor, globalpaldata, globalpalsize);
    while (1) {
        gifdecodeframe(g_gdctxt, &type, &g_mybmp, &xpos, &ypos, &g_ctrl, &g_text);
        gifgetlocalpal(g_gdctxt, &localpalflag);
        gifsetlocalpal(g_gectxt,  localpalflag);
        gifencodeframe(g_gectxt,  type, &g_mybmp,  xpos,  ypos, &g_ctrl, &g_text);
        if (type == GIF_FRAME_TYPE_END) break;
    }

    createbmp(&SCREEN);
    while (!kbhit()) {
        gifdecodeframe(g_gdctxt, &type , &g_mybmp, &xpos, &ypos, &g_ctrl, &g_text);
        switch (type) {
        case GIF_FRAME_TYPE_IMAGE:
            putbmp(&SCREEN, xpos, ypos, &g_mybmp, FS_AUTO_LOCK, 0, 0, NULL);
            break;
        case GIF_FRAME_TYPE_CTRL:
            task_delay(g_ctrl.delay);
            break;
        case GIF_FRAME_TYPE_TEXT:
            break;
        }
        if (type == GIF_FRAME_TYPE_END) break;
    }
    destroybmp(&SCREEN);

    gifdecodefree(g_gdctxt);
    gifdecodefree(g_gectxt);
    DEF_FIO.close(fpdec);
    DEF_FIO.close(fpenc);

    log_done();
    ffkernel_exit();
    return 0;
}
#endif



