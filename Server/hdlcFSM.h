#ifndef _HDLC_FSM
#define _HDLC_FSM
#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "framedef.h"
//#include "eventhandler.h"
#define INT8U unsigned short
#define INT16U unsigned short

//mac layer parameters
//timeout
#define TO_INACTIVE 20000//Inactivity time - out
#define TO_WAIT_RESP 20000// response timeout
#define TO_INTERFRAME 20000// NO USED RECENTLY, Inter - frame time - out, between reciving bytes
#define MAX_INF_LEN 0x80//Default Maximum information field length
#define WIN_SIZE 1//window size
#define MAX_NB_OF_RETRIES 10//retrying times

typedef struct hdlc hdlc, *hdlcpointer;
//States of Primary FSM
enum{
	STATE_NDM=0,
	STATE_WAIT_CONNECT,
	STATE_NRM,
	STATE_WAIT_DISCONNECT,
	STATE_WAIT_FRMR,
	STATE_FRMR
};

//States of NRM FSM
enum{
	STATE_NRM_IDLE = 0,
	STATE_IREPONSE_HANDLE,
	STATE_HANDLE2,
};

//States of I Responding FSM
enum{
	STATE_WAIT_CMD = 0,
	STATE_WAIT_RES,
	STATE_HANDLE3,
};

typedef struct _statparam
{
	unsigned char started;//值为1进入状态循环
	unsigned char rcv_num;      //我自己记录的接收序号
	unsigned char send_num;   //我自己记录的发送序号
	unsigned char nr;
	unsigned char ns;
	//unsigned char frmr_flag;          //值为1发送FRMR帧
	unsigned char frame_p_f;           //记录P/F位
	CServerDlg *hWnd;
	unsigned char send_flag;
	u_char *save_inf; //数据存储区
	u_int m; //数据计数

	unsigned char frmr_flag;			   //待发FRMR标志
	unsigned char frame_discard_flag;   //无效帧标致

	unsigned char disc;

	unsigned char discard;
	unsigned char frmr;

	unsigned int max_rcv_info_size;
	unsigned int windowsize;

	unsigned char canUISend;
	unsigned char isUIWaiting;

	unsigned char isTransFinish;
} HdlcStationParam;

typedef struct _hdlctcb
{
	unsigned int curstate;
	int condition;
	int(**listhandler)(HdlcStationParam*, hdlc*, hdlc*);;
	int error;
	unsigned int fsmtype;
#define FSMTypePrimary 0
#define FSMTypePrehandleFrame 1
#define FSMTypeNRM 2
#define FSMTypeIResponse 3
	struct _hdlctcb *next;
	struct _hdlctcb *prev;
} HdlcTcb;

extern HdlcTcb *fsmstack;



typedef struct {
	INT8U frame_type:3;            //帧类型
	INT8U s_flag:1;               //长帧分段标志位	
	INT8U P:1;                   //查询位（多窗口最后一个窗口标识）
	INT8U frmr_flag:1;            //frmr错误标识
	INT8U frame_discard_flag:1;    //帧未成功接收标识
}T_HdlcHandleResult;

//FSM栈管理
int FSMinit();
int FSMreturn(void);
int FSMenter(u_int fsmtype);
int HdlcSetOpt(hdlcpointer frame);
void HdlcParamInit();
int HdlcSetParam(u_char* paramstr, u_int len);
#endif