#include "stdafx.h"
#include "hdlc.h"
#include <string>
#include <vector>
#include <regex>
using namespace std;
int outHexStr(_TCHAR* S, u_char *pRawData, u_int index, u_int *bytesidx, u_char byte){
	if (S!=NULL)
	{
		u_int rs = _stprintf_s(S + index, 255, _T("%02x"), byte);
		index += rs;
	}
	if (pRawData!=NULL)
	{
		pRawData[(*bytesidx)++] = (u_char)byte;
	}
	return index;
}

//根据脚本生成帧
int hs_loadstring(string scriptline, _TCHAR *upData, u_char *pRawData)
{
	string type;
	string strRInd, strSInd;
	string strDstAddr, strSrcAddr;
	string strDstLen;
	string strPF;
	string strSplitBit;
	string strInfo;
	if (scriptline.size() > 0)
	{
		regex szReg("(\\+)?([0-9]+) (<|>) ([A-Z]{0,4}) (P|F) (S|N) ([0-9]*):([0-9]*)\\(([124])\\) ([0-9]*):([0-9]*) DAT=([A-F0-9\\s]+)");
		smatch regresult;
		string srcstr = "+34 > FRMR P S 123:345(2) 8:4 DAT=AE342312EFCD";
		vector<string> resstr;
		regex_match(scriptline, regresult, szReg);

		type = regresult[4];
		strPF = regresult[5];
		strSplitBit = regresult[6];
		strSrcAddr = regresult[7];
		strDstAddr = regresult[8];
		strDstLen = regresult[9];
		strRInd = regresult[10];
		strSInd = regresult[11];
		strInfo = regresult[12];

		BOOL bPF = strPF == "P" ? 0 : 1;

		_TCHAR *pData = new _TCHAR[256];
		CString tempFrameT;
		CString tempDstA;
		//int type = -1;
		hdlc frame;
		u_char ctrlByte;
		u_char *pRawData = new u_char[256];

		int RInd, SInd;

		frame.start_flag = 0x7E;
		frame.end_flag = 0x7E;
		frame.dst_addr = atoi(strDstAddr.c_str());

		if (type == "I"){
			ctrlByte = GETTYPE(I);
			//RInd = _ttoi(m_strRInd.Trim());
			//SInd = _ttoi(m_strSInd.Trim());
			RInd = atoi(strRInd.c_str());
			SInd = atoi(strSInd.c_str());
			ASSIGNRIND(ctrlByte, RInd);
			ASSIGNSIND(ctrlByte, SInd);
			SWITCHPF(ctrlByte, (u_char)bPF);
		}
		else if (type == "RR"){
			ctrlByte = GETTYPE(RR);
			//RInd = _ttoi(m_strRInd.Trim());
			RInd = atoi(strRInd.c_str());
			ASSIGNRIND(ctrlByte, RInd);
			SWITCHPF(ctrlByte, (u_char)bPF);
		}
		else if (type == "RNR"){
			ctrlByte = GETTYPE(RNR);
			//RInd = _ttoi(m_strRInd.Trim());
			RInd = atoi(strRInd.c_str());
			ASSIGNRIND(ctrlByte, RInd);
			SWITCHPF(ctrlByte, (u_char)bPF);
		}
		else if (type == "SNRM"){
			ctrlByte = GETTYPE(SNRM);
			SWITCHPF(ctrlByte, PIN);
		}
		else if (type == "DISC"){
			ctrlByte = GETTYPE(DISC);
			SWITCHPF(ctrlByte, PIN);
		}
		else if (type == "UA"){
			ctrlByte = GETTYPE(UA);
			SWITCHPF(ctrlByte, FIN);
		}
		else if (type == "DM"){
			ctrlByte = GETTYPE(DM);
			SWITCHPF(ctrlByte, FIN);
		}
		else if (type == "FRMR"){
			ctrlByte = GETTYPE(FRMR);
			SWITCHPF(ctrlByte, FIN);
		}
		else if (type == "UI"){
			ctrlByte = GETTYPE(UI);
			SWITCHPF(ctrlByte, FIN);
		}
		else{
			ctrlByte = GETTYPE(I);
			SWITCHPF(ctrlByte, (u_char)bPF);
		}
		frame.frame_ctl = ctrlByte;
		frame.f_format.frame_seg = strSplitBit == "S" ? 0 : 1;
		frame.f_format.frame_type = 0xA0;

		frame.dst_addrlen = atoi(strDstLen.c_str());

		frame.src_addr = atoi(strSrcAddr.c_str());
		frame.dst_addr = atoi(strDstAddr.c_str());

		CString csInfo(strInfo.c_str());
		csInfo.Replace(_T(" "), _T(""));

		CString tempstr;
		u_int tempint;
		if (!csInfo.IsEmpty())
		{
			for (int i = 0; i < csInfo.GetLength() / 2; i++)
			{
				tempstr = _T("0x") + csInfo.Mid(2 * i, 2);
				_stscanf_s(tempstr, _T("%x"), &tempint);
				frame.info_buff[i] = (u_char)tempint;
			}
			frame.infolen = csInfo.GetLength() / 2;
			frame.f_format.frame_sublen = frame.dst_addrlen + frame.infolen + 8;
		}
		else
		{
			frame.infolen = 0;
			frame.f_format.frame_sublen = frame.dst_addrlen + 6;
		}

		*pData = frame.f_format.frame_type;
		return frame.f_format.frame_sublen;
	}
	return 0;
}