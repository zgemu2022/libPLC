#include "client.h"
#include "plc_main.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>



int plc_main(void* para)
{
    memcpy((void*)pPara_plc,para,sizeof(PARA_PLC));
	CreateThreads();
	printf("12345 ssdlh\n");
	return 0;
}

int SubscribePlcData(outData2Other pfun) //订阅PLC数据
{
	pFun=pfun;
	// pfun((void *)&fun03_PlcData);
	return 0;
}