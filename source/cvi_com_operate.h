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

#ifndef	CVI_COMM_OPERATE_H
#define	CVI_COMM_OPERATE_H

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

/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/
#define		COM_TIMEOUT_VAL		5

/********************************************************************************************/
/* 串口配置结构体																				    */
/********************************************************************************************/
typedef struct	_stcUartConfig_
{
	char	port;			//端口号
	char	bits;			//数据位
	char	parity;			//校验位
	char	stops;			//停止位
	int		baut;			//波特率
	char	open;			//打开
	char	config;			//配置更改 	
	char	tmp[2];			//备用
}stcUartConfig;				//串口配置结构体

/********************************************************************************************/
/* 串口通讯结构体 																	    */
/********************************************************************************************/
typedef struct	_stcUartContrl_
{
	char		rdbuf[512];		//接收数据区
	char		wrbuf[512];		//发送数据位

	int			rdlen;			//接收数据长度
	int			wrlen;			//发送数据长度

	char*		p_rd;			//读指针
	char*		p_wr;			//写指针

	int			timeout;		//超时
	int			rdend;			//接收结束
	
	stcUartConfig   sConfig;	//配置
	
	stcAnsyTimerMgr	AnsyTimer;	//异步时钟，超时控制
	
}stcUartContrl;					//串口配置结构体


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//串口结构体及通讯

/********************************************************************************************/
/* Prototypes																				*/
/********************************************************************************************/

/********************************************************************************************/
//CloseCom1 关串口1	 （redmorningcn 2017-09-23）
/********************************************************************************************/
void cvi_CloseCom(stcUartContrl * sCom);

/********************************************************************************************/
//cvi_OpenCom 开串口1	 （redmorningcn 2017-09-23）
/********************************************************************************************/
void cvi_OpenCom(stcUartContrl* sCom);

/********************************************************************************************/
//读取接收标示，收到结束表示。（超时或者缓冲区满） （redmorningcn 2017-09-23）
/********************************************************************************************/
char cvi_GetComRecEndFlg(stcUartContrl* sCom);

/********************************************************************************************/
//接受标识清楚清零操作	（redmorningcn 2017-09-23）
/********************************************************************************************/
void cvi_ClearComRecEndFlg(stcUartContrl* sCom);

/********************************************************************************************/
//根据输入的串口结构，从串口取数据  （redmorningcn 2017-09-23）
/********************************************************************************************/
int	cvi_ReadCom(stcUartContrl* sCom,char *buf,int len);

/********************************************************************************************/
//从串口发送数据	（redmorningcn 2017-09-23）
/********************************************************************************************/
int	cvi_SendCom(stcUartContrl* sCom,char *buf,int len);

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/

#endif
