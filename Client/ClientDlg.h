
// ClientDlg.h : 头文件
//

#pragma once
#include "ClientSocket.h"
#include "afxwin.h"
#include "HdlcFrameGenDlg.h"
#define MAX_LEN 2048

#define WM_AUTOSEND WM_USER+108
// CClientDlg 对话框
class CClientDlg : public CDialogEx
{
// 构造
public:
	CClientSocket m_sClientSocket;    //构造CClientSocket类的对象
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENT_DIALOG };

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
	POINT old;
	CString m_strServerName;
	int m_nServerPort;
	//CString m_strToServer;
	CListBox m_listReceived;
	CListBox m_listSent;
	afx_msg void OnClickedButtonConnect();   //响应函数
	afx_msg void OnClickedButtonDisconnect();
	afx_msg void OnClickedButtonSend();
	void onConnect(void);   //当执行到CClientSocket类的 OnConnect（）函数时，转到此处来执行
	void onReceive(void);
	void onClose(void);        
	//int readoneline (FILE* q,u_char oneLineData[MAX_LEN]);
	afx_msg void OnEnChangeEditFile();
	CString m_editfile;     //文件名，文件路径编辑框的成员变量
    CListBox m_listcontent;  //文件内容，文件内容编辑框的成员变量
	
	afx_msg void OnBnClickedButtonView();
	afx_msg void OnSelchangeListReceived();
	afx_msg void OnSelchangeListSent();
	//afx_msg void OnChangeEditContent();
	//afx_msg void OnButton3();
	//afx_msg void OnSelchangeListConnect();
	afx_msg void OnSelchangeListContent();
	CString m_listToServer;
//	CListBox m_listboxrec;
	CString m_strSendData;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonmake();
	CHdlcFrameGenDlg m_makedlg;
protected:
	afx_msg LRESULT OnAutoSend(WPARAM wParam, LPARAM lParam);
};
