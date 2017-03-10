#pragma once
#ifndef _HDLC
#define _HDLC
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
//#include "ServerDlg.h"
struct _hdlctcb;
typedef struct _hdlctcb HdlcTcb;

struct _hdlc;
struct _statparam;
typedef struct _statparam HdlcStationParam;
typedef struct _hdlc hdlc, *hdlcpointer;

typedef unsigned int u_int;   //32位
typedef unsigned char u_char;  //8位
typedef unsigned short u_short;//16位

extern u_char errorcode[];
extern int glen;
extern int gUAdatalen;
//extern hdlc m_UIFrame;
//extern u_char m_UIInfoBuf[];
//extern u_int m_UIInfoLen;
//extern HdlcStationParam *stpar;

#define MAX_LEN 2048
#define PPPINITFCS16 0xffff;             //初始FCS值
#define PPPGOODFCS16 0xf0b8;   //好的最终FCS值
#define ASSIGNRIND(CtrlByte, RIND) (CtrlByte &= 0x1F, CtrlByte |= (RIND<<5))
#define ASSIGNSIND(CtrlByte, SIND) (CtrlByte &= 0xF8, CtrlByte |= SIND)
#define SWITCHPF(CtrlByte, flag) (CtrlByte &= 0xEF, CtrlByte |= flag<<4)

#define CSEXCEPTION(code, ecptbit) (code & (1<<ecptbit))

class CServerDlg;

unsigned short fcstab[];

typedef struct frame_format{
	u_char frame_type;   //帧类型
#define FFORMTYPE 0xA0

	u_char frame_seg;    //分段位
	u_short frame_sublen;  //帧长度子域
} frame_format, *frame_formatpointer;

struct _hdlc{
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

};// hdlc, *hdlcpointer;

int readoneline(FILE* q, u_char oneLineData[MAX_LEN]);//void readoneline(FILE* q,u_char* t);
void writetxt(FILE* z, u_char write[MAX_LEN], u_char out_len);
u_short h_cs_cal(u_char h_cs[MAX_LEN], u_char hcs_len);
u_short f_cs_cal(u_char f_cs[MAX_LEN], u_char fcs_len);
u_short cs_cal(u_char data[MAX_LEN], u_char data_len);

u_int gethighaddr(u_int addr, u_int addrlen);
u_int getlowaddr(u_int addr, u_int addrlen);
BOOL vailfyBoardCastAddr(HdlcStationParam *stpar, hdlc *frame);
void DecodeTransChannel(CString strdata, CString &outdata);
void EncodeTransChannel(CString strdata, CString &outdata);
enum{
	SNRM1 = 0,
	SNRM2,
	SNRM4,

};
enum{
	ERROR_STARTFLAG = 1,
	ERROR_ENDFLAG,
	ERROR_FORMAT_TYPE,
	ERROR_DST_ADDR_LEN,
	ERROR_SRC_ADDR_LEN,
	ERROR_INF_OVERFLOW,
	ERROR_FCS,
	ERROR_HCS,
	ERROR_NOT_HDLC_TFRAME,
	ERROR_TIMEOUT_HDLC,
	ERROR_UNKONW_TYPE
};

enum{
	EXCEPTION_FCS = 0,
	EXCEPTION_HCS,
};


enum
{
	WAIT_FOR_FLAG,
	BEGIN_FRAME,
	READ_TYPE,
	READ_SRCADDR,
	READ_DSTADDR,
	READ_CONTROLBYTE,
	READ_HCS,
	READ_FCS,
	READ_INFO,
	END_NORMAL,
	WAIT_FOR_ENDFLAG
};

enum{
	NO_START_FLAG=1,
	NO_ENDFLAG,
	NOT_MATCH_LEN,
	FTYPE_FIELD_ERROR,
	DSTADDR_FIELD_ERROR,
	SRCADDR_FIELD_ERROR,
	CONTROLBYTE_FIELD_ERROR,
	HCS_FIELD_ERROR,
	INFO_FIELD_ERROR
};
struct _statparam
{
	u_int slav_pot_addr;
	u_int main_pot_addr;
	u_char slav_pot_addrlen;
	u_char main_pot_addrlen;
	unsigned char started;//值为1进入状态循环
	unsigned char rcv_num;      //记录的接收序号
	unsigned char send_num;   //记录的发送序号
	unsigned char nr;
	unsigned char ns;
	//unsigned char frmr_flag;          //值为1发送FRMR帧
	unsigned char frame_p_f;           //记录P/F位
	unsigned char seg;				//记录seg位

	u_char *save_inf; //数据存储区
	u_int m; //记录信息分帧存储的变量

	unsigned char frmr_flag : 1;			   //待发FRMR标志
	unsigned char frame_discard_flag : 1;   //无效帧标致
	unsigned char disc : 1;
	unsigned char discard : 1;

	unsigned char frmr : 1;
	unsigned char canUISend : 1;
	unsigned char isUIWaiting : 1;
	unsigned char isTransFinish : 1;

	unsigned char isSendData : 1;
	unsigned char send_flag : 1;

	unsigned int max_rcv_info_size;
#define	MAX_RCVINFO_SIZE 0xf2
#define ARG_RCVINFO_INDEX 9

	unsigned int rcvwindowsize;
#define MAX_RCV_WINDOW_SIZE 0xf2
#define ARG_RCVWINSIZE_INDEX 19

	unsigned int max_snd_info_size;
#define	MAX_SNDINFO_SIZE 0xf2
#define ARG_SNDINFO_INDEX 5 

	unsigned int sendwindowsize;
#define MAX_SEND_WINDOW_SIZE 0xf2
#define ARG_SNDWINSIZE_INDEX 13

	unsigned char *sendbuf;
	unsigned int sendlen;
	unsigned int seglen;
	unsigned int segtail;
	unsigned int numSegSended;

	u_char *settingdata;
	HdlcTcb *fsmstack;
	CServerDlg *hWnd;
	bool SendComplete;
};// HdlcStationParam;

struct _hdlcsetting
{
	u_int maxInfoSize;
	u_int winSize;
};

typedef struct _hdlcparam
{
	u_char arg;
#define ARG_MAX_SND_SIZE 0x05
#define ARG_SND_WIN_SIZE 0x07
#define ARG_MAX_RCV_SIZE 0x06
#define ARG_RCV_WIN_SIZE 0x08

	u_short value;
	_hdlcparam *next;
}hdlcparam;

#define TM_IDLETIMEOUT 1
#define TM_SENDDATA 100
#define TM_RESENDWND 101
#define TM_INTERFRAME_TIMEOUT 102
#define TM_SERVEREVENT 103

#endif bool DataTranFinish; u_int IndFrameOfWnd; int IndFrameOfWnd;