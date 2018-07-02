# makefile for RGE project
# written by rockcarry

# 编译器定义
CC      = tcc
CCFLAGS = -1 -O -d -Z -f87 -w -ml

# 所有的目标文件
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

# 所有的可执行目标
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

# 输出的库文件
LIB = rge.lib

# 编译规则
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



