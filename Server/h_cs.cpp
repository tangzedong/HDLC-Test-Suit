#include"framedef.h"
u_short h_cs_cal(u_char h_cs[MAX_LEN],u_char hcs_len)
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
	/*for(unsigned int i = 0;i<256;i++)
    {
		printf("0x%04x  ",crc16_ccitt_table[i]);
		if (!((i+1) % 8))
			printf("\n");
	}//生成表格*/
    unsigned short hcs = 0xffff;    // 初始化
	//unsigned char str[] = {0xA0,0x08,0x21,0x02,0x23,0x1F};
	unsigned int nLength = hcs_len + 4;//不计算开始和结束标志
	unsigned char*pData = h_cs;
	pData++;//从第二个字节开始
    while(nLength>0)
    {
        hcs = (hcs >> 8) ^ crc16_ccitt_table[(hcs ^ *pData) & 0xff];
        nLength--;
        pData++;
    }
	hcs = ~hcs;    // 取反
	unsigned short hcs_ch = hcs;
	hcs_ch  = (hcs_ch>> 8);//取第一个字节
	hcs =hcs<< 8;//取第二个字节
	hcs = hcs_ch | hcs;
	 //printf("0x%04x",hcs);
	return hcs;
}
