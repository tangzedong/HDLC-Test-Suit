#ifndef _HDLC_FSM
#define _HDLC_FSM
//#include "framedef.h"
//#include "eventhandler.h"
enum{
	STATE_NDM=1,
	STATE_WAIT_CONNECT,
	STATE_NRM,
	STATE_WAIT_DISCONNECT,
};
typedef struct _hdlctcb
{
	unsigned int curstate;
	int nxtstate;
	int condition;
	unsigned char started;//值为1进入状态循环
	unsigned char rcv_num;      //我自己记录的接收序号
	unsigned char send_num;   //我自己记录的发送序号
	unsigned char nr;
	unsigned char ns;
	unsigned char frmr_flag;          //值为1发送FRMR帧
	unsigned char frame_p_f;           //记录P/F位
} HdlcTcb;
#endif