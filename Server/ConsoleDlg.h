#pragma once
#include "afxwin.h"


// CConsoleDlg �Ի���

class CConsoleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConsoleDlg)

public:
	CConsoleDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConsoleDlg();

// �Ի�������
	enum { IDD = IDD_CONSOLEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CToolBar m_toolbar;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_listconsole;
	afx_msg void OnClose();
	afx_msg void OnDblclkConsole();
};
