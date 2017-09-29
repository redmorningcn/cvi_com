/*******************************************************************************
 *   Filename:       cvi_com1_operate.h
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:
					 1、打开和关闭串口。
					 2、串口数据发送，串口数据读取。
					 3、通过串口回调函数及异步定时器控制串口收发管理。


					 双击选中 otr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Otr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 OTR 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_COMM_PRIO     ）
 *                                            和 任务堆栈（ APP_TASK_COMM_STK_SIZE ）大小
 *
 *   Notes:
 *     				E-mail: redmorningcn@qq.com
 *
 *******************************************************************************/

#ifndef	CVI_COMM1_H
#define	CVI_COMM1_H

/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include "asynctmr.h"
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <formatio.h>
#include <comconfig.h>

#include "cvi_ansytimer.h"
#include "cvi_com_operate.h"


/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/
//uir文件名
#define		UIR_COMCONFIG_NAME		"comconfig.uir"

//定义打开配置串口面板名称
#define		OPEN_COM1_PANEL			PANEL_OPENCOM1

//参数配置文件名
#define		MAX_PATHNAME_LEN		512
#define		COM_CONFIG_FILE_NAME	"commconfig.ini"
#define		Enabled					1

/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//面板句柄
extern	int				gCom1PanelHandle;	//面板句柄
//串口结构体及通讯
extern	stcUartContrl	gsCom1Contrl;		//串口1通讯

/********************************************************************************************/
/* Prototypes																				*/
/********************************************************************************************/

/********************************************************************************************/
/* PanelCB ():  串口1配置 gsCom1Config													   	*/
/********************************************************************************************/
void	Com1Config(stcUartConfig * sCom);

/********************************************************************************************/
/*打开串口1面板.	       	配置并打开串口																            */
/********************************************************************************************/
int CVICALLBACK OpenCom1Callback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2);



/********************************************************************************************/
/* PanelCB ():  OpenCom1														*/
/********************************************************************************************/
void	CloseCom1(void);

/********************************************************************************************/
//通过串口1发送数据
/********************************************************************************************/
void	SendCom1(char *buf,int len);

/********************************************************************************************/
/* Event_Char_Detect_Func ():  */
/* com1收发回调函数*/  				//redmorningcn  20170924
/********************************************************************************************/
void CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData);

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/

#endif
