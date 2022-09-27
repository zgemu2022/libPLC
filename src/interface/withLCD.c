#include "withLCD.h"
#include <stddef.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "YX_Define.h"
#include "YC_Define.h"

LCD_YC_YX_DATA yc_data[MAX_TOTAL_PCS_NUM];
int Yc_sn;
short Yc_PW_Data[MAX_TOTAL_PCS_NUM]; //
unsigned int Yx_Pcs_Status = 0;

int recvfromlcd(unsigned char type, void *pdata)
{
	switch (type)
	{
	case _YC_:
	{
		LCD_YC_YX_DATA temp;
		temp = *(LCD_YC_YX_DATA *)pdata;
		Yc_PW_Data[temp.sn - 1] = temp.pcs_data[Active_power];
		Yc_sn = temp.sn;
		yc_data[temp.sn] = temp;
	}
	break;
	case _YX_:
	{
		unsigned char b, b1, b2;
		LCD_YC_YX_DATA temp;
		temp = *(LCD_YC_YX_DATA *)pdata;
		b = temp.pcs_data[0];
		b1 = b & (1 << bPcsRunning);
		b2 = Yx_Pcs_Status & (1 << (32 - temp.sn));
		if (b1 != b2)
		{
			if (b & (1 << bPcsRunning))
			{
				setbit(Yx_Pcs_Status, (32 - temp.sn));
				//			Yx_Pcs_Status |= (1<<(32-temp.sn));
			}
			else
			{
				clrbit(Yx_Pcs_Status, (32 - temp.sn));
			}
		}
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

	my_func(_YC_, recvfromlcd);
	// my_func(YC_POST,recvfromdllcan);
	// my_func(CANERR_POST,recvfromdllcan);
}
