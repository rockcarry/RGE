# makefile for RGE project
# written by rockcarry

# ����������
CC      = i586-mingw32msvc-gcc
CCFLAGS = -mwindows -Wall

# ���е�Ŀ���ļ�
OBJS = \
    log.o \
    bmp.o \
    bmpfile.o  \
    palette.o  \
    palutil.o  \
    draw2d.o   \
    bitblt.o   \
    font.o     \
    matchpal.o \
    win.o      \
    pcx.o      \
    lzw.o      \
    gif.o      \
    fiodrv.o   \
    pack.o     \
    fnps/tfps.o   \
    fnps/ftpa.o   \
    fnps/tfss.o   \
    fnps/tfsa.o   \
    fnps/tfsbfc.o \
    fnps/tfsbcc.o \
    fnps/tfsbma.o \
    fnps/pfspmp.o

# ���еĿ�ִ��Ŀ��
EXES = \
    bmpfile.exe \
    palette.exe \
    palutil.exe \
    draw2d.exe  \
    bitblt.exe  \
    font.exe    \
    matchpal.exe\
    win.exe     \
    pcx.exe     \
    lzw.exe     \
    gif.exe     \
    pack.exe

# ����Ŀ��ļ�
DLL = rge.dll

# �������
all : $(DLL) $(EXES)

$(DLL) : $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $(OBJS) --share -lddraw

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

%.exe : %.cpp %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL) -D_TEST_

clean :
	-rm -f *.o
	-rm -f *.dll
	-rm -f *.exe
	-rm -f *.bmp
	-rm -f fnps/*.o

# RockCarry
# 2006.10.4
# THE END



