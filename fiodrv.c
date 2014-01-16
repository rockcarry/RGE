/* 包含头文件 */
#include <stdio.h>
#include "fiodrv.h"

/* 内部类型定义 */
typedef struct
{
    BYTE    bitbuf;
    int     bitflag;
    FILE   *fp;
} DEF_FIO_CONTEXT;

static void* _def_fio_open(const char *url, const char *mode)
{
    DEF_FIO_CONTEXT *ctxt = NULL;

    ctxt = malloc(sizeof(DEF_FIO_CONTEXT));
    if (!ctxt) return NULL;

    ctxt->fp = fopen(url, mode);
    if (!ctxt->fp)
    {
        free(ctxt);
        return NULL;
    }

    ctxt->bitbuf  = 0;
    ctxt->bitflag = 0;
    return ctxt;
}

static int _def_fio_close(void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt)
    {
        retv = fclose(ctxt->fp);
        free(ctxt);
    }
    return retv;
}

static int _def_fio_getc(void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        retv = fgetc(ctxt->fp);
    }
    return retv;
}

static int _def_fio_putc(int c, void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        retv = fputc(c, ctxt->fp);
    }
    return retv;
}

static size_t _def_fio_read(void *buf, size_t size, size_t count, void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    size_t retv = 0;

    if (ctxt) {
        retv = fread(buf, size, count, ctxt->fp);
    }
    return retv;
}

static size_t _def_fio_write(const void *buf, size_t size, size_t count, void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    size_t retv = 0;

    if (ctxt) {
        retv = fwrite(buf, size, count, ctxt->fp);
    }
    return retv;
}

static long _def_fio_tell(void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    long retv = -1l;

    if (ctxt) {
        retv = ftell(ctxt->fp);
    }
    return retv;
}

static int _def_fio_seek(void *fp, long offset, int org)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        retv = fseek(ctxt->fp, offset, org);
    }
    return retv;
}

static int _def_fio_eof(void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        retv = feof(ctxt->fp);
    }
    return retv;
}

static int _def_fio_flush(void *fp)
{
    DEF_FIO_CONTEXT *ctxt = (DEF_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        retv = fflush(ctxt->fp);
    }
    return retv;
}

/* 全局变量定义 */
FIODRV DEF_FIO_DRV =
{
    _def_fio_open,
    _def_fio_close,
    _def_fio_getc,
    _def_fio_putc,
    _def_fio_read,
    _def_fio_write,
    _def_fio_tell,
    _def_fio_seek,
    _def_fio_eof,
    _def_fio_flush,
};


/* MEM_FIO_DRV 实现 */
typedef struct
{
    BYTE    bitbuf;
    int     bitflag;
    BYTE   *pdata;
    long    length;
    long    curpos;
    int     openflag;
} MEM_FIO_CONTEXT;

static void* _mem_fio_open(const char *pbuf, const char *length)
{
    MEM_FIO_CONTEXT *ctxt = NULL;

    ctxt = malloc(sizeof(MEM_FIO_CONTEXT));
    if (!ctxt) return NULL;

    ctxt->pdata  = (BYTE*)pbuf;
    ctxt->length = (int)length;

    if (!ctxt->pdata) {
        ctxt->pdata = malloc(ctxt->length);
        ctxt->openflag = 1;
    }
    else ctxt->openflag = 0;

    ctxt->bitbuf  = 0;
    ctxt->bitflag = 0;
    ctxt->curpos  = 0;
    return ctxt;
}

static int _mem_fio_close(void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt)
    {
        if (ctxt->openflag) {
            free(ctxt->pdata);
        }
        free(ctxt);
        retv = 0;
    }
    return retv;
}

static int _mem_fio_getc(void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        if (ctxt->pdata) {
            if (ctxt->curpos < ctxt->length) {
                retv = ctxt->pdata[ctxt->curpos++];
            }
        }
    }
    return retv;
}

static int _mem_fio_putc(int c, void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        if (ctxt->pdata) {
            if (ctxt->curpos < ctxt->length) {
                ctxt->pdata[ctxt->curpos++] = (BYTE)c;
                retv = (BYTE)c;
            }
        }
    }
    return retv;
}

static size_t _mem_fio_read(void *buf, size_t size, size_t count, void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    size_t retv  = 0;
    size_t total = size * count;
    size_t num;

    if (ctxt) {
        if (ctxt->pdata) {
            num = total < ctxt->length - ctxt->curpos ? total : ctxt->length - ctxt->curpos;
            memcpy(buf, ctxt->pdata + ctxt->curpos, num);
            ctxt->curpos += num;
            retv = num / size;
        }
    }
    return retv;
}

static size_t _mem_fio_write(const void *buf, size_t size, size_t count, void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    size_t retv = 0;
    size_t total = size * count;
    size_t num;

    if (ctxt) {
        if (ctxt->pdata) {
            num = total < ctxt->length - ctxt->curpos ? total : ctxt->length - ctxt->curpos;
            memcpy(ctxt->pdata + ctxt->curpos, buf, num);
            ctxt->curpos += num;
            retv = num / size;
        }
    }
    return retv;
}

static long _mem_fio_tell(void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    long retv = -1l;

    if (ctxt) {
        if (ctxt->pdata) {
            retv = ctxt->curpos;
        }
    }
    return retv;
}

static int _mem_fio_seek(void *fp, long offset, int org)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        if (ctxt->pdata) {
            switch (org)
            {
            case SEEK_CUR:
                ctxt->curpos += offset;
                break;
            case SEEK_END:
                ctxt->curpos  = ctxt->length;
                ctxt->curpos += offset;
                break;
            case SEEK_SET:
                ctxt->curpos  = offset;
                break;
            }
            if (ctxt->curpos < 0) {
                ctxt->curpos = 0;
            }
            else if (ctxt->curpos > ctxt->length) {
                ctxt->curpos = ctxt->length;
            }
            else retv = 0;
        }
    }
    return retv;
}

static int _mem_fio_eof(void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        if (ctxt->pdata) {
            if (ctxt->curpos < ctxt->length) {
                retv = 0;
            }
        }
    }
    return retv;
}

static int _mem_fio_flush(void *fp)
{
    MEM_FIO_CONTEXT *ctxt = (MEM_FIO_CONTEXT*)fp;
    int retv = EOF;

    if (ctxt) {
        if (ctxt->pdata) {
            retv = 0;
        }
    }
    return retv;
}

/* 全局变量定义 */
FIODRV MEM_FIO_DRV =
{
    _mem_fio_open,
    _mem_fio_close,
    _mem_fio_getc,
    _mem_fio_putc,
    _mem_fio_read,
    _mem_fio_write,
    _mem_fio_tell,
    _mem_fio_seek,
    _mem_fio_eof,
    _mem_fio_flush,
};


/* 函数实现 */
BOOL getbits(void *fp, FIODRV *drv, DWORD *data, int size)
{
    FIO_CONTEXT *ctxt = (FIO_CONTEXT*)fp;
    int pos, byte, need, i;

    need = (size - ctxt->bitflag + 7) / 8;
    if (need > 0)
    {
       *data = ctxt->bitbuf;
        pos  = ctxt->bitflag;
        for (i=0; i<need; i++) {
            byte = drv->getc(fp);
            if (byte == EOF) return FALSE;

           *data |= byte << pos;
            pos  += 8;
        }
        ctxt->bitflag = need * 8 + ctxt->bitflag - size;
        ctxt->bitbuf  = byte >> (8 - ctxt->bitflag);
    }
    else
    {
       *data  = ctxt->bitbuf;
        ctxt->bitflag -= size;
        ctxt->bitbuf >>= size;
    }
    *data &= (1L << size) - 1;
    return TRUE;
}

BOOL putbits(void *fp, FIODRV *drv, DWORD data, int size)
{
    FIO_CONTEXT *ctxt = (FIO_CONTEXT*)fp;
    int nbit;

    data &= (1L << size) - 1;

    if (ctxt->bitflag)
    {
        nbit = size < 8 - ctxt->bitflag ? size : 8 - ctxt->bitflag;
        ctxt->bitbuf |= data << ctxt->bitflag;
        ctxt->bitflag+= nbit;
        if (ctxt->bitflag < 8) return TRUE;

        if (EOF == drv->putc(ctxt->bitbuf, fp)) return FALSE;
        data >>= nbit;
        size  -= nbit;
    }

    while (size >= 8)
    {
        if (EOF == drv->putc(data, fp)) return FALSE;
        data >>= 8;
        size  -= 8;
    }

    ctxt->bitflag = size;
    ctxt->bitbuf  = (BYTE)data;
    return TRUE;
}

BOOL flushbits(void *fp, FIODRV *drv, int flag)
{
    FIO_CONTEXT *ctxt = (FIO_CONTEXT*)fp;
    DWORD fill;

    if (!ctxt->bitflag) return TRUE;

    if (flag) fill = 0xffffffff;
    else      fill = 0;

    if (!putbits(fp, drv, fill, 8 - ctxt->bitflag)) return FALSE;
    else return TRUE;
}

BOOL alignbyte(void *fp, FIODRV *drv)
{
    FIO_CONTEXT *ctxt = (FIO_CONTEXT*)fp;
    ctxt->bitflag = 0;
    ctxt->bitbuf  = 0;
    return TRUE;
}

