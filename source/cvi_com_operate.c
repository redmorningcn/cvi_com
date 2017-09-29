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
//关串口1	 （redmorningcn 2017-09-23）
/********************************************************************************************/
void cvi_CloseCom(stcUartContrl * sCom)
{
	if( sCom->sConfig.open )								//串口为打开状态，关闭串口
	{
		sCom->sConfig.open = 0;								//置关闭标识
		DelTimeOutVarFromAnsyTimer(sCom->AnsyTimer);		//从异步时钟删除信号量

		CloseCom (sCom->sConfig.port);						//关闭串口
	}
}

/********************************************************************************************/
//初始化COM1的控制结构体	(redmorningcn 2017-09-27)	
/********************************************************************************************/
void cvi_InitComContrl(stcUartContrl* sCom)
{
	sCom->p_rd 		= sCom->rdbuf;		//接收缓冲区
	sCom->p_wr 		= sCom->wrbuf;		//发送缓冲区
	sCom->rdlen		= 0;				//接收字长度
	sCom->wrlen 	= 0;				//发送字长度
	sCom->timeout	= 0;				//超时
	sCom->rdend		= 0;				//接收结束标示
}


/********************************************************************************************/
//开串口1	 （redmorningcn 2017-09-23）
/********************************************************************************************/
void cvi_OpenCom(stcUartContrl* sCom)
{
	int status;

	cvi_InitAnsyTimer();									//初始化异步定时器   

	cvi_InitComContrl(sCom);								//初始化sCom
	
	Com1Config(&sCom->sConfig);								//配置串口	

	if( sCom->sConfig.open == 0)							//未打开，直接打开
	{
		sCom->AnsyTimer.flg 		= 1;
		sCom->AnsyTimer.Settime		= COM_TIMEOUT_VAL;
		sCom->AnsyTimer.ptimeout	= &sCom->timeout;		//超时
		sCom->AnsyTimer.poverFlg	= &sCom->rdend;			//结束标识

		AddTimeOutVarToAnsyTimer(sCom->AnsyTimer);			//异步时钟信号加入
		
		status = OpenComConfig (	sCom->sConfig.port,		//打开串口
								   	"",
									sCom->sConfig.baut,
									sCom->sConfig.parity,
									sCom->sConfig.bits,
									sCom->sConfig.stops,
									sizeof(sCom->rdbuf),
									sizeof(sCom->wrbuf)
					  			);

		if(status < 0 )										//端口打开错误
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
							Com1RecvAndSend, 
							0);
		
		sCom->sConfig.open = 1;						//打开串口成功标识置位
	}
}

/********************************************************************************************/
//初始化串口参数。	 （redmorningcn 2017-09-28）
/********************************************************************************************/
void cvi_ScanComConfig(stcUartContrl* sCom)
{
	if(sCom != NULL)
	{
		if(sCom->sConfig.config)					//配置信息更改	
			cvi_OpenCom(sCom);						//重新打开串口
	}
}

/********************************************************************************************/
//读取接收标示，收到结束表示。（超时或者缓冲区满） （redmorningcn 2017-09-23）
/********************************************************************************************/
char cvi_GetComRecEndFlg(stcUartContrl* sCom)
{
	return	(char)sCom->rdend;
}

/********************************************************************************************/
//接受标识清楚清零操作	（redmorningcn 2017-09-23）
/********************************************************************************************/
void	cvi_ClearComRecEndFlg(stcUartContrl* sCom)
{
	sCom->rdend = 0;
}

/********************************************************************************************/
//根据输入的串口结构，从串口取数据  （redmorningcn 2017-09-23）
/********************************************************************************************/
int	cvi_ReadCom(stcUartContrl* sCom,char *buf,int len)
{
	int	temp;

	if(len < 1)											//无数据，返回处理失败
		return 0;

	temp = len < sCom->rdlen?len: sCom->rdlen; 			//取合适长度

	memcpy(buf,sCom->rdbuf,temp);						//复制数据到指定的数据区

	sCom->rdlen = 0;									//数据长度，及指针复位
	sCom->p_rd  = sCom->rdbuf;

	return	temp;										//返回数据长度
}

/********************************************************************************************/
/* 从指定串口发送数据	*/ // (启动发送)   写1字节，启动发送区空事件
/********************************************************************************************/
void cvi_StartSendCom(stcUartContrl* sCom)
{
	int	len = sCom->wrlen;

	if((sCom->wrlen) > 15)
		len = sCom->wrlen / 3;							//数据较长，先发1/3

	ComWrt(sCom->sConfig.port,sCom->p_wr,len);			//写数据

	sCom->p_wr  += len;									//写指针加1
	sCom->wrlen -= len;									//要写数据长度减1
}


/********************************************************************************************/
//从串口发送数据	（redmorningcn 2017-09-23）
/********************************************************************************************/
int	cvi_SendCom(stcUartContrl* sCom,char *buf,int len)
{
	if( len < 1 )	   									//无数据，返回处理失败
		return 0;

	//增加接收状态判断。如果正在接收，则等待10ms再发送。
	int	times = GetAnsySysTime();						//取串口定时器时间。
	while(sCom->timeout <= 1 )							//总是在收数据
	{
		 if(	(GetAnsySysTime() > times + 100) || 	//超过定时器100个周期装置还是接收状态，退出数据发送
				 GetAnsySysTime() < times				//定时器溢出处理
			)
		 	return 0;
	}

	//增加发送判断，缓冲区不空，则等待10ms再发送。 （多线程兼容）
	times = GetAnsySysTime();							//取串口定时器时间。
	while( sCom->wrlen  )								//发送区不空，等待数据发送完成
	{
		 if(  (	GetAnsySysTime() > times + 100) || 		//超过定时器100个周期装置还是接收状态，退出数据发送
				GetAnsySysTime() < times				//定时器溢出处理
		   )
			 return 0;
	}

	//////////////////////准备数据发送
	int	temp = len;										//数长度

	if( len > sizeof(sCom->wrbuf) )						//数据区>发送区,只发数据区大小
		temp = sizeof(sCom->wrbuf);

	sCom->p_wr  = sCom->wrbuf;							//赋值
	memcpy(sCom->wrbuf,buf,temp);
	sCom->wrlen = temp;

//	ComRecvAndSend(sCom,LWRS_TXEMPTY,0);				//启动发送 （多线程处理）
	cvi_StartSendCom(sCom);								//启动发送

	return	temp;										//返回读取的信息
}


