#ifndef __RGE_SCANLINE_H__
#define __RGE_SCANLINE_H__

// ����ͷ�ļ�
#include "bmp.h"

// ��������
/* fill style */
enum {
    FS_NONE           = (1 << 31),
    FS_AUTO_LOCK      = (1 << 30),
    FS_BAR_SOLID      = (1 << 1 ),
    FS_BAR_ALPHA      = (1 << 2 ),
    FS_BMP_COPY       = (1 << 3 ),
    FS_BMP_MASK       = (1 << 4 ),
    FS_BMP_ALPHA      = (1 << 5 ),
    FS_256_COPYDATA   = (1 << 6 ),
    FS_256_COPYPAL    = (1 << 7 ),
    FS_256_PALMAPDST  = (1 << 8 ),
    FS_256_PALMAPSRC  = (1 << 9 ),
};

// ���Ͷ���
typedef struct {
    BMP  *srcbmp;
    DWORD style;
    DWORD fillc;
    DWORD maskc;
    int   alpha;
    BYTE *palmap;
    void *start;
    void *end;
} SCANLINEPARAMS;

typedef void (*PFNSCANLINE)(void *dst, void *src, int w, SCANLINEPARAMS *params);

// ȫ�ֱ�������
extern PFNSCANLINE TABFN_SCANLINEBAR_SOLID    [];
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