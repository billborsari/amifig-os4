
/*----------------------------------------------------------------------------*/
/*     GetStringFromUser - Simple string requester (Modal)                    */
/*----------------------------------------------------------------------------*/
ULONG GetStringFromUser(STRPTR buffer, ULONG maxlen, STRPTR title)
{
    APTR WinReq, StrReq, ButOk, ButCancel;
    ULONG sigs = 0, id;
    BOOL running = TRUE;
    ULONG result = 0;
    
    if (!buffer) return 0;
    
    WinReq = WindowObject,
        MUIA_Window_Title, title,
        MUIA_Window_RefWindow, WinMode,
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
             App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
             
    DoMethod(ButOk, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, 1001);
             
    DoMethod(ButCancel, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, 1002);
             
    DoMethod(StrReq, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             App, 2, MUIM_Application_ReturnID, 1001);

    DoMethod(App, OM_ADDMEMBER, WinReq);
    set(WinReq, MUIA_Window_Open, TRUE);
    set(WinReq, MUIA_Window_ActiveObject, StrReq);

    while(running)
    {
        id = DoMethod(App, MUIM_Application_Input, &sigs);
        
        if (id == MUIV_Application_ReturnID_Quit || id == 1002)
        {
            running = FALSE;
            result = 0;
        }
        else if (id == 1001)
        {
            STRPTR temp = NULL;
            get(StrReq, MUIA_String_Contents, &temp);
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
    DoMethod(App, OM_REMOVEMEMBER, WinReq);
    MUI_DisposeObject(WinReq);
    
    return result;
}
