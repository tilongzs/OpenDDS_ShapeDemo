#pragma once

/*
����OpenDDSͼ�ι���
*/

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
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