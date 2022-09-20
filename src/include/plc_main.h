#ifndef _PLC_MAIN_H_
#define _PLC_MAIN_H_

#define MAX_CONN_NUM 2
typedef struct
{
	int conn_num;
	char  server_ip[MAX_CONN_NUM][64];
	unsigned short server_port[MAX_CONN_NUM];
} PARA_PLC; //从主控传到plc模块的结构1
#endif