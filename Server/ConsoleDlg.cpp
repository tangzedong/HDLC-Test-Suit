// ConsoleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ConsoleDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "ServerDlg.h"
// CConsoleDlg 对话框

IMPLEMENT_DYNAMIC(CConsoleDlg, CDialogEx)

CConsoleDlg::CConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConsoleDlg::IDD, pParent)
{

}

CConsoleDlg::~CConsoleDlg()
{
}

void CConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CON, m_listconsole);
}


BEGIN_MESSAGE_MAP(CConsoleDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST_CON, &CConsoleDlg::OnDblclkConsole)
END_MESSAGE_MAP()


// CConsoleDlg 消息处理程序


BOOL CConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_toolbar.CreateEx(this);
	m_toolbar.LoadToolBar(IDR_TOOLBAR_CON);

	CRect rect;
	GetClientRect(&rect);
	m_toolbar.MoveWindow(2, 2, rect.Width(), 25);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CConsoleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST_CON);
	if (pList) 
		pList->MoveWindow(0, 25, rect.Width(), rect.Height() - 26);
}


void CConsoleDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	ShowWindow(IsWindowVisible() ? SW_HIDE : SW_SHOW);
	//CDialogEx::OnClose();
}


void CConsoleDlg::OnDblclkConsole()
{
	// TODO:  在此添加控件通知处理程序代码
	int sel = m_listconsole.GetCurSel();
	if (sel >= 0)
	{
		CAboutDlg *pSer = ((CAboutDlg*)AfxGetMainWnd());
		if (pSer->serverlist[sel])
			pSer->serverlist[sel]->ShowWindow(pSer->serverlist[sel]->IsWindowVisible() ? SW_HIDE : SW_SHOW);
	}
}
