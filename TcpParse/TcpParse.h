
// TcpParse.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTcpParseApp:
// �йش����ʵ�֣������ TcpParse.cpp
//

class CTcpParseApp : public CWinApp
{
public:
	CTcpParseApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTcpParseApp theApp;