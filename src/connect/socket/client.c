#include "client.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "sys.h"
#include "crc.h"
#include "plc_main.h"
#include "my_socket.h"
#include <sys/socket.h>
#include <sys/msg.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "withLCD.h"
#include "YX_Define.h"
#include "YC_Define.h"
#include "modbus.h"
//当使用Modbus/TCP时，modbus poll一般模拟客户端，modbus slave一般模拟服务端

char modbus_sockt_state;
int modbus_client_sockptr;

int g_comm_qmegid_plc;

unsigned short g_num_frame = 1;

PARA_PLC Para_plc;
PARA_PLC *pPara_plc = (PARA_PLC *)&Para_plc;
int wait_flag = 0;
int myprintbuf(int len, unsigned char *buf)
{
	int i = 0;
	printf("\n PLC buflen=%d\n", len);
	for (i = 0; i < len; i++)
		printf("0x%x ", buf[i]);
	printf("\n");
	return 0;
}
int doFun03Tasks(void)
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

	printf("03向plc 发送数据:");
	myprintbuf(pos, framebuf);

	if (send(modbus_client_sockptr, framebuf, pos, 0) < 0)
	{
		printf("plc 发送失败！！！！\n");
		return 0xffff;
	}
	else
	{

		g_num_frame++;
		if (g_num_frame == 0x10000)
			g_num_frame = 1;
		wait_flag = 1;
		printf("plc 03任务包发送成功！！！！\n");
	}
	return 0;
}
int CreateFun06Frame(unsigned char dev_id, unsigned short regAddr, unsigned short val)
{
	unsigned char framebuf[256];
	// // CreateFun06Frame(lcdid*6+pcsid-1,val,&plc_data.len,plc_data.buf);

	int pos = 0;
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
	framebuf[pos++] = val / 256;
	framebuf[pos++] = val % 256;

	// g_num_frame++;
	// if (g_num_frame == 0x10000)
	// 	g_num_frame = 1;

	printf("06向plc 发送数据:dev_id=%d regAddr=%x  val=%x\n", dev_id, regAddr, val);
	myprintbuf(pos, framebuf);

	if (send(modbus_client_sockptr, framebuf, pos, 0) < 0)
	{
		printf("plc 发送失败！！！！\n");
		return 0xffff;
	}
	else
	{

		g_num_frame++;
		if (g_num_frame == 0x10000)
			g_num_frame = 1;
		wait_flag = 1;
		printf("plc 06 任务包发送成功！！！！\n");
	}

	return 0;
}
void *Modbus_clientSend_thread(void *arg) // 25
{

	// printf("aaaaaaaaaaaoofoifdf %d\n",id_thread);
	int ret_value = 0;
	msgClient pmsg;
	MyData plcdata;
	PcsData_send *pdata;
	int delayTim = 0;
	printf("PLC_clientSend_thread  is Starting!\n");
	key_t key = 0;
	g_comm_qmegid_plc = os_create_msgqueue(&key, 1);

	// unsigned char code_fun[] = {0x03, 0x06, 0x10};
	// unsigned char errid_fun[] = {0x83, 0x86, 0x90};

	while (modbus_sockt_state == STATUS_OFF)
	{
		sleep(1);
	}

	printf("modbus_sockt_state[id_thread] == STATUS_ON\n");
	while (modbus_sockt_state == STATUS_ON) //
	{
		// printf("wait_flag:%d\n", wait_flag);
		while (wait_flag == 1)
		{
			usleep(10);
		}
		ret_value = os_rev_msgqueue(g_comm_qmegid_plc, &pmsg, sizeof(msgClient), 0, 8); // 50ms
		if (ret_value >= 0)
		{
			memcpy((char *)&plcdata, (char *)&pmsg.data, sizeof(MyData));
			pdata = (PcsData_send *)plcdata.buf;
			CreateFun06Frame(pdata->dev_id, pdata->regaddr, pdata->val);
			//usleep(5);
		}
		else
		{
			delayTim++;
			if (delayTim >= 10)
			{
				delayTim=0;

				printf("PLC 暂无数据发送 执行功能码03！！！！！g_num_frame=%x \n", g_num_frame);
				doFun03Tasks();
			//	usleep(5);
			}
		}
	}

	return NULL;
}

static int recvFrame(int fd, int qid, MyData *recvbuf)
{
	int readlen;

	// int index = 0, length = 0;
	//  int i;
	// MyData *precv = (MyData *)&msg.data;
	readlen = recv(fd, recvbuf->buf, MAX_MODBUS_FLAME, 0);
	//		readlen = recv(fd, (recvbuf.buf + recvbuf.len),
	//				(MAX_BUF_SIZE - recvbuf.len), 0);
	//		printf("*****  F:%s L:%d recv readlen=%d\n", __FUNCTION__, __LINE__,	readlen);
	if (readlen < 0)
	{
		printf("PLC 连接断开或异常\r\n");
		return -1;
	}
	else if (readlen == 0)
		return 1;
	recvbuf->len = readlen;
	return 0;
}

//参数初始化
void *Modbus_clientRecv_thread(void *arg) // 25
{
	int fd = -1;
	fd_set maxFd;
	struct timeval tv;
	int ret;
	int i = 0, jj = 0;
	MyData recvbuf;
	int id_frame;
	int id_frame_last = 0;
	//int flag=0;
	_SERVER_SOCKET server_sock;
	server_sock.protocol = TCP;

	printf("PLC 设备: ip：%s,端口:%d\n", Para_plc.server_ip, Para_plc.server_port);
	server_sock.port = htons(Para_plc.server_port);
	server_sock.addr = inet_addr(Para_plc.server_ip);
	server_sock.fd = -1;

	sleep(4);
loop:
	while (1)
	{
		server_sock.fd = -1;
		if (_socket_client_init(&server_sock) != 0)
		{
			sleep(10);
		}
		else
			break;
	}
	printf("PLC 连接服务器成功！！！！\n");

	modbus_client_sockptr = server_sock.fd;
	modbus_sockt_state = STATUS_ON;

	jj = 0; //未接收到数据累计标志，大于1000清零
	i = 0;

	while (1)
	{
		//flag=0;
		fd = modbus_client_sockptr;
		if (fd == -1)
			break;
		FD_ZERO(&maxFd);
		FD_SET(fd, &maxFd);
		tv.tv_sec = 0;
		//    tv.tv_usec = 50000;
		tv.tv_usec = 8000;
		ret = select(fd + 1, &maxFd, NULL, NULL, &tv);
		if (ret < 0)
		{

			printf("PLC 网络有问题！！！！！！！！！！！！");
			break;
		}
		else if (ret == 0)
		{
			jj++;

			if (jj > 1000)
			{
				printf("PLC 暂时没有数据传入！！！！未接收到数据次数=%d！！！！！！！！！！！！！！！！\r\n", jj);
				jj = 0;

				//				break;
			}
			continue;
		}
		else
		{

			jj = 0;

			// printf("貌似收到数据！！！！！！！！！！！！");
			if (FD_ISSET(fd, &maxFd))
			{
				ret = recvFrame(fd, g_comm_qmegid_plc, &recvbuf);
				if (ret == -1)
				{
					i++;

					if (i > 30)
					{
						printf("PLC 接收不成功！！！！！！！！！！！！！！！！i=%d\r\n", i);
						break;
					}
					else
						continue;
				}
				else if (ret == 1)
				{
					// i++;

					// if(i>30)
					// {
					// 	printf("接收数据长度为0！！！！！！！！！！！！！！！！\r\n");

					// 	i=0;

					// }
					continue;
				}
				else
				{
					i = 0;
					myprintbuf(recvbuf.len, recvbuf.buf);
					AnalysModbus(recvbuf.buf, recvbuf.len);		 
					id_frame = recvbuf.buf[0] * 256 + recvbuf.buf[1];

					if (id_frame == (id_frame_last + 1))
					{
						printf("PLC recv接收成功！帧序号正常  recvbuf.len=%d id_frame=%x\r\n", recvbuf.len, id_frame);
						wait_flag = 0;

					}
					else
						printf("PLC recv接收成功！帧序号出现错乱  recvbuf.len=%d %x %x\r\n", recvbuf.len, id_frame, id_frame_last);
					id_frame_last = id_frame;

			
				}
			}
			else
			{
				printf("PLC 未知错误////////////////////////////////r/n");
				break;
			}
		}
	}
	modbus_sockt_state = STATUS_OFF;
	printf("PLC 网络断开，重连！！！！");
	goto loop;
}
unsigned int countRecvFlag(int num_read)
{
	unsigned int flag = 0;
	int i;
	for (i = 0; i < num_read; i++)
	{
		flag |= 1 << i;
	}
	return flag;
}
void CreateThreads(void)
{
	pthread_t ThreadID;
	pthread_attr_t Thread_attr;
	// pPara_Modtcp->pcsnum[1] = 1;
	// pPara_Modtcp->pcsnum[2] = 1;
	printf("PLC 设备:,ip：%s,端口:%d\n", Para_plc.server_ip, Para_plc.server_port);

	modbus_sockt_state = STATUS_OFF;
	int i,j,sn;

	for (i = 0; i < Para_plc.lcdnum; i++)
	{
		if (Para_plc.pcsnum[i] > 0)
		{
			for(j=0;j<Para_plc.pcsnum[i];j++)
			{
				sn=i*6+j;
				if(sn>=0 && sn<16)
				{
                    flag_RecvNeed_PCS[0] |= (1<<sn);
				}
				else if(sn>=16 && sn<32)
				{
                    flag_RecvNeed_PCS[1] |= (1<<sn);
				}
				else
				{
                    flag_RecvNeed_PCS[2] |= (1<<sn);
				}

			}
		}
			
	}

	if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr, (void *)Modbus_clientRecv_thread, NULL, 1, 1))
	{
		printf("PLC CONNECT THTREAD CREATE ERR!\n");

		exit(1);
	}
	if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr, (void *)Modbus_clientSend_thread, NULL, 1, 1))
	{
		printf("PLC THTREAD CREATE ERR!\n");
		exit(1);
	}
	subscribeFromLcd();
	printf("PLC THTREAD CREATE success!\n");
}
