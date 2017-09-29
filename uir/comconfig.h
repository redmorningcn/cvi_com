/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  COM1_PANEL                       1       /* callback function: Com1PanelCB */
#define  COM1_PANEL_STOPS                 2       /* control type: ring, callback function: (none) */
#define  COM1_PANEL_PARITY                3       /* control type: ring, callback function: (none) */
#define  COM1_PANEL_BITS                  4       /* control type: ring, callback function: (none) */
#define  COM1_PANEL_BAUT                  5       /* control type: ring, callback function: (none) */
#define  COM1_PANEL_PORT                  6       /* control type: ring, callback function: (none) */
#define  COM1_PANEL_OK                    7       /* control type: command, callback function: Com1ConfigOkCallback */
#define  COM1_PANEL_COM1TIMER             8       /* control type: timer, callback function: Com1TimerCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Com1ConfigOkCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Com1PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Com1TimerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
