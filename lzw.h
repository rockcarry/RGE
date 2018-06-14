#ifndef __RGE_LZW_H__
#define __RGE_LZW_H__

/* 包含头文件 */
#include "fio.h"

/* 类型定义 */
/* LZW 编解码器类型定义 */
typedef struct {
    /* 编解码进度控制函数 */
    void (*ppc)(long total, long cur);

    /* 编解码器参数控制 */
    /* for user */
    int LZW_ROOT_SIZE;
    int LZW_CODE_SIZE_OUT;
    int LZW_CODE_SIZE_MIN;
    int LZW_CODE_SIZE_MAX;

    /* 内部使用的变量 */
    int LZW_ROOT_NUM;
    int LZW_CLEAR_CODE;
    int LZW_END_CODE;
    int LZW_TAB_SIZE;

    /* 内部使用的变量 */
    void  *_lzw_str_tab;
    DWORD *_lzw_str_buf;
    int    _str_tab_pos;
} LZWCODEC, *PLZWCODEC;

/* 函数声明 */
BOOL initlzwcodec (LZWCODEC *plc);
void resetlzwcodec(LZWCODEC *plc);
void closelzwcodec(LZWCODEC *plc);

BOOL lzwencode(LZWCODEC *plc, void *fpout, FIO *fioout, void *fpin, FIO *fioin);
BOOL lzwdecode(LZWCODEC *plc, void *fpout, FIO *fioout, void *fpin, FIO *fioin);

#endif


