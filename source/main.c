/*******************************************************************************
 *   Filename:       main.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					���Ժ���

					 ˫��ѡ�� otr �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Otr �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� OTR �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_COMM_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_COMM_STK_SIZE ����С
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
#include "main.h"
#include "cvi_com_operate.h"
#include "cvi_com1.h"
#include <formatio.h> 

/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/
//uir�ļ���
#define		UIR_MAIN_NAME			"main.uir"

/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
int	 	gExiting = 0;
int		gSendDataFlg  = 0;
int		gPanelHandle;
int		gthreadFunctionId1;
//���ڽṹ�弰ͨѶ

/********************************************************************************************/
/* local Prototypes																				*/
/********************************************************************************************/
/* First thread function */		//���ڽ���
static int CVICALLBACK ThreadFunction1 (void *functionData);

typedef struct	_stcSysCtrl_
{
	stcUartContrl *sCom1;
	int	flg;
}stcSysCtrl;

stcSysCtrl	sSysCtrl;


/********************************************************************************************/
/* Application entry point.																	*/
/********************************************************************************************/
int main (int argc, char *argv[])
{
	
	if (InitCVIRTE (0, argv, 0) == 0)	/* Initialize CVI libraries */
		return -1;	/* out of memory */

    gPanelHandle 	= LoadPanel (0, UIR_MAIN_NAME, PANEL);
	
	/* Schedule two thread functions */				 //���̣߳��������̺߳���
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE,
								   ThreadFunction1, NULL,
								   &gthreadFunctionId1);	
	
	sSysCtrl.sCom1 = &gsCom1Contrl;		//��COM1��������
	cvi_OpenCom(sSysCtrl.sCom1);	
	/* 	Display the panel and run the UI */
	DisplayPanel (gPanelHandle);
	
	RunUserInterface ();
	
	cvi_CloseCom(sSysCtrl.sCom1);
	
	gExiting = 1;
	
	/* Wait for the thread functions to finish executing */
	CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE,
											gthreadFunctionId1,
											OPT_TP_PROCESS_EVENTS_WHILE_WAITING);	
	/* Release thread functions */
	CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, gthreadFunctionId1);
	
	/*	Discard the loaded panels from memory */
	DiscardPanel (gPanelHandle);
	
	return 0;
}

/* First thread function */		//���ڽ���
static int CVICALLBACK ThreadFunction1 (void *functionData)
{
	char recbuf[1024];
	char sendbuf[1024];
	int	 reclen;
	
	while (!gExiting)													//�߳��˳�
	{
		if(cvi_GetComRecEndFlg(&gsCom1Contrl) && gsCom1Contrl.rdlen)	//�������ݣ�����ʾ
		{
			cvi_ClearComRecEndFlg(&gsCom1Contrl);
			reclen = cvi_ReadCom(&gsCom1Contrl,recbuf,sizeof(recbuf));
			
			if(reclen < sizeof(recbuf))
				recbuf[reclen] = '\0';		//��ӽ�����ʾ	
				
			SetCtrlVal(gPanelHandle,PANEL_COM1RECVTEXTBOX,recbuf);
			SetCtrlVal(gPanelHandle,PANEL_COM1RECVTEXTBOX,"\r\n");
			//InsertTextBoxLine (gPanelHandle, PANEL_COM1RECVTEXTBOX,-1, recbuf);
		}
		
		GetCtrlVal(gPanelHandle,PANEL_COM1SENDTEXTBOX,sendbuf);
		
		if(		
					strlen(sendbuf)
				&& 	gsCom1Contrl.sConfig.open 
				&&  gSendDataFlg == 1
		  )
		{
			gSendDataFlg = 0;
			cvi_SendCom(&gsCom1Contrl,sendbuf,strlen(sendbuf));
		}
	}

	return 0;
}

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK PanelCB (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event) 
		{
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
		}
	return 0;
}

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK Quit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
		}
	return 0;
}

int CVICALLBACK SendDataCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			gSendDataFlg = 1;
			break;
	}
	return 0;
}

int CVICALLBACK TimerCallback (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:

			if(gsCom1Contrl.sConfig.open == 0)
				SetCtrlAttribute (panel, PANEL_OPENCOM1, ATTR_LABEL_TEXT, "�򿪴���");
			
			if(gsCom1Contrl.sConfig.open == 1)
				SetCtrlAttribute (panel, PANEL_OPENCOM1, ATTR_LABEL_TEXT, "�رմ���");

			break;
	}
	return 0;
}
