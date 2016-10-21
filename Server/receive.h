#ifndef _RECEIVE
#define _RECEIVE

#include <stdio.h>
#include<malloc.h>
# include <stdlib.h>
#include <cctype>
#include <assert.h>
#pragma comment(lib,"ws2_32.lib")
#include"string.h"
typedef unsigned int u_int;   //32位
typedef unsigned char u_char;  //8位
typedef unsigned short u_short;//16位
/*typedef unsigned double u_double;//64位  8字节*/

#define MAX_LEN 2048
#define PPPINITFCS16 0xffff;             //初始FCS值
#define PPPGOODFCS16 0xf0b8;   //好的最终FCS值

typedef struct frame_format{
	u_char frame_type;   //帧类型
	u_char frame_seg;    //分段位
	u_short frame_sublen;  //帧长度子域
} frame_format  ,*frame_formatpointer;

typedef struct hdlc{
    u_char start_flag;//开始标志1byte
	frame_format  f_format;//定义结构体来存放hdlc帧各个域的内容
	u_int dst_addr;//目的地址
	u_char src_addr; //源地址
	u_char frame_ctl;//控制域
	u_short h_cs;//头校验序列
	u_char info_buff[MAX_LEN];//存储信息域，最大位128字节
	u_short f_cs;//帧校验序列
	u_char end_flag;//结束标志
}hdlc , *hdlcpointer;

int readoneline(FILE* q,u_char oneLineData[MAX_LEN]);//void readoneline(FILE* q,u_char* t);
void writetxt(FILE* z,u_char write[MAX_LEN],u_char out_len);
u_short h_cs_cal(u_char h_cs[MAX_LEN],u_char hcs_len);
u_short f_cs_cal(u_char f_cs[MAX_LEN],u_char fcs_len);
#endif