#ifndef _TEST_

/* 包含头文件 */
#include "lzw.h"
#include "log.h"

/* 预编译开关 */
#define ENABLE_PROGRESS_CALLBACK  TRUE
#define DYNMIC_MEMORY_ALLOCATION  TRUE

/* 内部类型定义 */
/* LZW 串表项类型定义 */
/* prefixe code -1 means null */
typedef struct
{
    int  prefix;    /* 前缀       */
    int  firstbyte; /* 首字节     */
    int  lastbyte;  /* 尾字节     */
    int  head;      /* 前缀链头   */
    int  next;      /* 前缀链指针 */
} LZWSTRITEM;

/* 内部函数实现 */
/* 向 LZW 编码表中加入一个字符串 */
static BOOL AddToLZWStringTable(LZWCODEC *plc, int prefix, int byte, BOOL flag)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;

    /* 前缀为空的字符串直接返回成功 */
    if (prefix == -1) return TRUE;

    /* 如果 LZW 编码表满则返回失败 */
    if (plc->_str_tab_pos >= plc->LZW_TAB_SIZE) return FALSE;

    /* 在编码表中加入新字符串 */
    strtab[plc->_str_tab_pos].prefix    = prefix;
    strtab[plc->_str_tab_pos].firstbyte = strtab[prefix].firstbyte;
    strtab[plc->_str_tab_pos].lastbyte  = byte;

    if (flag)
    {
        /* 更新前缀链表 */
        strtab[plc->_str_tab_pos].next = strtab[prefix].head;
        strtab[prefix].head            = plc->_str_tab_pos;
    }

    /* 增加编码表当前位置指针 */
    plc->_str_tab_pos++;

    /* 返回成功 */
    return TRUE;
}

/*
  在编码表中查找一个字符串的编码
  找到则返回其编码，否则返回 -1 .
 */
static int FindInLZWStringTable(LZWCODEC *plc, int prefix, int byte)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    int         link;

    /* 如果前缀为 -1 则直接返回 */
    if (prefix == -1) return byte;

    /* 在 while 循环中遍历前缀链表 */
    link = strtab[prefix].head;
    while (link)
    {
        if (strtab[link].lastbyte == byte) return link;
        link = strtab[link].next;
    }

    /* 没有找到则返回 -1 */
    return -1;
}

/* 判断当前编码表中是否已经存在某个编码 */
static BOOL IsCodeInLZWStringTable(LZWCODEC *plc, int code)
{
    if (code >= 0 && code < plc->_str_tab_pos) return TRUE;
    else return FALSE;
}

/* 取得一个编码对应字符串的首字符 */
static int GetFirstByteOfLZWCode(LZWCODEC *plc, int code)
{
    return ((LZWSTRITEM*)plc->_lzw_str_tab)[code].firstbyte;
}

/* 输出解码后的 lzw 字符串 */
static BOOL OutputLZWString(LZWCODEC *plc, void *fp, FIODRV *drv, int code)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    DWORD      *pbuf   = plc->_lzw_str_buf;
    int         len    = 0;

    /* 构造 LZW 字符串 */
    while (code != -1 && len++ < plc->LZW_TAB_SIZE)
    {
       *pbuf++ = strtab[code].lastbyte;
        code   = strtab[code].prefix;
    }

    /* 将字符串输出到比特流 */
    while (len--) {
        putbits(fp, drv, *--pbuf, plc->LZW_CODE_SIZE_OUT);
    }

    /* 返回成功 */
    return TRUE;
}

/* 初始化 lzw 编解码器 */
BOOL initlzwcodec(LZWCODEC *plc)
{
    LZWSTRITEM *strtab = NULL;
    int i;

#if !(DYNMIC_MEMORY_ALLOCATION)
    #define MAX_LZW_TAB_SIZE 4096
    static LZWSTRITEM s_lzw_str_tab[MAX_LZW_TAB_SIZE];
    static DWORD      s_lzw_str_buf[MAX_LZW_TAB_SIZE];
#endif

    /* try to use default codec param */
    if (plc->LZW_ROOT_SIZE     == 0) plc->LZW_ROOT_SIZE     = 8;
    if (plc->LZW_CODE_SIZE_OUT == 0) plc->LZW_CODE_SIZE_OUT = 8;
    if (plc->LZW_CODE_SIZE_MIN == 0) plc->LZW_CODE_SIZE_MIN = 9;
    if (plc->LZW_CODE_SIZE_MAX == 0) plc->LZW_CODE_SIZE_MAX = 12;

    plc->LZW_ROOT_NUM   = 1 << plc->LZW_ROOT_SIZE;
    plc->LZW_CLEAR_CODE = plc->LZW_ROOT_NUM + 0;
    plc->LZW_END_CODE   = plc->LZW_ROOT_NUM + 1;
    plc->LZW_TAB_SIZE   = 1 << plc->LZW_CODE_SIZE_MAX;

#if DYNMIC_MEMORY_ALLOCATION
    /* allocate memory for lzw string table */
    plc->_lzw_str_tab = malloc(sizeof(LZWSTRITEM) * plc->LZW_TAB_SIZE);
    if (!plc->_lzw_str_tab) return FALSE;

    /* allocate memory for lzw string construction */
    plc->_lzw_str_buf = malloc(sizeof(DWORD) * plc->LZW_TAB_SIZE);
    if (!plc->_lzw_str_buf) return FALSE;
#else
    plc->_lzw_str_tab = s_lzw_str_tab;
    plc->_lzw_str_buf = s_lzw_str_buf;
#endif

    /* init lzw root code */
    strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    for (i=0; i<=plc->LZW_END_CODE; i++)
    {
        strtab[i].prefix    = -1;
        strtab[i].firstbyte =  i;
        strtab[i].lastbyte  =  i;
    }

    return TRUE;
}

/* 复位 lzw 编解码器 */
void resetlzwcodec(LZWCODEC *plc)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    int i;

    /* init lzw prefix link list */
    for (i=0; i<plc->LZW_TAB_SIZE; i++) strtab[i].head = 0;

    /* reset lzw table position pointer */
    plc->_str_tab_pos = plc->LZW_END_CODE + 1;
}

/* 关闭 lzw 编解码器 */
void closelzwcodec(LZWCODEC *plc)
{
#if DYNMIC_MEMORY_ALLOCATION
    if (plc->_lzw_str_tab)
    {
        free(plc->_lzw_str_tab);
        plc->_lzw_str_tab = NULL;
    }

    if (plc->_lzw_str_buf)
    {
        free(plc->_lzw_str_buf);
        plc->_lzw_str_buf = NULL;
    }
#endif
}

#if 0
static void dump_gif_codec(LZWCODEC *plc)
{
    LZWSTRITEM *item;
    int         i;

    log_printf("++ gif codec context ++\r\n");
    log_printf("LZW_ROOT_SIZE = %d\r\n", plc->LZW_ROOT_SIZE);
    log_printf("LZW_CODE_SIZE_OUT = %d\r\n", plc->LZW_CODE_SIZE_OUT);
    log_printf("LZW_CODE_SIZE_MIN = %d\r\n", plc->LZW_CODE_SIZE_MIN);
    log_printf("LZW_CODE_SIZE_MAX = %d\r\n", plc->LZW_CODE_SIZE_MAX);
    log_printf("LZW_ROOT_NUM      = %d\r\n", plc->LZW_ROOT_NUM);
    log_printf("LZW_CLEAR_CODE    = %d\r\n", plc->LZW_CLEAR_CODE);
    log_printf("LZW_END_CODE      = %d\r\n", plc->LZW_END_CODE);
    log_printf("LZW_TAB_SIZE      = %d\r\n", plc->LZW_TAB_SIZE);
    log_printf("_str_tab_pos      = %d\r\n", plc->_str_tab_pos);
    log_printf("_lzw_str_tab:\r\n");
    log_printf("  prefix  firstbyte  lastbyte  head  next%d\r\n");
    item = (LZWSTRITEM*)plc->_lzw_str_tab;
    for (i=0; i<plc->_str_tab_pos; i++) {
        log_printf("  %4d       %4d      %4d  %4d   %4d\r\n", item[i].prefix,
                   item[i].firstbyte, item[i].lastbyte, item[i].head, item[i].next);
    }
    log_printf("-- gif codec context --\r\n\r\n");
}
#endif

/* LZW 编码 */
BOOL lzwencode(LZWCODEC *plc, void *fpout, FIODRV *drvout, void *fpin, FIODRV *drvin)
{
    int   curcodesize = plc->LZW_CODE_SIZE_MIN;
    int   findcode    = -1;
    int   prefixcode  = -1;
    DWORD currentbyte =  0;

    /* default file io drv */
    if (!drvin ) drvin  = &DEF_FIO_DRV;
    if (!drvout) drvout = &DEF_FIO_DRV;

    if (!plc || !fpin || !fpout) return FALSE;

    /* reset lzw codec */
    resetlzwcodec(plc);

    /* while until there is no data in source bit stream */
    while (getbits(fpin, drvin, &currentbyte, plc->LZW_ROOT_SIZE))
    {
        /* 在 LZW 编码表中查找由当前前缀和当前字符组成的字符串 */
        findcode = FindInLZWStringTable(plc, prefixcode, currentbyte);

        if (findcode == -1)
        {   /* 在 LZW 编码表中没有该字符串 */
            /* 以当前的 codesize 写出当前前缀 */
            if (!putbits(fpout, drvout, prefixcode, curcodesize)) {
                log_printf("failed to write prefixcode !\n");
                return FALSE;
            }

            /* 将当前前缀和当前字符所组成的字符串加入编码表 */
            if (!AddToLZWStringTable(plc, prefixcode, currentbyte, TRUE))
            {   /* 加入失败说明编码表已满 */
                /* 写出一个 LZW_CLEAR_CODE 到比特流 */
                if (!putbits(fpout, drvout, plc->LZW_CLEAR_CODE, curcodesize)) {
                    log_printf("failed to write LZW_CLEAR_CODE !\n");
                    return FALSE;
                }
                resetlzwcodec(plc); /* reset lzw codec */
                curcodesize = plc->LZW_CODE_SIZE_MIN; /* reset curcodesize */
            }
            else 
            {   /* 加入到编码表成功 */
                /* 根据 _str_tab_pos 重新计算新的 curcodesize */
                if (   plc->_str_tab_pos - 1 == (1 << curcodesize)
                    && curcodesize < plc->LZW_CODE_SIZE_MAX) /* note: 这个限制条件必须要 */
                {
                    /* 增加 curcodesize 的值 */
                    curcodesize++;
                }
            }

            /* 置当前前缀码为 currentbyte */
            prefixcode = currentbyte;
        }
        else
        {   /* 在 LZW 编码表中找到该字符串 */
            /* 置当前前缀码为该字符串的编码 */
            prefixcode = findcode;
        }

#if ENABLE_PROGRESS_CALLBACK
        /* 调用编码进度回调函数 */
        if (plc->ppc) plc->ppc(0, drvin->tell(fpin));
#endif
    }

    /* 最后需要写出剩余的码字和结束结束码 */
    if (!putbits(fpout, drvout, prefixcode, curcodesize)) return FALSE;
    if (!putbits(fpout, drvout, plc->LZW_END_CODE, curcodesize)) return FALSE;
    if (!flushbits(fpout, drvout, 0)) return FALSE; /* 对比特流作 flush 操作 */

    /* 返回成功 */
    return TRUE;
}

/* LZW 解码 */
#define resetlzwcodecfordecode(plc)  do { (plc)->_str_tab_pos = (plc)->LZW_END_CODE + 1; } while (0)
BOOL lzwdecode(LZWCODEC *plc, void *fpout, FIODRV *drvout, void *fpin, FIODRV *drvin)
{
    int   curcodesize = plc->LZW_CODE_SIZE_MIN;
    int   oldcode     = -1;
    DWORD curcode     =  0;
    BOOL  bfind       =  FALSE;

    /* default file io drv */
    if (!drvin ) drvin  = &DEF_FIO_DRV;
    if (!drvout) drvout = &DEF_FIO_DRV;

    if (!plc || !fpin || !fpout) {
        log_printf("lzwdecode:: invalid param !\n");
        return FALSE;
    }

    /* reset lzw codec */
    resetlzwcodecfordecode(plc);

    /* 在一个 while 循环中进行解码
       每次读入 LZW 编码到 curcode 中
       然后进行 LZW 的解码处理
       直到读到 LZW_END_CODE 结束 */
    while ( getbits(fpin, drvin, &curcode, curcodesize)
          && (int)curcode != plc->LZW_END_CODE )
    {
        /*
        log_printf("curcode = %d, curcodesize = %d\r\n", curcode, curcodesize);
        dump_gif_codec(plc);
        */

        if ((int)curcode == plc->LZW_CLEAR_CODE)
        {   /* 如果读到的是清除码 */
            resetlzwcodecfordecode(plc); /* reset lzw codec */
            oldcode     = -1; /* set -1 to restart */
            curcodesize = plc->LZW_CODE_SIZE_MIN; /* reset curcodesize */
        }
        else
        {   /* 如果读到的不是清除码 */
            /* 查找编码表中是否有该编码 */
            bfind = IsCodeInLZWStringTable(plc, (int)curcode);

            /* 根据查找结果向编码表中加入新的字符串 */
            if (bfind) AddToLZWStringTable(plc, oldcode, GetFirstByteOfLZWCode(plc, (int)curcode), FALSE);
            else AddToLZWStringTable(plc, oldcode, GetFirstByteOfLZWCode(plc, oldcode), FALSE);

            /* 根据 _str_tab_pos 重新计算新的 curcodesize */
            if (   plc->_str_tab_pos == (1 << curcodesize)
                && curcodesize < plc->LZW_CODE_SIZE_MAX) // note: 这个限制条件必须要
            {
                /* 增加 curcodesize 的值 */
                curcodesize++;
            }

            /* 写出当前字符串到输出比特流 */
            if (!OutputLZWString(plc, fpout, drvout, (int)curcode)) {
                log_printf("failed to write lzw string !\n");
                return FALSE;
            }

            /* oldcode 置为 curcode */
            oldcode = (int)curcode;
        }

#if ENABLE_PROGRESS_CALLBACK
        /* 调用编码进度回调函数 */
        if (plc->ppc) plc->ppc(0, drvin->tell(fpin));
#endif
    }

    /* 对比特流作 flush 操作 */
    if (!flushbits(fpout, drvout, 0)) return FALSE;

    /* 返回成功 */
    return TRUE;
}

#else
/* 包含头文件 */
#include "lzw.h"
#include "log.h"

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    char orgfile[MAX_PATH];
    char encfile[MAX_PATH];
    char decfile[MAX_PATH];
    void *fpin, *fpout;
    LZWCODEC lzwcodec = {0};

    log_init("log.txt");

    if (*lpszCmdLine == '\0') lpszCmdLine = "lzw.c";
    log_printf("lpszCmdLine = %s\n", lpszCmdLine);

    strcpy(orgfile, lpszCmdLine);
    strcpy(encfile, lpszCmdLine); strcat(encfile, ".enc");
    strcpy(decfile, lpszCmdLine); strcat(decfile, ".dec");

    initlzwcodec(&lzwcodec);
    fpin  = DEF_FIO_DRV.open(orgfile, "rb");
    fpout = DEF_FIO_DRV.open(encfile, "wb");
    lzwencode(&lzwcodec, fpout, &DEF_FIO_DRV, fpin, &DEF_FIO_DRV);
    DEF_FIO_DRV.close(fpin );
    DEF_FIO_DRV.close(fpout);
    closelzwcodec(&lzwcodec);

    initlzwcodec(&lzwcodec);
    fpin  = DEF_FIO_DRV.open(encfile, "rb");
    fpout = DEF_FIO_DRV.open(decfile, "wb");
    lzwdecode(&lzwcodec, fpout, &DEF_FIO_DRV, fpin, &DEF_FIO_DRV);
    DEF_FIO_DRV.close(fpin );
    DEF_FIO_DRV.close(fpout);
    closelzwcodec(&lzwcodec);

    log_done();
    return 0;
}
#endif

