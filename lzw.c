#ifndef _TEST_

/* ����ͷ�ļ� */
#include "lzw.h"
#include "log.h"

/* Ԥ���뿪�� */
#define ENABLE_PROGRESS_CALLBACK  TRUE
#define DYNMIC_MEMORY_ALLOCATION  TRUE

/* �ڲ����Ͷ��� */
/* LZW ���������Ͷ��� */
/* prefixe code -1 means null */
typedef struct
{
    int  prefix;    /* ǰ׺       */
    int  firstbyte; /* ���ֽ�     */
    int  lastbyte;  /* β�ֽ�     */
    int  head;      /* ǰ׺��ͷ   */
    int  next;      /* ǰ׺��ָ�� */
} LZWSTRITEM;

/* �ڲ�����ʵ�� */
/* �� LZW ������м���һ���ַ��� */
static BOOL AddToLZWStringTable(LZWCODEC *plc, int prefix, int byte, BOOL flag)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;

    /* ǰ׺Ϊ�յ��ַ���ֱ�ӷ��سɹ� */
    if (prefix == -1) return TRUE;

    /* ��� LZW ��������򷵻�ʧ�� */
    if (plc->_str_tab_pos >= plc->LZW_TAB_SIZE) return FALSE;

    /* �ڱ�����м������ַ��� */
    strtab[plc->_str_tab_pos].prefix    = prefix;
    strtab[plc->_str_tab_pos].firstbyte = strtab[prefix].firstbyte;
    strtab[plc->_str_tab_pos].lastbyte  = byte;

    if (flag)
    {
        /* ����ǰ׺���� */
        strtab[plc->_str_tab_pos].next = strtab[prefix].head;
        strtab[prefix].head            = plc->_str_tab_pos;
    }

    /* ���ӱ����ǰλ��ָ�� */
    plc->_str_tab_pos++;

    /* ���سɹ� */
    return TRUE;
}

/*
  �ڱ�����в���һ���ַ����ı���
  �ҵ��򷵻�����룬���򷵻� -1 .
 */
static int FindInLZWStringTable(LZWCODEC *plc, int prefix, int byte)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    int         link;

    /* ���ǰ׺Ϊ -1 ��ֱ�ӷ��� */
    if (prefix == -1) return byte;

    /* �� while ѭ���б���ǰ׺���� */
    link = strtab[prefix].head;
    while (link)
    {
        if (strtab[link].lastbyte == byte) return link;
        link = strtab[link].next;
    }

    /* û���ҵ��򷵻� -1 */
    return -1;
}

/* �жϵ�ǰ��������Ƿ��Ѿ�����ĳ������ */
static BOOL IsCodeInLZWStringTable(LZWCODEC *plc, int code)
{
    if (code >= 0 && code < plc->_str_tab_pos) return TRUE;
    else return FALSE;
}

/* ȡ��һ�������Ӧ�ַ��������ַ� */
static int GetFirstByteOfLZWCode(LZWCODEC *plc, int code)
{
    return ((LZWSTRITEM*)plc->_lzw_str_tab)[code].firstbyte;
}

/* ��������� lzw �ַ��� */
static BOOL OutputLZWString(LZWCODEC *plc, void *fp, FIODRV *drv, int code)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    DWORD      *pbuf   = plc->_lzw_str_buf;
    int         len    = 0;

    /* ���� LZW �ַ��� */
    while (code != -1 && len++ < plc->LZW_TAB_SIZE)
    {
       *pbuf++ = strtab[code].lastbyte;
        code   = strtab[code].prefix;
    }

    /* ���ַ�������������� */
    while (len--) {
        putbits(fp, drv, *--pbuf, plc->LZW_CODE_SIZE_OUT);
    }

    /* ���سɹ� */
    return TRUE;
}

/* ��ʼ�� lzw ������� */
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

/* ��λ lzw ������� */
void resetlzwcodec(LZWCODEC *plc)
{
    LZWSTRITEM *strtab = (LZWSTRITEM*)plc->_lzw_str_tab;
    int i;

    /* init lzw prefix link list */
    for (i=0; i<plc->LZW_TAB_SIZE; i++) strtab[i].head = 0;

    /* reset lzw table position pointer */
    plc->_str_tab_pos = plc->LZW_END_CODE + 1;
}

/* �ر� lzw ������� */
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

/* LZW ���� */
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
        /* �� LZW ������в����ɵ�ǰǰ׺�͵�ǰ�ַ���ɵ��ַ��� */
        findcode = FindInLZWStringTable(plc, prefixcode, currentbyte);

        if (findcode == -1)
        {   /* �� LZW �������û�и��ַ��� */
            /* �Ե�ǰ�� codesize д����ǰǰ׺ */
            if (!putbits(fpout, drvout, prefixcode, curcodesize)) {
                log_printf("failed to write prefixcode !\n");
                return FALSE;
            }

            /* ����ǰǰ׺�͵�ǰ�ַ�����ɵ��ַ����������� */
            if (!AddToLZWStringTable(plc, prefixcode, currentbyte, TRUE))
            {   /* ����ʧ��˵����������� */
                /* д��һ�� LZW_CLEAR_CODE �������� */
                if (!putbits(fpout, drvout, plc->LZW_CLEAR_CODE, curcodesize)) {
                    log_printf("failed to write LZW_CLEAR_CODE !\n");
                    return FALSE;
                }
                resetlzwcodec(plc); /* reset lzw codec */
                curcodesize = plc->LZW_CODE_SIZE_MIN; /* reset curcodesize */
            }
            else 
            {   /* ���뵽�����ɹ� */
                /* ���� _str_tab_pos ���¼����µ� curcodesize */
                if (   plc->_str_tab_pos - 1 == (1 << curcodesize)
                    && curcodesize < plc->LZW_CODE_SIZE_MAX) /* note: ���������������Ҫ */
                {
                    /* ���� curcodesize ��ֵ */
                    curcodesize++;
                }
            }

            /* �õ�ǰǰ׺��Ϊ currentbyte */
            prefixcode = currentbyte;
        }
        else
        {   /* �� LZW ��������ҵ����ַ��� */
            /* �õ�ǰǰ׺��Ϊ���ַ����ı��� */
            prefixcode = findcode;
        }

#if ENABLE_PROGRESS_CALLBACK
        /* ���ñ�����Ȼص����� */
        if (plc->ppc) plc->ppc(0, drvin->tell(fpin));
#endif
    }

    /* �����Ҫд��ʣ������ֺͽ��������� */
    if (!putbits(fpout, drvout, prefixcode, curcodesize)) return FALSE;
    if (!putbits(fpout, drvout, plc->LZW_END_CODE, curcodesize)) return FALSE;
    if (!flushbits(fpout, drvout, 0)) return FALSE; /* �Ա������� flush ���� */

    /* ���سɹ� */
    return TRUE;
}

/* LZW ���� */
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

    /* ��һ�� while ѭ���н��н���
       ÿ�ζ��� LZW ���뵽 curcode ��
       Ȼ����� LZW �Ľ��봦��
       ֱ������ LZW_END_CODE ���� */
    while ( getbits(fpin, drvin, &curcode, curcodesize)
          && (int)curcode != plc->LZW_END_CODE )
    {
        /*
        log_printf("curcode = %d, curcodesize = %d\r\n", curcode, curcodesize);
        dump_gif_codec(plc);
        */

        if ((int)curcode == plc->LZW_CLEAR_CODE)
        {   /* ���������������� */
            resetlzwcodecfordecode(plc); /* reset lzw codec */
            oldcode     = -1; /* set -1 to restart */
            curcodesize = plc->LZW_CODE_SIZE_MIN; /* reset curcodesize */
        }
        else
        {   /* ��������Ĳ�������� */
            /* ���ұ�������Ƿ��иñ��� */
            bfind = IsCodeInLZWStringTable(plc, (int)curcode);

            /* ���ݲ��ҽ���������м����µ��ַ��� */
            if (bfind) AddToLZWStringTable(plc, oldcode, GetFirstByteOfLZWCode(plc, (int)curcode), FALSE);
            else AddToLZWStringTable(plc, oldcode, GetFirstByteOfLZWCode(plc, oldcode), FALSE);

            /* ���� _str_tab_pos ���¼����µ� curcodesize */
            if (   plc->_str_tab_pos == (1 << curcodesize)
                && curcodesize < plc->LZW_CODE_SIZE_MAX) // note: ���������������Ҫ
            {
                /* ���� curcodesize ��ֵ */
                curcodesize++;
            }

            /* д����ǰ�ַ�������������� */
            if (!OutputLZWString(plc, fpout, drvout, (int)curcode)) {
                log_printf("failed to write lzw string !\n");
                return FALSE;
            }

            /* oldcode ��Ϊ curcode */
            oldcode = (int)curcode;
        }

#if ENABLE_PROGRESS_CALLBACK
        /* ���ñ�����Ȼص����� */
        if (plc->ppc) plc->ppc(0, drvin->tell(fpin));
#endif
    }

    /* �Ա������� flush ���� */
    if (!flushbits(fpout, drvout, 0)) return FALSE;

    /* ���سɹ� */
    return TRUE;
}

#else
/* ����ͷ�ļ� */
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

