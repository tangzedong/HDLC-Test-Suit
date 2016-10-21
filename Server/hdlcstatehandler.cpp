#include "eventhandler.h"
int makeUA(HdlcTcb *tcb,hdlc *frame,hdlc *outframe);
DEFHANDLER(STATE_WAIT_CONNECT, SNRM)
{
	makeUA(tcb, frame, outframe);
}
DEFHANDLER(WAITDIS, DISC)
{
	makeUA(tcb, frame, outframe);
}
DEFHANDLER(WAITCON, SNRM)
{
	makeUA(tcb, frame, outframe);
}
DEFHANDLER(NRM, SNRM)
{
	makeUA(tcb, frame, outframe);
}

DEFHANDLER(NRM, RR)
{
	int frame_p_f = ((frame->frame_ctl & 16) >> 4);
	if (frame->nr == tcb->sendnum && frame_p_f == 1)
	{
		tcb->sendnum++;//要检查它的原语中的FRAME_type  继续发送信息帧？？？
		write_str[0] = 0x7E;
		write_str[1] = 0xA0;
		write_str[2] = 0x08;
		write_str[3] = 0x21;
		write_str[4] = 0x02;
		write_str[5] = 0x23;
		write_str[6] = (tcb->recnum << 5) | 17;//RR帧ctl
		write_str[7] = 0x6E;
		write_str[8] = 0x03;
		write_str[9] = 0x7E;
		frame_p_f = 1;
	}
	else
	{
		//make FRMR
		makeFRMR(tcb, frame, outframe, errorinf, len);
	}
}

//util functions
int makeFRMR(HdlcTcb *tcb, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->frame_ctl = TYPEFRMR;
	outframe->infolen = infolen;
	outframe->dst_addr = frame->src_addr;
	outframe->dst_addrlen = frame->src_addrlen;
	outframe->src_addr = frame->dst_addr;
	outframe->src_addrlen = frame->dst_addrlen;
	if (infolen != 0)
	{
		for (int i = 0; i < infolen; i++)
		{
			outframe->info_buff[i] = infobuf[i];
		}
	}
	outframe->f_format.frame_sublen = frame->src_addrlen + frame->dst_addrlen + infolen + 5;
	outframe->end_flag = 0x7e;
	convFrameHex(*outframe, pData);
	return 0;
}

int makeUA(HdlcTcb *tcb,hdlc *frame,hdlc *outframe)
{
	u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
		0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
	int d_addr_length = frame->dst_addrlen;
	outframe->start_flag = 0x7e;
	outframe->f_format.frame_type = 0xa0;
	outframe->frame_ctl = 0x21;
	outframe->infolen = 23;
	outframe->end_flag = 0x7e;
	u_char *write_str = outframe->info_buff;
	if (d_addr_length == 1)
	{
		outframe->f_format.frame_sublen = 0x20;
		outframe->dst_addrlen = 1;
		outframe->dst_addr = 0x03;
		outframe->src_addr = 0x39;
	}
	else if (d_addr_length == 2)
	{
		outframe->f_format.frame_sublen = 0x21;
		outframe->dst_addrlen = 2;
		outframe->dst_addr = 0x91;
		outframe->src_addr = 0x73;
	}
	else if (d_addr_length == 4)
	{
		outframe->f_format.frame_sublen = 0x23;
		outframe->dst_addrlen = 4;
		outframe->dst_addr = 0x4011;
		outframe->src_addr = 0x73;
	}
	for (int i = 0; i < sizeof(settingdata); i++)
	{
		outframe->info_buff[i] = settingdata[i];
	}
	return 0;
}