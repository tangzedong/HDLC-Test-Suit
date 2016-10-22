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
	unsigned char started;//ֵΪ1����״̬ѭ��
	unsigned char rcv_num;      //���Լ���¼�Ľ������
	unsigned char send_num;   //���Լ���¼�ķ������
	unsigned char nr;
	unsigned char ns;
	unsigned char frmr_flag;          //ֵΪ1����FRMR֡
	unsigned char frame_p_f;           //��¼P/Fλ
} HdlcTcb;
#endif