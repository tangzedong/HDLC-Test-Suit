
// HdlcFrameGen.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHdlcFrameGenApp: 
// �йش����ʵ�֣������ HdlcFrameGen.cpp
//

class CHdlcFrameGenApp : public CWinApp
{
public:
	CHdlcFrameGenApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHdlcFrameGenApp theApp;