/*

    AIR TRAFFIC CONTROL CONTAINER

    This sample demonstrates the following:

        Multiple Container views
        Inserting multiple container records
        Explicitly positioning records
        Enumerating container contents
        Using a PM Timer to move the container objects at regular intervals
        Multiple context menus
        Container background owner-drawing
        Bitmaps for screen drawing performance
        Randomly generating aircraft (always good to know)

    See online help for more information.

    Dave Briccetti              Phone: 510 945-7565
    Dave Briccetti & Assoc.     Fax: 510 945-7436
    P.O. Box 1713               CIS: 74475,1072
    Lafayette, CA 94549-7013    Internet: daveb@netcom.com

    Copyright (c) David C. Briccetti, 1993
    All rights reserved.

    Dave Briccetti has been developing OS/2 applications on a consulting
    basis since 1987, and specializes in PM user interfaces.

*/

#define INCL_PM
#include <os2.h>

#include "atccnr.h"
#include "help.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/*
 *  AIRCRAFT CONTAINER RECORD STRUCTURE
 */

typedef struct _APPCNRREC     // acr
{
    MINIRECORDCORE  recc;
    LONG            iRecord;            // Index of container record
    LONG            Altitude;           // Aircraft attributes
    LONG            Heading;            // ...
    LONG            Speed;
    LONG            AssignedAltitude;
    LONG            AssignedHeading;
    LONG            AssignedSpeed;
    PSZ             pszCallsign;
} APPCNRREC, *PAPPCNRREC;


/*
 *  APPLICATION WINDOW "INSTANCE" DATA
 */

typedef struct _INST     // inst
{
    HAB             hab;                // Anchor block handle for application
    HPS             hpsMemRadar;        // Memory PS where Radar bitmap stored
    HDC             hdcMem;             // Memory DC where Radar bitmap stored
    HBITMAP         hbmRadar;           // Radar screen bitmap
    HWND            hwndCnr;            // Container window handle
    HWND            hwndAircraftMenu;   // Aircraft manipulation menu
    HWND            hwndContainerMenu;  // Container manipulation menu
    PAPPCNRREC      pacrSelected;       // Ptr to selected container record
    LONG            CurrentView;        // Current view (IDMVIEW_ICON, etc.)
}   INST, *PINST;


/*
 *  FUNCTION DECLARATIONS
 */

static FNWP AirTrafficControlDlg;
static INT InsertContainerColumns (PINST pinst);
static INT InsertContainerRecords (PINST pinst);
static PSZ SetRecordText (PAPPCNRREC pacr);
static PFNWP pfnwpCnr;
static FNWP CnrSubclassWndProc;
static MRESULT EXPENTRY ControlInstructionDlg (HWND hwnd, ULONG msg,
    MPARAM mp1, MPARAM mp2);
static BOOL ManeuverAircraft (PAPPCNRREC pacrTraverse);
static HBITMAP CreateBitmap (HPS hpsMem, PRECTL prclTargetWnd);
static INT CreateMemoryPS (PINST pinstMain, HPS hpsTarget,
    PRECTL prclTargetWnd);
static INT
DrawBitmap
(
    PINST           pinstMain,
    HPS             hpsTarget,
    PRECTL          prclTargetWndDraw
);


/*
 *  CONSTANTS AND MACROS
 */

#define IDTIMER_ADJUSTAIRCRAFT              100
#define AIRCRAFT_ADJUST_FREQUENCY           1000    // milliseconds

#define FIRST_CNR_ITEM(hwnd) WinSendMsg (hwnd, CM_QUERYRECORD, \
    0, MPFROM2SHORT (CMA_FIRST,CMA_ITEMORDER))

#define NEXT_CNR_ITEM(hwnd,prev) WinSendMsg (hwnd, CM_QUERYRECORD, \
    MPFROMP (prev), MPFROM2SHORT (CMA_NEXT,CMA_ITEMORDER))



/*
 *  MAIN FUNCTION
 */

int main( void )
{
    HAB     hab;
    HMQ     hmq;
    HWND    hwndHelpInstance;   /* Handle to Help window    */
    HWND    hwndDlg;            // Main dlg window
    static SWCNTRL swctl;


    freopen ("err", "w", stderr);

    assert (hab = WinInitialize (0));
    assert (hmq = WinCreateMsgQueue (hab, 0));

    // Load main application dialog
    hwndDlg = WinLoadDlg (HWND_DESKTOP, HWND_DESKTOP,
        AirTrafficControlDlg, 0, IDDLG_CNRSAMP, 0);

    swctl.hwnd = hwndDlg;
    strcpy( swctl.szSwtitle, "Air Traffic Control Container" );
    WinAddSwitchEntry( &swctl );

    hwndHelpInstance = InitializeHelp (hab, hwndDlg, IDDLG_CNRSAMP,
        (PSZ) "atccnr.HLP", (
        PSZ) "Air Traffic Control Container Help Window", 0);

    WinProcessDlg (hwndDlg);

    if (hwndHelpInstance)
        WinDestroyHelpInstance (hwndHelpInstance);

    WinDestroyWindow (hwndDlg);

    WinDestroyMsgQueue (hmq);
    WinTerminate (hab);

    return 0;
}



/*
 *  DIALOG PROCEDURE FOR MAIN APPLICATION WINDOW
 */

MRESULT EXPENTRY AirTrafficControlDlg (HWND hwnd, ULONG msg,
    MPARAM mp1, MPARAM mp2)
{
    PINST   pinst;

    pinst = WinQueryWindowPtr(hwnd,QWL_USER);

    switch (msg)
    {
        case WM_INITDLG:
        {
            /*
             *  INITIALIZE APPLICATION
             */

            CNRINFO     ccinfo;         // Container info structure

            // Initialize instance data
            pinst = calloc (1, sizeof *pinst);
            WinSetWindowPtr(hwnd,QWL_USER,pinst);
            pinst->hwndCnr = WinWindowFromID (hwnd, IDCNR_SAMPLE);
            pinst->hab     = WinQueryAnchorBlock (hwnd);
            pinst->hwndAircraftMenu =               // Load aircraft menu
                WinLoadMenu( hwnd, 0, IDMENU_AIRCRAFT);
            pinst->hwndContainerMenu =              // Load container menu
                WinLoadMenu( hwnd, 0, IDMENU_CONTAINER);
            pinst->CurrentView = IDMVIEW_RADAR;

            // Set container info to request owner background painting
            memset (&ccinfo, 0, sizeof ccinfo);
            ccinfo.flWindowAttr = CA_OWNERPAINTBACKGROUND;
            WinSendMsg (pinst->hwndCnr, CM_SETCNRINFO, &ccinfo,
                MPFROMLONG(CMA_FLWINDOWATTR));

            // Subclass container window to provide background painting
            pfnwpCnr = WinSubclassWindow (pinst->hwndCnr,
                CnrSubclassWndProc);
            assert (pfnwpCnr);

            // Insert the columns into the container
            InsertContainerColumns (pinst);

            // Insert the initial records into the container
            InsertContainerRecords (pinst);

            // Start a timer to control aircraft
            WinStartTimer (pinst->hab, hwnd,
                IDTIMER_ADJUSTAIRCRAFT, AIRCRAFT_ADJUST_FREQUENCY);

            // The container is ready for viewing
            WinShowWindow (pinst->hwndCnr, TRUE);

            return 0;
        }

        case WM_TIMER:

            /*
             *  DO TIME-RELATED AIRCRAFT HANDLING
             */

            if (SHORT1FROMMP (mp1) == IDTIMER_ADJUSTAIRCRAFT)
            {
                PAPPCNRREC      pacrTraverse;

                // Loop through all container records
                for (pacrTraverse = (PAPPCNRREC)
                    FIRST_CNR_ITEM (pinst->hwndCnr);
                    pacrTraverse;
                    pacrTraverse = (PAPPCNRREC)
                    NEXT_CNR_ITEM(pinst->hwndCnr,pacrTraverse))
                {
                    BOOL        fInvalid = FALSE;

                    if (pinst->CurrentView == IDMVIEW_RADAR)
                    {
                        /* Erase the record from previous location. */
                        WinSendMsg (pinst->hwndCnr, CM_ERASERECORD,
                            MPFROMP(pacrTraverse), NULL);
                    }

                    // Maneuver the aircraft
                    if (ManeuverAircraft (pacrTraverse))
                    {
                        // Update aircraft text if necessary
                        SetRecordText (pacrTraverse);
                        fInvalid = TRUE;
                    }

                    if (fInvalid || pinst->CurrentView == IDMVIEW_RADAR)
                        /* Paint the record in its new position */
                        WinSendMsg (pinst->hwndCnr, CM_INVALIDATERECORD,
                            MPFROMP(&pacrTraverse),
                            MPFROMSHORT(1));
                }
            }

            return 0;


        case WM_CONTROL:
            if (SHORT1FROMMP (mp1) == IDCNR_SAMPLE)
                switch (SHORT2FROMMP (mp1))
                {
                    case CN_CONTEXTMENU:
                    {
                        /*
                         *  MENU HANDLING
                         */

                        POINTL  ptlMouse;
                        HWND    hwndMenu;

                        WinQueryMsgPos (pinst->hab, &ptlMouse);

                        // Save pointer to record mouse is over, if any
                        pinst->pacrSelected = (PAPPCNRREC) mp2;

                        hwndMenu = pinst->pacrSelected ?
                            pinst->hwndAircraftMenu :
                            pinst->hwndContainerMenu;

                        // Display menu
                        WinPopupMenu (HWND_DESKTOP, hwnd, hwndMenu,
                            ptlMouse.x, ptlMouse.y, 0,
                            PU_HCONSTRAIN | PU_VCONSTRAIN |
                            PU_KEYBOARD | PU_MOUSEBUTTON1 |
                            PU_MOUSEBUTTON2 |
                            PU_MOUSEBUTTON3);
                    }
                }
            return 0;


        case WM_COMMAND:

            /*
             *  COMMAND HANDLING
             */

            switch (SHORT1FROMMP (mp1))
            {
                case IDMAIRCRAFT_CONTROLINSTRUCT:   // Issue instruction
                {
                    // Present "Control Aircraft" dialog
                    WinDlgBox (HWND_DESKTOP, hwnd,
                        ControlInstructionDlg, 0,
                        IDDLG_CONTROLAIRCRAFT, pinst);
                    return 0;
                }

                case IDMVIEW_RADAR:
                case IDMVIEW_NAME:
                case IDMVIEW_DETAILS:
                case IDMVIEW_TEXT:
                {
                    static const ULONG  aViews [] =
                        { CV_ICON, CV_NAME, CV_DETAIL, CV_TEXT };

                    CNRINFO ccinfo;
                    memset (&ccinfo, 0, sizeof ccinfo);
                    ccinfo.flWindowAttr = CA_DETAILSVIEWTITLES |
                        CA_TITLESEPARATOR |
                        CA_OWNERPAINTBACKGROUND |
                        aViews [SHORT1FROMMP (mp1) - IDMVIEW_BASE];
                    WinSendMsg (pinst->hwndCnr, CM_SETCNRINFO, &ccinfo,
                        MPFROMLONG(CMA_FLWINDOWATTR));
                    pinst->CurrentView = SHORT1FROMMP (mp1);

                    WinInvalidateRect (pinst->hwndCnr, 0, 0);
                    return 0;
                }
            }

            return WinDefDlgProc (hwnd, msg, mp1, mp2);

        default:
            return WinDefDlgProc (hwnd, msg, mp1, mp2);
    }
}





/*
 *  INSERT COLUMNS INTO CONTAINER
 */

static INT InsertContainerColumns (PINST pinst)
{
    PFIELDINFO      pfldinfo;
    PFIELDINFO      pfldinfoFirst;
    FIELDINFOINSERT fiins;
    LONG            iField = 0;
    static const FIELDINFO afldinfo[] =
    {
        {0, CFA_STRING | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY, "Call sign",
         FIELDOFFSET(APPCNRREC,pszCallsign), 0, 0, 0},
        {0, CFA_ULONG | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY, "Heading",
         FIELDOFFSET(APPCNRREC,Heading), 0, 0, 0},
        {0, CFA_ULONG | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY, "Assigned\nHeading",
         FIELDOFFSET(APPCNRREC,AssignedHeading), 0, 0, 0},
        {0, CFA_ULONG | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY, "Altitude\n(Hundreds\nof Feet)",
         FIELDOFFSET(APPCNRREC,Altitude), 0, 0, 0},
        {0, CFA_ULONG | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY,
         "Assigned\nAltitude\n(Hundreds\nof Feet)",
         FIELDOFFSET(APPCNRREC,AssignedAltitude), 0, 0, 0},
        {0, CFA_ULONG | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY, "Speed\n(Tens of\nknots)",
         FIELDOFFSET(APPCNRREC,Speed), 0, 0, 0},
        {0, CFA_ULONG | CFA_FIREADONLY | CFA_HORZSEPARATOR | CFA_LEFT,
         CFA_LEFT | CFA_FITITLEREADONLY, "Assigned\nSpeed\n(Tens of\nknots",
         FIELDOFFSET(APPCNRREC,AssignedSpeed), 0, 0, 0}
    };
    static const INT cFields = sizeof afldinfo / sizeof afldinfo[0];

    pfldinfo = pfldinfoFirst = WinSendMsg (pinst->hwndCnr,
        CM_ALLOCDETAILFIELDINFO,
        MPFROMLONG(cFields), NULL);

    for (iField = 0; iField < cFields; ++iField)
    {
        pfldinfo->cb         = sizeof(FIELDINFO);
        pfldinfo->flData     = afldinfo [iField].flData;
        pfldinfo->flTitle    = afldinfo [iField].flTitle;
        pfldinfo->pTitleData = afldinfo [iField].pTitleData;
        pfldinfo->offStruct  = afldinfo [iField].offStruct;
        pfldinfo = pfldinfo->pNextFieldInfo;
    }

    fiins.cb = sizeof fiins;
    fiins.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
    fiins.cFieldInfoInsert = cFields;
    fiins.fInvalidateFieldInfo = TRUE;

    /*
     *  INSERT DETAIL VIEW INFORMATION
     */

    WinSendMsg (pinst->hwndCnr, CM_INSERTDETAILFIELDINFO,
        MPFROMP(pfldinfoFirst), MPFROMP(&fiins));


    return 0;
}



/*
 *  INSERT AIRCRAFT INTO RADAR SCREEN CONTAINER
 */

static INT InsertContainerRecords (PINST pinst)
{
    PAPPCNRREC      pacrRoot;           // Ptr to root of list of records
    PAPPCNRREC      pacrTraverse;       // Ptr to traverse list of records
    RECORDINSERT    recins;             // Record insertion information
    HPOINTER        hptrIcon;           // Handle of jet icon
    RECTL           rclCnr;             // Container dimensions
    LONG            iRecord = 0;        // Record index
    static const PSZ apszCallSignPrefixes [] = { "AA", "DA", "UA", "TW" };
    static const INT cPrefixes = sizeof apszCallSignPrefixes /
        sizeof apszCallSignPrefixes[0];
    CHAR            szBuf [30];

    /*
     *  SET UP RECORD INSERT STRUCTURE
     */

    memset (&recins, 0, sizeof recins);
    recins.cb                = sizeof recins;
    recins.pRecordOrder      = (PRECORDCORE) CMA_END;
    recins.zOrder            = CMA_TOP;
    recins.cRecordsInsert    = 5;
    recins.fInvalidateRecord = TRUE;


    /*
     *  ALLOCATE CONTAINER ITEM LINKED LIST
     */

    pacrRoot = (PAPPCNRREC) WinSendMsg (
        pinst->hwndCnr, CM_ALLOCRECORD,
        MPFROMLONG(sizeof(APPCNRREC) - sizeof(MINIRECORDCORE)),
        MPFROMLONG(recins.cRecordsInsert));


    /*
     *  SET UP USER RECORD LINKED LIST
     */

    hptrIcon = WinLoadPointer (HWND_DESKTOP, 0, IDICON_JET);
    WinQueryWindowRect (pinst->hwndCnr, &rclCnr);
    srand (clock());


    for (iRecord = 0, pacrTraverse = pacrRoot;
        iRecord < recins.cRecordsInsert;
        ++iRecord, pacrTraverse = (PAPPCNRREC)
        pacrTraverse->recc.preccNextRecord)
    {
        pacrTraverse->iRecord       = iRecord;
        pacrTraverse->Altitude      =
        pacrTraverse->AssignedAltitude = rand() % 50 + 50;
        pacrTraverse->Heading       =
        pacrTraverse->AssignedHeading  = rand() % 360 + 1;
        pacrTraverse->Speed         =
        pacrTraverse->AssignedSpeed = rand() % 20 + 10;
        sprintf (szBuf, "%s%d", apszCallSignPrefixes [rand() % cPrefixes],
					rand() % 1000);
        pacrTraverse->pszCallsign   = (PSZ) strdup (szBuf);

        SetRecordText (pacrTraverse);
        pacrTraverse->recc.hptrIcon = hptrIcon;
        pacrTraverse->recc.ptlIcon.x = rclCnr.xRight / 10 * (rand() % 9 + 1);
        pacrTraverse->recc.ptlIcon.y = rclCnr.yTop   / 10 * (rand() % 9 + 1);
    }


    /*
     *  INSERT ALL CONTAINER ITEMS
     */

    WinSendMsg (pinst->hwndCnr, CM_INSERTRECORD,
        MPFROMP(pacrRoot), MPFROMP(&recins));

    /*
     *  DESELECT FIRST AIRPLANE, WHICH BECOMES SELECTED AUTOMATICALLY
     */

    WinSendMsg (pinst->hwndCnr, CM_SETRECORDEMPHASIS,
        MPFROMP(pacrRoot), MPFROM2SHORT(FALSE, CRA_SELECTED));

    return 0;
}



/*
 *  GENERATE THE TEXT FOR THE AIRCRAFT'S RADAR DISPLAY, BASED ON ITS
 *  CURRENT ATTRIBUTES
 */

static PSZ SetRecordText (PAPPCNRREC pacr)
{
    CHAR    szBuf [40];

    if (pacr->recc.pszIcon)
        free (pacr->recc.pszIcon);

    sprintf (szBuf, "%s\n%3ld %2ld", pacr->pszCallsign,
        pacr->Altitude, pacr->Speed);

    return pacr->recc.pszIcon = (PSZ) strdup (szBuf);
}



/*
 *  DIALOG FOR ISSUING CONTROL INSTRUCTIONS TO AN AIRCRAFT
 */

static MRESULT EXPENTRY ControlInstructionDlg (HWND hwnd, ULONG msg,
    MPARAM mp1, MPARAM mp2)
{
    PINST   pinstMain;      // Ptr to main application instance data

    pinstMain = WinQueryWindowPtr(hwnd,QWL_USER);

    switch (msg)
    {
        case WM_INITDLG:
        {
            // A ptr to main application instance data is passed to
            // this dialog.  Store it in our QWL_USER data.
            pinstMain = PVOIDFROMMP (mp2);
            WinSetWindowPtr(hwnd,QWL_USER,pinstMain);

            // Set the limits for the spin controls
            WinSendDlgItemMsg (hwnd, IDSPB_HEADING,
                SPBM_SETLIMITS, MPFROMLONG (360), MPFROMLONG (1));
            WinSendDlgItemMsg (hwnd, IDSPB_ALTITUDE,
                SPBM_SETLIMITS, MPFROMLONG (500), MPFROMLONG (10));
            WinSendDlgItemMsg (hwnd, IDSPB_SPEED,
                SPBM_SETLIMITS, MPFROMLONG (100), MPFROMLONG (5));

            // Set current values for the spin controls
            WinSendDlgItemMsg (hwnd, IDSPB_HEADING,
                SPBM_SETCURRENTVALUE,
                MPFROMLONG (pinstMain->pacrSelected->Heading), 0);
            WinSendDlgItemMsg (hwnd, IDSPB_ALTITUDE,
                SPBM_SETCURRENTVALUE,
                MPFROMLONG (pinstMain->pacrSelected->Altitude), 0);
            WinSendDlgItemMsg (hwnd, IDSPB_SPEED,
                SPBM_SETCURRENTVALUE,
                MPFROMLONG (pinstMain->pacrSelected->Speed), 0);

            return 0;
        }

        case WM_COMMAND:
            switch (SHORT1FROMMP (mp1))
            {
                case DID_OK:
                {
                    // Get current values for the spin controls
                    WinSendDlgItemMsg (hwnd, IDSPB_HEADING,
                        SPBM_QUERYVALUE,
                        MPFROMLONG (&pinstMain->pacrSelected->
                            AssignedHeading), 0);
                    WinSendDlgItemMsg (hwnd, IDSPB_ALTITUDE,
                        SPBM_QUERYVALUE,
                        MPFROMLONG (&pinstMain->pacrSelected->
                            AssignedAltitude), 0);
                    WinSendDlgItemMsg (hwnd, IDSPB_SPEED,
                        SPBM_QUERYVALUE,
                        MPFROMLONG (&pinstMain->pacrSelected->
                            AssignedSpeed), 0);

                    break;
                }
            }

            return WinDefDlgProc (hwnd, msg, mp1, mp2);

        default:
            return WinDefDlgProc (hwnd, msg, mp1, mp2);
    }
}



/*
 *  MAKE THE PLANES FLY!
 */

static BOOL ManeuverAircraft (PAPPCNRREC pacrTraverse)
{
    BOOL    fManeuverRequired = FALSE;

    if (pacrTraverse->AssignedHeading !=
        pacrTraverse->Heading)
    {
        BOOL    fTurningRight =
            pacrTraverse->AssignedHeading >
            pacrTraverse->Heading;
        pacrTraverse->Heading +=
            (fTurningRight ? 1 : -1);
        fManeuverRequired = TRUE;
    }

    if (pacrTraverse->AssignedAltitude !=
        pacrTraverse->Altitude)
    {
        BOOL    fAscending =
            pacrTraverse->AssignedAltitude >
            pacrTraverse->Altitude;
        pacrTraverse->Altitude +=
            (fAscending ? 1 : -1);
        fManeuverRequired = TRUE;
    }

    if (pacrTraverse->AssignedSpeed !=
        pacrTraverse->Speed)
    {
        BOOL    fAccelerating =
            pacrTraverse->AssignedSpeed >
            pacrTraverse->Speed;
        pacrTraverse->Speed +=
            (fAccelerating ? 1 : -1);
        fManeuverRequired = TRUE;
    }

    if (pacrTraverse->Heading > 45 &&
        pacrTraverse->Heading < 135)
        pacrTraverse->recc.ptlIcon.x++;
    else
        if (pacrTraverse->Heading > 225 &&
            pacrTraverse->Heading < 315)
            pacrTraverse->recc.ptlIcon.x--;

    if (pacrTraverse->Heading > 315 &&
        pacrTraverse->Heading < 45)
        pacrTraverse->recc.ptlIcon.y++;
    else
        if (pacrTraverse->Heading > 135 &&
            pacrTraverse->Heading < 225)
            pacrTraverse->recc.ptlIcon.y--;

    return fManeuverRequired;
}



/*
 *  DRAW THE RADAR SCREEN (THE CONTAINER BACKGROUND)
 */

static MRESULT EXPENTRY CnrSubclassWndProc
(
    HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2
)
{
    if (msg == CM_PAINTBACKGROUND)
    {
        POWNERBACKGROUND    pownbckg = mp1;
        RECTL               rclCnr;
        PINST               pinstMain;

        pinstMain = WinQueryWindowPtr
            (WinQueryWindow (hwnd, QW_OWNER), QWL_USER);

        if (pinstMain->CurrentView != IDMVIEW_RADAR)
            return FALSE;

        WinQueryWindowRect (hwnd, &rclCnr);

        /*
         *  The radar screen is drawn once into a bitmap and
         *  subsequently GpiBitBlt'd onto the screen
         */

        if (! pinstMain->hbmRadar)
        {
            RECTL               rclRunway;
            LONG                cxRunway;
            LONG                cyRunway;
            ARCPARAMS           arcp;           // arc parameters
            POINTL              ptlScreenMidpoint;
            POINTL              ptlMarkerMidpoint;
            INT                 RadiusMultiplier;

            CreateMemoryPS (pinstMain, pownbckg->hps, &rclCnr);
            pinstMain->hbmRadar =
                CreateBitmap (pinstMain->hpsMemRadar, &rclCnr);

            WinFillRect (pinstMain->hpsMemRadar, &rclCnr,
                CLR_DARKGREEN);

            ptlScreenMidpoint.x = rclCnr.xRight / 2;
            ptlScreenMidpoint.y = rclCnr.yTop / 2;


            /*
             * Draw runway
             */

            cxRunway          = rclCnr.xRight / 60;
            cyRunway          = rclCnr.yTop / 10;
            rclRunway.xLeft   = ptlScreenMidpoint.x - cxRunway / 2;
            rclRunway.xRight  = rclRunway.xLeft + cxRunway;
            rclRunway.yBottom = ptlScreenMidpoint.y - cyRunway / 2;
            rclRunway.yTop    = rclRunway.yBottom + cyRunway;
            WinFillRect (pinstMain->hpsMemRadar, &rclRunway, CLR_WHITE);


            /*
             * Draw approach outer marker (3 elipses)
             */

            ptlMarkerMidpoint.x = ptlScreenMidpoint.x;
            ptlMarkerMidpoint.y = ptlScreenMidpoint.y - rclCnr.yTop / 6;
            GpiMove (pinstMain->hpsMemRadar, &ptlMarkerMidpoint);

            arcp.lP = rclCnr.xRight / 20;
            arcp.lQ = rclCnr.yTop / 80;
            arcp.lR = arcp.lS = 0L;
            GpiSetArcParams(pinstMain->hpsMemRadar,&arcp);
            GpiSetColor (pinstMain->hpsMemRadar, CLR_WHITE);
            GpiFullArc (pinstMain->hpsMemRadar, DRO_OUTLINE, MAKEFIXED(1,0));

            arcp.lP =
            arcp.lQ = rclCnr.yTop / 60;
            arcp.lR = arcp.lS = 0L;
            GpiSetArcParams(pinstMain->hpsMemRadar,&arcp);
            GpiSetColor (pinstMain->hpsMemRadar, CLR_WHITE);
            GpiFullArc (pinstMain->hpsMemRadar, DRO_OUTLINE, MAKEFIXED(1,0));

            arcp.lP =
            arcp.lQ = 1;
            arcp.lR = arcp.lS = 0L;
            GpiSetArcParams(pinstMain->hpsMemRadar,&arcp);
            GpiSetColor (pinstMain->hpsMemRadar, CLR_WHITE);
            GpiFullArc (pinstMain->hpsMemRadar, DRO_OUTLINE, MAKEFIXED(1,0));


            /*
             * Draw distance rings concentric circles
             */

            GpiMove (pinstMain->hpsMemRadar, &ptlScreenMidpoint);
            arcp.lP = arcp.lQ = min (rclCnr.xRight, rclCnr.yTop) / 9;
            arcp.lR = arcp.lS = 0L;
            GpiSetArcParams(pinstMain->hpsMemRadar,&arcp);

            GpiSetColor (pinstMain->hpsMemRadar, CLR_BLACK);

            for (RadiusMultiplier = 1; RadiusMultiplier <= 4; ++RadiusMultiplier)
                GpiFullArc(pinstMain->hpsMemRadar,DRO_OUTLINE,
                    MAKEFIXED(RadiusMultiplier,0));
        }

        DrawBitmap (pinstMain, pownbckg->hps, &pownbckg->rclBackground);

        return (MRESULT) TRUE;
    }
    else
        return pfnwpCnr (hwnd, msg, mp1, mp2);
}




static INT
DrawBitmap
(
    PINST           pinstMain,
    HPS             hpsTarget,
    PRECTL          prclTargetWndDraw
)
{
    POINTL            ptl[3];                 // point definitions

    //
    // Copy the bitmap into the window. Prepare the coordinates and sizes.
    //
    memset (ptl, 0, sizeof ptl);
    ptl[0].x = prclTargetWndDraw->xLeft;
    ptl[0].y = prclTargetWndDraw->yBottom;
    ptl[1].x = prclTargetWndDraw->xRight;
    ptl[1].y = prclTargetWndDraw->yTop;
    ptl[2].x = prclTargetWndDraw->xLeft;
    ptl[2].y = prclTargetWndDraw->yBottom;

    GpiBitBlt(hpsTarget,pinstMain->hpsMemRadar,3L,
        ptl,ROP_SRCCOPY,BBO_IGNORE);

   return 0;                                        // done ok
}


static INT CreateMemoryPS (PINST pinstMain, HPS hpsTarget,
    PRECTL prclTargetWnd)
{
    SIZEL   sizl;
    HDC     hdcTarget = GpiQueryDevice(hpsTarget);

    pinstMain->hdcMem = DevOpenDC(pinstMain->hab,OD_MEMORY,(PCSZ) "*",
        0L,NULL,hdcTarget);
    assert (pinstMain->hdcMem != DEV_ERROR);

    //
    // Set the size of the presentation space to be created.
    //
    sizl.cx = prclTargetWnd->xRight;
    sizl.cy = prclTargetWnd->yTop;

    //
    // Create the memory presentation space for the bitmap.
    //
    pinstMain->hpsMemRadar = GpiCreatePS(pinstMain->hab,
       pinstMain->hdcMem,&sizl,
       PU_PELS | GPIA_ASSOC | GPIT_MICRO);
    assert (pinstMain->hpsMemRadar != GPI_ERROR);

    return 0;
}


static HBITMAP CreateBitmap (HPS hpsMem, PRECTL prclTargetWnd)
{
    LONG        lFormats[24];              // bitmap data formats
    BITMAPINFOHEADER2 bmp;                       // bitmap info header
    HBITMAP         hbm;

    GpiQueryDeviceBitmapFormats( hpsMem, 24L, lFormats );
    memset( &bmp, 0, sizeof bmp);
    bmp.cbFix      = sizeof bmp;          // size of the header
    bmp.cx         = prclTargetWnd->xRight;
    bmp.cy         = prclTargetWnd->yTop;
    bmp.cPlanes    = (USHORT)lFormats[0];  // planes
    bmp.cBitCount  = (USHORT)lFormats[1];  // bit count

    hbm = GpiCreateBitmap(hpsMem,&bmp,0,NULL,NULL);
    assert (hbm != GPI_ERROR);

    GpiSetBitmap(hpsMem,hbm);

    return hbm;
}
