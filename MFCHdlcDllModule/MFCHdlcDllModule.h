// MFCHdlcDllModule.h : MFCHdlcDllModule DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCHdlcDllModuleApp
// �йش���ʵ�ֵ���Ϣ������� MFCHdlcDllModule.cpp
//

class CMFCHdlcDllModuleApp : public CWinApp
{
public:
	CMFCHdlcDllModuleApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
