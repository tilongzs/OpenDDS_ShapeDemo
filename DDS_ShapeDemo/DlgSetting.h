#pragma once
#include "afxwin.h"
#include "DDS_IDL/ShapeInfoTypeSupportImpl.h"

using namespace DDS;
using namespace ShapeDemo;

/*
发布/订阅配置选项对话框
*/

class CDlgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(shared_ptr<ShapeInfo> shapeInfo, bool isPublish, CWnd* pParent = NULL);
	virtual ~CDlgSetting();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	CComboBox	_comboShapeType;
	CComboBox	_comboColor;
	CEdit		_editSize;

	shared_ptr<ShapeInfo>	_shapeInfo;
	bool					_isPublish = false;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:

};
