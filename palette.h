#ifndef _PALETTE_H_
#define _PALETTE_H_

/* ����ͷ�ļ� */
#include "stdefine.h"

/* �������� */
/* ���� 256 ɫ��׼��ɫ�� */
void createstdpal(BYTE *pal);

/* ��ɫ��ħ�� */
void magicpal(BYTE *pal1, BYTE *pal2);

/* �������Ƶ�ɫ�� */
void rightrotpal(BYTE *pal);
void leftrotpal (BYTE *pal);

/* ������ͻҶȻ���ɫ�� */
void randpal(BYTE *pal);
void graypal(BYTE *pal);

/* swap rgb order in palette */
enum
{
    SWAP_PAL_RGB,
    SWAP_PAL_RBG,
    SWAP_PAL_GRB,
    SWAP_PAL_GBR,
    SWAP_PAL_BRG,
    SWAP_PAL_BGR,
};
void swappal(BYTE *pal, int order);

/* ���ҵ�ɫ������ƥ�����ɫ */
BYTE PALRGB(BYTE *pal, BYTE r, BYTE g, BYTE b);

#endif







