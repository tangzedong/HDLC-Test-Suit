#pragma once


// CPropertyDlg �Ի���

class CPropertyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPropertyDlg();

// �Ի�������
	enum { IDD = IDD_PROPERTYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CMFCPropertyGridCtrl m_propertysheet;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
