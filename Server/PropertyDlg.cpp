// PropertyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "PropertyDlg.h"
#include "afxdialogex.h"


// CPropertyDlg �Ի���

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


// CPropertyDlg ��Ϣ�������


BOOL CPropertyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CRect rect;
	GetClientRect(&rect);
	//rect.SetRectEmpty();
	m_propertysheet.Create(WS_CHILD | WS_VISIBLE, rect, this, 1);

	CMFCPropertyGridProperty *pA = new CMFCPropertyGridProperty(_T("��ַ"), (_variant_t)1024, _T("��ַ"));
	m_propertysheet.AddProperty(pA);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
