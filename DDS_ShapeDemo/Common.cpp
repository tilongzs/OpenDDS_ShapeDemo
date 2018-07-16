#include "stdafx.h"
#include "Common.h"

CString GetModuleDir()
{
	WCHAR buf[MAX_PATH];
	HINSTANCE hInstance = AfxGetApp()->m_hInstance;
	GetModuleFileName(NULL, buf, MAX_PATH * sizeof(WCHAR));
	GetLongPathName(buf, buf, MAX_PATH * sizeof(WCHAR));
	PathRemoveFileSpec(buf);
	return buf;
}