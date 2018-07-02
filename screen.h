#ifndef __RGE_SCREEN_H__
#define __RGE_SCREEN_H__

/* ����ͷ�ļ� */
#include "bmp.h"

/* Ԥ���뿪�� */
#define USE_GDI_SCREEN
/* #define USE_DDRAW_SCREEN */

/* �������� */
void RGE_WIN_INIT(HINSTANCE hInst); /* ��ʼ�� */
void RGE_MSG_LOOP(void); /* ��Ϣѭ�� */
HINSTANCE GET_APP_INSTANCE(void); /* ��ȡ RGE windows app ��ʵ����� */
HWND      GET_SCREEN_HWND (void); /* ��ȡ SCREEN �Ĵ��ھ�� */

/* WINSCREEN Ĭ�ϵĴ�����Ϣ������ */
LRESULT CALLBACK DEF_SCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
);

/* ȫ�ֱ������� */
extern BMP SCREEN;

#endif
