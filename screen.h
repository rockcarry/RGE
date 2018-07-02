#ifndef __RGE_SCREEN_H__
#define __RGE_SCREEN_H__

/* 包含头文件 */
#include "bmp.h"

/* 预编译开关 */
#define USE_GDI_SCREEN
/* #define USE_DDRAW_SCREEN */

/* 函数声明 */
void RGE_WIN_INIT(HINSTANCE hInst); /* 初始化 */
void RGE_MSG_LOOP(void); /* 消息循环 */
HINSTANCE GET_APP_INSTANCE(void); /* 获取 RGE windows app 的实例句柄 */
HWND      GET_SCREEN_HWND (void); /* 获取 SCREEN 的窗口句柄 */

/* WINSCREEN 默认的窗口消息处理函数 */
LRESULT CALLBACK DEF_SCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);

/* 全局变量声明 */
extern BMP SCREEN;

#endif
