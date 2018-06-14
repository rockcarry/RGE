/* 标准头文件 */
#ifndef __STDEFINE_H__
#define __STDEFINE_H__

#if defined(WIN32) || defined(__MINGW32__)
#include <windows.h>
#else

/* 常量定义 */
#define TRUE   1
#define FALSE  0

/* 标准的类型定义 */
typedef int BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long     int   LONG;

/* 矩形类型定义 */
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} RECT;

#endif

#define offsetof(type, member)          ((size_t)&((type*)0)->member)
#define container_of(ptr, type, member) (type*)((char*)(ptr) - offsetof(type, member))

/* 该宏用于消除变量未使用的警告 */
#define DO_USE_VAR(var)  do { var = var; } while (0)

/* 编译开关 */
// #define ENABLE_WIN32_DDRAW

#endif


