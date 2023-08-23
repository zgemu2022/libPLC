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

unsigned short flag_RecvNeed_PCS_YC[6];
unsigned short flag_RecvNeed_PCS_YX[3];

short yc_PW[MAX_TOTAL_PCS_NUM];

unsigned short HL_BitConvert(unsigned short sval)
{
	return ((sval & 0x00ff) << 8) + ((sval & 0xff00) >> 8);
}

void recvLcdPara(void *para){
	memcpy((void *)pPara_plc, para, sizeof(PARA_PLC));
	printf("plc模块  接收到的配置参数  flag_RecvNeed_LCD:%d pPara_plc->lcdnum:%d\n",pPara_plc->flag_RecvNeed_LCD,pPara_plc->lcdnum);
	int i,sn=0;

	for (i = 0; i < pPara_plc->lcdnum; i++)
	{
		total_pcsnum += pPara_plc->pcsnum[i];
		if ((pPara_plc->flag_RecvNeed_LCD & (1 << i)) != 0)
		{
			flag_RecvNeed_PCS_YC[i] = countRecvFlag(pPara_plc->pcsnum[i]);
			// printf("PLC 处理 flag_RecvNeed_PCS_YC[%d]:%d\n",i,flag_RecvNeed_PCS_YC[i]);
		}
	}
	printf("plc模块 接收到的 pcs总数:%d\n",total_pcsnum);
	
	for (sn = 0; sn < total_pcsnum; sn++)
	{
		if(sn>=0 && sn<16)
		{
			flag_RecvNeed_PCS_YX[0] |= (1<<sn);
		}
		else if(sn>=16 && sn<32)
		{
			flag_RecvNeed_PCS_YX[1] |= (1 << (sn-16));
		}
		else{
			flag_RecvNeed_PCS_YX[2] |= (1 << (sn - 32));
		}
	}

	// for (i = 0; i < pPara_plc->lcdnum; i++)
	// {
	// 	if (pPara_plc->pcsnum[i] > 0)
	// 	{
	// 		for(j=0;j<pPara_plc->pcsnum[i];j++)
	// 		{
	// 			sn=i*6+j;
	// 			if(sn>=0 && sn<16)
	// 			{
	// 				flag_RecvNeed_PCS_YX[0] |= (1<<sn);
	// 			}
	// 			else if(sn>16 && sn<32)
	// 			{
	// 				flag_RecvNeed_PCS_YX[1] |= (1 << (sn-16));
	// 			}
	// 			else{
	// 				flag_RecvNeed_PCS_YX[2] |= (1 << (sn - 32));
	// 			}
	// 		}
	// 	}
	// 	printf("PLC 处理 flag_RecvNeed_PCS_YX[%d]:%d\n",i,flag_RecvNeed_PCS_YX[i]);
	// }
}

int recvfromlcd(unsigned char type, void *pdata)
{
	switch (type)
	{
	case _YC_:
	{
		int pw_total=0,i;
		static unsigned char flag_recv_pcs[] = {0, 0, 0, 0, 0, 0};
		static int flag_recv_lcd = 0;
		

		LCD_YC_YX_DATA temp;
		temp = *(LCD_YC_YX_DATA *)pdata;
		short pw, qw, aw, tw;
		int sn = 0;
		unsigned short temp_pw, temp_qw, temp_aw, temp_tw;
		temp_pw = temp.pcs_data[Active_power];

		
		temp_pw = temp.pcs_data[Active_power];
		pw = (temp_pw % 256) * 256 + temp_pw / 256;
		temp_qw = temp.pcs_data[Reactive_power];
		qw = (temp_qw % 256) * 256 + temp_qw / 256;

		temp_aw = temp.pcs_data[Apparent_power];
		aw = (temp_aw % 256) * 256 + temp_aw / 256;
		temp_tw = temp.pcs_data[Temperature_IGBT];
		tw = (temp_tw % 256) * 256 + temp_tw / 256;
		printf("plc模块 接收到的遥测  sn:%d \n",sn);
		printf("plc模块 1PLC收到的有功功率 lcdid=%d pcsid=%d pw=%hd %hx\n", temp.lcdid, temp.pcsid, pw, temp_pw);
		// printf("2PLC收到的无功功率 lcdid=%d pcsid=%d qw=%hd %hx\n", temp.lcdid, temp.pcsid, qw, temp_qw);
		// printf("3PLC收到的视在功率 lcdid=%d pcsid=%d aw=%hd %hx \n", temp.lcdid, temp.pcsid, aw, temp_aw);
		printf("plc模块 4PLC收到的IGBT温度 lcdid=%d pcsid=%d aw=%hd %hx \n", temp.lcdid, temp.pcsid, tw, temp_tw);
		// sn = temp.lcdid * 6 + temp.pcsid - 1;
		sn = temp.sn;
		if(pw < 0){
			pw=-pw;
		}
		yc_PW[temp.sn]=pw;
		SendLcdDataToThread(sn + 5, pw);
		SendLcdDataToThread(sn + 53, tw);

		flag_recv_pcs[temp.lcdid] |= (1 << (temp.pcsid - 1));
		if (flag_recv_pcs[temp.lcdid] == flag_RecvNeed_PCS_YC[temp.lcdid])
		{
			flag_recv_lcd |= (1 << temp.lcdid);
			flag_recv_pcs[temp.lcdid] = 0;
		}

		

		if(flag_recv_lcd == pPara_plc->flag_RecvNeed_LCD){
			for(i = 0;i < total_pcsnum;i++){
				pw_total+=yc_PW[i];
				// printf("plc接收到pcs的遥测数据：%d val:%d \n",i,pw_total);
			}
			SendLcdDataToThread(52, pw_total);
		}	
	}
	break;

		case _YX_:
	{
		static unsigned short flag_recv_pcs[] = {0, 0, 0};
		static unsigned short outdata[] = {0, 0, 0};
		unsigned short b;
		LCD_YC_YX_DATA temp;
		int val;
		int flag = 0;
		unsigned short regAddr;
		temp = *(LCD_YC_YX_DATA *)pdata;
		int sn,id=0;
		// sn=temp.lcdid*6+temp.pcsid-1;
		sn = temp.sn;
		printf("plc模块 接收到的遥信  sn:%d 变流运行状态1:%d\n",sn,temp.pcs_data[u16_InvRunState1]);

		
		if(sn>=0 && sn<16)
		{
			id=0;
			
		}
		else if(sn>=16 && sn<32)
		{
			id=1;
			sn-=16;
		}
		else
		{
			id=2;
			sn-=32;
		}
		// printf("plc 接收到的遥信  sn:%d id：%d\n",sn,id);

		b = temp.pcs_data[u16_InvRunState1] & (1 << bPcsRunning);
		
		if (b > 0)
		{
			// printf("plc遥信置1 %d %d\n",outdata[id],sn);
			outdata[id] |= (1 << sn);
		}else if(b == 0){
			// printf("plc遥信置0 %d %d\n",outdata[id],sn);
			outdata[id] &= ~(1 << sn);
		}

		flag_recv_pcs[id] |= (1 << sn);
		
		if (id==0)
		{
			if (flag_recv_pcs[0] == flag_RecvNeed_PCS_YX[0])
			{
				val = HL_BitConvert(outdata[0]);
				regAddr = 2;
				flag = 1;
			}
		}
		else if (id==1)
		{
			// printf(" plc 遥信flag_recv_pcs[1]:%d flag_RecvNeed_PCS_YX[1]：%d\n",flag_recv_pcs[1],flag_RecvNeed_PCS_YX[1]);
			if (flag_recv_pcs[1] == flag_RecvNeed_PCS_YX[1])
			{

				val = HL_BitConvert(outdata[1]);
				regAddr = 3;
				flag = 1;
			}
		}
		else if (id==2)
		{
			// printf(" plc 遥信flag_recv_pcs[2]:%d flag_RecvNeed_PCS_YX[2]:%d\n",flag_recv_pcs[2],flag_RecvNeed_PCS_YX[2]);
			if (flag_recv_pcs[2] == flag_RecvNeed_PCS_YX[2])
			{
				val = HL_BitConvert(outdata[2]);
				regAddr = 4;
				flag = 1;
			}
		}
		else
			printf("plc模块 PLC接收遥信数据出现错误！！\n");

		if (flag == 1){
			SendLcdDataToThread(regAddr, val);
			flag_recv_pcs[id]=0;
			outdata[id]=0;
		}
			
	}
	break;
	
/*
	case _YX_:
	{
		static unsigned short flag_recv_pcs[] = {0, 0, 0};
		static unsigned short outdata[] = {0, 0, 0};
		unsigned short b;
		LCD_YC_YX_DATA temp;
		int val;
		int flag = 0;
		unsigned short regAddr;
		temp = *(LCD_YC_YX_DATA *)pdata;
		int sn,id;
		// sn=temp.lcdid*6+temp.pcsid-1;
		sn = temp.sn;
		
        if(sn>=0 && sn<16)
		{
			id=0;
			
		}
		else if(sn>16 && sn<32)
		{
			id=1;
			sn-=16;
		}
		else
		{
			id=2;
			sn-=32;
		}

		printf("plc 接收到的遥信  sn:%d id：%d\n",sn,id);
		// b = temp.pcs_data[u16_InvRunState1];
		b = temp.pcs_data[u16_InvRunState1] & (1 << bPcsRunning);
		// printf("PCSid:%d ")
		if (b > 0)
		{
			outdata[id] |= (1 << sn);
		}

		flag_recv_pcs[id] |= (1 << sn);

		if (id==0)
		{
			if (flag_recv_pcs[0] == flag_RecvNeed_PCS_YX[0])
			{
				val = HL_BitConvert(outdata[0]);
				printf("0-1 val :%x\n",val);
				regAddr = 2;
				flag = 1;
			}
		}
		else if (id==1)
		{
			if (flag_recv_pcs[1] == flag_RecvNeed_PCS_YX[1])
			{
				val = HL_BitConvert(outdata[1]);
				printf("2-4 val :%x\n", val);
				regAddr = 3;
				flag = 1;
			}
		}
		else if (id==2)
		{
			if (flag_recv_pcs[2] == flag_RecvNeed_PCS_YX[2])
			
				{
					val = HL_BitConvert(outdata[2]);
					printf("-4 val :%x\n", val);
					regAddr = 4;
					flag = 1;
				}
		}
		else
			printf("PLC接收遥信数据出现错误！！\n");
		if (flag == 1){
			SendLcdDataToThread(regAddr, val);
			flag_recv_pcs[id]=0;
			outdata[id]=0;
		}
			
	}
	break;
*/

	default:
		break;
	}
	return 0;
}

void subscribeFromLcd(void)
{

	void *handle;
	char *error;
#define LIB_LCD_PATH "/usr/local/lib/libmodtcp.so"
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

	printf("plc模块 订阅LCD遥测、遥信数据\n");
	my_func(_YC_, recvfromlcd);
	my_func(_YX_, recvfromlcd);

}
