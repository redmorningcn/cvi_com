/*******************************************************************************
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:

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
#include "cvi_com_operate.h"
#include <formatio.h>

#include <cvi_com1.h>   
/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/

/********************************************************************************************/
/* local Prototypes																			*/
/********************************************************************************************/

/********************************************************************************************/
//�ش���1	 ��redmorningcn 2017-09-23��
/********************************************************************************************/
void cvi_CloseCom(stcUartContrl * sCom)
{
	if( sCom->sConfig.open )								//����Ϊ��״̬���رմ���
	{
		sCom->sConfig.open = 0;								//�ùرձ�ʶ
		DelTimeOutVarFromAnsyTimer(sCom->AnsyTimer);		//���첽ʱ��ɾ���ź���

		CloseCom (sCom->sConfig.port);						//�رմ���
	}
}

/********************************************************************************************/
//��ʼ��COM1�Ŀ��ƽṹ��	(redmorningcn 2017-09-27)	
/********************************************************************************************/
void cvi_InitComContrl(stcUartContrl* sCom)
{
	sCom->p_rd 		= sCom->rdbuf;		//���ջ�����
	sCom->p_wr 		= sCom->wrbuf;		//���ͻ�����
	sCom->rdlen		= 0;				//�����ֳ���
	sCom->wrlen 	= 0;				//�����ֳ���
	sCom->timeout	= 0;				//��ʱ
	sCom->rdend		= 0;				//���ս�����ʾ
}


/********************************************************************************************/
//������1	 ��redmorningcn 2017-09-23��
/********************************************************************************************/
void cvi_OpenCom(stcUartContrl* sCom)
{
	int status;

	cvi_InitAnsyTimer();									//��ʼ���첽��ʱ��   

	cvi_InitComContrl(sCom);								//��ʼ��sCom
	
	Com1Config(&sCom->sConfig);								//���ô���	

	if( sCom->sConfig.open == 0)							//δ�򿪣�ֱ�Ӵ�
	{
		sCom->AnsyTimer.flg 		= 1;
		sCom->AnsyTimer.Settime		= COM_TIMEOUT_VAL;
		sCom->AnsyTimer.ptimeout	= &sCom->timeout;		//��ʱ
		sCom->AnsyTimer.poverFlg	= &sCom->rdend;			//������ʶ

		AddTimeOutVarToAnsyTimer(sCom->AnsyTimer);			//�첽ʱ���źż���
		
		status = OpenComConfig (	sCom->sConfig.port,		//�򿪴���
								   	"",
									sCom->sConfig.baut,
									sCom->sConfig.parity,
									sCom->sConfig.bits,
									sCom->sConfig.stops,
									sizeof(sCom->rdbuf),
									sizeof(sCom->wrbuf)
					  			);

		if(status < 0 )										//�˿ڴ򿪴���
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
							Com1RecvAndSend, 
							0);
		
		sCom->sConfig.open = 1;						//�򿪴��ڳɹ���ʶ��λ
	}
}

/********************************************************************************************/
//��ʼ�����ڲ�����	 ��redmorningcn 2017-09-28��
/********************************************************************************************/
void cvi_ScanComConfig(stcUartContrl* sCom)
{
	if(sCom != NULL)
	{
		if(sCom->sConfig.config)					//������Ϣ����	
			cvi_OpenCom(sCom);						//���´򿪴���
	}
}

/********************************************************************************************/
//��ȡ���ձ�ʾ���յ�������ʾ������ʱ���߻��������� ��redmorningcn 2017-09-23��
/********************************************************************************************/
char cvi_GetComRecEndFlg(stcUartContrl* sCom)
{
	return	(char)sCom->rdend;
}

/********************************************************************************************/
//���ܱ�ʶ����������	��redmorningcn 2017-09-23��
/********************************************************************************************/
void	cvi_ClearComRecEndFlg(stcUartContrl* sCom)
{
	sCom->rdend = 0;
}

/********************************************************************************************/
//��������Ĵ��ڽṹ���Ӵ���ȡ����  ��redmorningcn 2017-09-23��
/********************************************************************************************/
int	cvi_ReadCom(stcUartContrl* sCom,char *buf,int len)
{
	int	temp;

	if(len < 1)											//�����ݣ����ش���ʧ��
		return 0;

	temp = len < sCom->rdlen?len: sCom->rdlen; 			//ȡ���ʳ���

	memcpy(buf,sCom->rdbuf,temp);						//�������ݵ�ָ����������

	sCom->rdlen = 0;									//���ݳ��ȣ���ָ�븴λ
	sCom->p_rd  = sCom->rdbuf;

	return	temp;										//�������ݳ���
}

/********************************************************************************************/
/* ��ָ�����ڷ�������	*/ // (��������)   д1�ֽڣ��������������¼�
/********************************************************************************************/
void cvi_StartSendCom(stcUartContrl* sCom)
{
	int	len = sCom->wrlen;

	if((sCom->wrlen) > 15)
		len = sCom->wrlen / 3;							//���ݽϳ����ȷ�1/3

	ComWrt(sCom->sConfig.port,sCom->p_wr,len);			//д����

	sCom->p_wr  += len;									//дָ���1
	sCom->wrlen -= len;									//Ҫд���ݳ��ȼ�1
}


/********************************************************************************************/
//�Ӵ��ڷ�������	��redmorningcn 2017-09-23��
/********************************************************************************************/
int	cvi_SendCom(stcUartContrl* sCom,char *buf,int len)
{
	if( len < 1 )	   									//�����ݣ����ش���ʧ��
		return 0;

	//���ӽ���״̬�жϡ�������ڽ��գ���ȴ�10ms�ٷ��͡�
	int	times = GetAnsySysTime();						//ȡ���ڶ�ʱ��ʱ�䡣
	while(sCom->timeout <= 1 )							//������������
	{
		 if(	(GetAnsySysTime() > times + 100) || 	//������ʱ��100������װ�û��ǽ���״̬���˳����ݷ���
				 GetAnsySysTime() < times				//��ʱ���������
			)
		 	return 0;
	}

	//���ӷ����жϣ����������գ���ȴ�10ms�ٷ��͡� �����̼߳��ݣ�
	times = GetAnsySysTime();							//ȡ���ڶ�ʱ��ʱ�䡣
	while( sCom->wrlen  )								//���������գ��ȴ����ݷ������
	{
		 if(  (	GetAnsySysTime() > times + 100) || 		//������ʱ��100������װ�û��ǽ���״̬���˳����ݷ���
				GetAnsySysTime() < times				//��ʱ���������
		   )
			 return 0;
	}

	//////////////////////׼�����ݷ���
	int	temp = len;										//������

	if( len > sizeof(sCom->wrbuf) )						//������>������,ֻ����������С
		temp = sizeof(sCom->wrbuf);

	sCom->p_wr  = sCom->wrbuf;							//��ֵ
	memcpy(sCom->wrbuf,buf,temp);
	sCom->wrlen = temp;

//	ComRecvAndSend(sCom,LWRS_TXEMPTY,0);				//�������� �����̴߳���
	cvi_StartSendCom(sCom);								//��������

	return	temp;										//���ض�ȡ����Ϣ
}


