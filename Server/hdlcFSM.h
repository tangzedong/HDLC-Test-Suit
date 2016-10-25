#ifndef _HDLC_FSM
#define _HDLC_FSM
#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "framedef.h"
#include "eventhandler.h"
#define INT8U unsigned short
#define INT16U unsigned short
enum{
	STATE_NDM=1,
	STATE_WAIT_CONNECT,
	STATE_NRM,
	STATE_WAIT_DISCONNECT,
};
typedef struct _hdlctcb
{
	unsigned int curstate;
	int condition;
	int (*listhandler)(struct _hdlctcb*, hdlc*, hdlc*);;
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

typedef struct _statparam
{
	unsigned char started;//值为1进入状态循环
	unsigned char rcv_num;      //我自己记录的接收序号
	unsigned char send_num;   //我自己记录的发送序号
	unsigned char nr;
	unsigned char ns;
	unsigned char frmr_flag;          //值为1发送FRMR帧
	unsigned char frame_p_f;           //记录P/F位
	CServerDlg *hWnd;
	unsigned char send_flag;
	u_char *save_inf; //数据存储区
	u_int m; //数据计数

	INT8U frmr_flag;			   //待发FRMR标志
	INT8U frame_discard_flag;   //无效帧标致
} HdlcStationParam;

typedef struct {
	INT8U frame_type:3;            //帧类型
	INT8U s_flag:1;               //长帧分段标志位	
	INT8U P:1;                   //查询位（多窗口最后一个窗口标识）
	INT8U frmr_flag:1;            //frmr错误标识
	INT8U frame_discard_flag:1;    //帧未成功接收标识
}T_HdlcHandleResult;

//FSM栈管理
int FSMinit(HdlcTcb *tcb);
int FSMreturn(void);
int FSMenter(HdlcTcb *tcb);
#endif