#pragma once
#include "afxwin.h"


// CConsoleDlg 对话框

class CConsoleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConsoleDlg)

public:
	CConsoleDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConsoleDlg();

// 对话框数据
	enum { IDD = IDD_CONSOLEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CToolBar m_toolbar;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_listconsole;
	afx_msg void OnClose();
	afx_msg void OnDblclkConsole();
};
