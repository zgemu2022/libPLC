#include <stdlib.h>
#include <stdio.h>
#include "withLCD.h"
#include "YX_Define.h"
#include "YC_Define.h"
#include "modbus.h"
#include "client.h"

#include <string.h>
#include "crc.h"
#include "my_socket.h"
#include "sys.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include "unistd.h"
#include "stdlib.h"
#include "plc_main.h"

int curStatusTaskId = 0;
short fun03_PlcData;
PcsData_send g_send_data[MAX_LCD_NUM];

unsigned char PLC_PW_Addr[] =
	{
		0x0028, 0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,
		0x002a, 0x000b, 0x000d, 0x000f, 0x0010,
		0x0011, 0x0012, 0x0013, 0x0014, 0x0015,
		0x0016, 0x0017, 0x0018, 0x0019, 0x001a,
		0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
		0x0020, 0x0021, 0x0022};

unsigned char PLC_STATUS_Addr[] =
	{
		0x0001,
		0x0002,
		0x0003,
		0x0004};

int doPwFun06Task(int id_thread)
{
	printf("\n\n");
	printf("06 PW...\n");
	int pos = 0;
	int yc_id = Yc_sn;
	printf("   Yc_sn:%d \n", Yc_sn);
	unsigned char framebuf[256];
	unsigned short regAddr = PLC_PW_Addr[yc_id];

	unsigned char send_data = Yc_PW_Data[yc_id];
	printf("send_data:%d\n", Yc_PW_Data[yc_id]);
	unsigned char dev_id = 0x01;
	printf("regAddr:%d\n", regAddr);

	framebuf[pos++] = g_num_frame / 256;
	framebuf[pos++] = g_num_frame % 256;
	framebuf[pos++] = 0;
	framebuf[pos++] = 0;
	framebuf[pos++] = 0;
	framebuf[pos++] = 6;
	framebuf[pos++] = dev_id; //设备地址
	framebuf[pos++] = 0x06;
	framebuf[pos++] = regAddr / 256;
	framebuf[pos++] = regAddr % 256;
	framebuf[pos++] = send_data / 256;
	framebuf[pos++] = send_data % 256;

	g_num_frame++;
	if (g_num_frame == 0x10000)
		g_num_frame = 1;

	printf("向plc 发送数据:");
	int i;
	for (i = 0; i < pos; i++)
	{
		printf("%#x ", framebuf[i]);
	}
	printf("\n");

	if (send(modbus_client_sockptr[id_thread], framebuf, pos, 0) < 0)
	{
		printf("plc 发送失败！！！！id_thread=%d\n", id_thread);
		return 0xffff;
	}
	else
	{
		wait_flag = 1;
		printf("plc任务包发送成功！！！！\n");
	}

	return 0;
}

int doStatusFun06Task(int id_thread)
{
	printf("\n\n");
	printf("06 Status...\n");
	int numTask = ARRAY_LEN(PLC_STATUS_Addr);
	int pos = 0;
	int _taskid = curStatusTaskId;
	printf("taskid:%d\n", _taskid);
	// unsigned char status_Addr = PLC_STATUS_Addr[_taskid]; 
	 unsigned char status_Addr = PLC_STATUS_Addr[3];//测试
	unsigned char framebuf[256];
	unsigned short regAddr = status_Addr;
	unsigned short send_data = 0;
	unsigned char dev_id = 0x01;

	printf("Yx_Pcs_Status:%d\n", Yx_Pcs_Status);
	printf("regAddr:%d\n", regAddr);

	if (regAddr == 0x0002)
	{
		// send_data = Yx_Pcs_Status;
		send_data = MyConvert(0xff00);
	}
	else if (regAddr == 0x0003)
	{
		// send_data = Yx_Pcs_Status;
		send_data = MyConvert(0xff00);
	}
	else if (regAddr == 0x0004)
	{
		// send_data = Yx_Pcs_Status;
		send_data = MyConvert(0xff00);
	}else if(regAddr == 0x0001){
		send_data = MyConvert(0x00ff);
		printf("senddata:%d\n",send_data);
	}

	framebuf[pos++] = g_num_frame / 256;
	framebuf[pos++] = g_num_frame % 256;
	framebuf[pos++] = 0;
	framebuf[pos++] = 0;
	framebuf[pos++] = 0;
	framebuf[pos++] = 6;
	framebuf[pos++] = dev_id; //设备地址
	framebuf[pos++] = 0x06;
	framebuf[pos++] = regAddr / 256;
	framebuf[pos++] = regAddr % 256;
	framebuf[pos++] = send_data / 256;
	framebuf[pos++] = send_data % 256;

	_taskid++;
	if (_taskid >= numTask)
		_taskid = 0;

	g_num_frame++;
	if (g_num_frame == 0x10000)
		g_num_frame = 1;

	printf("plc 任务包发送成功！！！！\n");
	printf("plc 发送数据:");
	int i;
	for (i = 0; i < pos; i++)
	{
		printf("%#x ", framebuf[i]);
	}
	printf("\n");

	if (send(modbus_client_sockptr[id_thread], framebuf, pos, 0) < 0)
	{
		printf("plc 发送失败！！！！id_thread=%d\n", id_thread);
		return 0xffff;
	}
	else
	{
		wait_flag = 1;
		printf("plc 任务包发送成功！！！！\n");
	}

	curStatusTaskId = _taskid;

	return 0;
}

int doFun03Tasks(int id_thread)
{
	printf("\n\n");
	printf("plc 03 YX\n");
	int pos = 0;
	unsigned char framebuf[256];
	unsigned short regAddr = 0x0000;
	unsigned char num = 1;
	unsigned char dev_id = 0x01;

	printf("regAddr:%d\n", regAddr);
	framebuf[pos++] = g_num_frame / 256;
	framebuf[pos++] = g_num_frame % 256;
	framebuf[pos++] = 0;
	framebuf[pos++] = 0;
	framebuf[pos++] = 0;
	framebuf[pos++] = 6;
	framebuf[pos++] = dev_id; //设备地址
	framebuf[pos++] = 0x03;
	framebuf[pos++] = regAddr / 256;
	framebuf[pos++] = regAddr % 256;
	framebuf[pos++] = num / 256;
	framebuf[pos++] = num % 256;

	printf("向plc 发送数据:");
	int i;
	for (i = 0; i < pos; i++)
	{
		printf("%#x ", framebuf[i]);
	}
	printf("\n");

	if (send(modbus_client_sockptr[id_thread], framebuf, pos, 0) < 0)
	{
		printf("plc 发送失败！！！！id_thread=%d\n", id_thread);
		return 0xffff;
	}
	else
	{
		wait_flag = 1;
		printf("任务包发送成功！！！！");
		g_send_data[id_thread].num_frame = g_num_frame;
		g_send_data[id_thread].flag_waiting = 1;
		g_send_data[id_thread].code_fun = 3;
		g_send_data[id_thread].dev_id = dev_id;
		g_send_data[id_thread].numdata = 1;
		g_send_data[id_thread].regaddr = regAddr;

		g_num_frame++;
		if (g_num_frame == 0x10000)
			g_num_frame = 1;

		printf("g_send_data[%d]flag_waiting:%x, code_fun:%x,dev_id:%x,numdata:%x,regaddr:%x\n", id_thread, g_send_data[id_thread].flag_waiting,
			   g_send_data[id_thread].code_fun, g_send_data[id_thread].dev_id, g_send_data[id_thread].numdata, g_send_data[id_thread].regaddr);
		printf("plc 任务包发送成功！！！！\n");
	}
	return 0;
}

int AnalysModbus(int id_thread, unsigned char *pdata, int len)
{
	unsigned char emudata[256];
	unsigned char funid;
	unsigned short regAddr;

	unsigned short val;
	printf("解析收到的LCD数据！！！！！\n");

	if (g_send_data[id_thread].flag_waiting == 0)
		return 1;
	if ((pdata[0] * 256 + pdata[1]) != g_send_data[id_thread].num_frame)
	{
		return 2;
	}


    // printf("需要解析的数据为:");
    // int i;
    // for(i=0;i<len;i++){
    //     printf("%#x ",pdata);
    // }
    // printf("\n");


	g_send_data[id_thread].flag_waiting = 0;

	memcpy(emudata, &pdata[6], len - 6);
	funid = emudata[1];
	printf("funid:%d\n", funid);

	if (funid != g_send_data[id_thread].code_fun)
		return 3;

	if (funid == 0x6)
	{
		regAddr = emudata[2] * 256 + emudata[3];
		if (regAddr != g_send_data[id_thread].regaddr)
		{
			return 4;
		}
	}

// <<<<<<< HEAD
	if (funid == 0x3)
	{
		fun03_PlcData = emudata[3] * 256 + emudata[4];
		pFun((void *)&fun03_PlcData);
	}
// =======
//     if (funid == 0x3)
//     {
//         fun03_PlcData = emudata[3] * 256 + emudata[4];
//         pFun((void *)&fun03_PlcData);
//     }
// >>>>>>> 015db44a32471e447303fb375f2024d6389b8aa9

	return 0;
}