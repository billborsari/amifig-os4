# -------------------------------------------------
# Project created by QtCreator 2009-10-21T01:02:16
# -------------------------------------------------
# QT -= gui
CONFIG += qt

# CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += dev/setfigfont.c \
    dev/readxbm.c \
    dev/readtif.c \
    dev/readppm.c \
    dev/readpng.cpp \
    dev/readpics.c \
    dev/readpcx.c \
    dev/readjpg.c \
    dev/readeps.c \
    dev/psencode.c \
    dev/gentpic.c \
    dev/gentk.c \
    dev/gentextyl.c \
    dev/gensvg.c \
    dev/genshape.c \
    dev/genptk.c \
    dev/genpstricks.c \
    dev/genpstex.c \
    dev/genps.oldpatterns.c \
    dev/genpictex.c \
    dev/genpic.c \
    dev/genpdf.c \
    dev/genmp.c \
    dev/genmf.c \
    dev/genmap.c \
    dev/genlatex.c \
    dev/genibmgl.c \
    dev/genge.c \
    dev/genepic.c \
    dev/genemf.c \
    dev/gendxf.c \
    dev/gencgm.c \
    dev/genbox.c \
    dev/genbitmaps.c \
    dev/asc85ec.c \
    Windows/win32supp.cpp \
    src/trans_spline.c \
    src/strstr.c \
    src/stdafx.cpp \
    src/read_images.cpp \
    src/read.c \
    src/read1_3.c \
    src/psfonts.c \
    src/localmath.c \
    src/latex_line.c \
    src/iso2tex.c \
    src/getopt.c \
    src/free.c \
    src/fig2dev.cpp \
    src/colors.c \
    src/bound.c \
    src/arrow.c
HEADERS += stdafx.h \
    dev/tpicfonts.h \
    dev/texfonts.h \
    dev/setfigfont.h \
    dev/readxbm.h \
    dev/psimage.h \
    dev/psfonts.h \
    dev/psencode.h \
    dev/picpsfonts.h \
    dev/picfonts.h \
    dev/genps.oldpatterns.h \
    dev/genps.h \
    dev/genpdf.h \
    dev/genlatex.h \
    dev/genemf.h \
    Windows/win32supp.h \
    src/read.h \
    src/pi.h \
    src/patchlevel.h \
    src/object.h \
    src/localmath.h \
    src/free.h \
    src/fig2dev.h \
    src/drivers.h \
    src/bound.h \
    src/alloc.h \
    src/trans_spline.h
OTHER_FILES += dev/Makefile \
    dev/linewidth.ps \
    dev/korean.ps \
    dev/japanese.ps \
    dev/gensvg.c325p7 \
    dev/gensvg.c325 \
    dev/genpstricks.c.old2 \
    dev/genpstricks.c.old \
    dev/cs_CZ.ps
INCLUDEPATH += src
CONFIG += debug_and_release
CONFIG(debug, debug|release) { 
    TARGET = fig2devd
    DESTDIR = ./debug
}
CONFIG(release, debug|release) { 
    TARGET = fig2dev
    DESTDIR = ./release
}
DEFINES += BITMAPDIR \
    \"bitmaps\"
DEFINES += USE_PNG
DEFINES += I18N
DEFINES += FIG2DEV_LIBDIR \
    \"dev\"
DEFINES += NFSS
DEFINES += LATEX2E_GRAPHICS
DEFINES += DVIPS
DEFINES += WINFIG
