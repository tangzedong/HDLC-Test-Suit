#include "stdafx.h"
#include "afxdialogex.h"

#include "hdlc.h"
#include "hdlcutil.h"
#include "hdlcFSM.h"
#include "hdlceventhandler.h"
#include "ServerDlg.h"
#include "ApplInterface.h"

statehandler PrimaryStateHandler[] = {
	_HANDLER(hdlcStateNDM),
	_HANDLER(hdlcStateWCN),
	_HANDLER(hdlcStateNRM),
	_HANDLER(hdlcStateWDC),
	_HANDLER(hdlcStateFRMR)
};

statehandler PrehandleStateHandler[] = {
	NULL
};

statehandler NRMStateHandler[] = {
	_HANDLER(hdlcStateNRMIdle),
	_HANDLER(hdlcStateNRMIR),
	_HANDLER(hdlcStateNRMHandle2),
	
};

statehandler IRStateHandler[] = {
	_HANDLER(hdlcStateIRWaitCmd),
	_HANDLER(hdlcStateIRWaitRes),
	_HANDLER(hdlcStateIRHandle3),
	_HANDLER(hdlcStateIRSendData)
};
statehandler *StateHandlers[] = { PrimaryStateHandler, PrehandleStateHandler, NRMStateHandler, IRStateHandler };
//-----------------------------------Primary FSM---------------------------------------------------------------
DEFHANDLER(hdlcStateNDM)
{
		if (GetTypes(*frame) == TYPESNRM && stpar->started == 1 && stpar->frame_p_f == 1)    //֡����ΪSNRM��
		{
			if (frame->infolen == 0)
			{
				stpar->fsmstack->curstate = STATE_WAIT_CONNECT;
				stpar->isTransFinish = 0;
				//GETHANDLER(STATE_WAIT_CONNECT)(stpar, frame, outframe);
			}
			else
			{
				stpar->fsmstack->curstate = STATE_WAIT_CONNECT;
				stpar->isTransFinish = 0;
				//HdlcSetParam(frame->info_buff, frame->infolen);
				//GETHANDLER(STATE_WAIT_CONNECT)(stpar, frame, outframe);
			}
			
		}
		else if (GetTypes(*frame) == TYPEDISC && stpar->started == 1 && stpar->frame_p_f == 1)     //֡����ΪDISC��
		{
			if (frame->infolen == 0)
			{
				makeDM(stpar, frame, outframe);
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�Ѿ�����NDM״̬��"),NULL);
			}
			else
			{
				stpar->frmr_flag = 1;
				stpar->isTransFinish = 0;
				stpar->fsmstack = FSMenter(stpar->fsmstack, STATE_FRMR);
				stpar->fsmstack->curstate = STATE_FRMR;
				//makeFRMR(stpar, frame, outframe, errorcode, glen);
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:�ܾ�������Ϣ��DISC֡��"), NULL);
			}
		}
		else
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:����������͵�֡��"), NULL);
		}
		//else if (GetTypes(*frame) == TYPERR && stpar->started == 1 && stpar->frame_p_f == 1)//֡����ΪRR��
		//{
		//	if (stpar->isUIWaiting)
		//	{
		//		makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
		//		stpar->canUISend = 1;
		//		stpar->isUIWaiting = 0;
		//	}
		//	if (frame->infolen > 0)
		//	{
		//		makeFRMR(stpar, frame, outframe, errorcode, glen);
		//	}
		//	stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
		//}
		//else if (GetTypes(*frame) == TYPERNR && stpar->started == 1 && stpar->frame_p_f == 1) //֡����ΪRNR��
		//{
		//	//if (frame->infolen > 0)
		//	//{
		//	//	makeFRMR(stpar, frame, outframe, errorcode, glen);
		//	//}
		//	stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
		//}

	return 0;
}


DEFHANDLER(hdlcStateWCN)
{
	if (stpar->started == 1 && stpar->frame_p_f == 1)
	{
		if (GetTypes(*frame) == TYPESNRM)    //֡����ΪSNRM��
		{
			//Ĭ���ϲ㷵��result�ǵ���OK									
			stpar->rcv_num = 0;
			stpar->send_num = 0;
			//make UA ֡������Ӧ  
			//doUAResponse();
			//make UA response;
			//DOHANDLER(WAITCON, SNRM)(&stpar, hdlc_p, &outframe);
			if (frame->infolen > 0)    //Э�̲�����
			{
				//Ĭ���ϲ㷵��result�ǵ���OK									
				HdlcSetParam(stpar, frame->info_buff, frame->infolen);
			}
			if (ApplId()==OK)
			{
				
				stpar->fsmstack->curstate = STATE_NRM;              //ת�Ƶ�NRM״̬
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�Ѿ�����NRM״̬��"), NULL);
				stpar->slav_pot_addrlen = frame->dst_addrlen;
				stpar->slav_pot_addr = frame->dst_addr;
				stpar->main_pot_addrlen = frame->src_addrlen;
				stpar->main_pot_addr = frame->src_addr;
				stpar->disc = 0;
				stpar->isTransFinish = 0;
				makeUA(stpar, frame, outframe, stpar->settingdata, gUAdatalen);
			}
			else if (ApplId() == DENY)
			{
				stpar->fsmstack->curstate = STATE_NDM;              //ת�Ƶ�NRM״̬
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����ʧ�ܣ�"), NULL);
				stpar->disc = 1;
				stpar->isTransFinish = 0;
			}
			else
			{
				stpar->isTransFinish = 1;
			}
		}
		else if (GetTypes(*frame) == TYPEDISC)     //֡����ΪDISC��
		{
			//�ϲ�Ĭ��result ΪOK 
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&stpar, hdlc_p, &outframe);
			//ͨ��send()����������Ӧ
			
			stpar->fsmstack->curstate = STATE_NDM;
			//��UA��״̬ת�Ƶ�NDM
			stpar->isTransFinish = 0;
			makeUA(stpar, frame, outframe, stpar->settingdata, gUAdatalen);
		}
		else
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:����Ӧ��"), NULL);
		}
	}
	return 0;
}


DEFHANDLER(hdlcStateNRM)
{
	stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypeNRM);
	stpar->isTransFinish = 0;
	//GETHANDLER(fsmstack->curstate)(stpar, frame, outframe);
	return 0;
}


DEFHANDLER(hdlcStateWDC)
{
	if (GetTypes(*frame) == TYPESNRM && stpar->started == 1 && stpar->frame_p_f == 1)    //֡����ΪSNRM��
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:����Ӧ��"), NULL);
	}
	else if (GetTypes(*frame) == TYPEDISC && stpar->started == 1 && stpar->frame_p_f == 1)     //֡����ΪDISC��
	{
		if (frame->infolen > 0)
		{
			stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
			stpar->fsmstack->curstate = STATE_FRMR;
			stpar->isTransFinish = 0;
			//makeFRMR(stpar, frame, outframe, errorcode, glen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:�ܾ�����Ϣ��DISC֡��"), NULL);
		}
		else
		{
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&stpar, hdlc_p, &outframe);
			//ͨ��send()����������Ӧ
			
			stpar->fsmstack->curstate = STATE_NDM;
			stpar->disc = 1;
			makeUA(stpar, frame, outframe, stpar->settingdata, gUAdatalen);
		}
	}
	else
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:����������͵�֡��"), NULL);
	}
	return 0;
}
DEFHANDLER(hdlcStateFRMR)
{
	if (GetTypes(*frame) == TYPESNRM)    //֡����ΪSNRM��
	{
		stpar->fsmstack->curstate = STATE_NRM;
		stpar->isTransFinish = 0;
		//GETHANDLER(STATE_NRM)(stpar, frame, outframe);
	}
	else
	{
		stpar->frmr_flag = 1;
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:expected snrm to resume connect��"), NULL);
		stpar->fsmstack->curstate = STATE_FRMR;
		
		stpar->isTransFinish = 1;
		makeFRMR(stpar, frame, outframe, errorcode, glen);
	}
	return 0;
}

//-----------------------------------NRM FSM---------------------------------------------------------------
//to do
DEFHANDLER(hdlcStateNRMIdle)
{
	if (stpar->started == 1 && stpar->frame_p_f == 1)
	{
		if (GetTypes(*frame) == TYPESNRM)    //֡����ΪSNRM��
		{
			//doNRM_SNRM();
			//make UA
			//DOHANDLER(NRM, SNRM)(&stpar, hdlc_p, &outframe);
			//makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�Ѿ�����NRM��״̬����"), NULL);
		}
		else if (GetTypes(*frame) == TYPEDISC)     //֡����ΪDISC��
		{
			stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
			stpar->fsmstack->curstate = STATE_WAIT_DISCONNECT;
			stpar->isTransFinish = 0;
			//GETHANDLER(STATE_WAIT_DISCONNECT)(stpar, frame, outframe);
		}
		else if (GetTypes(*frame) == TYPERR)//֡����ΪRR��
		{
			if (stpar->isUIWaiting)
			{
				
				stpar->canUISend = 1;
				makeUI(stpar, frame, &(stpar->hWnd->m_UIFrame), (u_char*)(stpar->hWnd->m_UIInfoBuf), stpar->hWnd->m_UIInfoLen);
			}
			//DOHANDLER(NRM, RR)(&stpar, hdlc_p, &outframe);
			//ͨ��send()����������Ӧ

			if (stpar->nr == stpar->send_num && stpar->frame_p_f == 1)
			{
				//make RR
				if (stpar->nr == 0)
				{
					makeRR(stpar, frame, outframe, 0, stpar->frame_p_f);
				}
				else
				{
					stpar->send_num++;
					makeRR(stpar, frame, outframe, stpar->rcv_num, stpar->frame_p_f);
				}
			}
			else //֡��������FRMR
			{
				//make FRMR
				//store errorcode
				stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
				stpar->fsmstack->curstate = STATE_FRMR;
				stpar->isTransFinish = 0;
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:RR��������"), NULL);
				//makeFRMR(stpar, frame, outframe, errorcode, glen);
			}
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����NRM״̬��"), NULL);
		}
		else if (GetTypes(*frame) == TYPERNR) //֡����ΪRNR��
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("δ׼���ý������ݣ���æ��"), NULL);
		}
		else if (GetTypes(*frame) == TYPEI)     //֡����ΪI��
		{
			stpar->fsmstack->curstate = STATE_IREPONSE_HANDLE;
			stpar->isTransFinish = 0;
			//GETHANDLER(STATE_IREPONSE_HANDLE)(stpar, frame, outframe);
		}
		else
		{
			stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
			stpar->fsmstack->curstate = STATE_FRMR;
			stpar->isTransFinish = 0;
			//makeFRMR(stpar, frame, outframe, errorcode, glen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:����������͵�֡��"), NULL);
		}
	}
	return 0;
}

DEFHANDLER(hdlcStateNRMHandle2)
{
	return 0;
}

DEFHANDLER(hdlcStateNRMIR)
{
	//HdlcTcb *fsmstack = stpar->fsmstack;
	stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypeIResponse);
	stpar->fsmstack->curstate = STATE_WAIT_CMD;
	stpar->isTransFinish = 0;
	//GETHANDLER(fsmstack->curstate)(stpar, frame, outframe);
	return 0;
}
//-----------------------------------IR FSM---------------------------------------------------------------
//to do
DEFHANDLER(hdlcStateIRWaitCmd)
{
	//HdlcTcb *fsmstack = stpar->fsmstack;
	u_char write_str[255];
	if (frame->nr != stpar->send_num || stpar->ns != stpar->rcv_num)
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:��Ŵ��󣬾ܾ����ո�֡��"), NULL);
		//make FRMR   ���
		stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
		stpar->fsmstack->curstate = STATE_FRMR;
		stpar->isTransFinish = 0;
		/*stpar->frmr_flag = 1;
		makeFRMR(stpar, frame, outframe, (u_char*)"", 0);*/
	}
	else
	{
		if (GetTypes(*frame) == TYPERR)
		{
			stpar->m = 0;

			stpar->fsmstack->curstate = STATE_WAIT_RES;
			stpar->isTransFinish = 0;
			stpar->isSendData = 1;

			//todoȷ��ǰһ�����ڣ���ʼ������һ������
			stpar->m += stpar->sendwindowsize;
		}
		//�ֶε����һ֡����m����F=1, S=0, FRAME_type ΪI_COMPLETE, I_FINISH&& stpar->windowsize > 1
		//main to slave�˽�������
		else if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 )
		{
			if (stpar->isUIWaiting)//UI����ʱ��2 ÿ�����һ֡����֮ǰ
			{
				makeUI(stpar, frame, &(stpar->hWnd->m_UIFrame), (u_char*)stpar->hWnd->m_UIInfoBuf, stpar->hWnd->m_UIInfoLen);
				stpar->canUISend = 1;
				stpar->isUIWaiting = 0;
			}
			//for(u_int n = 0;n < (m + 1);n++)//�ϱ��������Ҫ���ص���Ϣ��
			// {

			for (u_int n = 0; n < frame->infolen; n++)
			{
				stpar->save_inf[stpar->m] = frame->info_buff[n];
				stpar->m++;
			}


			u_char s_rcv = frame->ns + 1;
			u_char s_send = frame->nr;
			stpar->rcv_num++;
			stpar->rcv_num = stpar->rcv_num % 8;
			stpar->m = 0;

			stpar->fsmstack->curstate = STATE_WAIT_RES;
			stpar->isTransFinish = 0;
		}
		else if (frame->f_format.frame_seg == 1 && frame->pollfin == 0)//ÿһ����֡��һ����F=0, S=1
		{
			for (u_int n = 0; n < frame->infolen; n++)
			{
				stpar->save_inf[stpar->m] = frame->info_buff[n];
				stpar->m++;
			}

			stpar->rcv_num++;
			stpar->rcv_num = stpar->rcv_num % 8;

			//makeI(stpar, frame, outframe, (u_char*)"", 0, 1, 0);
		}
		else if (frame->f_format.frame_seg == 1 && frame->pollfin == 1)//ÿһ����֡�����һ����F=1, S=1
		{
			for (u_int n = 0; n < frame->infolen; n++)
			{
				stpar->save_inf[stpar->m] = frame->info_buff[n];
				stpar->m++;
			}
			stpar->rcv_num++;
			stpar->rcv_num = stpar->rcv_num % 8;
			//stpar->send_num = 0;
			//stpar->rcv_num = 0;
			//make RR   ��Ҫ���յ����ҲҪ��
			makeRR(stpar, frame, outframe, stpar->rcv_num, stpar->frame_p_f);
			stpar->isSendData = 1;
		}
	}
	return 0;
}

DEFHANDLER(hdlcStateIRWaitRes) //�ȴ���վȷ�ϵ�ǰ���͵Ĵ���
{
	if (GetTypes(*frame) == TYPERR)
	{	
		//todoȷ��ǰһ�����ڣ���ʼ������һ������
		if (stpar->SendComplete) //������ݴ������
		{
			stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypeNRM);
			stpar->fsmstack->curstate = STATE_NRM_IDLE;
		}
		else //������ݴ���û����ɣ� ������һ������
		{
			stpar->fsmstack->curstate = STATE_SEND_DATA;
			stpar->isTransFinish = 0;
			stpar->isSendData = 0;
		}
	}
	else if (GetTypes(*frame) == TYPEI)
	{
		stpar->fsmstack->curstate = STATE_SEND_DATA;
		stpar->isTransFinish = 0;
		stpar->isSendData = 0;
	}
	return 0;
}

DEFHANDLER(hdlcStateIRSendData)
{
	HdlcTcb *fsmstack = stpar->fsmstack;
	u_char *send_buf = stpar->sendbuf;
	u_int m = stpar->m;
	int i = 0, j = 0;
	u_char seg, pf;
	u_char data[2048];

	u_int numSegSended = stpar->numSegSended;
	u_int windowsize = stpar->sendwindowsize;
	//do{
	//todoGet data from app layer
	//׼��֡������
	stpar->hWnd->GenFrameBuf();
	//�򿪷���֡��ʱ��
	stpar->hWnd->PostMessage(IDM_SENDDATA);

	//todo��ʱ����,�ȴ���ʱ������������������ ��ʱ�ش�
	fsmstack->curstate =  STATE_WAIT_RES;
	stpar->isTransFinish = 1;
	stpar->isSendData = 0;
	return 0;
}

DEFHANDLER(hdlcStateIRHandle3)
{
	return 0;
}