#include <exec/types.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>
#include <datatypes/pictureclass.h>
#include <proto/dos.h>
#include <stdio.h>

int main() {
    uint32 width = 100, height = 100;
    
    struct BitMap *bm = AllocBitMap(width, height, 32, BMF_MINPLANES | BMF_STANDARD, NULL);
    if (!bm) return 1;

    Object *dt = NewDTObject((APTR)NULL,
        DTA_SourceType, DTST_RAM,
        DTA_GroupID, GID_PICTURE,
        DTA_BaseName, (ULONG)"png",
        DTA_NominalHoriz, width,
        DTA_NominalVert, height,
        PDTA_BitMap, bm,
        TAG_DONE);

    if (!dt) { printf("Failed to create DTObject\n"); FreeBitMap(bm); return 1; }

    BPTR file = Open("RAM Disk:test.png", MODE_NEWFILE);
    if (file) {
        struct dtWrite dtw;
        dtw.MethodID = DTM_WRITE;
        dtw.dtw_GInfo = NULL;
        dtw.dtw_FileHandle = file;
        dtw.dtw_Mode = DTWM_RAW;
        dtw.dtw_AttrList = NULL;
        if (DoMethodA(dt, (Msg)&dtw)) {
            printf("WRITE Success\n");
        } else {
            printf("WRITE Failed\n");
        }
        Close(file);
    }
    DisposeDTObject(dt);
    return 0;
}
