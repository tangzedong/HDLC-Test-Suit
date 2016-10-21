#ifndef _HDLC_
#define _HDLC_

#include <stdio.h>
#include<malloc.h>
# include <stdlib.h>
#include <cctype>
#include <assert.h>
#pragma comment(lib,"ws2_32.lib")
#include"string.h"
#include <Windows.h>
typedef unsigned int u_int;   //32位
typedef unsigned char u_char;  //8位
typedef unsigned short u_short;//16位
/*typedef unsigned double u_double;//64位  8字节*/

#define MAX_LEN 2048
#define PPPINITFCS16 0xffff;             //初始FCS值
#define PPPGOODFCS16 0xf0b8;   //好的最终FCS值
#define ASSIGNRIND(CtrlByte, RIND) (CtrlByte &= 0x1F, CtrlByte |= (RIND<<5))
#define ASSIGNSIND(CtrlByte, SIND) (CtrlByte &= 0xF8, CtrlByte |= SIND)
#define SWITCHPF(CtrlByte, flag) (CtrlByte &= 0xEF, CtrlByte |= flag<<4)

//Protype of each Frame Type
#define TYPEI 0x00
#define TYPERR 0x01
#define TYPERNR 0x05
#define TYPESNRM 0x83
#define TYPEDISC 0x43
#define TYPEUA 0x63
#define TYPEDM 0x0F
#define TYPEFRMR 0x87
#define TYPEUI 0x03
#define GETTYPE(frame) TYPE##frame

#define PIN 1
#define FIN 0
unsigned short fcstab[];

typedef struct frame_format{
	u_char frame_type;   //帧类型
	u_char frame_seg;    //分段位
	u_short frame_sublen;  //帧长度子域
} frame_format, *frame_formatpointer;

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
	u_int infolen;
	u_int framelen;
}hdlc, *hdlcpointer;

u_short h_cs_cal(u_char h_cs[MAX_LEN], u_char hcs_len);
u_short f_cs_cal(u_char f_cs[MAX_LEN], u_char fcs_len);
int outHexStr(_TCHAR* S, u_char *pRawData, u_int index, u_int *bytesidx, u_char byte);
#endif