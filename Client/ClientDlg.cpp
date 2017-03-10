
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT fileLen=0; //文件长度，初始化为0
FILE* fq;
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
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_SIZE()
END_MESSAGE_MAP()


// CClientDlg 对话框




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
	, m_strSendData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strServerName = _T("");
	m_nServerPort = 0;
	m_editfile = _T("");
	//m_listcontent = _T("");
	m_listToServer = _T("");
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_strServerName);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);
	DDV_MinMaxInt(pDX, m_nServerPort, 1024, 49151);
	//	DDX_Text(pDX, IDC_EDIT_TOSERVER, m_strToServer);
	DDX_Control(pDX, IDC_LIST_RECEIVED, m_listReceived);
	DDX_Control(pDX, IDC_LIST_SENT, m_listSent);
	DDX_Control(pDX, IDC_LIST_CONTENT, m_listcontent);
	DDX_Text(pDX, IDC_EDIT_FILE, m_editfile);
	//  DDX_Text(pDX, IDC_LIST_CONTENT, m_listcontent);
	//  DDX_Control(pDX, IDC_LIST_CONTENT, m_listcontent);
	DDX_LBString(pDX, IDC_LIST_CONTENT, m_listToServer);
	DDX_Text(pDX, IDC_EDIT1, m_strSendData);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	//ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CClientDlg::OnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CClientDlg::OnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CClientDlg::OnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, &CClientDlg::OnBnClickedButtonView)//
	ON_EN_CHANGE(IDC_EDIT_FILE, &CClientDlg::OnEnChangeEditFile)
	ON_LBN_SELCHANGE(IDC_LIST_RECEIVED, &CClientDlg::OnSelchangeListReceived)
	ON_LBN_SELCHANGE(IDC_LIST_SENT, &CClientDlg::OnSelchangeListSent)
	//ON_EN_CHANGE(IDC_EDIT_CONTENT, &CClientDlg::OnChangeEditContent)
//	ON_COMMAND(IDC_BUTTON3, &CClientDlg::OnButton3)
    ON_LBN_SELCHANGE(IDC_LIST_CONTENT, &CClientDlg::OnSelchangeListContent)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTONMAKE, &CClientDlg::OnBnClickedButtonmake)
	ON_MESSAGE(WM_AUTOSEND, &CClientDlg::OnAutoSend)
END_MESSAGE_MAP()

 
// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
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
	m_strServerName="localhost";    //客户机要连接的服务器主机名，设定本机位服务器
	m_nServerPort=1024;             //服务器端口
	UpdateData(FALSE);              //拷贝变量值(即localhost和1024)到相应的控件显示
	m_sClientSocket.setParentDlg(this);  //将指向CClientDlg类的指针this传到CClientSocket类中，此时this=m_pDlg
	GetDlgItem(IDC_LIST_CONTENT)->EnableWindow(TRUE);   //禁用发送消息文本框
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);   //禁用断开按钮
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);       //禁用发送按钮
	SendDlgItemMessage(IDC_LIST_SENT,LB_SETHORIZONTALEXTENT,(WPARAM)1000,0);
	SendDlgItemMessage(IDC_LIST_RECEIVED,LB_SETHORIZONTALEXTENT,(WPARAM)1000,0);
	SendDlgItemMessage(IDC_LIST_CONTENT,LB_SETHORIZONTALEXTENT,(WPARAM)1000,0);

	m_makedlg.Create(IDD_HDLCFRAMEGEN_DIALOG, this);
	m_makedlg.ShowWindow(SW_HIDE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//按动浏览按钮
void CClientDlg::OnBnClickedButtonView()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO broInfo={0};  //定义一个BROWSEINFO对象
	TCHAR szDisName[MAX_PATH]={0};  //定义一个字符串用来接收文件名

	broInfo.hwndOwner=this->m_hWnd; //制定父窗口，在对话框显示期间，父窗口将被禁用
	broInfo.pidlRoot=NULL;   //以桌面为根
	broInfo.pszDisplayName=szDisName;   //用来保存目录字符串的内存地址
	broInfo.lpszTitle=_T("选择要读取的文件");  //对话框标题
	//包含状态文本、显示目录和文件、只返回文件系统目录、返回系统一个节点、
	broInfo.ulFlags=BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES|BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS ;  
	broInfo.lpfn=NULL;    //应用程序定义的浏览对话框回调函数的地址  
	broInfo.lParam=NULL;   //对话框传递给回调函数的一个参数指针
	broInfo.iImage=IDR_MAINFRAME;   //与选中目录相关的图像。该图像将被指定为系统图像列表中的索引值

	LPITEMIDLIST pIDList=SHBrowseForFolder(&broInfo);  //创建一个结构指针存储路径
	if(pIDList!=NULL)
	{
		//如果用户单击“确定”
		memset(szDisName,0,sizeof(szDisName));
		SHGetPathFromIDList(pIDList,szDisName);   //把项目标识列表转化成字符串
		m_editfile=szDisName;   //将TCHAR类型的字符串转换为CString类型的字符串
		
		//读取文件内容并显示在编辑框中
		CStdioFile file;//读取一行
		file.Open (m_editfile,CFile::modeRead | CFile::typeBinary);
		CString strRead;
		char p_str[255];
		//file.ReadString(strRead);
		while(file.ReadString(strRead))
		{
			strcpy(p_str, (char*)strRead.GetBuffer());
			strRead = p_str;
			m_listcontent.AddString (strRead);
		}
		UpdateData(FALSE);       //将路径字符串和文件内容分别显示在编辑框内
	//	delete pBuf;
		file.Close();
	}
}

//按动连接按钮
void CClientDlg::OnClickedButtonConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);     //刷新控件的值到对应的变量(即服务器名和端口号)
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);   //禁用连接按钮
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(FALSE);  //禁用服务器名文本框
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(FALSE);   //禁用端口号文本框
	m_sClientSocket.Create();  //建立Socket的WSAStartup过程和bind过程被简化成为Create过程
	m_sClientSocket.Connect(m_strServerName,m_nServerPort);//连接成功或失败后会触发 OnConnect()函数
}

void CClientDlg::OnClickedButtonDisconnect()
{
	// TODO: 在此添加控件通知处理程序代码
	onClose();
}


void CClientDlg::OnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	int nMsgLen;
	int nSentLen;
	UpdateData(TRUE);
	if(!m_listToServer.IsEmpty()||!m_strSendData.IsEmpty())
	{
		//nMsgLen=m_listToServer.GetLength()*sizeof(m_listToServer);
		m_listToServer = m_strSendData;
		int len = WideCharToMultiByte(CP_ACP, 0, m_listToServer, -1, NULL, 0, NULL, NULL);
		char *ptxtTemp = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, m_listToServer, -1, ptxtTemp, len, NULL, NULL);
		nMsgLen = len;
		nSentLen = m_sClientSocket.Send(ptxtTemp, nMsgLen);
		delete ptxtTemp;
		ptxtTemp = NULL;
		if(nSentLen!=SOCKET_ERROR){
		     //发送成功
			m_listSent.AddString(m_listToServer);
		    UpdateData(FALSE);    //拷贝变量值(即m_strToServer)到相应的控件显示
		} 
		else{
		       AfxMessageBox(LPCTSTR("客户机向服务器发送信息出现错误！"),MB_OK|MB_ICONSTOP);
		}
		m_listToServer.Empty();
	    UpdateData(FALSE);
	}
}

//客户机已连接到服务器上
void CClientDlg::onConnect(void)
{
	GetDlgItem(IDC_LIST_CONTENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
}



void CClientDlg::onReceive(void)
{
	TCHAR buff[2048];
	int nBufferSize=2048;
	int nReceiveLen;
	CString strReceive;
	memset(buff,0,2048);
	
	nReceiveLen=m_sClientSocket.Receive(buff,nBufferSize);//Receive()函数调用后会触发OnReceive()函数
	if(nReceiveLen!=SOCKET_ERROR)
	{
	    //接受成功
	    CString szTemp(buff);   //将buff由char转换为CString类型
	    strReceive=szTemp;
		m_listReceived.AddString(strReceive);  //AddString参数要求为LPCTSTR(CString)类型
	    UpdateData(FALSE);   //拷贝变量值(即strReceive)到相应的控件显示
	}
	else {
	        AfxMessageBox(LPCTSTR("客户机向服务器接受信息出现错误！"),MB_OK|MB_ICONSTOP);
	}

}


void CClientDlg::onClose(void)
{
	m_sClientSocket.Close();      //Close()函数无论执行成功或失败都会触发OnClose()函数
	//GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	
	//清除两个列表框信息
	while(m_listSent.GetCount()!=0)
		m_listSent.DeleteString(0);
	while(m_listReceived.GetCount()!=0)
		m_listReceived.DeleteString(0);
	//更新按钮和文本框状态
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
}


void CClientDlg::OnEnChangeEditFile()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CClientDlg::OnSelchangeListReceived()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CClientDlg::OnSelchangeListSent()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CClientDlg::OnSelchangeListContent()
{
	// TODO: 在此添加控件通知处理程序代码
	int i=m_listcontent.GetCurSel();
	m_listcontent.GetText(i,m_listToServer);//获得所选中一行的字符串
	//		int nSentLen=m_sClientSocket.Send(m_listToServer,150);
}


//void CAboutDlg::OnSize(UINT nType, int cx, int cy)
//{
//	CDialogEx::OnSize(nType, cx, cy);
//
//	// TODO:  在此处添加消息处理程序代码
//}


void CClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
}


void CClientDlg::OnBnClickedButtonmake()
{
	// TODO:  在此添加控件通知处理程序代码
	m_makedlg.ShowWindow(m_makedlg.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}


afx_msg LRESULT CClientDlg::OnAutoSend(WPARAM wParam, LPARAM lParam)
{
	int nLen, nState;
	char szTemp[255];
	nState = m_sClientSocket.Send(szTemp, nLen);
	return 0;
}
