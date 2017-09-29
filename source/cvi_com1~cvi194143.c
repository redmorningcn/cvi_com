/*******************************************************************************
 *   Filename:       cvi_com1.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:


/////////////////////////////////////////////////////////////////////////////
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

/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include "asynctmr.h"
#include <utility.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <formatio.h>

#include "cvi_com1.h"


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//面板变量
int 				gCom1PanelHandle;								//串口1面板


/********************************************************************************************/
/* local Prototypes																			*/
/********************************************************************************************/

/********************************************************************************************/
/* Event_Char_Detect_Func ():  */
/* com1收发回调函数*/  				//redmorningcn  20170924
/********************************************************************************************/
void CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData)
{
//	int	len;
	stcUartContrl* sCom;

	sCom = &gsCom1Contrl;											//串口1,contrl

	if(eventMask ==  LWRS_TXEMPTY )									//发送数据
	{
		if(sCom->wrlen)												//发送缓冲区还有数据未发送完成，继续发送。
		{
			ComWrt(	sCom->sConfig.port,
					sCom->p_wr,
					sCom->wrlen);

			sCom->wrlen = 0;
		}
	}

	if(eventMask ==  LWRS_RXCHAR )									//接收到数据   （怎么确认，数据接收完成？）
	{
		while(GetInQLen(sCom->sConfig.port))						//有数据未取
		{
			if(sCom->rdlen < sizeof(sCom->rdbuf))
			{
				*sCom->p_rd = (char)ComRdByte (sCom->sConfig.port);

				sCom->p_rd++;
				sCom->rdlen++;
			}
			else
			{
				ComRdByte (sCom->sConfig.port);						//数据区满，读空
				sCom->rdend = 1;									//置数据接收完成。(模拟信号量发送)
			}

			sCom->timeout = 0;										//接收到数据，计算器清零
		}
	}

	return;
}

/********************************************************************************************/
/* ReadCom1ConfigFromFile  		从配置文件中读出串口的配置文件，如果有则按配置文件配置；
否则，显示默认值	*/
/********************************************************************************************/
int	ReadCom1ConfigFromFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//文件指针
	char 	projectDir[MAX_PATHNAME_LEN];				//工程路径
	char 	fullPath[MAX_PATHNAME_LEN];					//文件全路径
	ssize_t size;										//文件大小
	int		filecom1hand;								//文件句柄
	stcUartConfig   comconfig;
	int		readsize;
	int		flg = 0;

	fileName = COM_CONFIG_FILE_NAME;

	GetProjectDir (projectDir) ;					//取项目路径
//
	MakePathname (projectDir, fileName, fullPath);	//文件路劲 where fullPath has the following format: c:\myproject\myfile.dat

	if(GetFileInfo (fullPath, &size) == 0)			//文件不存在
	{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);   	//打开文件
		comconfig.baut = 57600;
		comconfig.port = 1;
		memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));
	}else{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//打开文件

		SetFilePtr (filecom1hand, 0, 0);													//文件从头开始

		readsize = ReadFile (filecom1hand, (char *)&comconfig, sizeof(stcUartConfig));		//配置文件读取

		if(readsize)																		//数据大于零
		{
			if(0 < comconfig.port && comconfig.port < 11)									//端口号正常
				if(comconfig.stops == 1  || comconfig.stops == 2)							//停止位1、2
					 if(5<comconfig.bits && comconfig.bits < 9)								//数据位5-8
					 {
						memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));
						flg = 1;
					 }
		}
		else
		{
			comconfig.baut 		= 57600;
			comconfig.port 		= 1;
			comconfig.stops 	= 1;
			comconfig.bits  	= 8; 
			comconfig.parity  	= 0;  

			memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));
		}
	}

	CloseFile(filecom1hand);							//关闭文件

	return	flg;
}

/********************************************************************************************/
/* WriteCom1ConfigToFile  		写配置数据到文件
/********************************************************************************************/
void	WriteCom1ConfigToFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//文件指针
	char 	projectDir[MAX_PATHNAME_LEN];				//工程路径
	char 	fullPath[MAX_PATHNAME_LEN];					//文件全路径
	int		filecom1hand;								//文件句柄

	fileName = COM_CONFIG_FILE_NAME;

	GetProjectDir (projectDir) ;					//取项目路径

	MakePathname (projectDir, fileName, fullPath);	//文件路劲 where fullPath has the following format: c:\myproject\myfile.dat

	filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//打开文件

	SetFilePtr (filecom1hand, 0, 0);													//文件从头开始

	WriteFile (filecom1hand, (char *)sUartConfig, sizeof(stcUartConfig));

	CloseFile(filecom1hand);															//关闭文件
}

/********************************************************************************************/
/* PanelCB ():  串口1配置 gsCom1Config													   	*/
/********************************************************************************************/
void	Com1Config(stcUartConfig * sCom)
{
	int status;

	ReadCom1ConfigFromFile(&sCom->sConfig);					//从文件中读取配置文件有效

	status = OpenComConfig (	sCom->sConfig.port,
							   	"",
								sCom->sConfig.baut,
								sCom->sConfig.parity,
								sCom->sConfig.bits,
								sCom->sConfig.stops,
								sizeof(sCom->rdbuf),
								sizeof(sCom->wrbuf)
							  );

	if(status < 0)											//端口打开错误
	{
		MessagePopup ("配置错误信息", "串口不能打开，请确认!");

		return ;
	}

	/* 	Make sure Serial buffers are empty */
	FlushInQ  (sCom->sConfig.port);
	FlushOutQ (sCom->sConfig.port);

	/* 	串口回调函数,数据区空或接受数据，执行回调函数 */
	InstallComCallback (sCom->sConfig.port,
						LWRS_RXCHAR | LWRS_TXEMPTY,
						1,
						'a' ,
						Com1RecvAndSend, 0);

	WriteCom1ConfigToFile(&sCom->sConfig);				//将配置信息写入文件

	if(gCom1PanelHandle == 0)							//没有打开配置面板，就直接打开串口；否则，退出面板时打开
		cvi_OpenCom(sCom);								//打开串口
}


/********************************************************************************************/
/* PanelCB ():  Quits panel.														*/
/********************************************************************************************/
void	QuitCom1Panel(int	panel)
{
	GetCtrlVal (panel, COM1_PANEL_PORT, &gsCom1Contrl.sConfig.port);			//取端口
	GetCtrlVal (panel, COM1_PANEL_BAUT, &gsCom1Contrl.sConfig.baut);			//取波特率
	GetCtrlVal (panel, COM1_PANEL_BITS, &gsCom1Contrl.sConfig.bits);			//取数据长度
	GetCtrlVal (panel, COM1_PANEL_PARITY, &gsCom1Contrl.sConfig.parity);		//取校验
	GetCtrlVal (panel, COM1_PANEL_STOPS, &gsCom1Contrl.sConfig.stops);			//取停止位

	WriteCom1ConfigToFile(&gsCom1Contrl.sConfig);								//将配置信息写入文件

	Com1Config();																//配置串口
	cvi_OpenCom(&gsCom1Contrl);													//打开串口

	HidePanel(gCom1PanelHandle);												//退出窗口
}

/********************************************************************************************/
/* PanelCB ():  OpenCom1														*/
/********************************************************************************************/
void	OpenCom1(stcUartConfig * sCom)
{
	Com1Config(sCom);																//配置串口
	cvi_OpenCom(sCom);													//打开串口
}

/********************************************************************************************/
//通过串口1发送数据
/********************************************************************************************/
void	SendCom1(char *buf,int len)
{
	cvi_SendCom(&gsCom1Contrl,buf,len);
}

/********************************************************************************************/
/* PanelCB ():  OpenCom1																	*/
/********************************************************************************************/
void	CloseCom1(void)
{
	cvi_CloseCom (&gsCom1Contrl);												//关闭串口
}

/********************************************************************************************/
/* PanelCB ():  串口1配置													             	*/
/********************************************************************************************/
int CVICALLBACK Com1ConfigOkCallback (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	//int status;
	switch (event)
	{
		case EVENT_COMMIT:

			QuitCom1Panel(panel);

			break;
	}
	return 0;
}

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK Com1PanelCB (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_CLOSE:

			QuitCom1Panel(panel);

			break;
		}
	return 0;
}

/********************************************************************************************/
/* 在com1_panel面板上显示配置信息*/
/********************************************************************************************/
void	SetCviCtrlCom1Config(stcUartConfig * sUartConfig)
{
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PORT, sUartConfig->port);			//取端口
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BAUT, sUartConfig->baut);			//取波特率
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BITS, sUartConfig->bits);			//取数据长度
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PARITY,sUartConfig->parity);		//取校验
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_STOPS, sUartConfig->stops);		//取停止位
}


/********************************************************************************************/
/*打开串口1面板.																	            */
/********************************************************************************************/
int CVICALLBACK OpenCom1Callback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(gsCom1Contrl.sConfig.open  == 0)
			{
				gCom1PanelHandle	= LoadPanel (0,UIR_COMCONFIG_NAME, COM1_PANEL);
				DisplayPanel (gCom1PanelHandle);									//显示设置面板

				Com1Config();														//配置，退出面板时打开串口

				SetCviCtrlCom1Config(&gsCom1Contrl.sConfig);						//参数值面板显示

			}else{

				cvi_CloseCom (&gsCom1Contrl);										//关闭串口
			}
			break;
	}
	return 0;
}
