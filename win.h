#ifndef _RGE_WIN_H_
#define _RGE_WIN_H_

/* ����ͷ�ļ� */
#include "bmp.h"

/* �������� */
void RGE_WIN_INIT(HINSTANCE hInst);
int  RGE_MSG_LOOP(void);

/* ��ȡ RGE windows app ��ʵ����� */
HINSTANCE RGE_GET_APP_INSTANCE(void);

/* ȫ�ֱ������� */
extern BMP WINSCREEN;

/* ��ȡ WINSCREEN �Ĵ��ھ�� */
HWND GET_WINSCREEN_HWND(void);

/* WINSCREEN Ĭ�ϵĴ�����Ϣ������ */
LRESULT CALLBACK DEF_WINSCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);

#ifdef ENABLE_WIN32_DDRAW
/* DDRAWSCREEN ���� */
extern BMP DDRAWSCREEN;

/* ��ȡ DDRAWSCREEN �Ĵ��ھ�� */
HWND GET_DDRAWSCREEN_HWND(void);

/* DDRAWSCREEN Ĭ�ϵĴ�����Ϣ������ */
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
