#include "eventhandler.h"
#include "framedef.h"
int makeUA(HdlcTcb *tcb,hdlc *frame,hdlc *outframe);
//DEFHANDLER(STATE_WAIT_CONNECT, SNRM)
//{
//	u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
//		0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
//	int len = 23;
//	makeUA(tcb, frame, outframe, settingdata, len);
//}
//DEFHANDLER(WAITDIS, DISC)
//{
//	u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
//		0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
//	int len = 23;
//	makeUA(tcb, frame, outframe, settingdata, len);
//}
//DEFHANDLER(WAITCON, SNRM)
//{
//	u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
//		0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
//	int len = 23;
//	makeUA(tcb, frame, outframe, settingdata, len);
//}
//DEFHANDLER(NRM, SNRM)
//{
//	u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
//		0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
//	int len = 23;
//	makeUA(tcb, frame, outframe, settingdata, len);
//}
//
//DEFHANDLER(NRM, RR)
//{
//	u_char errorinf[255];
//	int len;
//	int frame_p_f = ((frame->frame_ctl & 0x10) >> 4);
//	if (frame->nr == tcb->send_num && frame_p_f == 1)
//	{
//		tcb->send_num++;//要检查它的原语中的FRAME_type  继续发送信息帧？？？
//		makeRR(tcb, frame, outframe, tcb->rcv_num, POL); //RR帧ctl
//		tcb->frame_p_f = POL;
//	}
//	else
//	{
//		//make FRMR
//		errorinf[0] = 0x10; errorinf[1] = 0x00; errorinf[2] = 0x20;
//		len = 3;
//		makeFRMR(tcb, frame, outframe, errorinf, len);
//	}
//}



//------------------------------------util functions--------------------------------------------
int makeRR(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_int nr, u_int pf)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPERR;
	outframe->nr = nr;
	outframe->ns = 0;
	outframe->pollfin = pf;
	outframe->infolen = 0;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(outframe, pData);

	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}

int makeRNR(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_int nr, u_int pf)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPERNR;
	outframe->nr = nr;
	outframe->ns = 0;
	outframe->pollfin = pf;
	outframe->infolen = 0;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(outframe, pData);

	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}

int makeUI(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_int pf)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPEUI;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = pf;
	outframe->infolen = 0;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(outframe, pData);
	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}

int makeSNRM(HdlcTcb *tcb, hdlc *frame, hdlc *outframe)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPESNRM;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = FON;
	outframe->infolen = 0;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(outframe, pData);
	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}

int makeDM(HdlcTcb *tcb, hdlc *frame, hdlc *outframe)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPEDM;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = FON;
	outframe->infolen = 0;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(outframe, pData);

	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen-2);
	}
	return 0;
}

int makeDISC(HdlcTcb *tcb, hdlc *frame, hdlc *outframe)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPEDISC;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = FON;
	outframe->infolen = 0;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(outframe, pData);

	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}

int makeFRMR(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPEFRMR;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = FON;
	outframe->infolen = infolen;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	if (infolen != 0)
	{
		for (int i = 0; i < infolen; i++)
		{
			outframe->info_buff[i] = infobuf[i];
		}
		outframe->f_format.frame_sublen += infolen + 2;
	}
	outframe->end_flag = 0x7e;
	convFrameHex(outframe, pData);

	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}

int makeUA(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_char *settingdata, u_int len)
{
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPEUA;
	outframe->infolen = len;
	outframe->end_flag = STARTFLAG;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = FON;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;

	outframe->f_format.frame_sublen = outframe->dst_addrlen + outframe->src_addrlen + 5;
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			outframe->info_buff[i] = settingdata[i];
		}
		outframe->f_format.frame_sublen += len + 2;
	}

	u_char pData[255];
	convFrameHex(outframe, pData);
	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = f_cs_cal(pData, len + outframe->src_addrlen);
	}
	return 0;
}

int makeI(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen, u_int seg, u_int pf)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = seg;
	outframe->frame_ctl = TYPEI;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = pf;
	outframe->infolen = infolen;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + 5;
	if (infolen != 0)
	{
		for (int i = 0; i < infolen; i++)
		{
			outframe->info_buff[i] = infobuf[i];
		}
		outframe->f_format.frame_sublen += infolen + 2;
	}
	outframe->end_flag = 0x7e;
	convFrameHex(outframe, pData);

	//caluate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//caluate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	return 0;
}