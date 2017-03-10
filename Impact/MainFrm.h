// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "ReportView.h"
#include "ClientSocket.h"
#include "ISH.h"
#include <vcclr.h>

class CClientSocket;
class CMainFrame : public CFrameWndEx, public ISocketHandler
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

	
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CReportView		  m_wndReportView;


// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnButtonProperty();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnComboContype();
	afx_msg void OnEditRemoteAddr();
	afx_msg void OnEditRemotePort();
	afx_msg void OnEditLocAddr();
	afx_msg void OnButtonStart();
	afx_msg void OnBtnCon();
	CString m_strRemoteAddr;
	CString m_strRemotePort;
	CString m_strLocAddr;
	CString m_strLocPort;
	CClientSocket m_sServerSocket;  //监听套接字
	CClientSocket m_sClientSocket;  //连接套接字
	CClientSocket m_sLoopSocket;  //连接套接字
	int m_nConType;
	BOOL m_bConnected;
	CString strMsg;
	afx_msg void OnEditLocPort();
	afx_msg void OnButtonPlugin();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnButtonStop();
	virtual void onClose();
	virtual void onAccept();
	virtual void onReceive();
	virtual void onConnect();
};


