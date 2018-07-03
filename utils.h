#ifndef __RGE_UTILS_H__
#define __RGE_UTILS_H__

#include "bmp.h"

/* �������� */
/* �������ڵ�ɫ��ת������ɫӳ���
   ����˵����
   srcpal - ָ��Դ��ɫ�壬srcpal �� [i * 4 + 3] Ԫ�����ڱ���ӳ��� map
   dstpal - ָ��Ŀ�ĵ�ɫ��
   ӳ���ϵ��
   Ŀ�ĵ�ɫ���ϵ���ɫ c ���� map[c] ӳ��󼴿ɵõ�����ɫ��Դ��ɫ���ϵ�ֵ
 */
void palutils_bitmap_palmap(BYTE *palmap, BYTE *dstpal, BYTE *srcpal);

/* �������ڿ���͸����ϵ���ɫӳ���
   ����˵����
   pal   - ָ���ɫ���ָ�룬���Ϊ NULL ��ʹ�� STDPAL ��ɫ��
   color - ���� alpha ��ϵ�ǰ��ɫ��32bit ARGB ��ʽ
 */
void palutils_alpha_palmap(BYTE *palmap, BYTE *pal, DWORD color);

#endif















