# makefile for RGE project
# written by rockcarry

# ����������
CC      = gcc
STRIP   = strip
CCFLAGS = -mwindows -Wall

# ���е�Ŀ���ļ�
OBJS = \
    log.o \
    fio.o \
    pal.o \
    bmp.o \
    pcx.o \
    lzw.o \
    gif.o \
    win.o \
    pixel.o \
    slbasic.o \
    slcvert.o \
    slalpha.o \
    slpalmap.o \
    font.o \
    bitblt.o \
    draw2d.o \
    drawctxt.o \
    palutils.o


# ���еĿ�ִ��Ŀ��
EXES = \
    pal.exe \
    bmp.exe \
    pcx.exe \
    lzw.exe \
    gif.exe \
    win.exe \
    font.exe \
    bitblt.exe \
    draw2d.exe \
    palutils.exe

# ����Ŀ��ļ�
DLL = rge.dll

# �������
all : $(DLL) $(EXES)

$(DLL) : $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $(OBJS) --share -lddraw
	$(STRIP) $@

%.o : %.c %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.c stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.exe : %.c %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL) -D_TEST_
	$(STRIP) $@

%.exe : %.cpp %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL) -D_TEST_
	$(STRIP) $@

clean :
	-rm -f *.o
	-rm -f *.dll
	-rm -f *.exe
	-rm -f *.bmp
	-rm -f *.pcx
	-rm -f *.gif
	-rm -f *.log

# rockcarry
# 2018.6.12
# THE END



