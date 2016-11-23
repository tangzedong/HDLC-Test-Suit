// ConsoleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ConsoleDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "ServerDlg.h"
// CConsoleDlg �Ի���

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


// CConsoleDlg ��Ϣ�������


BOOL CConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_toolbar.CreateEx(this);
	m_toolbar.LoadToolBar(IDR_TOOLBAR_CON);

	CRect rect;
	GetClientRect(&rect);
	m_toolbar.MoveWindow(2, 2, rect.Width(), 25);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CConsoleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	CRect rect;
	GetClientRect(&rect);
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST_CON);
	if (pList) 
		pList->MoveWindow(0, 25, rect.Width(), rect.Height() - 26);
}


void CConsoleDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ShowWindow(IsWindowVisible() ? SW_HIDE : SW_SHOW);
	//CDialogEx::OnClose();
}


void CConsoleDlg::OnDblclkConsole()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int sel = m_listconsole.GetCurSel();
	if (sel >= 0)
	{
		CAboutDlg *pSer = ((CAboutDlg*)AfxGetMainWnd());
		if (pSer->serverlist[sel])
			pSer->serverlist[sel]->ShowWindow(pSer->serverlist[sel]->IsWindowVisible() ? SW_HIDE : SW_SHOW);
	}
}
