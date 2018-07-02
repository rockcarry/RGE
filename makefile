# makefile for RGE project
# written by rockcarry

# ����������
CC      = tcc
CCFLAGS = -1 -O -d -Z -f87 -w -ml

# ���е�Ŀ���ļ�
OBJS = \
    log.obj \
    fio.obj \
    pal.obj \
    bmp.obj \
    pcx.obj \
    lzw.obj \
    gif.obj \
    screen.obj \
    pixel.obj \
    slbasic.obj \
    slcvert.obj \
    slalpha.obj \
    slpalmap.obj \
    font.obj \
    bitblt.obj \
    draw2d.obj \
    drawctxt.obj \
    palutils.obj \
    fftask.obj \
    wavdev.obj

# ���еĿ�ִ��Ŀ��
EXES = \
    pal.exe \
    bmp.exe \
    pcx.exe \
    lzw.exe \
    gif.exe \
    font.exe \
    bitblt.exe \
    draw2d.exe \
    palutils.exe

# ����Ŀ��ļ�
LIB = rge.lib

# �������
all : $(LIB) $(EXES)

$(LIB) : $(OBJS)
	$(foreach n, $(OBJS), $(shell tlib $@ -+ $(n)))

%.obj : %.c %.h stdefine.h
	$(CC) $(CCFLAGS) -c -o$@ $<

%.obj : %.c stdefine.h
	$(CC) $(CCFLAGS) -c -o$@ $<

%.exe : %.c %.h $(LIB)
	$(CC) $(CCFLAGS) -c -otemp.obj -D_TEST_ $<
	$(CC) $(CCFLAGS) -e$@ temp.obj $(LIB)
	rm -f TEMP.OBJ

clean :
	-rm -f *.OBJ
	-rm -f *.LIB
	-rm -f *.BAK
	-rm -f *.EXE
	-rm -f *.bmp
	-rm -f *.pcx
	-rm -f *.gif
	-rm -f *.log

# rockcarry
# 2018.6.12
# THE END



