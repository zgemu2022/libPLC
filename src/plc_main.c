#include "client.h"
#include "plc_main.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include "modbus.h"

CallbackYK pbackBmsFun = NULL;

CallbackYK pbackBmsFun_YX = NULL;


int plc_main(void *para)
{
	memcpy((void *)pPara_plc, para, sizeof(PARA_PLC));
	CreateThreads();
	printf("12345 ssdlh\n");
	return 0;
}

// int SubscribePlcData(outData2Other pfun) //订阅PLC数据
// {
// 	pFun = pfun;
// 	// pfun((void *)&fun03_PlcData);
// 	return 0;
// }

int ykOrderFromBms(unsigned char type, YK_PARA *pYkPara, CallbackYK pfun)
{
	
	if(type==_BMS_YX_)
	{
		 printf("89998\n");
         pbackBmsFun_YX = pfun;
	}
    else if(type==_BMS_PLC_YK_)
	{
		printf("666_BMS_PLC_YK_ pYkPara->data[0]：%d\n",pYkPara->data[0]);
		pbackBmsFun = pfun;
		if(pYkPara->data[0]>0)
		   SendBmsDataToThread(pYkPara->item);
	}
	else
	    pbackBmsFun = pfun;

	return 0;
}