#include <stdlib.h>
#include <stdio.h>
#include "withLCD.h"
#include "YX_Define.h"
#include "YC_Define.h"
#include "modbus.h"
#include "client.h"

#include <string.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include "plc_main.h"
#include <sys/msg.h>
#include "client.h"
#include "sys.h"

int AnalysModbus(unsigned char *pdata, int len)
{
	unsigned char emudata[256];
	unsigned char funid;
	unsigned short regAddr;

	printf("解析收到的PlC数据！！！！！\n");

	memcpy(emudata, &pdata[6], len - 6);
	funid = emudata[1];
	printf("funid:%d\n", funid);

	if (funid == 6)
	{
		regAddr = emudata[2] * 256 + emudata[3];
		printf("PLC返回数据 功能码=6 regAddr=%x\n", regAddr);
	}

	if (funid == 3)
	{
		unsigned short data = *(unsigned short *)&emudata[3];
		int order = 6;


		if ((data & (1 << PLC_EMU_DSTART)) != 0)
			order = 1;
		else if ((data & (1 << PLC_EMU_STOP)) != 0)
			order = 2;


		if (order > 0)
			pPara_plc->funOrder(order);
		if(pbackBmsFun_YX!=NULL)
		{
			//data=0xff0f;
			pbackBmsFun_YX(0,(void*)&data);
		}

		printf("PLC返回数据 功能码=3\n");
	}

	return 0;
}

int SendBmsDataToThread(unsigned char order)
{
	msgClient msg;
	MyData data;
	PcsData_send datasend;
	datasend.dev_id = 0x01;
	datasend.regaddr = 0x0001;
	printf("=== order:%d ===\n",order);

	unsigned short val = 0;
	switch (order)
	{
	case BOX_35kV_ON: //				  // 35kV进线柜合闸
		val |= (1<<EMU_PLC_BOX_35kV_ON);
		break;
	case BOX_35kV_OFF: //				  // 35kV进线柜分闸
		val |= (1<<EMU_PLC_BOX_35kV_OFF);
		break;
	case BOX_SwitchD1_ON: //开关柜D1合闸
		val |= (1<<EMU_PLC_BOX_SwitchD1_ON);
		break;
	case BOX_SwitchD1_OFF: //			  //开关柜D1分闸
		val |= (1<<EMU_PLC_BOX_SwitchD1_OFF);
		break;
	case BOX_SwitchD2_ON: //			  //开关柜D2合闸
		val |= (1<<EMU_PLC_BOX_SwitchD2_ON);
		break;
	case BOX_SwitchD2_OFF: //开关柜D2分闸
		val |= (1<<EMU_PLC_BOX_SwitchD2_OFF);
		break;
	case 18: //风机
		val |= (1<<0);
		break;
	default:
		break;
	}
    if(val==0)
		return 1;
	datasend.val = HL_BitConvert(val);

	datasend.regaddr = 0x0001;
	msg.msgtype = 2;
	memcpy(data.buf, (unsigned char *)&datasend, sizeof(PcsData_send));
	data.len = sizeof(PcsData_send);
	memcpy((char *)&msg.data, (char *)&data, sizeof(MyData));
	if (msgsnd(g_comm_qmegid_plc, &msg, sizeof(msgClient), IPC_NOWAIT) != -1)
	{
		printf("33PLC send data to socket task succ  regAddr=%d val=%d!!!!!!!\n",datasend.regaddr, val);
		return 0;
	}
	else
	{
		printf("22PLC send data to socket task err !!!!!!!\n");
		return 1;
	}
}
int SendLcdDataToThread(unsigned short regAddr, unsigned short val)
{
	msgClient msg;
	MyData data;
	PcsData_send datasend;
	datasend.dev_id = 0x01;
	datasend.regaddr = regAddr;
	datasend.val = val;
	// printf("val");
	msg.msgtype = 2;
	memcpy(data.buf, (unsigned char *)&datasend, sizeof(PcsData_send));
	data.len = sizeof(PcsData_send);
	memcpy((char *)&msg.data, (char *)&data, sizeof(MyData));

	if (msgsnd(g_comm_qmegid_plc, &msg, sizeof(msgClient), IPC_NOWAIT) != -1)
	{

		printf("22PLC send data to socket task succ  regAddr=%d val=%d!!!!!!!\n", regAddr, val);
		return 0;
	}
	else
	{
		printf("22PLC send data to socket task err !!!!!!!\n");
		return 1;
	}
}
