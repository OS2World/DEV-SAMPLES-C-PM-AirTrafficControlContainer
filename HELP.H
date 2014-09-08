
HWND
InitializeHelp (HAB hab, HWND hwndFrame, USHORT idWnd,
    PSZ pszHelpFilename, PSZ pszHelpTitle,
    HMODULE hmodHelpTableModule);

MRESULT
ProcessHmMessages (HWND hwndHelpInstance,
    USHORT msg, MPARAM mp1, MPARAM mp2);
