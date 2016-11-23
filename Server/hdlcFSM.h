#pragma once
#ifndef _HDLC_FSM
#define _HDLC_FSM
//#include "stdafx.h"
//#include "Server.h"
//#include "ServerDlg.h"
//#include "afxdialogex.h"

#include "hdlc.h"
#include "hdlcutil.h"
//#include "hdlceventhandler.h"
struct _hdlctcb;

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

#define StateHandler int(**)(HdlcStationParam*, hdlc*, hdlc*)
#define DEFHANDLER(name) int H##name(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe)
#define _HANDLER(name) H##name
#define GETHANDLER(fsmstack) (*(fsmstack->listhandler[fsmstack->curstate]))
typedef int(*statehandler)(HdlcStationParam*, hdlc*, hdlc*);

//States of Primary FSM
enum{
	STATE_NDM=0,
	STATE_WAIT_CONNECT,
	STATE_NRM,
	STATE_WAIT_DISCONNECT,
	STATE_FRMR,
	STATE_WAIT_FRMR,
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
	STATE_SEND_DATA,
};


struct _hdlctcb
{
	unsigned int curstate;
	int condition;
	//int(**listhandler)(struct _hdlctcb *tcb, HdlcStationParam*, hdlc*, hdlc*);
	statehandler *listhandler;
	int error;
	unsigned int fsmtype;
#define FSMTypePrimary 0
#define FSMTypePrehandleFrame 1
#define FSMTypeNRM 2
#define FSMTypeIResponse 3
	struct _hdlctcb *next;
	struct _hdlctcb *prev;
};// HdlcTcb;

typedef struct _hdlctcb HdlcTcb;
//extern HdlcTcb *fsmstack;

typedef struct {
	INT8U frame_type:3;            //帧类型
	INT8U s_flag:1;               //长帧分段标志位	
	INT8U P:1;                   //查询位（多窗口最后一个窗口标识）
	INT8U frmr_flag:1;            //frmr错误标识
	INT8U frame_discard_flag:1;    //帧未成功接收标识
}T_HdlcHandleResult;

//FSM栈管理
HdlcTcb *FSMinit(HdlcTcb *fsmstack);
HdlcTcb *FSMreturn(HdlcTcb *fsmstack);
HdlcTcb *FSMenter(HdlcTcb *fsmstack, u_int fsmtype);



//statehandler listStateHandler[];
statehandler PrimaryStateHandler[];
statehandler PrehandleStateHandler[];
statehandler NRMStateHandler[];
statehandler IRStateHandler[];
statehandler *StateHandlers[];
#endif