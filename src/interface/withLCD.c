#include "withLCD.h"
#include <stddef.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "YX_Define.h"
#include "YC_Define.h"
#include "client.h"
#include "modbus.h"
#include "plc_main.h"

unsigned char flag_RecvNeed_PCS[MAX_PCS_NUM];

int recvfromlcd(unsigned char type, void *pdata)
{
	switch (type)
	{
	case _YC_:
	{
		LCD_YC_YX_DATA temp;
		temp = *(LCD_YC_YX_DATA *)pdata;
		int pw, qw, aw;
		int sn = 0;
		unsigned short temp_pw, temp_qw, temp_aw;
		temp_pw = temp.pcs_data[Active_power];
		pw = (temp_pw % 256) * 256 + temp_pw / 256;

		temp_pw = temp.pcs_data[Active_power];
		pw = (temp_pw % 256) * 256 + temp_pw / 256;
		temp_qw = temp.pcs_data[Reactive_power];
		qw = (temp_qw % 256) * 256 + temp_qw / 256;

		temp_aw = temp.pcs_data[Apparent_power];
		aw = (temp_aw % 256) * 256 + temp_aw / 256;
		printf("1PLC收到的有功功率 lcdid=%d pcsid=%d pw=%d %x\n", temp.lcdid, temp.pcsid, pw, temp_pw);
		printf("2PLC收到的无功功率 lcdid=%d pcsid=%d qw=%d %x\n", temp.lcdid, temp.pcsid, qw, temp_qw);
		printf("3PLC收到的视在功率 lcdid=%d pcsid=%d aw=%d %x \n", temp.lcdid, temp.pcsid, aw, temp_aw);
		sn = temp.lcdid * 6 + temp.pcsid - 1;

		SendLcdDataToThread(sn + 5, pw);
	}
	break;
	case _YX_:
	{
		static unsigned char flag_recv_pcs[] = {0, 0, 0, 0, 0, 0};
		static unsigned char outdata[] = {0, 0, 0, 0, 0, 0};
		unsigned short b;
		LCD_YC_YX_DATA temp;
		int val;
		int flag = 0;
		unsigned short regAddr;
		temp = *(LCD_YC_YX_DATA *)pdata;
		// b = temp.pcs_data[u16_InvRunState1];
		b = temp.pcs_data[u16_InvRunState1] & (1 << bPcsRunning);
		if (b > 0)
		{
			outdata[temp.lcdid] |= (1 << (temp.pcsid - 1));
		}

		flag_recv_pcs[temp.lcdid] |= (1 << (temp.pcsid - 1));

		if (temp.lcdid == 0 || temp.lcdid == 1)
		{

			if ((flag_recv_pcs[0] == flag_RecvNeed_PCS[0]) && (flag_recv_pcs[1] == flag_RecvNeed_PCS[1]))
			{
				val = outdata[1] * 256 + outdata[0];
				regAddr = 2;
				flag = 1;
			}
		}
		else if (temp.lcdid == 2 || temp.lcdid == 3)
		{
			if ((flag_recv_pcs[2] == flag_RecvNeed_PCS[2]) && (flag_recv_pcs[3] == flag_RecvNeed_PCS[3]))
			{
				val = outdata[3] * 256 + outdata[2];
				regAddr = 3;
				flag = 1;
			}
		}
		else if (temp.lcdid == 4 || temp.lcdid == 5)
		{
			if ((flag_recv_pcs[4] == flag_RecvNeed_PCS[4]) && (flag_recv_pcs[5] == flag_RecvNeed_PCS[5]))
			{
				val = outdata[5] * 256 + outdata[4];
				regAddr = 4;
				flag = 1;
			}
		}
		else
			printf("PLC接收遥信数据出现错误！！\n");
		if (flag == 1)
			SendLcdDataToThread(regAddr, val);
	}
	break;

	default:
		break;
	}
	return 0;
}

void subscribeFromLcd(void)
{

	void *handle;
	char *error;
#define LIB_LCD_PATH "/usr/lib/libmodtcp.so"
	typedef int (*outData2Other)(unsigned char, void *);		   //输出数据
	typedef int (*in_fun)(unsigned char type, outData2Other pfun); //命令处理函数指针
	in_fun my_func = NULL;
	//打开动态链接库

	handle = dlopen(LIB_LCD_PATH, RTLD_LAZY);
	if (!handle)
	{
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();

	*(void **)(&my_func) = dlsym(handle, "SubscribeLcdData");
	if ((error = dlerror()) != NULL)
	{
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	printf("PLC模块订阅LCD遥测、遥信数据\n");
	my_func(_YC_, recvfromlcd);
	my_func(_YX_, recvfromlcd);

}
