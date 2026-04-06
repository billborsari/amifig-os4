CC = /opt/ppc-amigaos/bin/ppc-amigaos-gcc
CXX = /opt/ppc-amigaos/bin/ppc-amigaos-g++
PREFIX  = 

DATADIR = 
CFLAGS  = -gstabs -O2 -Wall -fcommon -Isrc -Idev -DAMIGA -D__AROS__ -D__amigaos4__ -DGRADIENTS -DBITMAPDIR=\"progdir:bitmaps\" -Wno-write-strings -mcrt=newlib
CXXFLAGS= $(CFLAGS)
LDFLAGS = -lauto -mcrt=newlib

DEFINES = 
OBJS    =	dev/setfigfont.o \
			dev/readxbm.o \
			dev/readtif.o \
			dev/readppm.o \
			dev/readpng.o \
			dev/readpics.o \
			dev/readpcx.o \
			dev/readjpg.o \
			dev/readeps.o \
			dev/psencode.o \
			dev/gentpic.o \
			dev/gentk.o \
			dev/gentextyl.o \
			dev/gensvg.o \
			dev/genshape.o \
			dev/genptk.o \
			dev/genpstricks.o \
			dev/genpstex.o \
			dev/genps.oldpatterns.o \
			dev/genpictex.o \
			dev/genpic.o \
			dev/genpdf.o \
			dev/genmp.o \
			dev/genmf.o \
			dev/genmap.o \
			dev/genlatex.o \
			dev/genibmgl.o \
			dev/genge.o \
			dev/genepic.o \
			dev/genemf.o \
			dev/gendxf.o \
			dev/gencgm.o \
			dev/genbox.o \
			dev/genbitmaps.o \
			dev/asc85ec.o \
			src/trans_spline.o \
			src/strstr.o \
			src/stdafx.o \
			src/read_images.o \
			src/read.o \
			src/read1_3.o \
			src/psfonts.o \
			src/localmath.o \
			src/latex_line.o \
			src/iso2tex.o \
			src/getopt.o \
			src/free.o \
			src/fig2dev.o \
			src/colors.o \
			src/bound.o \
			src/arrow.o \
			src/mergesort.o \
            src/win32supp.o
	
TARGET  = fig2dev

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) -o $@ -c $<
	
clean:
	rm -f $(OBJS) $(TARGET) *~ src/*~
