// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// MainFrm.h : CMainFrame ��Ľӿ�
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
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

	
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CReportView		  m_wndReportView;


// ���ɵ���Ϣӳ�亯��
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
	CClientSocket m_sServerSocket;  //�����׽���
	CClientSocket m_sClientSocket;  //�����׽���
	CClientSocket m_sLoopSocket;  //�����׽���
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


