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

// ImpactView.h : CImpactView ��Ľӿ�
//

#pragma once
#include "IAppendLog.h"
#define WM_INFO WM_USER+1

public ref class HandlerT{
public:
	static void handler(System::Object ^sender, System::EventArgs ^e);
};

class CImpactView : public CView, public IAppendLog
{
protected: // �������л�����
	CImpactView();
	DECLARE_DYNCREATE(CImpactView)

// ����
public:
	CImpactDoc* GetDocument() const;

// ����
public:
	//void moveedit();
	virtual void AppendLog(LPTSTR strLog);
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CImpactView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	ICSharpCode::AvalonEdit::TextEditor ^GetHwnd(int x, int y, int width, int height, HWND *hWnd);
	HWND m_hCODE;
	gcroot<ICSharpCode::AvalonEdit::TextEditor^> m_ctlCodeEdit;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnInfo(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnButtonCls();
};

#ifndef _DEBUG  // ImpactView.cpp �еĵ��԰汾
inline CImpactDoc* CImpactView::GetDocument() const
   { return reinterpret_cast<CImpactDoc*>(m_pDocument); }
#endif

