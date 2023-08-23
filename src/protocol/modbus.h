#ifndef _MODBUS_H
#define _MODBUS_H

#define MAX_LCD_NUM 36

typedef struct
{
	unsigned char dev_id;	//从设备地址
	unsigned short regaddr; //寄存器地址
	unsigned short val;

} PcsData_send;



// 0 BIT0备用
#define PLC_EMU_BOX_35kV_ON 1// BIT135kV进线柜合闸
#define PLC_EMU_BOX_35kV_OFF 2 // BIT235kV进线柜分闸
#define PLC_EMU_BOX_SwitchD1_ON 3// BIT3开关柜D1合闸
#define PLC_EMU_BOX_SwitchD1_OFF 4// BIT4开关柜D1分闸
#define PLC_EMU_BOX_SwitchD2_ON 5// BIT5开关柜D2合闸
#define PLC_EMU_BOX_SwitchD2_OFF 6// BIT6开关柜D2分闸
#define PLC_EMU_TRANSFORMER_ROOM_OPEN 7// BIT7变压器室外门打开
#define PLC_EMU_DSTART 8  // BIT0系统一键放电
#define PLC_EMU_STOP   9  // BIT1系统一键停止
#define PLC_EMU_CSTART 10  // BIT2系统一键充电
// 11 BIT3备用
// 12 BIT4备用
// 13 BIT5备用
// 14 BIT6备用
// 15 BIT7备用



//BIT0备用	BOOL	EMU→PLC	V102.0	
#define EMU_PLC_BOX_35kV_ON 1 //BIT135kV进线柜合闸指令	BOOL	EMU→PLC	V102.1	Q0.1
#define EMU_PLC_BOX_35kV_OFF 2 //BIT235kV进线柜分闸指令	BOOL	EMU→PLC	V102.2	Q0.2
#define EMU_PLC_BOX_SwitchD1_ON 3 //BIT3开关柜D1合闸指令	BOOL	EMU→PLC	V102.3	Q0.3
#define EMU_PLC_BOX_SwitchD1_OFF 4 //BIT4开关柜D1分闸指令	BOOL	EMU→PLC	V102.4	Q0.4
#define EMU_PLC_BOX_SwitchD2_ON 5 //BIT5开关柜D2合闸指令	BOOL	EMU→PLC	V102.5	Q0.5
#define EMU_PLC_BOX_SwitchD2_OFF 6 //BIT6开关柜D2分闸指令	BOOL	EMU→PLC	V102.6	Q0.6
// 412IT7备用	BOOL	EMU→PLC	V102.7	Q0.7
// 421BIT0备用	BOOL	EMU→PLC	V103.0	/
// 421BIT1备用	BOOL	EMU→PLC	V103.1	/
// 421BIT2备用	BOOL	EMU→PLC	V103.2	/
// 421BIT3备用	BOOL	EMU→PLC	V103.3	/
// 421BIT4备用	BOOL	EMU→PLC	V103.4	/
// 421BIT5备用	BOOL	EMU→PLC	V103.5	/
// 421BIT6备用	BOOL	EMU→PLC	V103.6	/
// 421BIT7备用	BOOL	EMU→PLC	V103.7	/


#define Emu_Startup 1				  //【整机】启机命令
#define Emu_Stop 2					  //【整机】停机命令
#define Parallel_Away_conversion_en 3 //并转离切换使能
#define Away_Parallel_conversion_en 4 //离转并切换使能
#define EMS_SET_MODE 5				  //产品运行模式设置
#define EMS_VSG_MODE 6				  // VSG工作模式设置
#define EMS_PQ_MODE 7				  // PQ工作模式设置
#define BOX_35kV_ON 8				  // 35kV进线柜合闸
#define BOX_35kV_OFF 9			  // 35kV进线柜分闸
#define BOX_SwitchD1_ON 10			  //开关柜D1合闸
#define BOX_SwitchD1_OFF 11			  //开关柜D1分闸
#define BOX_SwitchD2_ON 12			  //开关柜D2合闸
#define BOX_SwitchD2_OFF 13			  //开关柜D2分闸
#define TURN_ON_THE_FAN  14           //打开风扇
#define EMS_PW_SETTING 15			  //有功功率
#define EMS_QW_SETTING 16			  //无功功率
#define ONE_FM_PW_SETTING 17		  //一次调频有功功率
#define ONE_FM_QW_SETTING 18		  //一次调频无功功率

typedef struct
{
	unsigned char item;	   //项目编号
	unsigned char el_tag;  //  数据类型
	unsigned char data[5]; //参数
} YK_PARA;				   //遥测、遥控参数

int SendBmsDataToThread(unsigned char order);
int SendLcdDataToThread(unsigned short regAddr, unsigned short val);
// int CreateFun06Frame(unsigned short regAddr, unsigned short val);
int AnalysModbus(unsigned char *pdata, int len);
#endif