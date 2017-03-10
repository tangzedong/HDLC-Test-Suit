
// HdlcFrameGenDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CHdlcFrameGenDlg 对话框
class CHdlcFrameGenDlg : public CDialogEx
{
// 构造
public:
	CHdlcFrameGenDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HDLCFRAMEGEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	bool 吗——;
//	bool m_splitbit;
//	CComboBox m_hAddrLength;
//	CComboBox m_hFrameType;
//	CString m_strRInd;
//	CString m_strSInd;
	// P-1,F-0
//	bool m_bPF;
	afx_msg void OnBnClickedCheck1();
//	CString m_strDst;
//	CString m_strSrc;
	BOOL m_bSplitBit;
	// P=1,F=0
	BOOL m_bPF;
	CComboBox m_hAddrLength;
	CComboBox m_hFrameType;
	CString m_strRInd;
	CString m_strSInd;
	CString m_strDstAddr;
	CString m_strSrcAddr;
	CString m_strInfo;
	CString m_hResultLog;
	afx_msg void OnBnClickedButton3();
};
