#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#include "hdlc.h"
#include "hdlcutil.h"
#include "hdlceventhandler.h"
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
		if (GetTypes(*frame) == TYPESNRM && stpar->started == 1 && stpar->frame_p_f == 1)    //帧类型为SNRM！
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
		else if (GetTypes(*frame) == TYPEDISC && stpar->started == 1 && stpar->frame_p_f == 1)     //帧类型为DISC！
		{
			if (frame->infolen == 0)
			{
				makeDM(stpar, frame, outframe);
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("已经处于NDM状态！"),NULL);
			}
			else
			{
				stpar->frmr_flag = 1;
				stpar->isTransFinish = 0;
				stpar->fsmstack = FSMenter(stpar->fsmstack, STATE_FRMR);
				stpar->fsmstack->curstate = STATE_FRMR;
				//makeFRMR(stpar, frame, outframe, errorcode, glen);
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:拒绝包含信息的DISC帧！"), NULL);
			}
		}
		else
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:不处理该类型的帧！"), NULL);
		}
		//else if (GetTypes(*frame) == TYPERR && stpar->started == 1 && stpar->frame_p_f == 1)//帧类型为RR！
		//{
		//	if (stpar->isUIWaiting)
		//	{
		//		makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
		//		stpar->canUISend = 1;
		//	}
		//	if (frame->infolen > 0)
		//	{
		//		makeFRMR(stpar, frame, outframe, errorcode, glen);
		//	}
		//	stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
		//}
		//else if (GetTypes(*frame) == TYPERNR && stpar->started == 1 && stpar->frame_p_f == 1) //帧类型为RNR！
		//{
		//	//if (frame->infolen > 0)
		//	//{
		//	//	makeFRMR(stpar, frame, outframe, errorcode, glen);
		//	//}
		//	stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
		//}

	return 0;
}


DEFHANDLER(hdlcStateWCN)
{
	if (stpar->started == 1 && stpar->frame_p_f == 1)
	{
		if (GetTypes(*frame) == TYPESNRM)    //帧类型为SNRM！
		{
			//默认上层返回result是等于OK									
			stpar->rcv_num = 0;
			stpar->send_num = 0;
			//make UA 帧做出响应  
			//doUAResponse();
			//make UA response;
			//DOHANDLER(WAITCON, SNRM)(&stpar, hdlc_p, &outframe);
			if (frame->infolen > 0)    //协商参数！
			{
				//默认上层返回result是等于OK									
				HdlcSetParam(stpar, frame->info_buff, frame->infolen);
			}
			if (ApplId()==OK)
			{
				
				stpar->fsmstack->curstate = STATE_NRM;              //转移到NRM状态
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("已经处于NRM状态！"), NULL);
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
				stpar->fsmstack->curstate = STATE_NDM;              //转移到NRM状态
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("连接失败！"), NULL);
				stpar->disc = 1;
				stpar->isTransFinish = 0;
			}
			else
			{
				stpar->isTransFinish = 1;
			}
		}
		else if (GetTypes(*frame) == TYPEDISC)     //帧类型为DISC！
		{
			//上层默认result 为OK 
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&stpar, hdlc_p, &outframe);
			//通过send()函数发送响应
			
			stpar->fsmstack->curstate = STATE_NDM;
			//回UA，状态转移到NDM
			stpar->isTransFinish = 0;
			makeUA(stpar, frame, outframe, stpar->settingdata, gUAdatalen);
		}
		else
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:不响应！"), NULL);
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
	if (GetTypes(*frame) == TYPESNRM && stpar->started == 1 && stpar->frame_p_f == 1)    //帧类型为SNRM！
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:不响应！"), NULL);
	}
	else if (GetTypes(*frame) == TYPEDISC && stpar->started == 1 && stpar->frame_p_f == 1)     //帧类型为DISC！
	{
		if (frame->infolen > 0)
		{
			stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
			stpar->fsmstack->curstate = STATE_FRMR;
			stpar->isTransFinish = 0;
			//makeFRMR(stpar, frame, outframe, errorcode, glen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:拒绝带信息的DISC帧！"), NULL);
		}
		else
		{
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&stpar, hdlc_p, &outframe);
			//通过send()函数发送响应
			
			stpar->fsmstack->curstate = STATE_NDM;
			stpar->disc = 1;
			makeUA(stpar, frame, outframe, stpar->settingdata, gUAdatalen);
		}
	}
	else
	{
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("warning:不处理该类型的帧！"), NULL);
	}
	return 0;
}
DEFHANDLER(hdlcStateFRMR)
{
	if (GetTypes(*frame) == TYPESNRM)    //帧类型为SNRM！
	{
		stpar->fsmstack->curstate = STATE_NRM;
		stpar->isTransFinish = 0;
		//GETHANDLER(STATE_NRM)(stpar, frame, outframe);
	}
	else
	{
		stpar->frmr_flag = 1;
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:expected snrm to resume connect！"), NULL);
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
		if (GetTypes(*frame) == TYPESNRM)    //帧类型为SNRM！
		{
			//doNRM_SNRM();
			//make UA
			//DOHANDLER(NRM, SNRM)(&stpar, hdlc_p, &outframe);
			//makeUA(stpar, frame, outframe, settingdata, gUAdatalen);
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("已经处于NRM子状态机！"), NULL);
		}
		else if (GetTypes(*frame) == TYPEDISC)     //帧类型为DISC！
		{
			stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
			stpar->fsmstack->curstate = STATE_WAIT_DISCONNECT;
			stpar->isTransFinish = 0;
			//GETHANDLER(STATE_WAIT_DISCONNECT)(stpar, frame, outframe);
		}
		else if (GetTypes(*frame) == TYPERR)//帧类型为RR！
		{
			if (stpar->isUIWaiting)
			{
				
				stpar->canUISend = 1;
				makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
			}
			//DOHANDLER(NRM, RR)(&stpar, hdlc_p, &outframe);
			//通过send()函数发送响应

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
			else //帧序数错误FRMR
			{
				//make FRMR
				//store errorcode
				stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
				stpar->fsmstack->curstate = STATE_FRMR;
				stpar->isTransFinish = 0;
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:RR序数错误！"), NULL);
				//makeFRMR(stpar, frame, outframe, errorcode, glen);
			}
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("处于NRM状态！"), NULL);
		}
		else if (GetTypes(*frame) == TYPERNR) //帧类型为RNR！
		{
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("未准备好接收数据，正忙！"), NULL);
		}
		else if (GetTypes(*frame) == TYPEI)     //帧类型为I！
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
			stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:不处理该类型的帧！"), NULL);
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
		stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("frmr:序号错误，拒绝接收该帧！"), NULL);
		//make FRMR   输出
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
		}
		//else if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 && stpar->windowsize == 1)//保证是完整的一帧  FRAME_type 为I_COMPLETE，整个帧F=1, S=0
		//{
		//	if (stpar->isUIWaiting)//UI发送时机1 I_COMPLETE 发送之前
		//	{
		//		makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
		//		stpar->canUISend = 1;
		//	}
		//	//makeRR(stpar, frame, outframe, stpar->rcv_num, stpar->frame_p_f);
		//	for (u_int n = 0; n < frame->infolen; n++)
		//	{
		//		stpar->save_inf[stpar->m] = frame->info_buff[n];
		//		stpar->m++;
		//	}


		//	u_char s_rcv = frame->ns + 1;
		//	u_char s_send = frame->nr;
		//	stpar->rcv_num++;
		//	stpar->rcv_num = stpar->rcv_num % 8;
		//	//	u_char data[] = { 0xE6, 0xE7, 0x00, 0x61, 0x1F, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08,
		//	//	0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x01, 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x0D, 0xBE,
		//	//	0x06, 0x04, 0x04, 0x0E, 0x01, 0x06, 0x01};
		//	//makeI(stpar, frame, outframe, data, sizeof(data)/sizeof(u_char), 0, 1, s_rcv, s_send);

		//	ApplSend(stpar->save_inf, stpar->m);
		//	stpar->m = 0;

		//	fsmstack->curstate = STATE_WAIT_RES;
		//	stpar->isTransFinish = 0;
		//	stpar->isSendData = 1;
		//}
		//分段的最后一帧，由m控制F=1, S=0, FRAME_type 为I_COMPLETE, I_FINISH&& stpar->windowsize > 1
		else if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 )
		{
			if (stpar->isUIWaiting)//UI发送时机2 每段最后一帧发送之前
			{
				makeUI(stpar, frame, &gUIFrame, gUIInfoBuf, gUIInfoLen);
				stpar->canUISend = 1;
			}
			//for(u_int n = 0;n < (m + 1);n++)//上报，获得需要返回的信息域
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
			//	u_char data[] = { 0xE6, 0xE7, 0x00, 0x61, 0x1F, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08,
			//	0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x01, 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x0D, 0xBE,
			//	0x06, 0x04, 0x04, 0x0E, 0x01, 0x06, 0x01};
			//makeI(stpar, frame, outframe, data, sizeof(data)/sizeof(u_char), 0, 1, s_rcv, s_send);
			
			ApplSend(stpar, frame, stpar->save_inf, stpar->m);
			stpar->m = 0;

			//fsmstack->curstate = STATE_WAIT_RES;
			//stpar->isTransFinish = 0;
			//stpar->isSendData = 1;
		}
		else if (frame->f_format.frame_seg == 1 && frame->pollfin == 0)//每一波分帧的一部分F=0, S=1
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
		else if (frame->f_format.frame_seg == 1 && frame->pollfin == 1)//每一波分帧的最后一部分F=1, S=1
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
			//make RR   将要接收的序号也要改
			makeRR(stpar, frame, outframe, stpar->rcv_num, stpar->frame_p_f);
		}
	}
	return 0;
}

DEFHANDLER(hdlcStateIRWaitRes) //等待Appl消息
{
	//HdlcTcb *fsmstack = stpar->fsmstack;
	u_int len;
	u_char *buf;
	u_int nwin, tail;
	/*if (ApplGetBuf(&buf, &len) != READY)
	{
		return 0;
	}*/
	buf = stpar->sendbuf;
	len = stpar->sendlen;
	stpar->seglen = nwin = len / stpar->max_rcv_info_size;
	stpar->segtail = len % stpar->max_rcv_info_size;
	if (stpar->segtail != 0)
	{
		stpar->seglen = stpar->seglen + 1;
	}
	stpar->fsmstack->curstate = STATE_SEND_DATA;
	stpar->isTransFinish = 0;
	stpar->m = 0;
	stpar->numSegHaveSend = 0;
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

	u_int numSegHaveSend = stpar->numSegHaveSend;
	u_int windowsize = stpar->rcvwindowsize;
	//do{
		while (m < stpar->sendlen && j < stpar->max_rcv_info_size)
		{
			data[j++] = send_buf[m++];
		}

		if (m == stpar->sendlen)
		{
			seg = 0; pf = 1;
		}
		else if (m < stpar->sendlen &&  numSegHaveSend != stpar->rcvwindowsize - 1)
		{
			seg = 1; pf = 0;
		}
		else if (m < stpar->sendlen && numSegHaveSend == stpar->rcvwindowsize - 1)
		{
			seg = 1; pf = 1;
		}
		stpar->send_num++;
		stpar->send_num = stpar->send_num % 8;
		makeI(stpar, frame, outframe, data, j, seg, pf, stpar->rcv_num, stpar->send_num);
		stpar->send_flag = 1;
		numSegHaveSend++;
	stpar->numSegHaveSend = numSegHaveSend;
	stpar->m = m;
	//} while (numSegHaveSend % windowsize != 0 && numSegHaveSend < stpar->seglen);
		if (numSegHaveSend % windowsize == 0 && numSegHaveSend < stpar->seglen)
		{
			fsmstack->curstate = STATE_WAIT_CMD;
		}
		else if (m == stpar->sendlen)
	{
		stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypeNRM);
		fsmstack->curstate = STATE_NRM_IDLE;
	}
		else
		{
			stpar->isTransFinish = 0;
		}
	return 0;
}

DEFHANDLER(hdlcStateIRHandle3)
{
	return 0;
}