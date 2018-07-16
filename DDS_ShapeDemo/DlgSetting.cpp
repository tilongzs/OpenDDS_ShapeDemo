#include "stdafx.h"
#include "DDS_ShapeDemo.h"
#include "DlgSetting.h"
#include "afxdialogex.h"
#include "Common.h"

IMPLEMENT_DYNAMIC(CDlgSetting, CDialogEx)

CDlgSetting::CDlgSetting(shared_ptr<ShapeInfo> shapeInfo, bool isPublish, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SETTING, pParent)
{
	_shapeInfo = shapeInfo;
	_isPublish = isPublish;
}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SHAPETYPE, _comboShapeType);
	DDX_Control(pDX, IDC_COMBO_COLOR, _comboColor);
	DDX_Control(pDX, IDC_EDIT_SIZE, _editSize);
}

BEGIN_MESSAGE_MAP(CDlgSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSetting::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDlgSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_comboShapeType.AddString(L"������");
	_comboShapeType.AddString(L"�ķ���");
	_comboShapeType.AddString(L"Բ��");
	_comboShapeType.SetCurSel(0);

	if (!_isPublish)
	{
		_comboColor.AddString(L"ȫ��");
	}
	_comboColor.AddString(L"��ɫ");
	_comboColor.AddString(L"��ɫ");
	_comboColor.AddString(L"��ɫ");
	_comboColor.AddString(L"��ɫ");
	_comboColor.SetCurSel(0);

	_editSize.SetWindowText(L"20");
	if (!_isPublish)
	{
		_editSize.SetReadOnly(TRUE);
		_editSize.EnableWindow(FALSE);
	}

	if (_isPublish)
	{
		SetWindowText(L"��������");
	}
	else
	{
		SetWindowText(L"��������");
	}

	return TRUE;
}

void CDlgSetting::OnBnClickedOk()
{
	CString shapeType;
	_comboShapeType.GetLBText(_comboShapeType.GetCurSel(), shapeType);
	if (shapeType == L"������")
	{
		_shapeInfo->shapeType = "triangle";
	}
	else if (shapeType == L"�ķ���")
	{
		_shapeInfo->shapeType = "square";
	}
	else if (shapeType == L"Բ��")
	{
		_shapeInfo->shapeType = "cricle";
	}

	CString color;
	_comboColor.GetLBText(_comboColor.GetCurSel(), color);
	if (color == L"ȫ��")
	{
		_shapeInfo->color = -1;
	}
	else if (color == L"��ɫ")
	{
		_shapeInfo->color = RGB(255, 0, 0);
	}
	else if (color == L"��ɫ")
	{
		_shapeInfo->color = RGB(0, 0, 255);
	}
	else if (color == L"��ɫ")
	{
		_shapeInfo->color = RGB(255, 255, 0);
	}
	else if (color == L"��ɫ")
	{
		_shapeInfo->color = RGB(0, 255, 0);
	}

	CString size;
	_editSize.GetWindowText(size);
	_shapeInfo->size = _wtoi(size);

	CDialogEx::OnOK();
}
