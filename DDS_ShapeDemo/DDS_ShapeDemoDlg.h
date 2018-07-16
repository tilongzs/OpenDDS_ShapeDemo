#pragma once
#include "afxwin.h"

#include <ace/Log_Msg.h>

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/StaticIncludes.h>

#include "DDS_IDL/ShapeInfoTypeSupportImpl.h"

#include <ppltasks.h>
using namespace concurrency;

using namespace DDS;
using namespace ShapeDemo;

struct TaskData
{
	~TaskData()
	{
		if (cts)
		{
			cts->cancel();

			if (task)
			{
				task->wait();
			}
		}
	}

	shared_ptr<cancellation_token_source>	cts;
	shared_ptr<task<void>>					task;
	shared_ptr<ShapeInfo>					shapeInfo;
};

struct SampleData
{
	SampleData()
	{
		InitializeSRWLock(&srwShape);
	}

	SRWLOCK							srwShape;
	shared_ptr<ShapeInfo>			shapeInfo;
};

class CDDS_ShapeDemoDlg : public CDialogEx
{
public:
	CDDS_ShapeDemoDlg(CWnd* pParent = NULL);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DDS_SHAPEDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV Ö§³Ö

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	SRWLOCK					_srwShapeBmp;
	shared_ptr<Bitmap>		_shapeBmp;

	DomainParticipantFactory_var	_dpf = nullptr;
	DomainParticipant_var			_participant = nullptr;
	ShapeInfoTypeSupport_var		_shapeInfo_TS = nullptr;

	map<int/*topicID*/, shared_ptr<TaskData>>	_taskPublish;
	map<int/*topicID*/, shared_ptr<TaskData>>	_taskSubscribe;

	SRWLOCK										_srwSamples;
	map<int/*sampleID*/, shared_ptr<SampleData>> _samples;

	CEdit					_editMsg;
	CListBox				_listData;


	LRESULT OnLog(WPARAM wParam, LPARAM lParam);
	void InitOpenDDS();
	void ShowShape();

	afx_msg void OnBtnPublish();
	afx_msg void OnBtnSubscribe();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	void AppendMSG(const CString& msg);
	void on_data_available(DDS::DataReader_ptr reader);
	
};
