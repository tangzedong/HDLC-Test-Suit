
// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <stdlib.h>
#include "hdlc.h"
#include "hdlcutil.h"
#include "hdlcFSM.h"
#include "hdlceventhandler.h"
#include "ApplInterface.h"
#include "resource.h"
#include <afxpropertygridctrl.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*-------------------------参数设置----------------------*/
////hdlcpointer hdlc_p =  NULL;
//hdlcpointer hdlc_p;
//u_char save_inf[MAX_LEN];
////tcb->rcv_num我自己记录的接收序号
////tcb->send_num我自己记录的发送序号
//u_char nr = 0;                //接收到的接收序号
//u_char ns = 0;                //接收到的发送序号
////初始时置1状态为NDM，2为WAIT_CONNECT , 3为NRM , 4为WAIT_DISCONNECT
//u_char str[MAX_LEN];
//u_char* str_pointer;
//FILE *wc;
//u_int tmp;
//u_char* str_p;
//static u_int m = 0; //记录信息分帧存储的变量
//u_short h_cs_val;  //计算出来的HCS序列
//u_short f_cs_val;   //计算出来的FCS序列
//u_char write_str[MAX_LEN];
//int nSentLen;
//HdlcTcb _tcb;
//HdlcTcb *tcb = &_tcb;
//HdlcStationParam _stpar;
//
//DWORD curtime;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

CMainDlg::CMainDlg() : CDialogEx(CMainDlg::IDD)
, m_strServerName(_T(""))
, m_nServerPort(0)
, m_bListening(0)
, m_bFrameIncomplete(0)
, m_nPos(0)
, m_delaySendData(1000)
, m_nIdleTimeout(60000)
, m_nResendTimeOut(5000)
, m_nInterTimeout(2000)
{
	for (int i = 0; i < 25; i++)
	{
		serverlist[i] = NULL;
	}
	memset(&m_procinfo, 0, sizeof(PROCESS_INFORMATION));
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERNAME, m_strServerName);
	DDX_Text(pDX, IDC_EDIT2, m_nServerPort);
	DDX_Text(pDX, IDC_EDIT6, m_delaySendData);
	DDX_Text(pDX, IDC_EDIT1, m_nIdleTimeout);
	DDX_Text(pDX, IDC_EDIT5, m_nResendTimeOut);
	DDX_Text(pDX, IDC_EDIT4, m_nInterTimeout);
	DDX_Control(pDX, IDC_SPIN1, m_SpinPort);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_NEWSER, &CMainDlg::OnBnClickedButtonNewser)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CN, &CMainDlg::OnClickedButtonCn)
	ON_COMMAND(ID_VIEW_CONSOLE, &CMainDlg::OnViewConsole)
	ON_NOTIFY(BCN_DROPDOWN, IDC_BUTTON_NEWSER, &CMainDlg::OnDropdownButtonNewser)
	ON_COMMAND(ID_NEW_SERVER, &CMainDlg::OnNewServer)
	ON_COMMAND(ID_NEW_EXIT, &CMainDlg::OnNewExit)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT1, &CMainDlg::OnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT4, &CMainDlg::OnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &CMainDlg::OnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, &CMainDlg::OnChangeEdit6)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CMainDlg::OnDeltaposSpin1)
	ON_COMMAND(ID_NEW_CLIENT, &CMainDlg::OnNewClient)
	ON_COMMAND(ID_PLUGIN_LOADMODULE, &CMainDlg::OnPluginLoadModule)
	ON_COMMAND(ID_PLUGIN_VIEWMODULEINFOMATION, &CMainDlg::OnPluginViewModuleInf)
	ON_COMMAND(ID_MACRO_EDITMACRO, &CMainDlg::OnMacroEditMacro)
END_MESSAGE_MAP()


// CServerDlg 对话框




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/) //
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_recdata(_T(""))
	, m(0)
	, pParent(NULL)
	, m_id(0)
	, pOutFrame(NULL)
	, m_nextSendFrame(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//  m_strServerName = _T("");
	//  m_nServerPort = 0;
	m_strToServer = _T("");
	u_char settingdata[] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
		0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
	m_settingdata = new u_char[sizeof(settingdata)];
	for (int i = 0; i < sizeof(settingdata); i++)
	{
		m_settingdata[i] = settingdata[i];
	}

	stpar = &_stpar;
	HdlcParamInit(stpar);
	HdlcTransParamInit(stpar);
	stpar->settingdata = m_settingdata;
	stpar->fsmstack = NULL;
	//stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);
	stpar->main_pot_addr = 0x10;
	stpar->slav_pot_addr = 0x91;
	stpar->main_pot_addrlen = 1;
	stpar->slav_pot_addrlen = 2;
	stpar->numSegSended = stpar->sendwindowsize * 2;
	m_totalFrameSend = stpar->sendwindowsize * 2;
	//memset(stpar->fsmstack, 0, sizeof(HdlcTcb));
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TOSERVER, m_strToServer);
	//DDX_Control(pDX, IDC_LIST_RECEIVED, m_listReceived);
	//DDX_Control(pDX, IDC_LIST_SENT, m_listSent);
	////  DDX_Control(pDX, IDC_LIST1, IDC_LIST_STATEINFO);
	////  DDX_Control(pDX, IDC_LIST_STATEINFO, m_liststateinfo);
	//DDX_Control(pDX, IDC_LIST_STATEINFO, m_liststateinfo);
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
	ON_MESSAGE(WM_WRITETOFILE, &CServerDlg::OnWriteToFile)
	ON_MESSAGE(WM_SENDLOG, &CServerDlg::OnSendLog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CServerDlg::OnBnClickedButton1)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CServerDlg::OnPropertyChanged)
	ON_BN_CLICKED(IDC_BUTTON_DATASND, &CServerDlg::OnBnClickedButtonDatasnd)
	ON_MESSAGE(WM_APPLAYERREADY, &CServerDlg::OnAppLayerReady)
	ON_MESSAGE(IDM_DATALINKREADY, &CServerDlg::OnDatalinkReady)
	ON_MESSAGE(IDM_RESENDWND, &CServerDlg::OnResendWnd)
	ON_MESSAGE(IDM_SENDDATA, &CServerDlg::OnSendData)
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
	//输出日志文件
#ifdef _OUTFILE
	fopen_s(&wc, "write.txt", "w");
	if (wc == NULL)
	{
		printf("failed to open file\n");
		return -1;
	}
#endif
	//	nr = 0; ns = 0;
	//	rcv_num = 0; send_num = 0;
	//	state_val = STATE_NDM;//初始状态为NDM

	UpdateData(FALSE);                   //将变量的值(即localhost和1024)传到控件

	CString strCapital;
	strCapital.Format(_T("Server %d: 0x%04x:%d"), m_id, stpar->slav_pot_addr, stpar->slav_pot_addrlen);
	SetWindowText(strCapital);

	m_propertydlg.Create(IDD_PROPERTYDLG, this);

	CRect rect;
	GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT3)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top += 3;
	rect.bottom -= 3;
	rect.left += 3;
	rect.right -= 3;
	//rect.SetRectEmpty();
	m_propertysheet.Create(WS_CHILD | WS_VISIBLE, rect, this, 1);

	rect.left = 0;
	rect.right -= 190;
	rect.top += 50;//350
	rect.bottom += 3;
	m_tabout.Create(CMFCTabCtrl::STYLE_FLAT, rect, this, 2);

	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_listReceived.Create(dwStyle, rectDummy, &m_tabout, 2) ||
		!m_listLog.Create(dwStyle, rectDummy, &m_tabout, 3) ||
		!m_liststateinfo.Create(dwStyle, rectDummy, &m_tabout, 4))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}
	//m_tabout.AddTab(&m_listReceived, _T("无用（接收）"), 0, FALSE);
	m_tabout.AddTab(&m_listLog, _T("日志"), 0, FALSE);
	m_tabout.AddTab(&m_liststateinfo, _T("状态"), 1, FALSE);

	m_tabout.SetWindowPos(FromHandle(HWND_TOP), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	UpdateData(FALSE);
	SendDlgItemMessage(IDC_LIST_SENT, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);
	SendDlgItemMessage(IDC_LIST_RECEIVED, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);
	SendDlgItemMessage(IDC_LIST_STATEINFO, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);

	OnApplicationLook();

	//FSMinit(stpar->fsmstack);
	stpar->fsmstack = FSMenter(stpar->fsmstack, FSMTypePrimary);

	HdlcParamInit(stpar);
	HdlcTransParamInit(stpar);

	_TCHAR bufval[30];
	int val = stpar->slav_pot_addrlen;
	_stprintf(bufval, _T("%0x"), val);
	CMFCPropertyGridProperty *pA = new CMFCPropertyGridProperty(_T("地址长度"), (_variant_t)bufval, _T("地址"));

	val = stpar->slav_pot_addr >> (stpar->slav_pot_addrlen/2 * 7);
	(stpar->slav_pot_addrlen == 1) && (val = 0);
	_stprintf(bufval, _T("%0x"), val);
	CMFCPropertyGridProperty *pB = new CMFCPropertyGridProperty(_T("高地址"), (_variant_t)bufval, _T("高地址"));

	val = stpar->slav_pot_addr & (0xffff << ((stpar->slav_pot_addrlen/2) * 7));
	_stprintf(bufval, _T("%0x"), val);
	CMFCPropertyGridProperty *pC = new CMFCPropertyGridProperty(_T("低地址"), (_variant_t)bufval, _T("低地址"));
	m_propertysheet.AddProperty(pA);
	m_propertysheet.AddProperty(pB);
	m_propertysheet.AddProperty(pC);

	SetTimer(TM_SERVEREVENT, 10000, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CMainDlg dlgAbout;
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

	//清除两个列表框信息
	while (m_listLog.GetCount() != 0)
		m_listLog.DeleteString(0);
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

}


void CServerDlg::onClose(void)
{
	HdlcParamInit(stpar);
	HdlcTransParamInit(stpar);
	FSMinit(stpar->fsmstack);
	m_listReceived.AddString(CString("服务器收到了onClose消息"));

	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LISTEN)->EnableWindow(TRUE);
}


void CServerDlg::onAccept(void)  //如果OnAccept函数执行成功会转来执行onAccept()
{
	//m_listReceived.AddString(CString("服务器收到了OnAccept消息"));
	//m_sServerSocket.Accept(m_sClientSocket);  //用连接套接字与客户端建立连接
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(TRUE);
}


void CServerDlg::onReceive(hdlcpointer frame)//如果OnReceive()函数执行成功会转来执行onReceive()
{

	//u_char buff[2048];
	int nBufferSize = 2048;
	//int nReceivedLen;
	CString strReceived;
	*hdlc_p = *frame;
	!stpar->disc && KillTimer(TM_IDLETIMEOUT);//关闭超时计数器
	stpar->send_flag = 0;	//是否发送响应帧

	////-----------------------------------------------------------将结构体赋值----------------------------------------------------------------------

	int res = RuntimeFrameCheck(stpar, hdlc_p);

	//-----------------------------------------------------------错误处理--------------------------------------------------------------------------



	switch (res)
	{
	case ERROR_STARTFLAG:
		m_liststateinfo.AddString(_T("开始标志位错误！"));
		return;
	case ERROR_FORMAT_TYPE:
		m_liststateinfo.AddString(_T("HDLC格式类型错误!"));
		return;
	case ERROR_DST_ADDR_LEN:
		m_liststateinfo.AddString(_T("目的地址长度不正确！"));
		return;
	case ERROR_SRC_ADDR_LEN:
		m_liststateinfo.AddString(_T("源地址长度不正确！"));
		return;
	case ERROR_INF_OVERFLOW://信息域超长
		{
			//Send FRMR
			//u_char data[255] = { 0x10, 0x00, 0x20 };
			//m_strToServer = " 7E A0 0D 21 02 23 97 04 C4 10 00 20 E0 6B 7E";
			//nSentLen = m_sClientSocket.Send(m_strToServer, 100);
			//if (nSentLen != SOCKET_ERROR)
			//{
			//	//发送成功
			//	m_listSent.AddString(m_strToServer);
			//	UpdateData(FALSE);
			//}
			////  m_liststateinfo.AddString(_T("信息域超长，超出协商的最大信息长度！"));
			m_liststateinfo.AddString(_T("信息域超长，超出协商的最大信息长度！！！"));
			return;
		}
		break;
	case ERROR_ENDFLAG:
		m_liststateinfo.AddString(_T("error:帧接收超时,没有结尾标志！！！"));//等待超时结束，没有检测到帧结束标志
		return;
	}
	

	//start inter_frame_timer

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
		m_liststateinfo.AddString(_T("帧类型为UI！"));
		break;
	default:
		m_liststateinfo.AddString(_T("frmr:控制域类型错误或源地址长度超过1字节！"));
		//stpar->frmr_flag = 1;
		//return;
	}
	m_liststateinfo.AddString(_T("一帧接收结束！"));

	char buf[32];
	_strtime(buf);
	CString strMsg;
	strMsg.Format(_T("%s %5s %8s %7s %3d,%3d"), CString(buf), CString(GetTypestr(*hdlc_p)), _T("receive"), _T("normal"), hdlc_p->ns, hdlc_p->nr);
	m_listLog.AddString(strMsg);
	int iCount = m_listLog.GetCount();
	if (iCount > 0)
		m_listLog.SetCurSel(iCount - 1);

	if (stpar->started == 0)
	{
		stpar->started = 1;//frame_ind = 1;//标志可以进入状态循环
		stpar->m = 0;
		stpar->save_inf = save_inf;
		stpar->hWnd = this;

		//fsmstack->curstate = STATE_NDM;
	}
	stpar->nr = hdlc_p->nr;
	stpar->ns = hdlc_p->ns;
	hdlc outframe;//输出帧
	memset(&outframe, 0, sizeof(hdlc));
	u_int i_length = hdlc_p->infolen;
	stpar->frame_p_f = hdlc_p->pollfin;

	if (GetTypes(*hdlc_p) == TYPEUI)//收到UI帧，发送未发送的UI帧
	{
		if (stpar->isUIWaiting == 1)
		{
			stpar->canUISend = 1;
			makeUI(stpar, frame, &(stpar->hWnd->m_UIFrame), (u_char*)(stpar->hWnd->m_UIInfoBuf), stpar->hWnd->m_UIInfoLen);
			nSentLen = SendFrame("UI", &(stpar->hWnd->m_UIFrame));
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				m_liststateinfo.AddString(_T("发送UI帧"));
				m_liststateinfo.AddString(m_strToServer);
			}
			stpar->canUISend = 0;
			stpar->isUIWaiting = 0;
		}
		return; //直接终止，不进入FSM
	}

	do{
		stpar->isTransFinish = 1;
		//CString outstr;
		//if (stpar->fsmstack)
		//	outstr.Format(_T("%d:%d"), stpar->fsmstack->curstate, stpar->fsmstack->fsmtype);
		//m_listSent.AddString(outstr);
		UpdateData(FALSE);
		GETHANDLER(stpar->fsmstack)(stpar, hdlc_p, &outframe);


		if (stpar->isUIWaiting == 1 && stpar->canUISend == 1)
		{
			//tang: to do 用ServerDlg::Send代替以下功能
			//_TCHAR  sendStr[1024];
			//u_char sendData[255];
			//convFrameHex(stpar, &(stpar->hWnd->m_UIFrame), write_str);
			//convFrameStr(stpar, &(stpar->hWnd->m_UIFrame), sendStr);
			//CString m_strToServer = sendStr;
			//writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
			//nSentLen = ((CAboutDlg*)AfxGetMainWnd())->DoSend(m_strToServer);
			nSentLen = SendFrame("UI", &(stpar->hWnd->m_UIFrame));
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				m_liststateinfo.AddString(_T("发送UI帧"));
				m_liststateinfo.AddString(m_strToServer);
			}
			stpar->canUISend = 0;
			stpar->isUIWaiting = 0;
		}

		if (stpar->send_flag == 1)
		{
			//tang: to do 用ServerDlg::Send代替以下功能
			//_TCHAR  sendStr[1024];
			//u_char sendData[255];
			//convFrameHex(stpar, &outframe, write_str);
			//convFrameStr(stpar, &outframe, sendStr);
			//CString m_strToServer = sendStr;
			//writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
			//nSentLen = ((CAboutDlg*)AfxGetMainWnd())->DoSend(m_strToServer);
			nSentLen = SendFrame(GetTypestr(outframe), &outframe);
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				m_liststateinfo.AddString(m_strToServer);
			}
			stpar->send_flag = 0;
		}

	} while (!stpar->isTransFinish);

	UpdateData(FALSE);

#ifndef _DEBUG
	!stpar->disc && SetTimer(TM_IDLETIMEOUT, 10000, NULL);
#endif

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

afx_msg LRESULT CMainDlg::OnInfo(WPARAM wParam, LPARAM lParam)
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
	switch (timerid)
	{
	case TM_IDLETIMEOUT:
	{
		m_liststateinfo.AddString(_T("warning:Idle Timeout Disconnect!"));
		m_liststateinfo.SetCurSel(m_liststateinfo.GetCount() - 1);

		//_TCHAR outstr[255];
		hdlcpointer outframe = (hdlcpointer)malloc(sizeof(hdlc));
		makeDISC(stpar, hdlc_p, outframe);
		memset(hdlc_p, 0, sizeof(*hdlc_p));//动态分配空间，全部置零
		FSMinit(stpar->fsmstack);
		stpar->nr = 0;
		stpar->ns = 0;
		stpar->started = 0;
		stpar->frame_p_f = 0;
		stpar->rcv_num = 0;
		stpar->send_num = 0;
		stpar->frmr_flag = 0;
		stpar->disc = 1;

		free(outframe);
		UpdateData(FALSE);
		KillTimer(timerid);
		break;

	}
	case TM_SENDDATA:
		if (stpar->m < stpar->sendwindowsize)
		{
			PostMessage(WM_APPLAYERREADY);
			KillTimer(TM_IDLETIMEOUT);
			//SetTimer(TM_IDLETIMEOUT, m_nIdleTimeOut, NULL);
		}
		else
		{
			SetTimer(TM_IDLETIMEOUT, ((CMainDlg*)AfxGetMainWnd())->m_nIdleTimeout, NULL);
			SetTimer(TM_RESENDWND, ((CMainDlg*)AfxGetMainWnd())->m_nResendTimeOut, NULL);
			KillTimer(TM_SENDDATA);
			stpar->m = 0;
		}
		break;
	case TM_RESENDWND:
		stpar->m = 0;
		KillTimer(TM_IDLETIMEOUT);
		KillTimer(TM_RESENDWND);
		PostMessage(IDM_RESENDWND);
		break;
	case TM_SERVEREVENT:
		if (!stpar->isUIWaiting)
		{
			time_t t = time(NULL);
			m_UIInfoLen = sprintf_s(m_UIInfoBuf, "%8d", t);
			stpar->isUIWaiting;
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


afx_msg LRESULT CServerDlg::OnReportEvent(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	u_int size = (u_int)lParam;
	for (u_int i = 0; i < size; i++)
	{
		m_UIInfoBuf[i] = data[i];
	}
	stpar->isUIWaiting = 1;
	return 0;
}


afx_msg LRESULT CServerDlg::OnApplmsg(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	m_recdata = data;
	UpdateData(false);
	u_char snddata[] = "DFkkdflsdfdllllllllllllhhhhhhhhhflsdj";
	ApplHdlcSend(stpar, snddata, sizeof(snddata)/sizeof(u_char));
	return 0;
}


afx_msg LRESULT CServerDlg::OnApplgetdata(WPARAM wParam, LPARAM lParam)
{
	u_char *pdata = (u_char *)wParam;
	u_int *plen = (u_int*)lParam;
	u_char data[] = "CAIDLKJFLDKJFLJDLKJFLJDLKFJLSDJLFD";
	*plen = sizeof(data) / sizeof(u_char);
	for (u_int i = 0; i < *plen; i++)
	{
		pdata[i] = data[i];
	}
	return 0;
}


//afx_msg LRESULT CAboutDlg::OnWriteToFiel(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}


//afx_msg LRESULT CAboutDlg::OnSendLog(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}


afx_msg LRESULT CServerDlg::OnWriteToFile(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	writetxt((FILE*)wc, data, (u_int)lParam);
	delete data;
	data = NULL;
	return 0;
}
afx_msg LRESULT CServerDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;
	int i = pProp->GetData();
	CString strProp = pProp->GetName();
	COleVariant val = pProp->GetValue();
	COleVariant oval = pProp->GetOriginalValue();
	CString strVal = val.bstrVal;
	CString strOval = oval.bstrVal;
	int ival, ioval;
	int n = _stscanf(strVal.GetBuffer(), _T("%x"), &ival);
	if (n == 0)
	{
		return 0;
	}
	n = _stscanf(strOval.GetBuffer(), _T("%x"), &ioval);
	if (n == 0)
	{
		return 0;
	}
	if (ioval != ival)
	{
		if (strProp == _T("地址长度"))
		{
			stpar->slav_pot_addrlen = ival;
		}
		else if (strProp == _T("高地址"))
		{
			switch (stpar->slav_pot_addrlen)
			{
			case 1:
				break;
			case 2:
				stpar->slav_pot_addr &= 0x7F;
				ival = ival << 7;
				stpar->slav_pot_addr |= ival;
				break;
			case 4:
				stpar->slav_pot_addr &= 0x5FFF;
				ival = ival << 14;
				stpar->slav_pot_addr |= ival;
				break;
			default:
				break;
			}
		}
		else if (strProp == _T("低地址"))
		{
			switch (stpar->slav_pot_addrlen)
			{
			case 1:
				stpar->slav_pot_addr = ival;
				break;
			case 2:
				stpar->slav_pot_addr &= 0x3F80;
				stpar->slav_pot_addr |= ival;
				break;
			case 4:
				stpar->slav_pot_addr &= 0xFFFC000;
				stpar->slav_pot_addr |= ival;
				break;
			default:
				break;
			}
		}
		CString strCapital;
		strCapital.Format(_T("Server %d: 0x%04x:%d"), m_id, stpar->slav_pot_addr, stpar->slav_pot_addrlen);
		SetWindowText(strCapital);
	}

	return 0;
}

afx_msg LRESULT CServerDlg::OnSendLog(WPARAM wParam, LPARAM lParam)
{
	CString *strlog = (CString*)wParam;
	m_listLog.AddString(*strlog);
	delete strlog;
	strlog = NULL;
	return 0;
}


void CServerDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//重写关闭函数，只隐藏
	if (this->close)
	{
		this->DoClear();
		CDialogEx::OnClose();
		return;
	}
	this->ShowWindow(SW_HIDE);
}

void CServerDlg::DoClear()
{
	if (wc != NULL)
	{
		fclose(wc);
		wc = NULL;
	}
	delete m_settingdata;
	m_settingdata = NULL;
}

void CServerDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	m_propertydlg.ShowWindow(m_propertydlg.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}


void CMainDlg::OnBnClickedButtonNewser()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m < 25)
	{
		serverlist[m] = new CServerDlg(this);
		serverlist[m]->SetAddr(0x91 + m);
		serverlist[m]->Create(IDD_SERVER_DIALOG, this);
		serverlist[m]->pParent = this;
		serverlist[m]->m_id = m;
		serverlist[m]->ShowWindow(SW_SHOW);
		m++;
		CString sername;
		sername.Format(_T("%02d:0x%04x 运行..."), m - 1, serverlist[m - 1]->stpar->slav_pot_addr);
		m_condlg.m_listconsole.AddString(sername);
	}
	else
	{
		AfxMessageBox(_T("Server数达到上限"), MB_OKCANCEL);
	}
}


void CMainDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_procinfo.hProcess != NULL)
	{
		if (!TerminateProcess(m_procinfo.hProcess, -1))
		{
			return;
		}
		WaitForSingleObject(m_procinfo.hProcess, INFINITE);
		CloseHandle(m_procinfo.hThread);
		CloseHandle(m_procinfo.hProcess);
	}

	if (m_hMod != NULL)
	{
		FreeLibrary(m_hMod);
	}

	for (u_int i = 0; i < m; i++)
	{
		if (serverlist[i] != NULL)
		{
			if (::IsWindow(serverlist[i]->m_hWnd))
			{
				serverlist[i]->close = TRUE;
				::CloseWindow(serverlist[i]->m_hWnd);
			}
			delete serverlist[i];
			serverlist[i] = NULL;
		}
	}
	m_condlg.CloseWindow();
	CDialogEx::OnOK();
}


// //接受处理函数
void CMainDlg::onReceive(void)
{
	u_char buff[2048];
	u_char *tempbuff = (u_char*) malloc(2048);
	int nBufferSize = 2048;
	int nReceivedLen;
	CString strReceived;
	hdlc frame;
	hdlc *hdlc_p = &frame;

	//m_liststateinfo.AddString(_T("开始接收HDLC帧！"));
	nReceivedLen = m_sClientSocket.Receive(buff, nBufferSize);  //Receive()函数真正执行接收功能
	//返回接收到的字节数     (如果连接被关闭了，返回0；否则返回SOCKET_ERROR)
	if (nReceivedLen == SOCKET_ERROR)  //接收成功
	{
		AfxMessageBox(_T("error:接收失败！"), MB_OK | MB_ICONSTOP);
		return;
	}
	buff[nReceivedLen] = _T('\0');

	CString szTemp(buff);
	szTemp.Replace(_T(" "), _T(""));
	strReceived = szTemp;
	UpdateData(FALSE);
	//接收到的内容存储在buff中，要赋到str中做处理

	memset(tempbuff, 0, MAX_LEN);//每次清空一下数组
	nReceivedLen = convStrHex(szTemp.GetBuffer(), tempbuff);

	if (m_nPos == 0)
	{
		int i;
		int startflag = 0;
		if (tempbuff[m_nPos] == 0x7E)
		{
			startflag = 1;
		}
		m_bFrameIncomplete = 1;
		str[m_nPos++] = tempbuff[0];
		for (i = 1; i < nReceivedLen; i++)
		{
			if (startflag && tempbuff[i] == 0x7E)
			{
				m_bFrameIncomplete = 0;
			}
			str[m_nPos++] = tempbuff[i];
		}
		if (m_bFrameIncomplete == 1)
		{
			SetTimer(TM_INTERFRAME_TIMEOUT, m_nInterTimeout, NULL);
			return;
		}
	}
	else if (m_nPos > 0 && m_bFrameIncomplete)
	{
		int i;
		for (i = 0; i < nReceivedLen && tempbuff[i] != 0x7E; i++)
		{
			str[m_nPos++] = tempbuff[i];
		}
		if (tempbuff[i] == 0x7E)
		{
			str[m_nPos++] = tempbuff[i];
			m_bFrameIncomplete = 0;
			m_nPos = 0;
		}
		else
		{
			m_bFrameIncomplete = 1;
		}
	}

	if (!m_bFrameIncomplete)
	{
		m_nPos = 0;
		KillTimer(TM_INTERFRAME_TIMEOUT);
	}
	free(tempbuff);
	tempbuff = NULL;
	//-----------------------------------------------------------将结构体赋值----------------------------------------------------------------------
	int exception;
	int res2 = readFrame(str, 2048,hdlc_p, &exception);

	switch (res2)
	{
	case NO_START_FLAG:
		AfxMessageBox(_T("read frame error:开始标志错误！"), MB_OK | MB_ICONSTOP);
		return;
	case NO_ENDFLAG:
		AfxMessageBox(_T("read frame error:结束标志错误！"), MB_OK | MB_ICONSTOP);
		return;
	case NOT_MATCH_LEN:
		AfxMessageBox(_T("read frame error:帧长度不匹配！"), MB_OK | MB_ICONSTOP);
		return;
	case FTYPE_FIELD_ERROR:
		AfxMessageBox(_T("read frame error:HDLC格式类型读取错误！"), MB_OK | MB_ICONSTOP);
		return;
	case DSTADDR_FIELD_ERROR:
		AfxMessageBox(_T("read frame error:目的地址读取错误！"), MB_OK | MB_ICONSTOP);
		return;
	case SRCADDR_FIELD_ERROR:
		AfxMessageBox(_T("read frame error:源地址读取错误！"), MB_OK | MB_ICONSTOP);
		return;
	case CONTROLBYTE_FIELD_ERROR:
		AfxMessageBox(_T("read frame error:控制域读取错误！"), MB_OK | MB_ICONSTOP);
		return;
	case HCS_FIELD_ERROR:
		AfxMessageBox(_T("read frame error:HCS域读取错误！"), MB_OK | MB_ICONSTOP);
		return;
	case INFO_FIELD_ERROR:
		AfxMessageBox(_T("read frame error:信息域读取错误！"), MB_OK | MB_ICONSTOP);
		return;
	case 0:
		break;
	}

	//----------------------------------------异常处理------------------------------------------------------------
	if (CSEXCEPTION(exception, EXCEPTION_HCS))
	{
		AfxMessageBox(_T("exception:HCS校验失败！"), MB_OKCANCEL);
		return;
	}
	if (CSEXCEPTION(exception, EXCEPTION_FCS))
	{
		AfxMessageBox(_T("exception:FCS校验失败！"), MB_OKCANCEL);
		return;
	}

	u_char rcvFlag = 0;
	CServerDlg *pDlg;
	for (u_int i = 0; i < m; i++)
	{
		pDlg = serverlist[i];
		if (pDlg->stpar->slav_pot_addr == hdlc_p->dst_addr)
		{
			pDlg->onReceive(hdlc_p);
			rcvFlag = 1;
		}
	}
	//组播处理
	if ((!rcvFlag) && (GetTypes(*hdlc_p) == TYPESNRM || GetTypes(*hdlc_p) == TYPEUI))
	{

		int dstlen = hdlc_p->dst_addrlen, srclen = hdlc_p->src_addrlen;
		int dstaddr = hdlc_p->dst_addr, srcaddr = hdlc_p->src_addr;


		for (int i = 0; i < m; i++)
		{

			pDlg = serverlist[i];
			int slavlen = pDlg->stpar->slav_pot_addrlen;
			int slavaddr = pDlg->stpar->slav_pot_addr;
			int mainlen = pDlg->stpar->main_pot_addrlen;
			if (vailfyBoardCastAddr(pDlg->stpar, hdlc_p))
			{
				switch (dstlen)
				{
				case 1:
				{
					if (slavlen == 1 && dstaddr == 0x7F)
					{

						pDlg->onReceive(hdlc_p);
						rcvFlag = 1;
					}
					break;
				}
				case 2:
				{
					if (slavaddr == 2)
					{
						if (gethighaddr(dstaddr, dstlen) == 0x7F)
						{
							if (getlowaddr(dstaddr, dstlen) == 0x7F)
							{
								pDlg->onReceive(hdlc_p);
								rcvFlag = 1;
							}
							else if (getlowaddr(dstaddr, dstlen) == getlowaddr(slavaddr, slavlen))
							{
								pDlg->onReceive(hdlc_p);
								rcvFlag = 1;
							}
						}
						else if (getlowaddr(dstaddr, dstlen) == 0x7F)
						{
							if (gethighaddr(dstaddr, dstlen) == gethighaddr(slavaddr, slavlen))
							{
								pDlg->onReceive(hdlc_p);
								rcvFlag = 1;
							}
						}
					}
					break;
				}
				case 4:
				{
					if (slavaddr == 4)
					{
						if (gethighaddr(srcaddr, srclen) == 0x3FFF)
						{
							if (getlowaddr(srcaddr, srclen) == 0x3FFF)
							{
								pDlg->onReceive(hdlc_p);
								rcvFlag = 1;
							}
							else if (getlowaddr(srcaddr, srclen) == getlowaddr(slavaddr, slavlen))
							{
								pDlg->onReceive(hdlc_p);
								rcvFlag = 1;
							}
						}
						else if (getlowaddr(srcaddr, srclen) == 0x3FFF)
						{
							if (gethighaddr(srcaddr, srclen) == gethighaddr(slavaddr, slavlen))
							{
								pDlg->onReceive(hdlc_p);
								rcvFlag = 1;
							}
						}
					}
					break;
				}
				}
			}
		}
	}
	if (!rcvFlag)
	{
		AfxMessageBox(_T("warning:找不到从机,discarding frame"), MB_OKCANCEL);
	}
}


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_sServerSocket.setParentDlg(this);
	m_sClientSocket.setParentDlg(this);   //关联两个类 此时this=m_pDlg
	m_strServerName = "localhost";
	m_nServerPort = 1024;                   //服务器的端口号是提前设置好的
	m_SpinPort.SetBuddy(GetDlgItem(IDC_EDIT2));
	m_SpinPort.SetRange(0, SHORT_MAX);
	UpdateData(FALSE);                   //将变量的值(即localhost和1024)传到控件

	m_condlg.Create(IDD_CONSOLEDLG);
	m_condlg.ShowWindow(SW_HIDE);
	m_condlg.SetWindowPos(FromHandle(HWND_BOTTOM), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CDockingManager::SetDockingMode(DT_SMART);

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CMainDlg::onAccept()
{
	m_sServerSocket.Accept(m_sClientSocket);  //用连接套接字与客户端建立连接
}


void CMainDlg::onClose()
{
	m_sClientSocket.Close();
	m_sServerSocket.Close();   //Close()函数执行后会触发OnClose()函数
	CDialogEx::OnClose();
}


void CMainDlg::OnClickedButtonCn()
{
	// TODO:  在此添加连接按钮通知处理程序代码
	if (!m_bListening)
	{
		int opt = 1;
		m_sServerSocket.Create(m_nServerPort);//创建套接字，并进行主机名解析以及端口号绑定
		m_sServerSocket.SetSockOpt(SO_REUSEADDR, &opt, sizeof(opt));
		m_sServerSocket.Listen(); //开始监听
		m_bListening = 1;
		GetDlgItem(IDC_BUTTON_CN)->SetWindowText(_T("断开"));
	}
	else
	{
		onClose();
		m_bListening = 0;
		GetDlgItem(IDC_BUTTON_CN)->SetWindowText(_T("监听"));
	}
	
}


int CMainDlg::DoSend(CString strToSend)
{
	int nMsgLen;
	int nSentLen = 0;
	//int nSentLen;
	UpdateData(TRUE);
	if (!strToSend.IsEmpty())
	{
		nMsgLen = strToSend.GetLength()*sizeof(strToSend);
		nSentLen = m_sClientSocket.Send(strToSend, nMsgLen);
		if (nSentLen != SOCKET_ERROR){
			//发送成功
			//m_listSent.AddString(m_strToServer);
			//	setHScroll();
			UpdateData(FALSE);
		}
		else {
			AfxMessageBox(_T("服务器向客户端发送信息出现错误！"), MB_OK | MB_ICONSTOP);
		}
		strToSend.Empty();
		UpdateData(FALSE);
	}
	return nSentLen;
}


void CMainDlg::OnViewConsole()
{
	// TODO:  在此添加命令处理程序代码
	if (!::IsWindow(m_condlg.m_hWnd))
	{
		m_condlg.Create(IDD_CONSOLEDLG, this);
	}
	m_condlg.ShowWindow(m_condlg.IsWindowVisible()?SW_HIDE:SW_SHOW);
}


void CMainDlg::OnDropdownButtonNewser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMBCDROPDOWN pDropDown = reinterpret_cast<LPNMBCDROPDOWN>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	
	*pResult = 0;
}


void CMainDlg::OnNewServer()
{
	// TODO:  在此添加命令处理程序代码
	this->OnBnClickedButtonNewser();
}


void CMainDlg::OnNewExit()
{
	// TODO:  在此添加命令处理程序代码
	this->OnClose();
}


void CServerDlg::SetAddr(int addr, int addrlen)
{
	stpar->slav_pot_addr = addr;
	stpar->slav_pot_addrlen = addrlen;
}

void CServerDlg::OnApplicationLook(UINT id)
{
	CWaitCursor wait;
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int nTimerID = (int)nIDEvent;
	switch (nTimerID)
	{
	case TM_INTERFRAME_TIMEOUT:
		KillTimer(nTimerID);
		AfxMessageBox(_T("error:帧内超时！"), MB_OKCANCEL);
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CServerDlg::OnBnClickedButtonDatasnd()
{
	// TODO:  在此添加控件通知处理程序代码
	/*
	if (!stpar->DLReady)
	{
		AfxMessageBox(_T("warning：请先建立连接！"), MB_OKCANCEL);
		return;
	}
	//开始传输帧
	//SetTimer(IDM_TIMER_FRAME);
	stpar->transformReady = 1;
	pOutFrame = new hdlc[m_nWndSize];
	for (int i = 0; i < m_nWndSize; i++)
	{
		hdlc *frame = &pOutFrame[i];
		frame->dst_addr = stpar->main_pot_addr;
		frame->dst_addrlen = stpar->main_pot_addrlen;
		frame->src_addr = stpar->slav_pot_addr;
		frame->src_addrlen = stpar->slav_pot_addrlen;
		frame->end_flag = 0x7E;
		frame->start_flag = 0x7E;
		frame->f_format.frame_seg = (i == m_nWndSize-1) ? 0 : 1;
		frame->f_format.frame_type = 0xA;
		frame->pollfin = 1;
		frame->ns = i;
		frame->nr = 0;
	}
	*/
}


afx_msg LRESULT CServerDlg::OnAppLayerReady(WPARAM wParam, LPARAM lParam)
{
	u_char strType[255];
	hdlc *outframe = new hdlc();
	//if (fsm->fsmtype == FSMTypeIResponse && fsm->curstate == STATE_SEND_DATA)
	//{
	SendFrame("I", pOutFrame + stpar->m);
	stpar->m++;
	if (m_nextSendFrame + 1 >= m_totalFrameSend)
	{
		stpar->SendComplete = 1;
	}
	Inc(m_nextSendFrame);
	delete outframe;
	outframe = NULL;
	return 0;
}

int CServerDlg::SendFrame(const char *frameKind, hdlc *outframe)
{
	_TCHAR  sendStr[1024];
	//u_char sendData[255];
	convFrameHex(stpar, outframe, write_str);
	convFrameStr(stpar, outframe, sendStr);
	CString strToServer = sendStr;
	writetxt((FILE*)wc, write_str, outframe->f_format.frame_sublen);
	int nSentLen = ToPhysicLayer(strToServer);
	if (nSentLen != SOCKET_ERROR)
	{
		//发送成功
		CString outstr;
		outstr.Format(_T("发送%5s帧"), CString(frameKind));
		m_liststateinfo.AddString(outstr);
		m_liststateinfo.AddString(strToServer);
		int iCount = m_liststateinfo.GetCount();
		if (iCount > 0)
			m_liststateinfo.SetCurSel(iCount - 1);
	}

	char buf[32];
	_strtime(buf);
	CString strErrmode;
	//if (frame == NO_ERR)
	strErrmode = "normal";
	//else if (pFrame->hdr.err == CKSUM_ERR)
	//	strErrmode = "chkerr";
	//else
	//	strErrmode = "lost";
	CString strMsg;
	strMsg.Format(_T("%s %5s %8s %7s %3d,%3d"), CString(buf), CString(GetTypestr(*outframe)), _T("sent"), strErrmode, outframe->ns, outframe->nr);
	m_listLog.AddString(strMsg);
	int iCount = m_listLog.GetCount();
	if (iCount > 0)
		m_listLog.SetCurSel(iCount - 1);
	SetTimer(TM_IDLETIMEOUT, ((CMainDlg*)AfxGetMainWnd())->m_nIdleTimeout, NULL);
	return nSentLen;
}

void CServerDlg::FromAppLayer(u_char *p, u_int *infolen, u_int nextSendFrame)
{
	//从应用层获取数据
	u_int *databuf = (u_int*)p;
	databuf[0] = nextSendFrame;
	*infolen = sizeof(nextSendFrame);
}

int CServerDlg::ToPhysicLayer(CString strToServer)
{
	int nSentLen = ((CMainDlg*)AfxGetMainWnd())->DoSend(strToServer);
	return nSentLen;
}

afx_msg LRESULT CServerDlg::OnDatalinkReady(WPARAM wParam, LPARAM lParam)
{
	//********待定不知道干吗用的
	//HdlcTcb *fsm = stpar->fsmstack;
	//u_char strType[255];
	//hdlc *outframe = new hdlc();
	//if (fsm->fsmtype == FSMTypeIResponse && fsm->curstate == STATE_SEND_DATA)
	//{
	//	//GETHANDLER(fsm)(stpar, NULL, outframe);
	//	GetTypeStr(outframe->type,strType);
	//	SendFrame(strType,outframe);
	//}
	//delete outframe;
	//outframe = NULL;
	return 0;
}


afx_msg LRESULT CServerDlg::OnResendWnd(WPARAM wParam, LPARAM lParam)
{
	//重发当前窗口
	stpar->m = 0;
	m_nextSendFrame -= stpar->sendwindowsize;
	PostMessage(IDM_SENDDATA);
	return 0;
}

void CServerDlg::GenFrameBuf()
{
	//准备发送帧缓冲区
	if (pOutFrame == NULL)
	{
		pOutFrame = new hdlc[];
		int nWndSize = stpar->sendwindowsize;
		for (int i = 0; i < nWndSize; i++)
		{
			hdlc *frame = &pOutFrame[i];
			frame->dst_addr = stpar->main_pot_addr;
			frame->dst_addrlen = stpar->main_pot_addrlen;
			frame->src_addr = stpar->slav_pot_addr;
			frame->src_addrlen = stpar->slav_pot_addrlen;
			frame->end_flag = 0x7E;
			frame->start_flag = 0x7E;
			frame->f_format.frame_seg = (i == nWndSize - 1) ? 0 : 1;
			frame->f_format.frame_type = 0xA;
			frame->pollfin = 1;
			frame->ns = i;
			frame->nr = 0;
		}
	}
	u_int segprpairs[3][2] = {
		1, 0, //I_Fragment
		1, 1, //I_WndFinial
		0, 1, //I_Compelete
	};
	u_char *databuf = new u_char[2048];
	u_int infolen;
	u_int nextSendFrame = m_nextSendFrame;
	for (u_int i = 0; i < stpar->sendwindowsize && nextSendFrame < m_totalFrameSend; i++, nextSendFrame++)
	{
		FromAppLayer(databuf, &infolen, nextSendFrame);
		if (nextSendFrame == m_totalFrameSend)
		{
			makeI(stpar, NULL, pOutFrame + i, databuf, infolen, segprpairs[2][0], segprpairs[2][1]);
			continue;
		}
		if (i == stpar->sendwindowsize)
		{
			makeI(stpar, NULL, pOutFrame + i, databuf, infolen, segprpairs[1][0], segprpairs[1][1]);
			continue;
		}
		makeI(stpar, NULL, pOutFrame + i, databuf, infolen, segprpairs[0][0], segprpairs[0][1]);
	}
	delete databuf;
	databuf = NULL;
}


afx_msg LRESULT CServerDlg::OnSendData(WPARAM wParam, LPARAM lParam)
{
	SetTimer(TM_SENDDATA, ((CMainDlg*)AfxGetMainWnd())->m_delaySendData, NULL);
	return 0;
}


void CMainDlg::OnChangeEdit1()
{
	UpdateData(true);
}
void CMainDlg::OnChangeEdit4()
{
	UpdateData(TRUE);
}
void CMainDlg::OnChangeEdit5()
{
	UpdateData(TRUE);
}
void CMainDlg::OnChangeEdit6()
{
	UpdateData(TRUE);
}


void CMainDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (pNMUpDown->iDelta < 0)
	{
		m_nServerPort += 1;
		m_nServerPort >= 0 ? m_nServerPort : 0;
	}
	else if (pNMUpDown->iDelta > 0)
	{
		m_nServerPort -= 1;
		m_nServerPort <= 65535 ? m_nServerPort : 65535;
	}
	UpdateData(FALSE);
	*pResult = 0;
}


void CMainDlg::OnNewClient()
{
	// TODO:  在此添加命令处理程序代码
	LPTSTR szCmdName = _T(".\\Client.exe");
	CString szCmdNmae2;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("可执行文件(*.exe)|*.exe"),this);
	if (fileDlg.DoModal() == IDOK)
	{
		szCmdNmae2 = fileDlg.GetPathName();
	}
	else
	{
		return;
	}
	STARTUPINFO startinf;
	memset(&startinf, 0, sizeof(STARTUPINFO));
	memset(&m_procinfo, 0, sizeof(PROCESS_INFORMATION));
	startinf.cb = sizeof(startinf);
	
	BOOL bRes = CreateProcess(NULL, szCmdNmae2.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startinf, &m_procinfo);

	if (!bRes)
	{
		AfxMessageBox(_T("未找到生成器的可执行文件"), MB_OKCANCEL);
		return;
	}
}


void CMainDlg::OnPluginLoadModule()
{
	// TODO:  在此添加命令处理程序代码
	if (m_hMod != NULL)
	{
		FreeLibrary(m_hMod);
		m_hMod = NULL;
	}
	CFileDialog fileDlg(TRUE, _T("dll"), NULL, OFN_HIDEREADONLY, _T("插件模块(*.dll)|*.dll"), this);
	CString dllname;
	if (fileDlg.DoModal() == IDOK)
	{
		dllname = fileDlg.GetPathName();
		m_hMod = LoadLibrary(dllname.GetBuffer());
	}
	::CloseWindow(fileDlg.m_hWnd);
	this->SetFocus();
}


void CMainDlg::OnPluginViewModuleInf()
{
	// TODO:  在此添加命令处理程序代码
	CString szModState;
	szModState.Preallocate(200);
	if (m_hMod == NULL)
	{
		szModState = _T("没有加载模块！");
		AfxMessageBox(szModState, MB_OKCANCEL);
		return;
	}
	QueryModuleFun qryfun = (QueryModuleFun)GetProcAddress(m_hMod, "QueryModule");
	if (qryfun != NULL)
	{
		qryfun(szModState.GetBuffer(), 200);
		CString outStr;
		outStr.Format(_T("已载入：%s"), szModState);
		AfxMessageBox(outStr, MB_OKCANCEL);
	}
	else
	{
		szModState = _T("载入非法模块！");
		AfxMessageBox(szModState, MB_OKCANCEL);
	}
}


void CMainDlg::OnMacroEditMacro()
{
	// TODO:  在此添加命令处理程序代码
	// TODO:  在此添加命令处理程序代码
	LPTSTR szCmdName = _T(".\\Client.exe");
	CString szCmdNmae2;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("可执行文件(*.exe)|*.exe"), this);
	if (fileDlg.DoModal() == IDOK)
	{
		szCmdNmae2 = fileDlg.GetPathName();
	}
	else
	{
		return;
	}
	STARTUPINFO startinf;
	memset(&startinf, 0, sizeof(STARTUPINFO));
	memset(&m_procinfo, 0, sizeof(PROCESS_INFORMATION));
	startinf.cb = sizeof(startinf);

	BOOL bRes = CreateProcess(NULL, szCmdNmae2.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startinf, &m_procinfo);

	if (!bRes)
	{
		AfxMessageBox(_T("未找到宏编辑器"), MB_OKCANCEL);
		return;
	}
}
