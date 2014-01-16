#ifndef _PALUTIL_H_
#define _PALUTIL_H_

#include "stdefine.h"

/* �������� */
/* �������ڵ�ɫ��ת������ɫӳ���
   ����˵����
   srcpal - ָ��Դ��ɫ�壬srcpal �� [i * 4 + 3] Ԫ�����ڱ���ӳ��� map
   dstpal - ָ��Ŀ�ĵ�ɫ��
   ӳ���ϵ��
   Ŀ�ĵ�ɫ���ϵ���ɫ c ���� map[c] ӳ��󼴿ɵõ�����ɫ��Դ��ɫ���ϵ�ֵ
 */
void palcolormap(BYTE *map, BYTE *srcpal, BYTE *dstpal);

/* �������ڿ���͸����ϵ���ɫӳ���
   ����˵����
   pal   - ָ���ɫ���ָ�룬���Ϊ NULL ��ʹ�� STDPAL ��ɫ��
   color - ���� alpha ��ϵ�ǰ��ɫ��32bit ARGB ��ʽ
 */
void alphacolormap(BYTE *map, BYTE *pal, DWORD color);

#endif















