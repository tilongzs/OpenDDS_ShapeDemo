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

CString GetColorText(long color)
{
	switch (color)
	{
	case RGB(255, 0, 0):
		return L"红色";
	case RGB(0, 0, 255):
		return L"蓝色";
	case RGB(255, 255, 0):
		return L"黄色";
	case RGB(0, 255, 0):
		return L"绿色";
	default:
		return L"全部";
	}
}

CString Int2CStr(int num)
{
	CString str;
	str.Format(L"%d", num);
	return str.Trim();
}