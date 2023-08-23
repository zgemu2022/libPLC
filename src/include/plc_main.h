#ifndef _PLC_MAIN_H_
#define _PLC_MAIN_H_
#include "modbus.h"
#define MAX_CONN_NUM 2

#define MAX_PCS_NUM 6
#define _BMS_YX_ 0
#define _BMS_YK_ 1
#define _PCS_YK_ 2
#define _BMS_PLC_YK_ 4
typedef int (*orderToLcd)(int); //输出指令
typedef int (*CallbackYK)(unsigned char, void *pdata);	  //遥控回调函数签名
typedef struct
{
	char server_ip[64];
	unsigned short server_port;
	unsigned char lcdnum;
	unsigned char pcsnum[MAX_PCS_NUM];
	orderToLcd funOrder;
	int flag_RecvNeed_LCD;
} PARA_PLC; //从主控传到plc模块的结构
extern CallbackYK pbackBmsFun_YX;

int ykOrderFromBms(unsigned char type, YK_PARA *pYkPara, CallbackYK pfun);

#endif