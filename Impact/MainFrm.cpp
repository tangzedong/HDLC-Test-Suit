// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// MainFrm.cpp : CMainFrame ���ʵ��
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

// CMainFrame ����/����

CMainFrame::CMainFrame()
	: m_strRemoteAddr(_T("127.0.0.1"))
	, m_strRemotePort(_T("1024"))
	, m_strLocAddr(_T("127.0.0.1"))
	, m_strLocPort(_T("1024"))
	, m_nConType(0)
	, m_bConnected(FALSE)
{
	// TODO:  �ڴ���ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
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
	
	m_wndStatusBar.GetElement(0)->SetText(_T("����"));
	m_wndStatusBar.GetElement(1)->SetText(_T("����:0 ����:0"));
	
	m_wndStatusBar.GetExElement(0)->SetText(_T("TCP����:127.0.0.1:8001"));
	m_wndStatusBar.GetExElement(0)->SetTextAlwaysOnRight(TRUE);
	//m_wndStatusBar.GetExElement(1)->SetText(_T("����:0 ����:0"));

	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ���ز˵���ͼ��(�����κα�׼��������): 
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// ����ͣ������
	if (!CreateDockingWindows())
	{
		TRACE0("δ�ܴ���ͣ������\n");
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
	cmbox->AddItem(_T("TCP�ͻ���"));
	cmbox->AddItem(_T("TCP�����"));
	cmbox->AddItem(_T("COM"));
	//��ʼ��AvalonEdit
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

	// ���ڳ־�ֵ�����Ӿ�����������ʽ
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
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// ��������ͼ
	//CString strClassView;
	//bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	//ASSERT(bNameValid);
	//if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("δ�ܴ���������ͼ������\n");
	//	return FALSE; // δ�ܴ���
	//}

	// �����ļ���ͼ
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("δ�ܴ������ļ���ͼ������\n");
		return FALSE; // δ�ܴ���
	}

	//�������洰��
	CString strReportView;
	bNameValid = strReportView.LoadString(IDS_CESHIBAOGAO);
	ASSERT(bNameValid);
	if (!m_wndReportView.Create(strReportView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_REPORT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("δ�ܴ�����������ͼ������\n");
		return FALSE; // δ�ܴ���
	}

	// �����������
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("δ�ܴ����������\n");
		return FALSE; // δ�ܴ���
	}

	// �������Դ���
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("δ�ܴ��������ԡ�����\n");
		return FALSE; // δ�ܴ���
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

// CMainFrame ���

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


// CMainFrame ��Ϣ�������

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
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // ǿ�ƹرա���ӡԤ����ģʽ
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
	// TODO:  �ڴ���������������
	if (!m_wndProperties.IsPaneVisible())
		m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	else
		m_wndProperties.ShowPane(FALSE, FALSE, FALSE);

	Invalidate(TRUE);
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWndEx::OnMove(x, y);

	// TODO:  �ڴ˴������Ϣ����������

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
	// TODO:  �ڴ���������������
	CMFCRibbonComboBox *pCmb = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CONTYPE));
	int cursel = pCmb->GetCurSel();
	CString str = pCmb->GetItem(cursel);

	if (str == _T("TCP�����"))
	{
		m_nConType = 1;
	}
	else if (str == _T("TCP�ͻ���"))
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
	// TODO:  �ڴ���������������
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_REMOTE_ADDR));
	m_strRemoteAddr = c->GetEditText();
}


void CMainFrame::OnEditRemotePort()
{
	// TODO:  �ڴ���������������
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_REMOTE_PORT));
	m_strRemotePort = c->GetEditText();
}


void CMainFrame::OnEditLocAddr()
{
	// TODO:  �ڴ���������������
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_LOC_ADDR));
	m_strLocAddr = c->GetEditText();
}

void CMainFrame::OnEditLocPort()
{
	// TODO:  �ڴ���������������
	CMFCRibbonEdit *c = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_LOC_PORT));
	m_strLocPort = c->GetEditText();
}

void CMainFrame::OnButtonStart()
{
	// TODO:  �ڴ���������������
	//m_wndRibbonBar.LoadFromResource(IDR_RIBBON_COM);
	SetTimer(1, 100, NULL);
}


void CMainFrame::OnBtnCon()
{
	// TODO:  �ڴ���������������
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
			//TCP�ͻ���
			m_sClientSocket.Create();  //����Socket��WSAStartup���̺�bind���̱��򻯳�ΪCreate����
			bRes = m_sClientSocket.Connect(m_strRemoteAddr, _ttoi(m_strRemotePort.GetBuffer()));//���ӳɹ���ʧ�ܺ�ᴥ�� OnConnect()����

			/*if (!bRes)
			{
				m_sClientSocket.Close();
				AfxMessageBox(_T("����Զ����������"), MB_OKCANCEL | MB_ICONEXCLAMATION);
				return;
			}*/
			break;
		case 1:
			//TCP����
			opt = 1;
			bRes = m_sServerSocket.Create(_ttoi(m_strLocPort.GetBuffer()));//�����׽��֣������������������Լ��˿ںŰ�
			if (!bRes)
			{
				m_sServerSocket.Close();
				AfxMessageBox(_T("�����˿ڴ���"), MB_OKCANCEL | MB_ICONEXCLAMATION);
				return;
			}
			m_sServerSocket.SetSockOpt(SO_REUSEADDR, &opt, sizeof(opt));
			m_sServerSocket.Listen(); //��ʼ����
			break; 
		case 2:
			//���ڴ���TODO
			break;
		}

		c->SetText(_T("�Ͽ�"));
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
			//TCP�ͻ���
			m_sClientSocket.Close();
			break;
		case 1:
			//TCP����
			m_sClientSocket.Close();
			m_sServerSocket.Close();
			break;
		case 2:
			//���ڴ���TODO
			break;
		}
		c->SetText(_T("����"));
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
	// TODO:  �ڴ���������������
	//char buf[32];
	//_strtime(buf);
	//CString strMsg;
	//strMsg.Format(_T("%s %5s %8s %7s %3d,%3d"), CString(buf), CString(GetTypestr(*hdlc_p)), _T("receive"), _T("normal"), hdlc_p->ns, hdlc_p->nr);
	//m_listLog.AddString(strMsg);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString *pstrMsg;
	char buf[32];
	_strtime_s(buf, 32);
	CString time1(buf);
	DWORD mscount = ::GetTickCount();
	mscount = mscount % 1000;
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	pstrMsg = new CString();
	pstrMsg->Format(_T(">[%04d��%02d��%02d�� %s.%03d]\n"), tm.wYear, tm.wMonth, tm.wDay, time1.GetBuffer(), mscount);
	//strMsg.Format(_T("[%s] %5s\n"), CString(buf), _T("df"));
	CView *a = (CView*)GetActiveView();
	::PostMessage(a->m_hWnd, WM_USER+1, (WPARAM)pstrMsg, NULL);

	_strtime_s(buf, 32);
	CString time2(buf);
	mscount = ::GetTickCount();
	mscount = mscount % 1000;
	pstrMsg = new CString();
	pstrMsg->Format(_T("<[%04d��%02d��%02d�� %s.%03d]\n"), tm.wYear, tm.wMonth, tm.wDay, time1.GetBuffer(), mscount);
	//strMsg.Format(_T("[%s] %5s\n"), CString(buf), _T("df"));
	a = (CView*)GetActiveView();
	::PostMessage(a->m_hWnd, WM_USER + 1, (WPARAM)pstrMsg, NULL);
	CFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnButtonStop()
{
	// TODO:  �ڴ���������������
	KillTimer(1);
}


void CMainFrame::onClose()
{
	m_sClientSocket.Close();
	m_sServerSocket.Close();   //Close()����ִ�к�ᴥ��OnClose()����
}


void CMainFrame::onAccept()
{
	m_sServerSocket.Accept(m_sClientSocket);  //�������׽�����ͻ��˽�������
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

	//m_liststateinfo.AddString(_T("��ʼ����HDLC֡��"));
	nReceivedLen = m_sClientSocket.Receive(buff, nBufferSize);  //Receive()��������ִ�н��չ���
	//���ؽ��յ����ֽ���     (������ӱ��ر��ˣ�����0�����򷵻�SOCKET_ERROR)
	if (nReceivedLen == SOCKET_ERROR)  //���ճɹ�
	{
		AfxMessageBox(_T("error:����ʧ�ܣ�"), MB_OK | MB_ICONSTOP);
		return;
	}
	buff[nReceivedLen] = _T('\0');
	char ʱ��[32];
	_strtime_s(ʱ��, 32);
	CString time1(ʱ��);
	DWORD mscount = ::GetTickCount();
	mscount = mscount % 1000;
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString *������־�� = new CString();
	CString ��������(buff);
	������־��->Format(_T("<[%04d��%02d��%02d�� %s.%03d] %s\n"), tm.wYear, tm.wMonth, tm.wDay, time1.GetBuffer(), mscount, ��������.GetBuffer());
	CView *a = (CView*)GetActiveView();
	::PostMessage(a->m_hWnd, WM_USER + 1, (WPARAM)������־��, NULL);
}
