#include "stdafx.h"
#include "hdlc.h"
#include "hdlcutil.h"
#include "hdlceventhandler.h"



//------------------------------------util functions--------------------------------------------
//int makeUA(HdlcStationParam *tcb, hdlc *frame, hdlc *outframe);
int makeRR(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_int nr, u_int pf)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(stpar, outframe, pData);

	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeRNR(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_int nr, u_int pf)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(stpar, outframe, pData);

	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeUI(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = 0;
	outframe->frame_ctl = TYPEUI;
	outframe->nr = 0;
	outframe->ns = 0;
	outframe->pollfin = 1;
	outframe->infolen = infolen;
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	for (int i = 0; i <= infolen; i++)
	{
		outframe->info_buff[i] = infobuf[i];
	}
	convFrameHex(stpar, outframe, pData);
	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;


	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeSNRM(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(stpar, outframe, pData);
	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeDM(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(stpar, outframe, pData);

	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen-2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeDISC(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	outframe->end_flag = STARTFLAG;
	convFrameHex(stpar, outframe, pData);

	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeFRMR(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->main_pot_addrlen + stpar->slav_pot_addrlen + 5;
	if (infolen != 0)
	{
		for (int i = 0; i < infolen; i++)
		{
			outframe->info_buff[i] = infobuf[i];
		}
		outframe->f_format.frame_sublen += infolen + 2;
	}
	outframe->end_flag = 0x7e;
	convFrameHex(stpar, outframe, pData);

	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeUA(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *settingdata, u_int len)
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
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;

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
	convFrameHex(stpar, outframe, pData);
	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = f_cs_cal(pData, len + outframe->src_addrlen);
	}
	stpar->send_flag = 1;
	return 0;
}

int makeI(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen, u_int seg, u_int pf, u_char rcv, u_char snd)
{
	u_char pData[255];
	outframe->start_flag = STARTFLAG;
	outframe->f_format.frame_type = FFORMTYPE;
	outframe->f_format.frame_seg = seg;
	outframe->frame_ctl = TYPEI;
	outframe->nr = rcv;
	outframe->ns = snd;
	outframe->pollfin = pf;
	outframe->infolen = infolen;
	outframe->dst_addr = stpar->main_pot_addr;
	outframe->dst_addrlen = stpar->main_pot_addrlen;
	outframe->src_addr = stpar->slav_pot_addr;
	outframe->src_addrlen = stpar->slav_pot_addrlen;
	outframe->f_format.frame_sublen = stpar->slav_pot_addrlen+ stpar->main_pot_addrlen+ 5;
	if (infolen != 0)
	{
		for (int i = 0; i < infolen; i++)
		{
			outframe->info_buff[i] = infobuf[i];
		}
		outframe->f_format.frame_sublen += infolen + 2;
	}
	outframe->end_flag = 0x7e;
	convFrameHex(stpar, outframe, pData);

	//calculate h_cs
	u_int offest = 3 + outframe->dst_addrlen + outframe->src_addrlen;
	outframe->h_cs = cs_cal(pData + 1, offest);
	*(pData + 1 + offest) = outframe->h_cs >> 8;
	*(pData + 1 + offest + 1) = outframe->h_cs & 0x0F;

	//calculate f_cs
	if (outframe->infolen > 0)
	{
		outframe->f_cs = cs_cal(pData + 1, outframe->f_format.frame_sublen - 2);
	}
	stpar->send_flag = 1;
	return 0;
}
