#ifndef _WITH_LCD_H_
#define _WITH_LCD_H_
#include "interface.h"
typedef struct
{
	int sn;
	int lcdid;
	int pcsid;
	unsigned short pcs_data[MAX_YCDATA_NUM];
	unsigned char data_len;
}LCD_YC_YX_DATA;//
typedef struct
{
	int pcsnum;

}LCD_PARA;//

// extern LCD_YC_YX_DATA yc_data[MAX_TOTAL_PCS_NUM];
unsigned short MyConvert(unsigned short sval); //测试用

extern short Yc_PW_Data[]; //
extern int Yc_sn;
// extern unsigned int Yx_Pcs_Status;
extern unsigned short Yx_Pcs_Status; // 测试代码
#endif