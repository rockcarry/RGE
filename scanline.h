#ifndef __RGE_SCANLINE_H__
#define __RGE_SCANLINE_H__

/* 包含头文件 */
#include "bmp.h"

/* 常量定义 */
/* fill style */
enum {
    FS_AUTO_LOCK      = (1 << 31),
    FS_NONE           = (1 << 0 ),
    FS_SOLID          = (1 << 1 ),
    FS_PATTERN        = (1 << 2 ),
    FS_ALPHA          = (1 << 3 ),
    FS_BMP_COPY       = (1 << 4 ),
    FS_BMP_MASK       = (1 << 5 ),
    FS_BMP_ALPHA      = (1 << 6 ),
    FS_256_COPYDATA   = (1 << 7 ),
    FS_256_COPYPAL    = (1 << 8 ),
    FS_256_PALMAPDST  = (1 << 9 ),
    FS_256_PALMAPSRC  = (1 << 10),
};

/* 类型定义 */
typedef struct {
    BMP   *srcbmp;
    DWORD  style;
    DWORD  fillc;
    DWORD  maskc;
    int    alpha;
    BYTE  *palmap;
    void  *start;
    void  *end;
    int    fillsrcx;
    int    fillsrcy;
    int    filldstx;
    int    filldsty;
    DWORD *pattern;
} SCANLINEPARAMS;

typedef void (*PFNSCANLINE)(void *dst, void *src, int w, SCANLINEPARAMS *params);

/* 全局变量声明 */
extern PFNSCANLINE TABFN_SCANLINEBAR_SOLID    [];
extern PFNSCANLINE TABFN_SCANLINEBAR_PATTERN  [];
extern PFNSCANLINE TABFN_SCANLINEBAR_ALPHA    [];
extern PFNSCANLINE TABFN_SCANLINEBMP_FAST     [];
extern PFNSCANLINE TABFN_SCANLINEBMP_CONVERT  [];
extern PFNSCANLINE TABFN_SCANLINEBMP_MASK     [];
extern PFNSCANLINE TABFN_SCANLINEBMP_ALPHA    [];
extern PFNSCANLINE TABFN_SCANLINEBMP_MASKALPHA[];
extern PFNSCANLINE PFN_SCANLINE_PALMAP_DST;
extern PFNSCANLINE PFN_SCANLINE_PALMAP_SRC;
extern PFNSCANLINE PFN_SCANLINE_NONE;

#endif
