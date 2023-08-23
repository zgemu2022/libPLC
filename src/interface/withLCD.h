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
} LCD_YC_YX_DATA; //
extern unsigned short HL_BitConvert(unsigned short sval);
extern unsigned short flag_RecvNeed_PCS[];
extern int total_pcsnum;

unsigned int countRecvFlag(int num_read);

void subscribeFromLcd(void);
#endif