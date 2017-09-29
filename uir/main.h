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

#define  PANEL                            1       /* callback function: PanelCB */
#define  PANEL_SENDDATA                   2       /* control type: command, callback function: SendDataCallback */
#define  PANEL_OPENCOM1                   3       /* control type: command, callback function: OpenCom1Callback */
#define  PANEL_COM1SENDTEXTBOX            4       /* control type: textBox, callback function: (none) */
#define  PANEL_COM1RECVTEXTBOX            5       /* control type: textBox, callback function: (none) */
#define  PANEL_DECORATION                 6       /* control type: deco, callback function: (none) */
#define  PANEL_TIMER                      7       /* control type: timer, callback function: TimerCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OpenCom1Callback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SendDataCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
