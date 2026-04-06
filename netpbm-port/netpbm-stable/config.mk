# AmigaOS 4 config.mk for Netpbm
SRCDIR = .
DEFAULT_TARGET = nonmerge
BUILD_FIASCO = N

CC = ppc-amigaos-gcc
LD = ppc-amigaos-gcc
AR = ppc-amigaos-ar
RANLIB = ppc-amigaos-ranlib
LINKERISCOMPILER = Y
LINKER_CAN_DO_EXPLICIT_LIBRARY = N

INTTYPES_H = <inttypes.h>
HAVE_INT64 = Y
WANT_SSE = N

CC_FOR_BUILD = gcc
LD_FOR_BUILD = gcc
CFLAGS_FOR_BUILD = -O2
LDFLAGS_FOR_BUILD = 

INSTALL = $(SRCDIR)/buildtools/install.sh
STRIPFLAG = -s
SYMLINK = ln -s
MANPAGE_FORMAT = nroff
LEX = flex

# Disable auto-detection tools that might pick up host libraries
PKG_CONFIG = false
LIBPNG_CONFIG = false
XML2_CONFIG = false

# AmigaOS 4 specific CFLAGS
# -D__amigaos4__ is already defined by the compiler
# We add -D__USE_INLINE__ for some Amiga headers if needed
CFLAGS = -O2 -fcommon -DAMIGA -D__amigaos__ -I/opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include -I$(SRCDIR)/$(IMPORT_INC) -I. -I$(SRCDIR)/include -I$(SRCDIR)/lib/util
CFLAGS_CONFIG = $(CFLAGS)

EXE = 
LDFLAGS = -L/opt/ppc-amigaos/ppc-amigaos/SDK/local/newlib/lib
LDSHLIB = -shared

NETPBMLIBTYPE = unixstatic
NETPBMLIBSUFFIX = a
STATICLIB_TOO = Y
STATICLIBSUFFIX = a
SHLIBPREFIXLIST = lib
NETPBMSHLIBPREFIX = lib

# Library dependencies
TIFFLIB = -ltiff
TIFFHDR_DIR = /opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include
TIFFLIB_NEEDS_JPEG = Y
TIFFLIB_NEEDS_Z = Y

JPEGLIB = -ljpeg
JPEGHDR_DIR = /opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include

PNGLIB = -lpng
PNGHDR_DIR = /opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include
PNGVER = 

ZLIB = -lz
ZHDR_DIR = /opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include

JBIGLIB = NONE
JASPERLIB = NONE
URTLIB = $(BUILDDIR)/urt/librle.a
URTHDR_DIR = $(SRCDIR)/urt
X11LIB = NONE
LINUXSVGALIB = NONE

OMIT_NETWORK = Y
# AMIGA DOES NOT HAVE FORK
DONT_HAVE_PROCESS_MGMT = Y

# Path for color database on Amiga
RGB_DB_PATH = PROGDIR:rgb.txt:ETC:rgb.txt
