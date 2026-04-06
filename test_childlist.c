#include <exec/types.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>

void test() {
    APTR list[10];
    list[0] = NULL;
    APTR grp = MUI_NewObject(MUIC_Group, MUIA_Group_ChildList, (IPTR)list, TAG_DONE);
}
int main() { return 0; }
