#!/bin/bash
set -e

CC=ppc-amigaos-gcc
AR=ppc-amigaos-ar
RANLIB=ppc-amigaos-ranlib

SDK_INC="-I/opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include"
NETPBM_INC="-I. -Iimportinc -Ilib/util -Ilib"
CFLAGS="-O2 -fcommon -DAMIGA -D__amigaos__ $SDK_INC $NETPBM_INC -DNDEBUG"

echo "Building lib/util objects..."
UTIL_FILES="bitio.c filename.c io.c mallocvar.c matrix.c nsleep.c nstring.c rand.c randsysrand.c randsysrandom.c randmersenne.c runlength.c shhopt.c token.c vasprintf.c"
for f in $UTIL_FILES; do
    echo "Compiling lib/util/$f"
    $CC $CFLAGS -c lib/util/$f -o lib/util/${f%.c}.o
done

echo "Building lib core objects..."
CORE_FILES="libpm.c pmfileio.c fileio.c colorname.c libpamd.c libpbm1.c libpbm2.c libpbm3.c libpbmfont0.c libpbmfont1.c libpbmfont2.c pbmfontdata0.c pbmfontdata1.c pbmfontdata2.c libpgm1.c libpgm2.c libppm1.c libppm2.c libppmcmap.c libppmcolor.c libppmfuzzy.c libppmd.c ppmdfont.c standardppmdfont.c path.c libpnm1.c libpnm2.c libpnm3.c libpam.c libpamread.c libpamwrite.c libpamn.c libpammap.c libpamcolor.c"
for f in $CORE_FILES; do
    echo "Compiling lib/$f"
    $CC $CFLAGS -c lib/$f -o lib/${f%.c}.o
done

# libsystem_dummy.o
echo "Compiling lib/libsystem_dummy.c"
$CC $CFLAGS -c lib/libsystem_dummy.c -o lib/libsystem_dummy.o

echo "Creating libnetpbm.a..."
$AR rc libnetpbm.a lib/util/*.o lib/*.o
$RANLIB libnetpbm.a

echo "Build complete."
