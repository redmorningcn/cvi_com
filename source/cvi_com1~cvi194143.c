/*******************************************************************************
 *   Filename:       cvi_com1.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:


/////////////////////////////////////////////////////////////////////////////
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
#include <formatio.h>

#include "cvi_com1.h"


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//������
int 				gCom1PanelHandle;								//����1���


/********************************************************************************************/
/* local Prototypes																			*/
/********************************************************************************************/

/********************************************************************************************/
/* Event_Char_Detect_Func ():  */
/* com1�շ��ص�����*/  				//redmorningcn  20170924
/********************************************************************************************/
void CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData)
{
//	int	len;
	stcUartContrl* sCom;

	sCom = &gsCom1Contrl;											//����1,contrl

	if(eventMask ==  LWRS_TXEMPTY )									//��������
	{
		if(sCom->wrlen)												//���ͻ�������������δ������ɣ��������͡�
		{
			ComWrt(	sCom->sConfig.port,
					sCom->p_wr,
					sCom->wrlen);

			sCom->wrlen = 0;
		}
	}

	if(eventMask ==  LWRS_RXCHAR )									//���յ�����   ����ôȷ�ϣ����ݽ�����ɣ���
	{
		while(GetInQLen(sCom->sConfig.port))						//������δȡ
		{
			if(sCom->rdlen < sizeof(sCom->rdbuf))
			{
				*sCom->p_rd = (char)ComRdByte (sCom->sConfig.port);

				sCom->p_rd++;
				sCom->rdlen++;
			}
			else
			{
				ComRdByte (sCom->sConfig.port);						//��������������
				sCom->rdend = 1;									//�����ݽ�����ɡ�(ģ���ź�������)
			}

			sCom->timeout = 0;										//���յ����ݣ�����������
		}
	}

	return;
}

/********************************************************************************************/
/* ReadCom1ConfigFromFile  		�������ļ��ж������ڵ������ļ���������������ļ����ã�
������ʾĬ��ֵ	*/
/********************************************************************************************/
int	ReadCom1ConfigFromFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//�ļ�ָ��
	char 	projectDir[MAX_PATHNAME_LEN];				//����·��
	char 	fullPath[MAX_PATHNAME_LEN];					//�ļ�ȫ·��
	ssize_t size;										//�ļ���С
	int		filecom1hand;								//�ļ����
	stcUartConfig   comconfig;
	int		readsize;
	int		flg = 0;

	fileName = COM_CONFIG_FILE_NAME;

	GetProjectDir (projectDir) ;					//ȡ��Ŀ·��
//
	MakePathname (projectDir, fileName, fullPath);	//�ļ�·�� where fullPath has the following format: c:\myproject\myfile.dat

	if(GetFileInfo (fullPath, &size) == 0)			//�ļ�������
	{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);   	//���ļ�
		comconfig.baut = 57600;
		comconfig.port = 1;
		memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));
	}else{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//���ļ�

		SetFilePtr (filecom1hand, 0, 0);													//�ļ���ͷ��ʼ

		readsize = ReadFile (filecom1hand, (char *)&comconfig, sizeof(stcUartConfig));		//�����ļ���ȡ

		if(readsize)																		//���ݴ�����
		{
			if(0 < comconfig.port && comconfig.port < 11)									//�˿ں�����
				if(comconfig.stops == 1  || comconfig.stops == 2)							//ֹͣλ1��2
					 if(5<comconfig.bits && comconfig.bits < 9)								//����λ5-8
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

	CloseFile(filecom1hand);							//�ر��ļ�

	return	flg;
}

/********************************************************************************************/
/* WriteCom1ConfigToFile  		д�������ݵ��ļ�
/********************************************************************************************/
void	WriteCom1ConfigToFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//�ļ�ָ��
	char 	projectDir[MAX_PATHNAME_LEN];				//����·��
	char 	fullPath[MAX_PATHNAME_LEN];					//�ļ�ȫ·��
	int		filecom1hand;								//�ļ����

	fileName = COM_CONFIG_FILE_NAME;

	GetProjectDir (projectDir) ;					//ȡ��Ŀ·��

	MakePathname (projectDir, fileName, fullPath);	//�ļ�·�� where fullPath has the following format: c:\myproject\myfile.dat

	filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//���ļ�

	SetFilePtr (filecom1hand, 0, 0);													//�ļ���ͷ��ʼ

	WriteFile (filecom1hand, (char *)sUartConfig, sizeof(stcUartConfig));

	CloseFile(filecom1hand);															//�ر��ļ�
}

/********************************************************************************************/
/* PanelCB ():  ����1���� gsCom1Config													   	*/
/********************************************************************************************/
void	Com1Config(stcUartConfig * sCom)
{
	int status;

	ReadCom1ConfigFromFile(&sCom->sConfig);					//���ļ��ж�ȡ�����ļ���Ч

	status = OpenComConfig (	sCom->sConfig.port,
							   	"",
								sCom->sConfig.baut,
								sCom->sConfig.parity,
								sCom->sConfig.bits,
								sCom->sConfig.stops,
								sizeof(sCom->rdbuf),
								sizeof(sCom->wrbuf)
							  );

	if(status < 0)											//�˿ڴ򿪴���
	{
		MessagePopup ("���ô�����Ϣ", "���ڲ��ܴ򿪣���ȷ��!");

		return ;
	}

	/* 	Make sure Serial buffers are empty */
	FlushInQ  (sCom->sConfig.port);
	FlushOutQ (sCom->sConfig.port);

	/* 	���ڻص�����,�������ջ�������ݣ�ִ�лص����� */
	InstallComCallback (sCom->sConfig.port,
						LWRS_RXCHAR | LWRS_TXEMPTY,
						1,
						'a' ,
						Com1RecvAndSend, 0);

	WriteCom1ConfigToFile(&sCom->sConfig);				//��������Ϣд���ļ�

	if(gCom1PanelHandle == 0)							//û�д�������壬��ֱ�Ӵ򿪴��ڣ������˳����ʱ��
		cvi_OpenCom(sCom);								//�򿪴���
}


/********************************************************************************************/
/* PanelCB ():  Quits panel.														*/
/********************************************************************************************/
void	QuitCom1Panel(int	panel)
{
	GetCtrlVal (panel, COM1_PANEL_PORT, &gsCom1Contrl.sConfig.port);			//ȡ�˿�
	GetCtrlVal (panel, COM1_PANEL_BAUT, &gsCom1Contrl.sConfig.baut);			//ȡ������
	GetCtrlVal (panel, COM1_PANEL_BITS, &gsCom1Contrl.sConfig.bits);			//ȡ���ݳ���
	GetCtrlVal (panel, COM1_PANEL_PARITY, &gsCom1Contrl.sConfig.parity);		//ȡУ��
	GetCtrlVal (panel, COM1_PANEL_STOPS, &gsCom1Contrl.sConfig.stops);			//ȡֹͣλ

	WriteCom1ConfigToFile(&gsCom1Contrl.sConfig);								//��������Ϣд���ļ�

	Com1Config();																//���ô���
	cvi_OpenCom(&gsCom1Contrl);													//�򿪴���

	HidePanel(gCom1PanelHandle);												//�˳�����
}

/********************************************************************************************/
/* PanelCB ():  OpenCom1														*/
/********************************************************************************************/
void	OpenCom1(stcUartConfig * sCom)
{
	Com1Config(sCom);																//���ô���
	cvi_OpenCom(sCom);													//�򿪴���
}

/********************************************************************************************/
//ͨ������1��������
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
	cvi_CloseCom (&gsCom1Contrl);												//�رմ���
}

/********************************************************************************************/
/* PanelCB ():  ����1����													             	*/
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
/* ��com1_panel�������ʾ������Ϣ*/
/********************************************************************************************/
void	SetCviCtrlCom1Config(stcUartConfig * sUartConfig)
{
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PORT, sUartConfig->port);			//ȡ�˿�
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BAUT, sUartConfig->baut);			//ȡ������
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BITS, sUartConfig->bits);			//ȡ���ݳ���
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PARITY,sUartConfig->parity);		//ȡУ��
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_STOPS, sUartConfig->stops);		//ȡֹͣλ
}


/********************************************************************************************/
/*�򿪴���1���.																	            */
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
				DisplayPanel (gCom1PanelHandle);									//��ʾ�������

				Com1Config();														//���ã��˳����ʱ�򿪴���

				SetCviCtrlCom1Config(&gsCom1Contrl.sConfig);						//����ֵ�����ʾ

			}else{

				cvi_CloseCom (&gsCom1Contrl);										//�رմ���
			}
			break;
	}
	return 0;
}
