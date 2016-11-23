// PropertyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "PropertyDlg.h"
#include "afxdialogex.h"


// CPropertyDlg 对话框

IMPLEMENT_DYNAMIC(CPropertyDlg, CDialogEx)

CPropertyDlg::CPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPropertyDlg::IDD, pParent)
{

}

CPropertyDlg::~CPropertyDlg()
{
}

void CPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyDlg, CDialogEx)
END_MESSAGE_MAP()


// CPropertyDlg 消息处理程序


BOOL CPropertyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetClientRect(&rect);
	//rect.SetRectEmpty();
	m_propertysheet.Create(WS_CHILD | WS_VISIBLE, rect, this, 1);

	CMFCPropertyGridProperty *pA = new CMFCPropertyGridProperty(_T("地址"), (_variant_t)1024, _T("地址"));
	m_propertysheet.AddProperty(pA);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
