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

// ImpactView.h : CImpactView 类的接口
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
protected: // 仅从序列化创建
	CImpactView();
	DECLARE_DYNCREATE(CImpactView)

// 特性
public:
	CImpactDoc* GetDocument() const;

// 操作
public:
	//void moveedit();
	virtual void AppendLog(LPTSTR strLog);
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
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

// 生成的消息映射函数
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

#ifndef _DEBUG  // ImpactView.cpp 中的调试版本
inline CImpactDoc* CImpactView::GetDocument() const
   { return reinterpret_cast<CImpactDoc*>(m_pDocument); }
#endif

