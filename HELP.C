#include <stdio.h>
#include <string.h>

#define INCL_WINDIALOGS
#define INCL_WINHELP
#include <os2.h>

#include "help.h"




/* ----------------------------------------------------------------- */

HWND
InitializeHelp (HAB hab, HWND hwndFrame, USHORT idWnd,
    PSZ pszHelpFilename, PSZ pszHelpTitle,
    HMODULE hmodHelpTableModule)
{
    HELPINIT    hinit;   /* Help initialization structure */
    HWND        hwndHelpInstance;
    CHAR        szMsg[100];

    memset (&hinit, 0, sizeof hinit);
    hinit.cb = sizeof (HELPINIT);
    hinit.ulReturnCode = 0;

    hinit.pszTutorialName = (PSZ)NULL;   /* if tutorial added, add name here */

    hinit.hmodHelpTableModule = hmodHelpTableModule;
    hinit.hmodAccelActionBarModule = 0;
    hinit.idAccelTable = 0;
    hinit.idActionBar = 0;
    hinit.phtHelpTable = (PHELPTABLE)MAKELONG(idWnd, 0xFFFF);
    hinit.pszHelpWindowTitle = pszHelpTitle;
    hinit.pszHelpLibraryName = pszHelpFilename;
//    hinit.usShowPanelId = CMIC_HIDE_PANEL_ID;

    hwndHelpInstance = WinCreateHelpInstance (hab, &hinit);

    if (hwndHelpInstance && hinit.ulReturnCode)
    {
	WinDestroyHelpInstance (hwndHelpInstance);
	hwndHelpInstance = 0;
    }

    if (!hwndHelpInstance)
    {
	sprintf (szMsg, "WinCreateHelpInstance failed, rc = %lx.  "
	    "Program will continue without help.",
	    hinit.ulReturnCode);

	WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, szMsg,
	    (PSZ) "Help Creation Error", 1,
	    MB_OK | MB_APPLMODAL | MB_MOVEABLE);
    }
    else
	WinAssociateHelpInstance (hwndHelpInstance, hwndFrame);

    return hwndHelpInstance;
}



/* ----------------------------------------------------------------- */

MRESULT
ProcessHmMessages (HWND hwndHelpInstance,
    USHORT msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case HM_ERROR:
	if (hwndHelpInstance && LONGFROMMP(mp1) == HMERR_NO_MEMORY)
	{
	    WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
		(PSZ) "Help Terminated Due to Error",
		(PSZ) "Help Error",
		1,
		MB_OK | MB_APPLMODAL | MB_MOVEABLE);
	    WinDestroyHelpInstance (hwndHelpInstance);
	}
	else
	{
            CHAR    szMsg[ 50 ];

            sprintf (szMsg, "Help Error %lx Occurred",
                LONGFROMMP (mp1));
	    WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
		szMsg, "Help Error", 0,
		MB_OK | MB_APPLMODAL | MB_MOVEABLE);
	}

	return (MRESULT) 0;
    }

    return (MRESULT) 0;
}

