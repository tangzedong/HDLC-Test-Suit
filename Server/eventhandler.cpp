#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "eventhandler.h"
#include "framedef.h"
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
	_HANDLER(hdlcStateNRMHandle2),
	_HANDLER(hdlcStateNRMIR)
};

statehandler IRStateHandler[] = {
	_HANDLER(hdlcStateIRWaitCmd),
	_HANDLER(hdlcStateIRWaitRes),
	_HANDLER(hdlcStateIRHandle3)
};
statehandler *StateHandlers[] = { PrimaryStateHandler, PrehandleStateHandler, NRMStateHandler, IRStateHandler };
//-----------------------------------Primary FSM---------------------------------------------------------------
DEFHANDLER(hdlcStateNDM)
{

		if (GetTypes(*frame) == TYPESNRM && stpar->started == 1 && stpar->frame_p_f == 1)    //֡����ΪSNRM��
		{
			if (frame->infolen == 0)
			{
				fsmstack->curstate = STATE_WAIT_CONNECT;
				stpar->isTransFinish = 0;
				//GETHANDLER(STATE_WAIT_CONNECT)(stpar, frame, outframe);
			}
			else
			{
				fsmstack->curstate = STATE_WAIT_CONNECT;
				stpar->isTransFinish = 0;
				HdlcSetParam(frame->info_buff, frame->infolen);
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
				FSMenter(STATE_FRMR);
				fsmstack->curstate = STATE_FRMR;
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
		//		gstpar->canUISend = 1;
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
		if (frame->infolen == 0)
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
				makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
				fsmstack->curstate = STATE_NRM;              //ת�Ƶ�NRM״̬
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�Ѿ�����NRM״̬��"), NULL);
				stpar->disc = 0;
				stpar->isTransFinish = 0;
			}
			if (GetTypes(*frame) == TYPEDISC)     //֡����ΪDISC��
			{
				//�ϲ�Ĭ��result ΪOK 
				//make UA
				//DOHANDLER(WAITDIS, DISC)(&stpar, hdlc_p, &outframe);
				//ͨ��send()����������Ӧ
				makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
				fsmstack->curstate = STATE_NDM;
				//��UA��״̬ת�Ƶ�NDM
				stpar->isTransFinish = 0;
			}
			else if (GetTypes(*frame) == TYPERR)//֡����ΪRR��
			{
				if (stpar->isUIWaiting)
				{
					makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
					gstpar->canUISend = 1;
				}
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
			}
			else if (GetTypes(*frame) == TYPERNR) //֡����ΪRNR��
			{
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
			}
		}
		else
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
				makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
				fsmstack->curstate = STATE_NRM;              //ת�Ƶ�NRM״̬
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�Ѿ�����NRM״̬��"), NULL);
				stpar->disc = 0;
			}
			else
			{
				fsmstack->curstate = STATE_FRMR;
				makeFRMR(stpar, frame, outframe, errorcode, glen);
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�ܾ�����Ϣ��RNR֡��"), NULL);
			}
		}
	}
	return 0;
}


DEFHANDLER(hdlcStateNRM)
{
	FSMenter(FSMTypeNRM);
	GETHANDLER(fsmstack->curstate)(stpar, frame, outframe);
	return 0;
}


DEFHANDLER(hdlcStateWDC)
{
	
	if (GetTypes(*frame) == TYPESNRM && stpar->started == 1 && stpar->frame_p_f == 1)    //֡����ΪSNRM��
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
	}
	else if (GetTypes(*frame) == TYPEDISC && stpar->started == 1 && stpar->frame_p_f == 1)     //֡����ΪDISC��
	{
		if (frame->infolen > 0)
		{
			makeFRMR(stpar, frame, outframe, errorcode, glen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�ܾ�����Ϣ��DISC֡��"), NULL);
		}
		else
		{
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&stpar, hdlc_p, &outframe);
			//ͨ��send()����������Ӧ
			makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
			fsmstack->curstate = STATE_NDM;
			stpar->disc = 1;
		}
	}
	else if (GetTypes(*frame) == TYPERR && stpar->started == 1 && stpar->frame_p_f == 1)//֡����ΪRR��
	{
		if (stpar->isUIWaiting)
		{
			makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
			gstpar->canUISend = 1;
		}
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
	}
	else if (GetTypes(*frame) == TYPERNR && stpar->started == 1 && stpar->frame_p_f == 1) //֡����ΪRNR��
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����Ӧ��"), NULL);
	}
	else
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����������͵�֡��"), NULL);
	}
	return 0;
}
DEFHANDLER(hdlcStateFRMR)
{
	if (GetTypes(*frame) == TYPESNRM)    //֡����ΪSNRM��
	{
		fsmstack->curstate = STATE_NRM;
		stpar->transfinish = 0;
		//GETHANDLER(STATE_NRM)(stpar, frame, outframe);
	}
	else
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:expected snrm to resume connect��"), NULL);
		fsmstack->curstate = STATE_FRMR;
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
			makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("�Ѿ���������״̬��"), NULL);
		}
		else if (GetTypes(*frame) == TYPEDISC)     //֡����ΪDISC��
		{
			FSMreturn();
			fsmstack->curstate = STATE_WAIT_DISCONNECT;
			GETHANDLER(STATE_WAIT_DISCONNECT)(stpar, frame, outframe);
		}
		else if (GetTypes(*frame) == TYPERR)//֡����ΪRR��
		{
			if (stpar->isUIWaiting)
			{
				makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
				gstpar->canUISend = 1;
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
			else
			{
				//make FRMR
				//store errorcode
				fsmstack->curstate = STATE_FRMR;
				makeFRMR(stpar, frame, outframe, errorcode, glen);
			}
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("����NRM״̬��"), NULL);
		}
		else if (GetTypes(*frame) == TYPERNR) //֡����ΪRNR��
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("δ׼���ý������ݣ���æ��"), NULL);
		}
		else if ((frame->frame_ctl & 1) == 0)     //֡����ΪI��
		{
			fsmstack->curstate = STATE_IREPONSE_HANDLE;
			GETHANDLER(STATE_IREPONSE_HANDLE)(stpar, frame, outframe);
		}
		else
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("error:����������͵�֡��"), NULL);
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
	FSMenter(FSMTypeIResponse);
	GETHANDLER(fsmstack->curstate)(stpar, frame, outframe);
	return 0;
}
//-----------------------------------IR FSM---------------------------------------------------------------
//to do
DEFHANDLER(hdlcStateIRWaitCmd)
{
	u_char write_str[255];
	if (frame->nr != stpar->send_num || stpar->ns != stpar->rcv_num)
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:��Ŵ��󣬾ܾ����ո�֡��"), NULL);
		//make FRMR   ���
		stpar->frmr_flag = 1;
		makeFRMR(stpar, frame, outframe, (u_char*)"", 0);
	}
	else
	{

		if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 && stpar->m == 0)//��֤��������һ֡  FRAME_type ΪI_COMPLETE������֡
		{
			if (stpar->isUIWaiting)//UI����ʱ��1 I_COMPLETE ����֮ǰ
			{
				makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
				gstpar->canUISend = 1;
			}
			makeRR(stpar, frame, outframe, stpar->rcv_num, stpar->frame_p_f);
		}
		else if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 && stpar->m != 0)//�ֶε����һ֡����m����F=1, S=0
		{
			if (stpar->isUIWaiting)//UI����ʱ��2 ÿ�����һ֡����֮ǰ
			{
				makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
				gstpar->canUISend = 1;
			}
			//for(u_int n = 0;n < (m + 1);n++)//�ϱ��������Ҫ���ص���Ϣ��
			// {
			write_str[0] = 0x7E;
			write_str[1] = 0xA0;
			write_str[2] = 0x2E;
			write_str[3] = 0x21;
			write_str[4] = 0x02;
			write_str[5] = 0x23;
			u_char s_rcv = (frame->ns + 1) << 5;
			u_char s_send = frame->nr;
			write_str[6] = s_rcv | s_send | 16;
			write_str[7] = 0xE0;//HCSҪ��������ģ�����
			write_str[8] = 0xAE;//
			write_str[9] = 0xE6;
			write_str[10] = 0xE7;
			write_str[11] = 0x00;
			write_str[12] = 0x61;
			write_str[13] = 0x1F;
			write_str[14] = 0xA1;
			write_str[15] = 0x09;
			write_str[16] = 0x06;
			write_str[17] = 0x07;
			write_str[18] = 0x60;
			write_str[19] = 0x85;
			write_str[20] = 0x74;
			write_str[21] = 0x05;
			write_str[22] = 0x08;
			write_str[23] = 0x01;
			write_str[24] = 0x01;
			write_str[25] = 0xA2;
			write_str[26] = 0x03;
			write_str[27] = 0x02;
			write_str[28] = 0x01;
			write_str[29] = 0x01;
			write_str[30] = 0xA3;
			write_str[31] = 0x05;
			write_str[32] = 0xA1;
			write_str[33] = 0x03;
			write_str[34] = 0x02;
			write_str[35] = 0x01;
			write_str[36] = 0x0D;
			write_str[37] = 0xBE;
			write_str[38] = 0x06;
			write_str[39] = 0x04;
			write_str[40] = 0x04;
			write_str[41] = 0x0E;
			write_str[42] = 0x01;
			write_str[43] = 0x06;
			write_str[44] = 0x01;
			write_str[45] = 0x6C;//
			write_str[46] = 0x71;//FCS�Ǽ�������ģ�����
			write_str[47] = 0x7E;
		}
		else if (frame->f_format.frame_seg == 1 && frame->pollfin == 0)//ÿһ����֡��һ����F=0, S=1
		{
			for (u_int n = 0; n < frame->infolen; n++)
			{
				stpar->save_inf[stpar->m] = frame->info_buff[n];
				stpar->m++;
			}

			stpar->rcv_num++;
			if (stpar->rcv_num % 8 == 0)
				stpar->rcv_num = 0;
		}
		else if (frame->f_format.frame_seg == 1 && frame->pollfin == 1)//ÿһ����֡�����һ����F=1, S=1
		{
			for (u_int n = 0; n < frame->infolen; n++)
			{
				stpar->save_inf[stpar->m] = frame->info_buff[n];
				stpar->m++;
			}
			stpar->rcv_num++;
			if (stpar->rcv_num % 8 == 0)
				stpar->rcv_num = 0;
			//make RR   ��Ҫ���յ����ҲҪ��
			makeRR(stpar, frame, outframe, stpar->rcv_num, stpar->frame_p_f);
		}
	}
	return 0;
}

DEFHANDLER(hdlcStateIRWaitRes)
{
	return 0;
}

DEFHANDLER(hdlcStateIRHandle3)
{
	return 0;
}