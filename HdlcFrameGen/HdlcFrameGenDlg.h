
// HdlcFrameGenDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "SxGroupBox.h"

// CHdlcFrameGenDlg �Ի���
class CHdlcFrameGenDlg : public CDialogEx
{
// ����
public:
	CHdlcFrameGenDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HDLCFRAMEGEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CBrush m_hbr;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	bool �𡪡�;
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
	CString m_strScript;
	afx_msg void OnBnClickedButton1();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CSxGroupBox m_ctlGroupCntl;
	CSxGroupBox m_ctlGroupInfo;
	CSxGroupBox m_ctlGroupScrt;
};
