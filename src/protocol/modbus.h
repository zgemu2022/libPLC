#ifndef _MODBUS_H
#define _MODBUS_H

#define MAX_LCD_NUM 36

typedef struct
{
    unsigned char flag_waiting; //等待接收 0 不等待 1等待
    unsigned short num_frame;   //帧序号
    unsigned char dev_id;       //从设备地址
    unsigned char code_fun;     //功能码
    unsigned short regaddr;     //寄存器地址
    unsigned short numdata;     //数据个数

} PcsData_send; //当前发送到led数据，发送前记录，作为接收对比依据

extern PcsData_send g_send_data[];
extern unsigned short g_num_frame;
extern int wait_flag;
extern short fun03_PlcData;

int doPwFun06Task(int id_thread);
int doStatusFun06Task(int id_thread);
int doFun03Tasks(int id_thread);
int AnalysModbus(int id_thread, unsigned char *pcsdata, int len);

#endif