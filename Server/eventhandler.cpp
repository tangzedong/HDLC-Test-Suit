#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "eventhandler.h"
#include "framedef.h"
statehandler PrimaryStateHandler[] = {
	NULL,
	_HANDLER(hdlcStateNDM),
	_HANDLER(hdlcStateWCN),
	_HANDLER(hdlcStateNRM),
	_HANDLER(hdlcStateWDC)
};

statehandler PrehandleStateHandler[] = {
	NULL,
};

statehandler NRMStateHandler[] = {
	NULL,
};

statehandler IRStateHandler[] = {
	NULL,
};

//-----------------------------------Primary FSM---------------------------------------------------------------
DEFHANDLER(hdlcStateNDM)
{

		if (GetTypes(*frame) == TYPESNRM && tcb->started == 1 && tcb->frame_p_f == 1)    //帧类型为SNRM！
		{
			if (frame->infolen == 0)
			{
				tcb->curstate = STATE_WAIT_CONNECT;
				GETHANDLER(STATE_WAIT_CONNECT)(tcb, frame, outframe);
			}
			else
			{
				tcb->curstate = STATE_WAIT_CONNECT;
				GETHANDLER(STATE_WAIT_CONNECT)(tcb, frame, outframe);
			}
			
		}
		else if (GetTypes(*frame) == TYPEDISC && tcb->started == 1 && tcb->frame_p_f == 1)     //帧类型为DISC！
		{
			if (frame->infolen == 0)
			{
				makeDM(tcb, frame, outframe);
				tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("已经处于NDM状态！"),NULL);
			}
			else
			{
				makeFRMR(tcb, frame, outframe, errorcode, glen);
				tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝包含信息的DISC帧！"), NULL);
			}
			tcb->send_flag = 1;
		}
		else if (GetTypes(*frame) == TYPERR && tcb->started == 1 && tcb->frame_p_f == 1)//帧类型为RR！
		{
			if (frame->infolen > 0)
			{
				makeFRMR(tcb, frame, outframe, errorcode, glen);
			}
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
		}
		else if (GetTypes(*frame) == TYPERNR && tcb->started == 1 && tcb->frame_p_f == 1) //帧类型为RNR！
		{
			if (frame->infolen > 0)
			{
				makeFRMR(tcb, frame, outframe, errorcode, glen);
			}
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
		}
		else
		{
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不处理该类型的帧！"), NULL);
		}
	return 0;
}


DEFHANDLER(hdlcStateWCN)
{
	if (frame->infolen == 0)
	if (GetTypes(*frame) == TYPESNRM && tcb->started == 1 && tcb->frame_p_f == 1)    //帧类型为SNRM！
	{
		//默认上层返回result是等于OK									
		tcb->rcv_num = 0;
		tcb->send_num = 0;
		//make UA 帧做出响应  
		//doUAResponse();
		//make UA response;
		//DOHANDLER(WAITCON, SNRM)(&tcb, hdlc_p, &outframe);
		makeUA(tcb, frame, outframe, settingdata, gUAdatalen);
		tcb->curstate = STATE_NRM;              //转移到NRM状态
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("已经处于NRM状态！"), NULL);
		tcb->send_flag = 1;
	}
	else if (GetTypes(*frame) == TYPEDISC && tcb->started == 1 && tcb->frame_p_f == 1)     //帧类型为DISC！
	{
		if (frame->infolen == 0)
		{
			//上层默认result 为OK 
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&tcb, hdlc_p, &outframe);
			//通过send()函数发送响应
			makeUA(tcb, frame, outframe, settingdata, gUAdatalen);
			tcb->send_flag = 1;
			tcb->curstate = STATE_NDM;
			//回UA，状态转移到NDM
		}
		else
		{
			makeFRMR(tcb, frame, outframe, errorcode, glen);
			tcb->send_flag = 1;
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝带信息的DISC帧！"), NULL);
		}
	}
	else if (GetTypes(*frame) == TYPERR && tcb->started == 1 && tcb->frame_p_f == 1)//帧类型为RR！
	{
		if (frame->infolen > 0)
		{
			makeFRMR(tcb, frame, outframe, errorcode, glen);
			tcb->send_flag = 1;
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝带信息的DISC帧！"), NULL);
		}
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"),NULL);
	}
	else if (GetTypes(*frame) == TYPERNR && tcb->started == 1 && tcb->frame_p_f == 1) //帧类型为RNR！
	{
		if (frame->infolen > 0)
		{
			makeFRMR(tcb, frame, outframe, errorcode, glen);
			tcb->send_flag = 1;
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝带信息的DISC帧！"),NULL);
		}
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"),NULL);
	}
	else
	{
		if (frame->infolen > 0)
		{
			makeFRMR(tcb, frame, outframe, errorcode, glen);
			tcb->send_flag = 1;
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝带信息的DISC帧！"), NULL);
		}
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不处理该类型的帧！"), NULL);
	}

	return 0;
}


DEFHANDLER(hdlcStateNRM)
{
	if (GetTypes(*frame) == TYPESNRM && tcb->started == 1 && tcb->frame_p_f == 1)    //帧类型为SNRM！
	{
		//doNRM_SNRM();
		//make UA
		//DOHANDLER(NRM, SNRM)(&tcb, hdlc_p, &outframe);
		makeUA(tcb, frame, outframe, settingdata, gUAdatalen);
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("已经处于连接状态！"), NULL);
		tcb->send_flag = 1;
	}
	else if (GetTypes(*frame) == TYPEDISC && tcb->started == 1 && tcb->frame_p_f == 1)     //帧类型为DISC！
	{
		if (frame->infolen > 0)
		{
			makeFRMR(tcb, frame, outframe, errorcode, glen);
			tcb->send_flag = 1;
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝带信息的DISC帧！"), NULL);
		}
		else
		{
			tcb->curstate = STATE_WAIT_DISCONNECT;
			GETHANDLER(STATE_WAIT_DISCONNECT)(tcb, frame, outframe);
		}
	}
	else if (GetTypes(*frame) == TYPERR && tcb->started == 1 && tcb->frame_p_f == 1)//帧类型为RR！
	{
		//DOHANDLER(NRM, RR)(&tcb, hdlc_p, &outframe);
		//通过send()函数发送响应
		if (tcb->nr == tcb->send_num && tcb->frame_p_f == 1)
		{
			//make RR
			if (tcb->nr == 0)
			{
				makeRR(tcb, frame, outframe, 0, tcb->frame_p_f);
			}
			else
			{
				tcb->send_num++;
				makeRR(tcb, frame, outframe, tcb->rcv_num, tcb->frame_p_f);
			}
		}
		else
		{
			//make FRMR
			makeFRMR(tcb, frame, outframe, errorcode, glen);
		}
		tcb->send_flag = 1;
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("处于NRM状态！"), NULL);
	}
	else if (GetTypes(*frame) == TYPERNR && tcb->started == 1 && tcb->frame_p_f == 1) //帧类型为RNR！
	{
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("未准备好接收数据，正忙！"), NULL);
	}
	else if ((frame->frame_ctl & 1) == 0 && tcb->started == 1 && tcb->frame_p_f == 1)     //帧类型为I！
	{
		u_char write_str[255];
		if (frame->nr != tcb->send_num || tcb->ns != tcb->rcv_num)
		{
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("序号错误，拒绝接收该帧！"),NULL);
			//make FRMR   输出
			write_str[0] = 0x7E;
			write_str[1] = 0xA0;
			write_str[2] = 0x08;
			write_str[3] = 0x21;
			write_str[4] = 0x02;
			write_str[5] = 0x23;
			write_str[6] = 0x97;
			write_str[7] = 0x50;
			write_str[8] = 0xE2;
			write_str[9] = 0x7E;//写入记事本
		}
		else
		{
			if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 && tcb->m != 0)//分段的最后一帧，由m控制
			{
				//for(u_int n = 0;n < (m + 1);n++)//上报，获得需要返回的信息域
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
				write_str[7] = 0xE0;//HCS要计算出来的？？？
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
				write_str[46] = 0x71;//FCS是计算出来的？？？
				write_str[47] = 0x7E;
			}
			else if (frame->f_format.frame_seg == 0 && frame->pollfin == 1 && tcb->m == 0)//保证是完整的一帧  FRAME_type 为I_COMPLETE，整个帧
			{
				write_str[0] = 0x7E;
				write_str[1] = 0xA0;
				write_str[2] = 0x08;
				write_str[3] = 0x21;
				write_str[4] = 0x02;
				write_str[5] = 0x23;
				switch (tcb->rcv_num)
				{
				case 0:write_str[6] = 0x11;	write_str[7] = 0x6E; write_str[8] = 0x03; break;
				case 1:write_str[6] = 0x31; write_str[7] = 0x6C; write_str[8] = 0x22; break;
				case 2:write_str[6] = 0x51; write_str[7] = 0x6A; write_str[8] = 0x41; break;
				case 3:write_str[6] = 0x71; write_str[7] = 0x68; write_str[8] = 0x60; break;
				case 4:write_str[6] = 0x91; write_str[7] = 0x66; write_str[8] = 0x87; break;
				case 5:write_str[6] = 0xB1; write_str[7] = 0x64; write_str[8] = 0xA6; break;
				case 6:write_str[6] = 0xD1; write_str[7] = 0x62; write_str[8] = 0xC5; break;
				case 7:write_str[6] = 0xF1; write_str[7] = 0x60; write_str[8] = 0xE4; break;
				}
				write_str[9] = 0x7E;
			}
			else if (frame->f_format.frame_seg == 1 && frame->pollfin == 0)//每一波分帧的一部分
			{
				for (u_int n = 0; n < frame->infolen; n++)
				{
					tcb->save_inf[tcb->m] = frame->info_buff[n];
					tcb->m++;
				}
				
				tcb->rcv_num++;
				if (tcb->rcv_num % 8 == 0)
					tcb->rcv_num = 0;
			}
			else if (frame->f_format.frame_seg == 1 && frame->pollfin == 1)//每一波分帧的最后一部分
			{
				for (u_int n = 0; n < frame->infolen; n++)
				{
					tcb->save_inf[tcb->m] = frame->info_buff[n];
					tcb->m++;
				}
				tcb->rcv_num++;
				if (tcb->rcv_num % 8 == 0)
					tcb->rcv_num = 0;
				//make RR   将要接收的序号也要改
				write_str[0] = 0x7E;
				write_str[1] = 0xA0;
				write_str[2] = 0x08;
				write_str[3] = 0x21;
				write_str[4] = 0x02;
				write_str[5] = 0x23;
				switch (tcb->rcv_num)
				{
				case 0:write_str[6] = 0x11;	write_str[7] = 0x6E; write_str[8] = 0x03; break;
				case 1:write_str[6] = 0x31; write_str[7] = 0x6C; write_str[8] = 0x22; break;
				case 2:write_str[6] = 0x51; write_str[7] = 0x6A; write_str[8] = 0x41; break;
				case 3:write_str[6] = 0x71; write_str[7] = 0x68; write_str[8] = 0x60; break;
				case 4:write_str[6] = 0x91; write_str[7] = 0x66; write_str[8] = 0x87; break;
				case 5:write_str[6] = 0xB1; write_str[7] = 0x64; write_str[8] = 0xA6; break;
				case 6:write_str[6] = 0xD1; write_str[7] = 0x62; write_str[8] = 0xC5; break;
				case 7:write_str[6] = 0xF1; write_str[7] = 0x60; write_str[8] = 0xE4; break;
				}
				write_str[9] = 0x7E;
			}
		}
	}
	else
	{
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不处理该类型的帧！"), NULL);
	}
	return 0;
}


DEFHANDLER(hdlcStateWDC)
{
	
	if (GetTypes(*frame) == TYPESNRM && tcb->started == 1 && tcb->frame_p_f == 1)    //帧类型为SNRM！
	{
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
	}
	else if (GetTypes(*frame) == TYPEDISC && tcb->started == 1 && tcb->frame_p_f == 1)     //帧类型为DISC！
	{
		if (frame->infolen > 0)
		{
			makeFRMR(tcb, frame, outframe, errorcode, glen);
			tcb->send_flag = 1;
			tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("拒绝带信息的DISC帧！"), NULL);
		}
		else
		{
			//make UA
			//DOHANDLER(WAITDIS, DISC)(&tcb, hdlc_p, &outframe);
			//通过send()函数发送响应
			makeUA(tcb, frame, outframe, settingdata, gUAdatalen);
			tcb->send_flag = 1;
			tcb->curstate = STATE_NDM;
		}
	}
	else if (GetTypes(*frame) == TYPERR && tcb->started == 1 && tcb->frame_p_f == 1)//帧类型为RR！
	{
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
	}
	else if (GetTypes(*frame) == TYPERNR && tcb->started == 1 && tcb->frame_p_f == 1) //帧类型为RNR！
	{
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不响应！"), NULL);
	}
	else
	{
		tcb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("不处理该类型的帧！"), NULL);
	}
	return 0;
}


//-----------------------------------NRM FSM---------------------------------------------------------------
//to do
//-----------------------------------IR FSM---------------------------------------------------------------
//to do