#!/bin/bash
set -e

CC=ppc-amigaos-gcc
AR=ppc-amigaos-ar
RANLIB=ppc-amigaos-ranlib

SDK_INC="-I/opt/ppc-amigaos/ppc-amigaos/SDK/local/common/include"
NETPBM_INC="-I. -Iimportinc -Ilib/util -Ilib"
CFLAGS="-O2 -fcommon -DAMIGA -D__amigaos__ $SDK_INC $NETPBM_INC -DNDEBUG"
LDFLAGS="-L/opt/ppc-amigaos/ppc-amigaos/SDK/local/newlib/lib -lm"

echo "Building helper objects..."
$CC $CFLAGS -c converter/other/pngx.c -o converter/other/pngx.o
$CC $CFLAGS -c converter/other/pngtxt.c -o converter/other/pngtxt.o
$CC $CFLAGS -c converter/other/jpegdatasource.c -o converter/other/jpegdatasource.o
$CC $CFLAGS -c converter/other/exif.c -o converter/other/exif.o
$CC $CFLAGS -c converter/other/tiff.c -o converter/other/tiff.o

# Create a small file with dummies for missing POSIX stuff if needed
cat > posix_amiga.c <<EOD
#include <errno.h>
#include <sys/types.h>
int pipe(int fildes[2]) { errno = ENOSYS; return -1; }
pid_t fork(void) { errno = ENOSYS; return -1; }
EOD
$CC $CFLAGS -c posix_amiga.c -o posix_amiga.o

echo "Building tools..."
$CC $CFLAGS converter/other/pamtogif.c libnetpbm.a $LDFLAGS -o pamtogif
$CC $CFLAGS converter/other/pnmtopng.c converter/other/pngx.o converter/other/pngtxt.o libnetpbm.a $LDFLAGS -lpng -lz -o pnmtopng
$CC $CFLAGS converter/other/pngtopam.c converter/other/pngx.o libnetpbm.a $LDFLAGS -lpng -lz -o pngtopam
$CC $CFLAGS converter/other/pnmtojpeg.c libnetpbm.a $LDFLAGS -ljpeg -o pnmtojpeg
$CC $CFLAGS converter/other/jpegtopnm.c converter/other/jpegdatasource.o converter/other/exif.o libnetpbm.a $LDFLAGS -ljpeg -o jpegtopnm
$CC $CFLAGS converter/other/tifftopnm.c converter/other/tiff.o posix_amiga.o libnetpbm.a $LDFLAGS -ltiff -ljpeg -lz -lwebp -lsharpyuv -lpthread -o tifftopnm
$CC $CFLAGS other/pnmcolormap.c libnetpbm.a $LDFLAGS -o pnmcolormap
$CC $CFLAGS editor/pnmremap.c libnetpbm.a $LDFLAGS -o pnmremap
$CC $CFLAGS converter/ppm/ppmtoxpm.c libnetpbm.a $LDFLAGS -o ppmtoxpm
$CC $CFLAGS converter/other/pnmtopclxl.c libnetpbm.a $LDFLAGS -o pnmtopclxl
$CC $CFLAGS converter/ppm/ppmtopcx.c libnetpbm.a $LDFLAGS -o ppmtopcx
$CC $CFLAGS converter/other/ppmtopgm.c libnetpbm.a $LDFLAGS -o ppmtopgm
$CC $CFLAGS converter/other/pgmtopbm.c libnetpbm.a $LDFLAGS -o pgmtopbm
$CC $CFLAGS converter/pbm/pbmtoxbm.c libnetpbm.a $LDFLAGS -o pbmtoxbm
$CC $CFLAGS converter/ppm/ppmtoacad.c libnetpbm.a $LDFLAGS -o ppmtoacad
$CC $CFLAGS converter/other/giftopnm.c libnetpbm.a $LDFLAGS -o giftopnm
$CC $CFLAGS converter/other/pamtopng.c converter/other/pngx.o converter/other/pngtxt.o libnetpbm.a $LDFLAGS -lpng -lz -o pamtopng
$CC $CFLAGS converter/other/pamtotiff.c converter/other/tiff.o posix_amiga.o libnetpbm.a $LDFLAGS -ltiff -ljpeg -lz -lwebp -lsharpyuv -lpthread -o pamtotiff

echo "Building pamtosvg..."
$CC $CFLAGS -c converter/other/pamtosvg/autotrace.c -o pamtosvg_at.o
$CC $CFLAGS -c converter/other/pamtosvg/bitmap.c -o pamtosvg_bm.o
$CC $CFLAGS -c converter/other/pamtosvg/curve.c -o pamtosvg_cv.o
$CC $CFLAGS -c converter/other/pamtosvg/epsilon-equal.c -o pamtosvg_ee.o
$CC $CFLAGS -c converter/other/pamtosvg/exception.c -o pamtosvg_ex.o
$CC $CFLAGS -c converter/other/pamtosvg/fit.c -o pamtosvg_ft.o
$CC $CFLAGS -c converter/other/pamtosvg/image-proc.c -o pamtosvg_ip.o
$CC $CFLAGS -c converter/other/pamtosvg/logreport.c -o pamtosvg_lr.o
$CC $CFLAGS -c converter/other/pamtosvg/output-svg.c -o pamtosvg_os.o
$CC $CFLAGS -c converter/other/pamtosvg/pxl-outline.c -o pamtosvg_po.o
$CC $CFLAGS -c converter/other/pamtosvg/spline.c -o pamtosvg_sp.o
$CC $CFLAGS -c converter/other/pamtosvg/thin-image.c -o pamtosvg_ti.o
$CC $CFLAGS -c converter/other/pamtosvg/vector.c -o pamtosvg_vc.o
$CC $CFLAGS converter/other/pamtosvg/pamtosvg.c pamtosvg_at.o pamtosvg_bm.o pamtosvg_cv.o pamtosvg_ee.o pamtosvg_ex.o pamtosvg_ft.o pamtosvg_ip.o pamtosvg_lr.o pamtosvg_os.o pamtosvg_po.o pamtosvg_sp.o pamtosvg_ti.o pamtosvg_vc.o libnetpbm.a $LDFLAGS -o pamtosvg

# Custom ppmquant wrapper
cat > ppmquant.c <<EOC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char cmd[2048];
    if (argc < 2) {
        fprintf(stderr, "Usage: ppmquant <ncolors> [file]\n");
        return 1;
    }
    const char *infile = (argc > 2) ? argv[2] : "-";
    
    // Step 1: Generate colormap
    snprintf(cmd, sizeof(cmd), "pnmcolormap %s %s > T:netpbm_colormap.tmp", argv[1], infile);
    if (system(cmd) != 0) return 1;
    
    // Step 2: Remap
    snprintf(cmd, sizeof(cmd), "pnmremap -mapfile=T:netpbm_colormap.tmp %s", infile);
    if (system(cmd) != 0) return 1;
    
    return 0;
}
EOC
$CC $CFLAGS ppmquant.c -o ppmquant

rm -rf bin
mkdir -p bin
cp pamtogif pnmtopng pngtopam pnmtojpeg jpegtopnm tifftopnm pnmcolormap pnmremap ppmtoxpm pnmtopclxl ppmtopcx ppmquant ppmtopgm pgmtopbm pbmtoxbm ppmtoacad giftopnm pamtopng pamtotiff pamtosvg libnetpbm.a bin/
cp pamtogif bin/ppmtogif

echo "Tools build complete."
