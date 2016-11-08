#ifndef _FRAMEDEF
#define _FRAMEDEF
#define MAX_FRAME 255

#include <stdio.h>
#include<malloc.h>
# include <stdlib.h>
#include <cctype>
#include <assert.h>
#pragma comment(lib,"ws2_32.lib")
#include"string.h"
//#include <Windows.h>
#include <tchar.h>
#include "hdlcFSM.h"
typedef struct _statparam HdlcStationParam;
typedef struct hdlc hdlc, *hdlcpointer;
typedef unsigned int u_int;   //32位
typedef unsigned char u_char;  //8位
typedef unsigned short u_short;//16位
/*typedef unsigned double u_double;//64位  8字节*/
u_char errorcode[];
extern int glen;

u_char settingdata[];
extern int gUAdatalen;
extern hdlc gUIFrame;
extern u_char gUIInfoBuf[];
extern u_int gUIInfoLen;
extern HdlcStationParam *gstpar;

#define MAX_LEN 2048
#define PPPINITFCS16 0xffff;             //初始FCS值
#define PPPGOODFCS16 0xf0b8;   //好的最终FCS值
#define ASSIGNRIND(CtrlByte, RIND) (CtrlByte &= 0x1F, CtrlByte |= (RIND<<5))
#define ASSIGNSIND(CtrlByte, SIND) (CtrlByte &= 0xF8, CtrlByte |= SIND)
#define SWITCHPF(CtrlByte, flag) (CtrlByte &= 0xEF, CtrlByte |= flag<<4)


unsigned short fcstab[];

#define MAX_LEN 2048
#define PPPINITFCS16 0xffff;             //初始FCS值
#define PPPGOODFCS16 0xf0b8;   //好的最终FCS值

typedef struct frame_format{
	u_char frame_type;   //帧类型
#define FFORMTYPE 0xA0

	u_char frame_seg;    //分段位
	u_short frame_sublen;  //帧长度子域
} frame_format, *frame_formatpointer;

typedef struct hdlc{
	u_char start_flag;//开始标志1byte
#define STARTFLAG 0x7e

	frame_format  f_format;//定义结构体来存放hdlc帧各个域的内容
	u_int dst_addr;//目的地址
	u_int src_addr; //源地址

	u_char frame_ctl;//控制域
//Protype of each Frame Type
#define TYPEI 0x00
#define TYPERR 0x01
#define TYPERNR 0x05
#define TYPESNRM 0x93
#define TYPEDISC 0x53
#define TYPEUA 0x63
#define TYPEDM 0x0F
#define TYPEFRMR 0x87
#define TYPEUI 0x03
#define GETTYPE(frame) TYPE##frame

#define MASKI 0x01
#define MASKRR 0x0F
#define MASKRNR 0x0F
#define MASKSNRM 0xFF
#define MASKDISC 0xFF
#define MASKUA 0xFF
#define MASKDM 0xFF
#define MASKFRMR 0xFF
#define MASKUI 0xEF

	u_short h_cs;//头校验序列
	u_char info_buff[MAX_LEN];//存储信息域，最大位128字节
	u_short f_cs;//帧校验序列
	u_char end_flag;//结束标志
#define STARTFALG 0x7e

	u_int infolen;
	u_int dst_addrlen;
	u_int src_addrlen;
	u_int nr;
	u_int ns;

	u_char pollfin;
#define POL 1
#define FIN 0
#define FON 1

#define CTLPOLL 0x10 //PF控制位
#define CTLFIN 0x00

}hdlc, *hdlcpointer;

int readoneline(FILE* q, u_char oneLineData[MAX_LEN]);//void readoneline(FILE* q,u_char* t);
void writetxt(FILE* z, u_char write[MAX_LEN], u_char out_len);
u_short h_cs_cal(u_char h_cs[MAX_LEN], u_char hcs_len);
u_short f_cs_cal(u_char f_cs[MAX_LEN], u_char fcs_len);
u_short cs_cal(u_char data[MAX_LEN], u_char data_len);

enum{
	SNRM1 = 0,
	SNRM2,
	SNRM4,

};
enum{
	ERROR_STARTFLAG = 1,
	ERROR_DST_ADDR_LEN,
	ERROR_SRC_ADDR_LEN,
	ERROR_INF_OVERFLOW,
	ERROR_FCS,
	ERROR_HCS,
	ERROR_NOT_HDLC_TFRAME,
	ERROR_TIMEOUT_HDLC,
	ERROR_UNKONW_TYPE
};
int convFrameStr(hdlc *frame, _TCHAR* S);
int convFrameHex(hdlc *frame, u_char* pData);
int convStrFrame(_TCHAR* S, hdlc *frame);
int convHexFrame(u_char *pData, hdlc *hdlc_p);
int convHexStr(u_char *pData, _TCHAR* S, int len);
int convStrHex(_TCHAR* S, u_char* pData);
int outHexStr(_TCHAR* S, u_char *pRawData, int index, int *bytesidx, u_char byte);
void genFrameData(_TCHAR *S, u_char *pRawData, hdlc *frame);

int makeRR(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe, u_int nr, u_int pf);
int makeRNR(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe, u_int nr, u_int pf);
int makeUI(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe, u_char* data, u_int infolen);
int makeSNRM(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe);
int makeDISC(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe);
int makeDM(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe);
int makeFRMR(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen);
int makeUA(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe, u_char *settingdata, u_int len);

u_int GetTypes(hdlc &frame);


#endif