#ifndef _RGE_PACK_H_
#define _RGE_PACK_H_

/* 包含头文件 */
#include "fiodrv.h"

/* 类型定义 */
enum {
    TDT_START,     /* 开始遍历 */
    TDT_DONE,      /* 完成遍历 */
    TDT_FILE,      /* 文件 */
    TDT_DIR_ENTER, /* 进入目录 */
    TDT_DIR_EXIT , /* 退出目录 */
};

typedef BOOL (*PFN_TRAVEL_FUNC)(char *name, char *path, int depth, int type, void *param);

/* 函数声明 */
/* 遍历目录 */
BOOL travel(char *path, int depth, PFN_TRAVEL_FUNC func, void *param);

/* 打包和解包 */
BOOL dirdopack(char *dir, char *file);
BOOL dirunpack(char *dir, char *file);

/* 用于访问打包文件 */
extern FIODRV PACK_FIO_DRV;

#endif
