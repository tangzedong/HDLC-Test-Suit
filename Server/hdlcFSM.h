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
	unsigned char started;//ֵΪ1����״̬ѭ��
	unsigned char rcv_num;      //���Լ���¼�Ľ������
	unsigned char send_num;   //���Լ���¼�ķ������
	unsigned char nr;
	unsigned char ns;
	unsigned char frmr_flag;          //ֵΪ1����FRMR֡
	unsigned char frame_p_f;           //��¼P/Fλ
	CServerDlg *hWnd;
	unsigned char send_flag;
	u_char *save_inf; //���ݴ洢��
	u_int m; //���ݼ���

	INT8U frmr_flag;			   //����FRMR��־
	INT8U frame_discard_flag;   //��Ч֡����
} HdlcStationParam;

typedef struct {
	INT8U frame_type:3;            //֡����
	INT8U s_flag:1;               //��֡�ֶα�־λ	
	INT8U P:1;                   //��ѯλ���ര�����һ�����ڱ�ʶ��
	INT8U frmr_flag:1;            //frmr�����ʶ
	INT8U frame_discard_flag:1;    //֡δ�ɹ����ձ�ʶ
}T_HdlcHandleResult;

//FSMջ����
int FSMinit(HdlcTcb *tcb);
int FSMreturn(void);
int FSMenter(HdlcTcb *tcb);
#endif