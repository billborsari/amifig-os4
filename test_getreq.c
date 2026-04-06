#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <proto/exec.h>
#include <stdio.h>
#include <string.h>

ULONG GetStringFromUser(STRPTR buffer, ULONG maxlen, STRPTR title, APTR app)
{
    APTR WinReq, StrReq, ButOk, ButCancel;
    ULONG sigs = 0, id;
    BOOL running = TRUE;
    ULONG result = 0;
    
    if (!buffer) return 0;
    
    WinReq = WindowObject,
        MUIA_Window_Title, title,
        MUIA_Window_ID, MAKEID('S','T','R','Q'),
        
        WindowContents, VGroup,
            Child, StrReq = StringObject,
                MUIA_String_Contents, buffer,
                MUIA_String_MaxLen, maxlen,
                MUIA_Frame, MUIV_Frame_String,
            End,
            Child, HGroup,
                Child, ButOk = SimpleButton("_Ok"),
                Child, ButCancel = SimpleButton("_Cancel"),
            End,
        End,
    End;

    if (!WinReq) return 0;
    
    DoMethod(WinReq, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
             
    DoMethod(ButOk, MUIM_Notify, MUIA_Pressed, FALSE,
             app, 2, MUIM_Application_ReturnID, 1001);
             
    DoMethod(ButCancel, MUIM_Notify, MUIA_Pressed, FALSE,
             app, 2, MUIM_Application_ReturnID, 1002);
             
    DoMethod(StrReq, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             app, 2, MUIM_Application_ReturnID, 1001);

    DoMethod(app, OM_ADDMEMBER, WinReq);
    set(WinReq, MUIA_Window_Open, TRUE);
    set(WinReq, MUIA_Window_ActiveObject, StrReq);

    while(running)
    {
        id = DoMethod(app, MUIM_Application_Input, &sigs);
        
        if (id == MUIV_Application_ReturnID_Quit || id == 1002)
        {
            running = FALSE;
            result = 0;
        }
        else if (id == 1001)
        {
            STRPTR temp = NULL;
            GetAttr(MUIA_String_Contents, StrReq, (ULONG *)&temp);
            printf("temp string from struct: '%s'\n", temp ? temp : "NULL");
            if (temp) {
                strncpy(buffer, temp, maxlen-1);
                buffer[maxlen-1] = '\0';
            }
            running = FALSE;
            result = 1;
        }
        
        if (running && sigs)
        {
            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
            if (sigs & SIGBREAKF_CTRL_C) running = FALSE;
        }
    }
    
    set(WinReq, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WinReq);
    MUI_DisposeObject(WinReq);
    
    return result;
}

int main() {
    APTR app = ApplicationObject, End;
    char buffer[256] = "Initial Text";
    // GetStringFromUser(buffer, 256, "Test", app); // This will block for UI interaction!
    return 0;
}
