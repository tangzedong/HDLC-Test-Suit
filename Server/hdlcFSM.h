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
typedef struct hdlc hdlc, *hdlcpointer;
//States of Primary FSM
enum{
	STATE_NDM=0,
	STATE_WAIT_CONNECT,
	STATE_NRM,
	STATE_WAIT_DISCONNECT,
	STATE_WAIT_FRMR
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
	unsigned char started;//ֵΪ1����״̬ѭ��
	unsigned char rcv_num;      //���Լ���¼�Ľ������
	unsigned char send_num;   //���Լ���¼�ķ������
	unsigned char nr;
	unsigned char ns;
	//unsigned char frmr_flag;          //ֵΪ1����FRMR֡
	unsigned char frame_p_f;           //��¼P/Fλ
	CServerDlg *hWnd;
	unsigned char send_flag;
	u_char *save_inf; //���ݴ洢��
	u_int m; //���ݼ���

	unsigned char frmr_flag;			   //����FRMR��־
	unsigned char frame_discard_flag;   //��Ч֡����

	unsigned char disc;
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
	INT8U frame_type:3;            //֡����
	INT8U s_flag:1;               //��֡�ֶα�־λ	
	INT8U P:1;                   //��ѯλ���ര�����һ�����ڱ�ʶ��
	INT8U frmr_flag:1;            //frmr�����ʶ
	INT8U frame_discard_flag:1;    //֡δ�ɹ����ձ�ʶ
}T_HdlcHandleResult;

//FSMջ����
int FSMinit();
int FSMreturn(void);
int FSMenter(u_int fsmtype);
#endif