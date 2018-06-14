#ifndef __RGE_FIO_H__
#define __RGE_FIO_H__

/* ����ͷ�ļ� */
#include "stdefine.h"

/* ������ fio context */
typedef struct {
    BYTE    bitbuf;
    int     bitflag;
    BYTE   *data;
} FIO_CONTEXT;

/* ���Ͷ��� */
typedef struct {
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
} FIO; /* file i/o driver */

extern FIO DEF_FIO;
extern FIO MEM_FIO;

/* �������� */
BOOL getbits  (void *fp, DWORD *data, int size, FIO *fio);
BOOL putbits  (void *fp, DWORD  data, int size, FIO *fio);
BOOL flushbits(void *fp, int flag, FIO *fio);
BOOL alignbyte(void *fp, FIO *fio);

#endif




