#ifndef _FIODRV_H_
#define _FIODRV_H_

/* ����ͷ�ļ� */
#include "stdefine.h"

/* ������ fio context */
typedef struct
{
    BYTE    bitbuf;
    int     bitflag;
    BYTE    data[0];
} FIO_CONTEXT;

/* ���Ͷ��� */
typedef struct
{
    void*  (*open )(const char *url, const char *mode);
    int    (*close)(void *fp);
    int    (*getc )(void *fp);
    int    (*putc )(int c, void *fp);
    size_t (*read )(void *buf, size_t size, size_t count, void *fp);
    size_t (*write)(const void *buf, size_t size, size_t count, void *fp);
    long   (*tell )(void *fp);
    int    (*seek )(void *fp, long offset, int org);
    int    (*eof  )(void *fp);
    int    (*flush)(void *fp);
} FIODRV; /* file i/o driver */

extern FIODRV DEF_FIO_DRV;
extern FIODRV MEM_FIO_DRV;

/* �������� */
BOOL getbits  (void *fp, FIODRV *drv, DWORD *data, int size);
BOOL putbits  (void *fp, FIODRV *drv, DWORD  data, int size);
BOOL flushbits(void *fp, FIODRV *drv, int flag);
BOOL alignbyte(void *fp, FIODRV *drv);

#endif




