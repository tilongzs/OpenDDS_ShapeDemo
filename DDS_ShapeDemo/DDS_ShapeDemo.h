#pragma once

/*
测试OpenDDS图形工程
*/

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"

class CDDS_ShapeDemoApp : public CWinApp
{
public:
	CDDS_ShapeDemoApp();

public:
	virtual BOOL InitInstance();
	
	DECLARE_MESSAGE_MAP()
};

extern CDDS_ShapeDemoApp theApp;