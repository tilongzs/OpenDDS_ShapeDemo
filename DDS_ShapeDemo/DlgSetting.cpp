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
	_comboShapeType.AddString(L"三角形");
	_comboShapeType.AddString(L"四方形");
	_comboShapeType.AddString(L"圆形");
	_comboShapeType.SetCurSel(0);

	if (!_isPublish)
	{
		_comboColor.AddString(L"全部");
	}
	_comboColor.AddString(L"红色");
	_comboColor.AddString(L"蓝色");
	_comboColor.AddString(L"黄色");
	_comboColor.AddString(L"绿色");
	_comboColor.SetCurSel(0);

	_editSize.SetWindowText(L"20");
	if (!_isPublish)
	{
		_editSize.SetReadOnly(TRUE);
		_editSize.EnableWindow(FALSE);
	}

	if (_isPublish)
	{
		SetWindowText(L"发布配置");
	}
	else
	{
		SetWindowText(L"订阅配置");
	}

	return TRUE;
}

void CDlgSetting::OnBnClickedOk()
{
	CString shapeType;
	_comboShapeType.GetLBText(_comboShapeType.GetCurSel(), shapeType);
	if (shapeType == L"三角形")
	{
		_shapeInfo->shapeType = "triangle";
	}
	else if (shapeType == L"四方形")
	{
		_shapeInfo->shapeType = "square";
	}
	else if (shapeType == L"圆形")
	{
		_shapeInfo->shapeType = "cricle";
	}

	CString color;
	_comboColor.GetLBText(_comboColor.GetCurSel(), color);
	if (color == L"全部")
	{
		_shapeInfo->color = -1;
	}
	else if (color == L"红色")
	{
		_shapeInfo->color = RGB(255, 0, 0);
	}
	else if (color == L"蓝色")
	{
		_shapeInfo->color = RGB(0, 0, 255);
	}
	else if (color == L"黄色")
	{
		_shapeInfo->color = RGB(255, 255, 0);
	}
	else if (color == L"绿色")
	{
		_shapeInfo->color = RGB(0, 255, 0);
	}

	CString size;
	_editSize.GetWindowText(size);
	_shapeInfo->size = _wtoi(size);

	CDialogEx::OnOK();
}
