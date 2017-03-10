#include "framedef.h"
u_short f_cs_cal(u_char f_cs[MAX_LEN],u_char fcs_len)
{
	typedef unsigned short u16;
    unsigned char index = 0;
    unsigned short to_xor;
    int i;
	unsigned short crc16_ccitt_table[256];
    //printf("unsigned short crc16_ccitt_table[256] =\n");
    while (1)
    {
        to_xor = index;      
        for (i = 0; i < 8; i++)
        {
            if (to_xor & 0x0001)
                to_xor = (to_xor >>1) ^ 0x8408;
            else
                to_xor >>= 1;          
        }           
		crc16_ccitt_table[index]=to_xor;
        if (index == 255)
            break;
        else
        {
            index++;
        }
    }
/*	for(unsigned int i = 0;i<256;i++)
    {
		printf("0x%04x  ",crc16_ccitt_table[i]);
		if (!((i+1) % 8))
			printf("\n");
	}//���ɱ��*/

    unsigned short fcs = 0xffff;    // ��ʼ��
	//unsigned char str[] = {0xA0,0x08,0x21,0x02,0x23,0x1F};
	unsigned int nLength = fcs_len + 6;
	unsigned char*pData = f_cs;
	pData++;//�ӵڶ����ֽڿ�ʼ
    while(nLength>0)
    {
        fcs = (fcs >> 8) ^ crc16_ccitt_table[(fcs ^ *pData) & 0xff];
        nLength--;
        pData++;
    }
	fcs = ~fcs;    // ȡ��
	unsigned short fcs_ch = fcs;
	fcs_ch  = (fcs_ch>> 8);//ȡ��һ���ֽ�
	fcs =fcs<< 8;//ȡ�ڶ����ֽ�
	fcs = fcs_ch | fcs;
	 //printf("0x%04x",fcs);
	return fcs;
}
