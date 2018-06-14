/* ��׼ͷ�ļ� */
#ifndef __STDEFINE_H__
#define __STDEFINE_H__

#if defined(WIN32) || defined(__MINGW32__)
#include <windows.h>
#else

/* �������� */
#define TRUE   1
#define FALSE  0

/* ��׼�����Ͷ��� */
typedef int BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long     int   LONG;

/* �������Ͷ��� */
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} RECT;

#endif

#define offsetof(type, member)          ((size_t)&((type*)0)->member)
#define container_of(ptr, type, member) (type*)((char*)(ptr) - offsetof(type, member))

/* �ú�������������δʹ�õľ��� */
#define DO_USE_VAR(var)  do { var = var; } while (0)

/* ���뿪�� */
// #define ENABLE_WIN32_DDRAW

#endif


