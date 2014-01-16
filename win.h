#ifndef _RGE_WIN_H_
#define _RGE_WIN_H_

/* 包含头文件 */
#include "bmp.h"

/* 函数声明 */
void RGE_WIN_INIT(HINSTANCE hInst);
int  RGE_MSG_LOOP(void);

/* 获取 RGE windows app 的实例句柄 */
HINSTANCE RGE_GET_APP_INSTANCE(void);

/* 全局变量声明 */
extern BMP WINSCREEN;

/* 获取 WINSCREEN 的窗口句柄 */
HWND GET_WINSCREEN_HWND(void);

/* WINSCREEN 默认的窗口消息处理函数 */
LRESULT CALLBACK DEF_WINSCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);

#ifdef ENABLE_WIN32_DDRAW
/* DDRAWSCREEN 对象 */
extern BMP DDRAWSCREEN;

/* 获取 DDRAWSCREEN 的窗口句柄 */
HWND GET_DDRAWSCREEN_HWND(void);

/* DDRAWSCREEN 默认的窗口消息处理函数 */
LRESULT CALLBACK DEF_DDRAWSCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);
#endif

#ifdef ENABLE_WIN32_DDRAW
#define SCREEN             DDRAWSCREEN
#define GET_SCREEN_HWND    GET_DDRAWSCREEN_HWND
#define DEF_SCREEN_WNDPROC DEF_DDRAWSCREEN_WNDPROC
#else
#define SCREEN             WINSCREEN
#define GET_SCREEN_HWND    GET_WINSCREEN_HWND
#define DEF_SCREEN_WNDPROC DEF_WINSCREEN_WNDPROC
#endif

#endif
