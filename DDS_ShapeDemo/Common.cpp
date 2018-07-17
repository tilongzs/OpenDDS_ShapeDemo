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
		return L"��ɫ";
	case RGB(0, 0, 255):
		return L"��ɫ";
	case RGB(255, 255, 0):
		return L"��ɫ";
	case RGB(0, 255, 0):
		return L"��ɫ";
	default:
		return L"ȫ��";
	}
}

CString Int2CStr(int num)
{
	CString str;
	str.Format(L"%d", num);
	return str.Trim();
}