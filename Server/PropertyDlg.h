#pragma once


// CPropertyDlg 对话框

class CPropertyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPropertyDlg();

// 对话框数据
	enum { IDD = IDD_PROPERTYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CMFCPropertyGridCtrl m_propertysheet;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
