
// HdlcFrameGenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HdlcFrameGen.h"
#include "HdlcFrameGenDlg.h"
#include "afxdialogex.h"
#include "hdlc.h"
#include <regex>
#include <fstream>
#ifdef _DEBUG
#include <regex>
#endif // _DEBUG
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
void genFrameStr(_TCHAR *S, u_char *pRawData, hdlc frame);
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHdlcFrameGenDlg 对话框



CHdlcFrameGenDlg::CHdlcFrameGenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHdlcFrameGenDlg::IDD, pParent)
	, m_bSplitBit(FALSE)
	, m_bPF(FALSE)
	, m_strRInd(_T(""))
	, m_strSInd(_T(""))
	, m_strDstAddr(_T(""))
	, m_strSrcAddr(_T(""))
	, m_strInfo(_T(""))
	, m_hResultLog(_T(""))
	, m_strScript(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHdlcFrameGenDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bSplitBit);
	DDX_Check(pDX, IDC_CHECK2, m_bPF);
	DDX_Control(pDX, IDC_COMBO1, m_hAddrLength);
	DDX_Control(pDX, IDC_COMBO2, m_hFrameType);
	DDX_Text(pDX, IDC_EDIT1, m_strRInd);
	DDX_Text(pDX, IDC_EDIT2, m_strSInd);
	DDX_Text(pDX, IDC_EDIT3, m_strDstAddr);
	DDX_Text(pDX, IDC_EDIT4, m_strSrcAddr);
	DDX_Text(pDX, IDC_RICHEDIT21, m_strInfo);
	DDX_Text(pDX, IDC_RICHEDIT22, m_hResultLog);
	DDX_Text(pDX, IDC_EDITSCRIPT, m_strScript);
	DDX_Control(pDX, IDC__GROUPCNTL, m_ctlGroupCntl);
	DDX_Control(pDX, IDC_GROUPINFO, m_ctlGroupInfo);
	DDX_Control(pDX, IDC_GROUPSCRT, m_ctlGroupScrt);
}

BEGIN_MESSAGE_MAP(CHdlcFrameGenDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CHdlcFrameGenDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CHdlcFrameGenDlg::OnBnClickedButton1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CHdlcFrameGenDlg 消息处理程序

BOOL CHdlcFrameGenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO:  在此添加额外的初始化代码
	CString tmpFrameType[9] = { _T("I"), _T("RR"), _T("RNR"), _T("SNRM"), _T("DISC"), _T("UA"), _T("DM"), _T("FRMR")
		, _T("UI") };
	CString str(_T("1"));
	m_hAddrLength.AddString(str);
	str = _T("2");
	m_hAddrLength.AddString(str);
	str = _T("4");
	m_hAddrLength.AddString(str);

	for (int t = 0; t < sizeof(tmpFrameType)/sizeof(CString); t++){
		m_hFrameType.AddString(tmpFrameType[t]);
	}
	
	m_hResultLog = _T("00 00 00 00 00 00 00");
	UpdateData(false);

	m_hbr.CreateSolidBrush(RGB(191, 219, 255));

	m_ctlGroupCntl.SetLineStyle(BS_3D);
	m_ctlGroupCntl.SetLineThickness(1);
	m_ctlGroupCntl.SetBoxColors(RGB(124, 174, 239), RGB(213, 231, 255));

	m_ctlGroupInfo.SetLineStyle(BS_3D);
	m_ctlGroupInfo.SetLineThickness(1);
	m_ctlGroupInfo.SetBoxColors(RGB(124, 174, 239), RGB(213, 231, 255));

	m_ctlGroupScrt.SetLineStyle(BS_3D);
	m_ctlGroupScrt.SetLineThickness(1);
	m_ctlGroupScrt.SetBoxColors(RGB(124, 174, 239), RGB(213, 231, 255));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHdlcFrameGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHdlcFrameGenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHdlcFrameGenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHdlcFrameGenDlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	_TCHAR *pData = new _TCHAR[256];
	CString tempFrameT;
	CString tempDstA;
	int type = -1;
	hdlc frame;
	u_char ctrlByte;
	u_char *pRawData = new u_char[256];

	int RInd, SInd;

	frame.start_flag = 0x7E;
	frame.end_flag = 0x7E;
	tempDstA = m_strDstAddr.Trim();
	frame.dst_addr = _ttoi(tempDstA);
	
	type = m_hFrameType.GetCurSel();

	switch (type)
	{
	case 0:
		ctrlByte = GETTYPE(I);
		RInd = _ttoi(m_strRInd.Trim());
		SInd = _ttoi(m_strSInd.Trim());
		ASSIGNRIND(ctrlByte, RInd);
		ASSIGNSIND(ctrlByte, SInd);
		SWITCHPF(ctrlByte, (u_char)m_bPF);
		break;
	case 1:
		ctrlByte = GETTYPE(RR);
		RInd = _ttoi(m_strRInd.Trim());
		ASSIGNRIND(ctrlByte, RInd);
		SWITCHPF(ctrlByte, (u_char)m_bPF);
		break;
	case 2:
		ctrlByte = GETTYPE(RNR);
		RInd = _ttoi(m_strRInd.Trim());
		ASSIGNRIND(ctrlByte, RInd);
		SWITCHPF(ctrlByte, (u_char)m_bPF);
		break;
	case 3:
		ctrlByte = GETTYPE(SNRM);
		SWITCHPF(ctrlByte, PIN);
		break;
	case 4:
		ctrlByte = GETTYPE(DISC);
		SWITCHPF(ctrlByte, PIN);
		break;
	case 5:
		ctrlByte = GETTYPE(UA);
		SWITCHPF(ctrlByte, FIN);
		break;
	case 6:
		ctrlByte = GETTYPE(DM);
		SWITCHPF(ctrlByte, FIN);
		break;
	case 7:
		ctrlByte = GETTYPE(FRMR);
		SWITCHPF(ctrlByte, FIN);
		break;
	case 8:
		ctrlByte = GETTYPE(UI);
		SWITCHPF(ctrlByte, FIN);
		break;
	default:
		ctrlByte = GETTYPE(I);
		SWITCHPF(ctrlByte, (u_char)m_bPF);
		break;
	}
	frame.frame_ctl = ctrlByte;
	frame.f_format.frame_seg = m_bSplitBit?0:1;
	frame.f_format.frame_type = 0xA0;

	m_hAddrLength.GetWindowTextW(tempDstA);
	frame.dst_addrlen = _ttoi(tempDstA);

	frame.src_addr = _ttoi(m_strSrcAddr.Trim());
	frame.dst_addr = _ttoi(m_strDstAddr.Trim());

	m_strInfo.Replace(_T(" "), _T(""));
	CString tempstr;
	u_int tempint;
	if (!m_strInfo.IsEmpty())
	{
		for (int i = 0; i < m_strInfo.GetLength() / 2; i++)
		{
			tempstr = _T("0x") + m_strInfo.Mid(2 * i, 2);
			_stscanf(tempstr, _T("%x"), &tempint);
			frame.info_buff[i] = (u_char)tempint;
		}
		frame.infolen = m_strInfo.GetLength() / 2;
		frame.f_format.frame_sublen = frame.dst_addrlen + frame.infolen + 8;
	}
	else
	{
		frame.infolen = 0;
		frame.f_format.frame_sublen = frame.dst_addrlen + 6;
	}

	*pData = frame.f_format.frame_type;
	genFrameStr(pData, pRawData, frame);
	m_hResultLog = pData;
	UpdateData(false);
	delete pData;
	delete pRawData;
	pRawData = NULL;
	pData = NULL;
}

void genFrameStr(_TCHAR *S, u_char *pRawData,hdlc frame){
	int rs;
	int index=0;
	int bytesidx = 0;
	u_short typebytes = 0;
	rs = _stprintf(S+index, _T("%02x"), frame.start_flag);
	index += rs;
	pRawData[bytesidx++] = frame.start_flag;

	typebytes = ((u_int)frame.f_format.frame_type << 8) | (frame.f_format.frame_sublen & 0x3F);
	typebytes = (typebytes & 0xBFFF) | ((u_int)frame.f_format.frame_seg << 13);
	rs = _stprintf(S + index, _T("%04x"), typebytes);
	index += rs;
	pRawData[bytesidx++] = (u_char)(typebytes >> 8);
	pRawData[bytesidx++] = (u_char)(typebytes & 0xFF);

	u_char addrbyte;
	for (int i = frame.dst_addrlen - 1; i >=0; i--)
	{
		addrbyte = 0;
		addrbyte = (frame.dst_addr >> (i*7)) & 0x7F;
		addrbyte = addrbyte << 1;
		if (i == 0) addrbyte |= 1;
		rs = _stprintf(S + index, _T("%02x"), addrbyte);
		index += rs;
		pRawData[bytesidx++] = addrbyte;
	}

	addrbyte = 0;
	addrbyte = frame.src_addr;
	addrbyte = addrbyte << 1;
	addrbyte |= 1;
	rs = _stprintf(S + index, _T("%02x"), addrbyte);
	index += rs;
	pRawData[bytesidx++] = addrbyte;

	rs = _stprintf(S + index, _T("%02x"), frame.frame_ctl);
	index += rs;
	pRawData[bytesidx++] = frame.frame_ctl;

	frame.h_cs = h_cs_cal(pRawData, frame.dst_addrlen);
	rs = _stprintf(S + index, _T("%02x"), frame.h_cs >> 8);
	index += rs;
	pRawData[bytesidx++] = (u_char)(frame.h_cs >> 8);
	rs = _stprintf(S + index, _T("%02x"), frame.h_cs & 0xFF);
	index += rs;
	pRawData[bytesidx++] = (u_char)(frame.h_cs & 0xFF);


	//to do: construct information field;
	for (int i = 0; i < frame.infolen; i++)
	{
		rs = _stprintf(S + index, _T("%02x"), frame.info_buff[i]);
		index += rs;
		pRawData[bytesidx++] = frame.info_buff[i];
	}

	if (frame.infolen > 0)
	{
		frame.f_cs = f_cs_cal(pRawData, frame.infolen + frame.dst_addrlen);
		rs = _stprintf(S + index, _T("%02x"), (frame.f_cs >> 8));
		index += rs;
		pRawData[bytesidx++] = (u_char)(frame.f_cs >> 8);
		rs = _stprintf(S + index, _T("%02x"), frame.f_cs & 0xFF);
		index += rs;
		pRawData[bytesidx++] = (u_char)(frame.f_cs & 0xFF);

	}
	rs = _stprintf(S + index, _T("%02x"), frame.end_flag);
	index += rs;
	pRawData[bytesidx++] = frame.end_flag;

}



void CHdlcFrameGenDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	//按脚本生成帧
	m_hResultLog.Empty();
	CFileDialog openfileDlg(TRUE, _T("txt"), NULL, 0, _T("Script file(*.txt)|*.txt||"), this);
	if (IDOK != openfileDlg.DoModal())
	{
		return;
	}

	CString strSptFileName = openfileDlg.GetPathName();
	CStringA strSptFileNameA(strSptFileName);
	string strFileName (strSptFileNameA.GetBuffer());
	ifstream ifscript(strFileName, ios::in);
	if (!ifscript.is_open())
	{
		return;
	}

	string type;
	string strRInd, strSInd;
	string strDstAddr,strSrcAddr;
	string strDstLen;
	string strPF;
	string strSplitBit;
	string strInfo;

	
	string scriptline;
	while (getline(ifscript, scriptline))
	{
		//if (scriptline.size()>0)
		//{
		//	regex szReg("(\\+)?([0-9]+) (<|>) ([A-Z]{0,4}) (P|F) (S|N) ([0-9]*):([0-9]*)\\(([124])\\) ([0-9]*):([0-9]*) DAT=([A-F0-9\s]+)");
		//	smatch regresult;
		//	string srcstr = "+34 > FRMR P S 123:345(2) 8:4 DAT=AE342312EFCD";
		//	vector<string> resstr;
		//	regex_match(scriptline, regresult, szReg);
		//	//for (int i = 0; i < regresult.size(); i++)
		//		MessageBox(CString(regresult[0].str().c_str()));
		//	
		//	type = regresult[4];
		//	strPF = regresult[5];
		//	strSplitBit = regresult[6];
		//	strSrcAddr = regresult[7];
		//	strDstAddr = regresult[8];
		//	strDstLen = regresult[9];
		//	strRInd = regresult[10];
		//	strSInd = regresult[11];
		//	strInfo = regresult[12];

		//	BOOL bPF = strPF == "P" ? 0 : 1;

			_TCHAR *pData = new _TCHAR[256];
		//	CString tempFrameT;
		//	CString tempDstA;
		//	//int type = -1;
		//	hdlc frame;
		//	u_char ctrlByte;
			u_char *pRawData = new u_char[256];

		//	int RInd, SInd;

		//	frame.start_flag = 0x7E;
		//	frame.end_flag = 0x7E;
		//	frame.dst_addr = atoi(strDstAddr.c_str());

		//	if (type == "I"){
		//		ctrlByte = GETTYPE(I);
		//		//RInd = _ttoi(m_strRInd.Trim());
		//		//SInd = _ttoi(m_strSInd.Trim());
		//		RInd = atoi(strRInd.c_str());
		//		SInd = atoi(strSInd.c_str());
		//		ASSIGNRIND(ctrlByte, RInd);
		//		ASSIGNSIND(ctrlByte, SInd);
		//		SWITCHPF(ctrlByte, (u_char)bPF);
		//	}
		//	else if (type == "RR"){
		//		ctrlByte = GETTYPE(RR);
		//		//RInd = _ttoi(m_strRInd.Trim());
		//		RInd = atoi(strRInd.c_str());
		//		ASSIGNRIND(ctrlByte, RInd);
		//		SWITCHPF(ctrlByte, (u_char)bPF);
		//	}
		//	else if (type == "RNR"){
		//		ctrlByte = GETTYPE(RNR);
		//		//RInd = _ttoi(m_strRInd.Trim());
		//		RInd = atoi(strRInd.c_str());
		//		ASSIGNRIND(ctrlByte, RInd);
		//		SWITCHPF(ctrlByte, (u_char)bPF);
		//	}
		//	else if (type == "SNRM"){
		//		ctrlByte = GETTYPE(SNRM);
		//		SWITCHPF(ctrlByte, PIN);
		//	}
		//	else if (type == "DISC"){
		//		ctrlByte = GETTYPE(DISC);
		//		SWITCHPF(ctrlByte, PIN);
		//	}
		//	else if (type == "UA"){
		//		ctrlByte = GETTYPE(UA);
		//		SWITCHPF(ctrlByte, FIN);
		//	}
		//	else if (type == "DM"){
		//		ctrlByte = GETTYPE(DM);
		//		SWITCHPF(ctrlByte, FIN);
		//	}
		//	else if (type == "FRMR"){
		//		ctrlByte = GETTYPE(FRMR);
		//		SWITCHPF(ctrlByte, FIN);
		//	}
		//	else if (type == "UI"){
		//		ctrlByte = GETTYPE(UI);
		//		SWITCHPF(ctrlByte, FIN);
		//	}
		//	else{ 
		//		ctrlByte = GETTYPE(I);
		//		SWITCHPF(ctrlByte, (u_char)bPF);
		//	}
		//	frame.frame_ctl = ctrlByte;
		//	frame.f_format.frame_seg = strSplitBit == "S" ? 0 : 1;
		//	frame.f_format.frame_type = 0xA0;

		//	frame.dst_addrlen = atoi(strDstLen.c_str());

		//	frame.src_addr = atoi(strSrcAddr.c_str());
		//	frame.dst_addr = atoi(strDstAddr.c_str());

		//	CString csInfo(strInfo.c_str());
		//	csInfo.Replace(_T(" "), _T(""));

		//	CString tempstr;
		//	u_int tempint;
		//	if (!csInfo.IsEmpty())
		//	{
		//		for (int i = 0; i < csInfo.GetLength() / 2; i++)
		//		{
		//			tempstr = _T("0x") + csInfo.Mid(2 * i, 2);
		//			_stscanf(tempstr, _T("%x"), &tempint);
		//			frame.info_buff[i] = (u_char)tempint;
		//		}
		//		frame.infolen = csInfo.GetLength() / 2;
		//		frame.f_format.frame_sublen = frame.dst_addrlen + frame.infolen + 8;
		//	}
		//	else
		//	{
		//		frame.infolen = 0;
		//		frame.f_format.frame_sublen = frame.dst_addrlen + 6;
		//	}

		//	*pData = frame.f_format.frame_type;
		//	genFrameStr(pData, pRawData, frame);
			hs_loadstring(scriptline, pData, pRawData);
			m_hResultLog += pData;
			m_hResultLog += _T("\r\n");

			delete pData;
			delete pRawData;
			pRawData = NULL;
			pData = NULL;

	}
	MessageBox(m_hResultLog);
	UpdateData(false);
}



HBRUSH CHdlcFrameGenDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_hbr;
	}
	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(191, 219, 255));
		return m_hbr;
	}
	if (nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkColor(RGB(191, 219, 255));
		return m_hbr;
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
