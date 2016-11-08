
// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <stdlib.h>
#include "framedef.h"
#include "hdlcFSM.h"
#include "eventhandler.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*-------------------------参数设置----------------------*/
//hdlcpointer hdlc_p =  NULL;
hdlcpointer hdlc_p;
u_char save_inf[MAX_LEN];
//tcb->rcv_num我自己记录的接收序号
//tcb->send_num我自己记录的发送序号
u_char nr = 0;                //接收到的接收序号
u_char ns = 0;                //接收到的发送序号
//初始时置1状态为NDM，2为WAIT_CONNECT , 3为NRM , 4为WAIT_DISCONNECT
u_char str[MAX_LEN];
u_char* str_pointer;
FILE *wc;
u_int tmp;
u_char* str_p;
static u_int m = 0; //记录信息分帧存储的变量
u_short h_cs_val;  //计算出来的HCS序列
u_short f_cs_val;   //计算出来的FCS序列
u_char write_str[MAX_LEN];
int nSentLen;
HdlcTcb _tcb;
HdlcTcb *tcb = &_tcb;
HdlcStationParam _stpar;

DWORD curtime;

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
	afx_msg LRESULT OnInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnApplmsg(WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_MESSAGE(WM_INFO, &CAboutDlg::OnInfo)
	ON_MESSAGE(WM_APPLMSG, &CAboutDlg::OnApplmsg)
END_MESSAGE_MAP()


// CServerDlg 对话框




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/) //
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_recdata(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strServerName = _T("");
	m_nServerPort = 0;
	m_strToServer = _T("");
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_strServerName);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);
	DDV_MinMaxInt(pDX, m_nServerPort, 1024, 49151);
	DDX_Text(pDX, IDC_EDIT_TOSERVER, m_strToServer);
	DDX_Control(pDX, IDC_LIST_RECEIVED, m_listReceived);
	DDX_Control(pDX, IDC_LIST_SENT, m_listSent);
	//  DDX_Control(pDX, IDC_LIST1, IDC_LIST_STATEINFO);
	//  DDX_Control(pDX, IDC_LIST_STATEINFO, m_liststateinfo);
	DDX_Control(pDX, IDC_LIST_STATEINFO, m_liststateinfo);
	DDX_Text(pDX, IDC_EDIT1, m_recdata);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_LISTEN, &CServerDlg::OnClickedButtonListen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSELISTEN, &CServerDlg::OnClickedButtonCloselisten)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CServerDlg::OnClickedButtonSend)
	ON_MESSAGE(WM_INFO, &CServerDlg::OnInfo)
//	ON_REGISTERED_MESSAGE(WM_TEST, &CServerDlg::OnTest)
ON_WM_TIMER()
ON_MESSAGE(WM_REPORTEVENT, &CServerDlg::OnReportEvent)
ON_MESSAGE(WM_APPLMSG, &CServerDlg::OnApplmsg)
ON_MESSAGE(WM_APPLGETDATA, &CServerDlg::OnApplgetdata)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	hdlc_p = (hdlcpointer)malloc(sizeof(*hdlc_p));//结构体指针的初始化
	memset(hdlc_p, 0, sizeof(*hdlc_p));//动态分配空间，全部置零
	//FILE *wc ;
	//static u_int m = 0; //记录信息分帧存储的变量,静态变量，不会第二次被赋初始值
	wc = fopen("write.txt", "w");
	if (wc == NULL)
	{
		printf("failed to open file\n");
		return -1;
	}
	//	nr = 0; ns = 0;
	//	rcv_num = 0; send_num = 0;
	//	state_val = STATE_NDM;//初始状态为NDM
	m_strServerName = "localhost";
	m_nServerPort = 1024;                   //服务器的端口号是提前设置好的
	UpdateData(FALSE);                   //将变量的值(即localhost和1024)传到控件
	m_sServerSocket.setParentDlg(this);
	m_sClientSocket.setParentDlg(this);   //关联两个类 此时this=m_pDlg

	SendDlgItemMessage(IDC_LIST_SENT, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);
	SendDlgItemMessage(IDC_LIST_RECEIVED, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);
	SendDlgItemMessage(IDC_LIST_STATEINFO, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);


	FSMinit();

	gstpar = &_stpar;
	HdlcParamInit();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnClickedButtonListen()
{
	// TODO: 在此添加控件通知处理程序代码
	int opt = 1;
	UpdateData(TRUE);   //刷新控件的值到对应的变量(外部输入值交给内部变量)
	GetDlgItem(IDC_BUTTON_LISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(FALSE);
	m_sServerSocket.Create(m_nServerPort);//创建套接字，并进行主机名解析以及端口号绑定
	m_sServerSocket.SetSockOpt(SO_REUSEADDR, &opt, sizeof(opt));
	m_sServerSocket.Listen(); //开始监听

	//清除两个列表框信息
	while (m_listSent.GetCount() != 0)
		m_listSent.DeleteString(0);
	while (m_listReceived.GetCount() != 0)
		m_listReceived.DeleteString(0);
	//更新按钮及文本框的状态
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
}


void CServerDlg::OnClickedButtonCloselisten()
{
	// TODO: 在此添加控件通知处理程序代码
	onClose();
}


void CServerDlg::OnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	int nMsgLen;
	//int nSentLen;
	UpdateData(TRUE);
	if (!m_strToServer.IsEmpty())
	{
		nMsgLen = m_strToServer.GetLength()*sizeof(m_strToServer);
		nSentLen = m_sClientSocket.Send(m_strToServer, nMsgLen);
		if (nSentLen != SOCKET_ERROR){
			//发送成功
			m_listSent.AddString(m_strToServer);
			//	setHScroll();
			UpdateData(FALSE);
		}


		else {
			AfxMessageBox(LPCTSTR("服务器向客户端发送信息出现错误！"), MB_OK | MB_ICONSTOP);
		}
		m_strToServer.Empty();
		UpdateData(FALSE);
	}
}


void CServerDlg::onClose(void)
{
	m_listReceived.AddString(CString("服务器收到了onClose消息"));
	m_sClientSocket.Close();
	m_sServerSocket.Close();   //Close()函数执行后会触发OnClose()函数
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LISTEN)->EnableWindow(TRUE);
}


void CServerDlg::onAccept(void)  //如果OnAccept函数执行成功会转来执行onAccept()
{
	m_listReceived.AddString(CString("服务器收到了OnAccept消息"));
	m_sServerSocket.Accept(m_sClientSocket);  //用连接套接字与客户端建立连接
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(TRUE);
}


void CServerDlg::onReceive(void)//如果OnReceive()函数执行成功会转来执行onReceive()
{

	u_char buff[2048];
	int nBufferSize = 2048;
	int nReceivedLen;
	CString strReceived;

	!gstpar->disc && KillTimer(1);//关闭超时计数器
	gstpar->send_flag = 0;	//是否发送响应帧

	nReceivedLen = m_sClientSocket.Receive(buff, nBufferSize);  //Receive()函数真正执行接收功能
	//返回接收到的字节数     (如果连接被关闭了，返回0；否则返回SOCKET_ERROR)
	if (nReceivedLen == SOCKET_ERROR)  //接收成功
	{
		AfxMessageBox(_T("接收失败！"), MB_OK | MB_ICONSTOP);
		return;
	}
	buff[nReceivedLen] = _T('\0');
	CString szTemp(buff);
	szTemp.Replace(_T(" "), _T(""));
	strReceived = szTemp;
	m_listReceived.AddString(strReceived);   //在接收列表里打印出接收的内容
	UpdateData(FALSE);
	//接收到的内容存储在buff中，要赋到str中做处理
	memset(str, 0, MAX_LEN);//每次清空一下数组

	convStrHex(szTemp.GetBuffer(), str);

	//-----------------------------------------------------------将结构体赋值----------------------------------------------------------------------

	FSMenter(FSMTypePrehandleFrame);
	u_int res = convHexFrame(str, hdlc_p);
	FSMreturn();
	
	//-----------------------------------------------------------错误处理--------------------------------------------------------------------------
	switch (res)
	{
	case ERROR_STARTFLAG:
		AfxMessageBox(_T("开始标志位错误！"), MB_OK | MB_ICONSTOP);
		return;
	case ERROR_DST_ADDR_LEN:
		AfxMessageBox(_T("目的地址长度不正确！"), MB_OK | MB_ICONSTOP);
		return;
	case ERROR_SRC_ADDR_LEN:
		AfxMessageBox(_T("源地址长度不正确！"), MB_OK | MB_ICONSTOP);
		return;
	case ERROR_INF_OVERFLOW:
		AfxMessageBox(_T("源地址长度不正确！"), MB_OK | MB_ICONSTOP);
		return;
	}
	if (res == ERROR_INF_OVERFLOW)//信息域超长
	{
		u_char data[255] = {0x10, 0x00, 0x20};
		m_strToServer = " 7E A0 0D 21 02 23 97 04 C4 10 00 20 E0 6B 7E";
		nSentLen = m_sClientSocket.Send(m_strToServer, 100);
		if (nSentLen != SOCKET_ERROR)
		{
			//发送成功
			m_listSent.AddString(m_strToServer);
			UpdateData(FALSE);
		}
		//  m_liststateinfo.AddString(_T("信息域超长，超出协商的最大信息长度！"));
		AfxMessageBox(_T("信息域超长，超出协商的最大信息长度！！！"), MB_OK | MB_ICONSTOP);
		return;
	}

	//printf("\n开始接收hdlc帧！\n");
	m_liststateinfo.AddString(_T("开始接收HDLC帧！"));
	//start inter_frame_timer
	if (hdlc_p->f_format.frame_type != 0xA0)//帧类型正确
	{
		AfxMessageBox(_T("HDLC格式类型错误!"));
		return;
	}
	u_short frame_length = hdlc_p->f_format.frame_sublen;//获得帧长度子域的值，为？个字节
	// u_char head_length = 2;//记录头序列的长度
	//u_int d_addr_length = addr_length();
	//head_length = head_length +d_addr_length+1;//最后的加一是将源地址的长度也加进去了


	//判断控制域
	switch (GetTypes(*hdlc_p))
	{
	case TYPESNRM:
		m_liststateinfo.AddString(_T("帧类型为SNRM！"));
		break;
	case TYPEDISC:
		m_liststateinfo.AddString(_T("帧类型为DISC！"));
		break;
	case TYPEI:
		m_liststateinfo.AddString(_T("帧类型为I！"));
		break;
	case TYPERR:
		m_liststateinfo.AddString(_T("帧类型为RR！"));
		break;
	case TYPERNR:
		m_liststateinfo.AddString(_T("帧类型为RNR！"));
		break;
	case TYPEUI:
		m_liststateinfo.AddString(_T("帧类型为RNR！"));
		break;
	default:
		m_liststateinfo.AddString(_T("frmr:控制域类型错误或源地址长度超过1字节！"));
		gstpar->frmr_flag = 1;
		return;
	}

	
	//进行HCS校验-----------------------计算h_cs_cal
	h_cs_val = h_cs_cal(str, hdlc_p->dst_addrlen);
	if (hdlc_p->h_cs != h_cs_val)
	{
		m_liststateinfo.AddString(_T("error:HCS校验出错！！！"));
		return;
	}

	//帧结束标志检测
	if (hdlc_p->end_flag != 0x7E)
	{
		m_liststateinfo.AddString(_T("error:帧接收超时,没有结尾标志！！！"));//等待超时结束，没有检测到帧结束标志
		return;
	}

	m_liststateinfo.AddString(_T("一帧接收结束！"));
	if (gstpar->started == 0)
	{
		gstpar->started = 1;//frame_ind = 1;//标志可以进入状态循环
		gstpar->m = 0;
		gstpar->save_inf = save_inf;
		gstpar->hWnd = this;

		//fsmstack->curstate = STATE_NDM;
	}
	gstpar->nr = hdlc_p->nr;
	gstpar->ns = hdlc_p->ns;
	hdlc outframe;//输出帧

	u_int i_length = hdlc_p->infolen;
	gstpar->frame_p_f = hdlc_p->pollfin;

	
	do{
		gstpar->isTransFinish = 1;
		GETHANDLER(fsmstack->curstate)(gstpar, hdlc_p, &outframe);
	} while (!gstpar->isTransFinish);


	if (gstpar->isUIWaiting == 1 && gstpar->canUISend == 1)
	{
		_TCHAR  sendStr[1024];
		u_char sendData[255];
		convFrameHex(&gUIFrame, write_str);
		convFrameStr(&gUIFrame, sendStr);
		CString m_strToServer = sendStr;
		writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
		nSentLen = m_sClientSocket.Send(m_strToServer, 1024);
		if (nSentLen != SOCKET_ERROR)
		{
			//发送成功
			m_listSent.AddString(_T("发送UI帧"));
			m_listSent.AddString(m_strToServer);
		}
		gstpar->canUISend = 0;
		gstpar->isUIWaiting = 0;
	}

	if (gstpar->send_flag == 1)
	{
		_TCHAR  sendStr[1024];
		u_char sendData[255];
		convFrameHex(&outframe, write_str);
		convFrameStr(&outframe, sendStr);
		CString m_strToServer = sendStr;
		writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
		nSentLen = m_sClientSocket.Send(m_strToServer, 1024);
		if (nSentLen != SOCKET_ERROR)
		{
			//发送成功
			m_listSent.AddString(m_strToServer);
		}
	}


	UpdateData(FALSE);

	!gstpar->disc && SetTimer(1, 10000, NULL);

	return;
}

/*void CServerDlg::setHScroll()
{
CDC* dc = GetDC();
SIZE s;
int index;
CString str;
long temp;
for(index= 0; index< m_listSent.GetCount(); index++)
{
m_listSent.GetText(index,str);
s = dc->GetTextExtent(str,str.GetLength()+1);   // 获取字符串的像素大小
// 如果新的字符串宽度大于先前的水平滚动条宽度，则重新设置滚动条宽度
// IDC_LISTBOX为m_List的资源ID
temp = (long)SendDlgItemMessage(IDC_LIST_SENT, LB_GETHORIZONTALEXTENT, 0, 0); //temp得到滚动条的宽度
if (s.cx > temp)
{
SendDlgItemMessage(IDC_LIST_SENT, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx,0);
}
}
ReleaseDC(dc);
}*/

afx_msg LRESULT CAboutDlg::OnInfo(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


afx_msg LRESULT CServerDlg::OnInfo(WPARAM wParam, LPARAM lParam)
{
	_TCHAR *info = (_TCHAR*)wParam;
	m_liststateinfo.AddString(info);
	return 0;
}


//afx_msg LRESULT CServerDlg::OnTest(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}


void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	u_int timerid = (u_int)nIDEvent;
	if (timerid == 1)
	{
		m_liststateinfo.AddString(_T("warning:Idle Timeout Disconnect!"));
		m_liststateinfo.SetCurSel(m_liststateinfo.GetCount() - 1);
	}
	_TCHAR outstr[255];
	hdlcpointer outframe = (hdlcpointer)malloc(sizeof(hdlc));
	makeDISC(gstpar, hdlc_p, outframe);
	memset(hdlc_p, 0, sizeof(*hdlc_p));//动态分配空间，全部置零
	FSMinit();
	gstpar->nr = 0;
	gstpar->ns = 0;
	gstpar->started = 0;
	gstpar->frame_p_f = 0;
	gstpar->rcv_num = 0;
	gstpar->send_num = 0;
	gstpar->frmr_flag = 0;
	gstpar->disc = 1;
	UpdateData(FALSE);
	KillTimer(timerid);
	CDialogEx::OnTimer(nIDEvent);
}


afx_msg LRESULT CServerDlg::OnReportEvent(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	u_int size = (u_int)lParam;
	for (u_int i = 0; i < size; i++)
	{
		gUIInfoBuf[i] = data[i];
	}
	gstpar->isUIWaiting = 1;
	return 0;
}


afx_msg LRESULT CAboutDlg::OnApplmsg(WPARAM wParam, LPARAM lParam)
{

	return 0;
}


afx_msg LRESULT CServerDlg::OnApplmsg(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	m_recdata = data;
	UpdateData(false);
	return 0;
}


afx_msg LRESULT CServerDlg::OnApplgetdata(WPARAM wParam, LPARAM lParam)
{
	u_char *pdata = (u_char *)wParam;
	u_int *plen = (u_int*)lParam;
	u_char data[] = "CAIDLKJFLDKJFLJDLKJFLJDLKFJLSDJLFD";
	*plen = sizeof(data) / sizeof(u_char);
	for (int i = 0; i < *plen; i++)
	{
		pdata[i] = data[i];
	}
	return 0;
}
