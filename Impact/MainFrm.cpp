// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Impact.h"

#include "MainFrm.h"
#include "IAppendLog.h"
#include "ImpactDoc.h"
#include "ClientSocket.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CImpactView;
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_BUTTON_PROPERTY, &CMainFrame::OnButtonProperty)
	ON_WM_MOVE()
	ON_COMMAND(ID_COMBO_CONTYPE, &CMainFrame::OnComboContype)
	ON_COMMAND(ID_EDIT_REMOTE_ADDR, &CMainFrame::OnEditRemoteAddr)
	ON_COMMAND(ID_EDIT_REMOTE_PORT, &CMainFrame::OnEditRemotePort)
	ON_COMMAND(ID_EDIT_LOC_ADDR, &CMainFrame::OnEditLocAddr)
	ON_COMMAND(ID_BUTTON_START, &CMainFrame::OnButtonStart)
	ON_COMMAND(ID_BTN_CON, &CMainFrame::OnBtnCon)
	ON_COMMAND(ID_EDIT_LOC_PORT, &CMainFrame::OnEditLocPort)
	ON_COMMAND(ID_BUTTON_PLUGIN, &CMainFrame::OnButtonPlugin)
	ON_WM_TIMER()
	ON_COMMAND(ID_BUTTON_STOP, &CMainFrame::OnButtonStop)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
	: m_strRemoteAddr(_T("127.0.0.1"))
	, m_strRemotePort(_T("1024"))
	, m_strLocAddr(_T("127.0.0.1"))
	, m_strLocPort(_T("1024"))
	, m_nConType(0)
	, m_bConnected(FALSE)
{
	// TODO:  在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), _T("CONNECTIONSTATE"));
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);
	
	m_wndStatusBar.GetElement(0)->SetText(_T("就绪"));
	m_wndStatusBar.GetElement(1)->SetText(_T("发送:0 接收:0"));
	
	m_wndStatusBar.GetExElement(0)->SetText(_T("TCP连接:127.0.0.1:8001"));
	m_wndStatusBar.GetExElement(0)->SetTextAlwaysOnRight(TRUE);
	//m_wndStatusBar.GetExElement(1)->SetText(_T("接收:0 发送:0"));

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上): 
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndReportView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	//m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	//pTabbedBar = NULL;
	m_wndReportView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	CMFCRibbonComboBox *cmbox = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CONTYPE));
	cmbox->AddItem(_T("TCP客户端"));
	cmbox->AddItem(_T("TCP服务端"));
	cmbox->AddItem(_T("COM"));
	//初始化AvalonEdit
	//RECT rectangle, rect2;
	//GetClientRect(&rectangle);
	//ClientToScreen(&rectangle);
	//int width = rectangle.right - rectangle.left;
	//int height = rectangle.bottom - rectangle.top;
	//POINT point;
	//point.x = rectangle.left;
	//point.y = rectangle.top;
	//int result = ::MapWindowPoints(NULL, this->m_hWnd, &point, 1);
	//m_hCODE = GetHwnd(point.x, point.y, width, height);

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	m_sClientSocket.setParentDlg((ISocketHandler*)this);
	m_sServerSocket.setParentDlg((ISocketHandler*)this);
	CMFCRibbonComboBox *cmb = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CONTYPE));
	cmb->SelectItem(0);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建类视图
	//CString strClassView;
	//bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	//ASSERT(bNameValid);
	//if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("未能创建“类视图”窗口\n");
	//	return FALSE; // 未能创建
	//}

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	//创建报告窗口
	CString strReportView;
	bNameValid = strReportView.LoadString(IDS_CESHIBAOGAO);
	ASSERT(bNameValid);
	if (!m_wndReportView.Create(strReportView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_REPORT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“报告视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndReportView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CDockingManager::SetDockingMode(DT_SMART);
	m_wndRibbonBar.SetWindows7Look(FALSE);
	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
	return;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 强制关闭“打印预览”模式
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnButtonProperty()
{
	// TODO:  在此添加命令处理程序代码
	if (!m_wndProperties.IsPaneVisible())
		m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	else
		m_wndProperties.ShowPane(FALSE, FALSE, FALSE);

	Invalidate(TRUE);
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWndEx::OnMove(x, y);

	// TODO:  在此处添加消息处理程序代码

	//a->moveedit();
	//RECT rectangle, rect2;
	//CView *a = GetActiveView();
	//if (!a)
	//{
	//	return;
	//}
	//a->GetClientRect(&rectangle);
	//ClientToScreen(&rectangle);
	//int width = rectangle.right - rectangle.left;
	//int height = rectangle.bottom - rectangle.top;
	//POINT point;
	//point.x = rectangle.left;
	//point.y = rectangle.top;
	////int result = ::MapWindowPoints(NULL, this->m_hWnd, &point, 1);
	//::MoveWindow(m_hCODE, point.x, point.y, 200, 200, TRUE);
}


void CMainFrame::OnComboContype()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonComboBox *pCmb = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CONTYPE));
	int cursel = pCmb->GetCurSel();
	CString str = pCmb->GetItem(cursel);

	if (str == _T("TCP服务端"))
	{
		m_nConType = 1;
	}
	else if (str == _T("TCP客户端"))
	{
		m_nConType = 0;
	}
	else if (str == _T("COM"))
	{
		m_nConType = 2;
	}


}


void CMainFrame::OnEditRemoteAddr()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_REMOTE_ADDR));
	m_strRemoteAddr = c->GetEditText();
}


void CMainFrame::OnEditRemotePort()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_REMOTE_PORT));
	m_strRemotePort = c->GetEditText();
}


void CMainFrame::OnEditLocAddr()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_LOC_ADDR));
	m_strLocAddr = c->GetEditText();
}

void CMainFrame::OnEditLocPort()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_LOC_PORT));
	m_strLocPort = c->GetEditText();
}

void CMainFrame::OnButtonStart()
{
	// TODO:  在此添加命令处理程序代码
	//m_wndRibbonBar.LoadFromResource(IDR_RIBBON_COM);
	SetTimer(1, 100, NULL);
}


void CMainFrame::OnBtnCon()
{
	// TODO:  在此添加命令处理程序代码
	CMFCRibbonButton *c = DYNAMIC_DOWNCAST(CMFCRibbonButton, m_wndRibbonBar.FindByID(ID_BTN_CON));
	CMFCRibbonComboBox *b = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CONTYPE));
	CMFCRibbonEdit *b1 = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_REMOTE_ADDR));
	CMFCRibbonEdit *b2 = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_REMOTE_PORT));
	CMFCRibbonEdit *b3 = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_LOC_ADDR));
	CMFCRibbonEdit *b4 = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_LOC_PORT));

	BOOL bRes;
	int opt = 1;
	if (!m_bConnected)
	{
		switch (m_nConType)
		{
		case 0:
			//TCP客户端
			m_sClientSocket.Create();  //建立Socket的WSAStartup过程和bind过程被简化成为Create过程
			bRes = m_sClientSocket.Connect(m_strRemoteAddr, _ttoi(m_strRemotePort.GetBuffer()));//连接成功或失败后会触发 OnConnect()函数

			/*if (!bRes)
			{
				m_sClientSocket.Close();
				AfxMessageBox(_T("连接远程主机错误"), MB_OKCANCEL | MB_ICONEXCLAMATION);
				return;
			}*/
			break;
		case 1:
			//TCP主机
			opt = 1;
			bRes = m_sServerSocket.Create(_ttoi(m_strLocPort.GetBuffer()));//创建套接字，并进行主机名解析以及端口号绑定
			if (!bRes)
			{
				m_sServerSocket.Close();
				AfxMessageBox(_T("监听端口错误"), MB_OKCANCEL | MB_ICONEXCLAMATION);
				return;
			}
			m_sServerSocket.SetSockOpt(SO_REUSEADDR, &opt, sizeof(opt));
			m_sServerSocket.Listen(); //开始监听
			break; 
		case 2:
			//串口传输TODO
			break;
		}

		c->SetText(_T("断开"));
		c->SetImageIndex(5, TRUE);
		b->OnEnable(FALSE);
		b1->OnEnable(FALSE);
		b2->OnEnable(FALSE);
		b3->OnEnable(FALSE);
		b4->OnEnable(FALSE);
		m_bConnected = TRUE;
	}
	else
	{
		switch (m_nConType)
		{
		case 0:
			//TCP客户端
			m_sClientSocket.Close();
			break;
		case 1:
			//TCP主机
			m_sClientSocket.Close();
			m_sServerSocket.Close();
			break;
		case 2:
			//串口传输TODO
			break;
		}
		c->SetText(_T("连接"));
		c->SetImageIndex(4, TRUE);
		b->OnEnable(TRUE);
		b1->OnEnable(TRUE);
		b2->OnEnable(TRUE);
		b3->OnEnable(TRUE);
		b4->OnEnable(TRUE);
		m_bConnected = FALSE;
	}
}




void CMainFrame::OnButtonPlugin()
{
	// TODO:  在此添加命令处理程序代码
	//char buf[32];
	//_strtime(buf);
	//CString strMsg;
	//strMsg.Format(_T("%s %5s %8s %7s %3d,%3d"), CString(buf), CString(GetTypestr(*hdlc_p)), _T("receive"), _T("normal"), hdlc_p->ns, hdlc_p->nr);
	//m_listLog.AddString(strMsg);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CString *pstrMsg;
	char buf[32];
	_strtime_s(buf, 32);
	CString time1(buf);
	DWORD mscount = ::GetTickCount();
	mscount = mscount % 1000;
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	pstrMsg = new CString();
	pstrMsg->Format(_T(">[%04d年%02d月%02d日 %s.%03d]\n"), tm.wYear, tm.wMonth, tm.wDay, time1.GetBuffer(), mscount);
	//strMsg.Format(_T("[%s] %5s\n"), CString(buf), _T("df"));
	CView *a = (CView*)GetActiveView();
	::PostMessage(a->m_hWnd, WM_USER+1, (WPARAM)pstrMsg, NULL);

	_strtime_s(buf, 32);
	CString time2(buf);
	mscount = ::GetTickCount();
	mscount = mscount % 1000;
	pstrMsg = new CString();
	pstrMsg->Format(_T("<[%04d年%02d月%02d日 %s.%03d]\n"), tm.wYear, tm.wMonth, tm.wDay, time1.GetBuffer(), mscount);
	//strMsg.Format(_T("[%s] %5s\n"), CString(buf), _T("df"));
	a = (CView*)GetActiveView();
	::PostMessage(a->m_hWnd, WM_USER + 1, (WPARAM)pstrMsg, NULL);
	CFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnButtonStop()
{
	// TODO:  在此添加命令处理程序代码
	KillTimer(1);
}


void CMainFrame::onClose()
{
	m_sClientSocket.Close();
	m_sServerSocket.Close();   //Close()函数执行后会触发OnClose()函数
}


void CMainFrame::onAccept()
{
	m_sServerSocket.Accept(m_sClientSocket);  //用连接套接字与客户端建立连接
}
void CMainFrame::onConnect()
{
	AfxMessageBox(_T("df"), MB_OKCANCEL);
}

void CMainFrame::onReceive()
{
	u_char buff[2048];
	u_char *tempbuff = (u_char*)malloc(2048);
	int nBufferSize = 2048;
	int nReceivedLen;
	CString strReceived;

	//m_liststateinfo.AddString(_T("开始接收HDLC帧！"));
	nReceivedLen = m_sClientSocket.Receive(buff, nBufferSize);  //Receive()函数真正执行接收功能
	//返回接收到的字节数     (如果连接被关闭了，返回0；否则返回SOCKET_ERROR)
	if (nReceivedLen == SOCKET_ERROR)  //接收成功
	{
		AfxMessageBox(_T("error:接收失败！"), MB_OK | MB_ICONSTOP);
		return;
	}
	buff[nReceivedLen] = _T('\0');
	char 时间[32];
	_strtime_s(时间, 32);
	CString time1(时间);
	DWORD mscount = ::GetTickCount();
	mscount = mscount % 1000;
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString *接收日志行 = new CString();
	CString 接收数据(buff);
	接收日志行->Format(_T("<[%04d年%02d月%02d日 %s.%03d] %s\n"), tm.wYear, tm.wMonth, tm.wDay, time1.GetBuffer(), mscount, 接收数据.GetBuffer());
	CView *a = (CView*)GetActiveView();
	::PostMessage(a->m_hWnd, WM_USER + 1, (WPARAM)接收日志行, NULL);
}
