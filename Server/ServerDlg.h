
// ServerDlg.h : 头文件
//

#pragma once
#include "ServerSocket.h"
#include "afxwin.h"

#define WM_INFO WM_USER+8
#define WM_REPORTEVENT WM_USER+9
// CServerDlg 对话框
class CServerDlg : public CDialogEx
{
// 构造
public:
	CServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVER_DIALOG };

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
	CString m_strServerName;
	int m_nServerPort;
	CString m_strToServer;
	CListBox m_listReceived;
	CListBox m_listSent;
//    CListBox m_liststateinfo;
	afx_msg void OnClickedButtonListen();
	afx_msg void OnClickedButtonCloselisten();
	afx_msg void OnClickedButtonSend();
	

	void onClose(void);  //对应处理OnClose()事件的函数
	void onAccept(void);
	void onReceive(void);
	CServerSocket m_sServerSocket;  //监听套接字
	CServerSocket m_sClientSocket;  //连接套接字
	CServerSocket m_sLoopSocket;  //连接套接字
	CListBox m_liststateinfo;

//	void setHScroll();
protected:
	afx_msg LRESULT OnInfo(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnTest(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnReportEvent(WPARAM wParam, LPARAM lParam);
};
