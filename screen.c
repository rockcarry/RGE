#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include <dos.h>
#include "screen.h"

/* 内部函数声明 */
static BOOL _vgadrv_createbmp (void *pb);
static void _vgadrv_destroybmp(void *pb);
static void _vgadrv_lock  (void *pb);
static void _vgadrv_unlock(void *pb);
static void _vgadrv_setpal(void *pb, int i, int n, BYTE *pal);
static void _vgadrv_getpal(void *pb, int i, int n, BYTE *pal);

BMP VGASCREEN =
{
    320, 200, 8, 320, {0, 0, 319, 199}, NULL, NULL, NULL,
    _vgadrv_createbmp,
    _vgadrv_destroybmp,
    _vgadrv_lock,
    _vgadrv_unlock,
    _vgadrv_setpal,
    _vgadrv_getpal,
};

/* 内部函数实现 */
static BOOL _vgadrv_createbmp(void *pb)
{
    BMP *pbmp = (BMP*)pb;
    union REGS regs = {0};
    regs.h.ah = 0x00;
    regs.h.al = 0x13;
    int86(0x10, &regs, &regs);
    pbmp->pdata = (void*)0xA0000000L;
    _vgadrv_setpal(pbmp, 0, 256, pbmp->ppal);
    return TRUE;
}

static void _vgadrv_destroybmp(void *pb)
{
    BMP *pbmp = (BMP*)pb;
    union REGS regs = {0};
    regs.h.ah = 0x00;
    regs.h.al = 0x03;
    int86(0x10, &regs, &regs);
    pbmp->pdata = NULL;
}

static void _vgadrv_lock(void *pb)
{
    DO_USE_VAR(pb);
}

static void _vgadrv_unlock(void *pb)
{
    DO_USE_VAR(pb);
}

static void _vgadrv_setpal(void *pb, int i, int n, BYTE *pal)
{
    DO_USE_VAR(pb);
    outportb(0x3C8, i);
    while (n--) {
        outportb(0x3C9, pal[2] >> 2);
        outportb(0x3C9, pal[1] >> 2);
        outportb(0x3C9, pal[0] >> 2);
        pal += 4;
    }
}

static void _vgadrv_getpal(void *pb, int i, int n, BYTE *pal)
{
    DO_USE_VAR(pb);
    outportb(0x3C7, i);
    while (n--) {
        pal[2] = inportb(0x3C9) << 2;
        pal[1] = inportb(0x3C9) << 2;
        pal[0] = inportb(0x3C9) << 2;
        pal += 4;
    }
}

#else
/* 包含头文件 */
#include "screen.h"
void main(void)
{
    int i, j;

    createbmp(&VGASCREEN);

    lockbmp(&VGASCREEN);
    for (i=0; i<VGASCREEN.height; i++)
        for (j=0; j<VGASCREEN.width; j++)
            *(WORD*)(VGASCREEN.pdata + i * VGASCREEN.stride + j * 2) = RGB332(i, j, i);
    unlockbmp(&VGASCREEN);

    getch();
    destroybmp(&SCREEN);
    return 0;
}
#endif


