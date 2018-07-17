#include "stdafx.h"
#include "DDS_ShapeDemo.h"
#include "DDS_ShapeDemoDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include "DlgSetting.h"
#include "DataReaderListenerImpl.h"
#include "DDS_IDL/ShapeInfoTypeSupportImpl.h"

#define WMSG_LOG		WM_USER + 1
#define TIMER_SHOW_SHAPE 1

static int gShapeBmpWidth = 300;	// 画布宽度
static int gShapeBmpHeight = 300;	// 画布长度
static int gShapeMoveInterval = 1;	// 移动步长

CDDS_ShapeDemoDlg::CDDS_ShapeDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DDS_SHAPEDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitializeSRWLock(&_srwSamplesPublish);
	InitializeSRWLock(&_srwSamplesSubscribe);
	InitializeSRWLock(&_srwShapeBmp);
}

void CDDS_ShapeDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MSG, _editMsg);
	DDX_Control(pDX, IDC_LIST_DATA, _listData);
}

BEGIN_MESSAGE_MAP(CDDS_ShapeDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WMSG_LOG, OnLog)
	ON_BN_CLICKED(IDC_BUTTON_PUBLISH, &CDDS_ShapeDemoDlg::OnBtnPublish)
	ON_BN_CLICKED(IDC_BUTTON_SUBSCRIBE, &CDDS_ShapeDemoDlg::OnBtnSubscribe)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CDDS_ShapeDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	AcquireSRWLockExclusive(&_srwShapeBmp);
	_shapeBmp = make_shared<Bitmap>(gShapeBmpWidth, gShapeBmpHeight);
	ReleaseSRWLockExclusive(&_srwShapeBmp);

	InitOpenDDS();

	SetTimer(TIMER_SHOW_SHAPE, 10, nullptr);

	return TRUE;
}

void CDDS_ShapeDemoDlg::InitOpenDDS()
{
	// ACE初始化
	ACE_Object_Manager::instance()->init();
	AppendMSG(L"ACE初始化完成");

	int argc = 2;
	char* argv[] = { "-DCPSConfigFile", "rtps.ini" };
	try
	{
		// 创建域参与者工厂 
		_dpf = TheParticipantFactoryWithArgs(argc, argv);
		if (!_dpf)
		{
			AppendMSG(L"启动发布服务失败");
			return;
		}

		// 创建域参与者
		_participant = _dpf->create_participant(233, // 域ID
			PARTICIPANT_QOS_DEFAULT,
			0,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (!_participant)
		{
			AppendMSG(L"创建域参与者失败");
			return;
		}

		// 注册类型
		_shapeInfo_TS = new ShapeInfoTypeSupportImpl;
		if (_shapeInfo_TS->register_type(_participant, "") != DDS::RETCODE_OK)
		{
			AppendMSG(L"注册类型失败");
			return;
		}
		AppendMSG(L"加入域成功");
	}
	catch (const CORBA::Exception& e)
	{
		CString strLog;
		strLog.Format(L"CDDS_ShapeDemoDlg::InitOpenDDS 异常:%s %s", CString(e._name()), CString(e._info().c_str()));
		AppendMSG(strLog);
		return;
	}
}

void CDDS_ShapeDemoDlg::ShowShape()
{
	auto DrawShape = [&](Graphics& tmpG, shared_ptr<SampleData>& sampleData, bool isSubscribe)
	{
		AcquireSRWLockShared(&sampleData->srwShape);
		auto& shapeInfo = sampleData->shapeInfo;
		if (shapeInfo->shapeType == CStringA("三角形"))
		{
			int height = shapeInfo->size / 2 * tan(60);
			Gdiplus::GraphicsPath path;
			Point points[3] = { Point(shapeInfo->posX + shapeInfo->size / 2, shapeInfo->posY),
				Point(shapeInfo->posX, shapeInfo->posY + shapeInfo->size),
				Point(shapeInfo->posX + shapeInfo->size, shapeInfo->posY + shapeInfo->size) };
			path.AddCurve(points, 3);

			tmpG.SetCompositingQuality(CompositingQualityHighQuality);
			tmpG.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			tmpG.SetSmoothingMode(SmoothingModeHighQuality);
			tmpG.FillPolygon(&SolidBrush(Color(GetRValue(shapeInfo->color), GetGValue(shapeInfo->color), GetBValue(shapeInfo->color))), points, 3);

			if (isSubscribe)
			{
				tmpG.DrawPolygon(&Pen(Color(50, 50, 50), 2.5), points, 3);
			}
		}
		else if (shapeInfo->shapeType == CStringA("四边形"))
		{
			Rect rect(shapeInfo->posX, shapeInfo->posY, shapeInfo->size, shapeInfo->size);
			tmpG.FillRectangle(&SolidBrush(Color(GetRValue(shapeInfo->color), GetGValue(shapeInfo->color), GetBValue(shapeInfo->color))), rect);

			if (isSubscribe)
			{
				tmpG.DrawRectangle(&Pen(Color(50, 50, 50), 2.5), rect);
			}
		}
		else if (shapeInfo->shapeType == CStringA("圆形"))
		{
			Rect rect(shapeInfo->posX, shapeInfo->posY, shapeInfo->size, shapeInfo->size);
			tmpG.SetCompositingQuality(CompositingQualityHighQuality);
			tmpG.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			tmpG.SetSmoothingMode(SmoothingModeHighQuality);
			tmpG.FillEllipse(&SolidBrush(Color(GetRValue(shapeInfo->color), GetGValue(shapeInfo->color), GetBValue(shapeInfo->color))), rect);

			if (isSubscribe)
			{
				tmpG.DrawEllipse(&Pen(Color(50, 50, 50), 2.5), rect);
			}
		}
		ReleaseSRWLockShared(&sampleData->srwShape);
	};

	// 绘制所有图形
	AcquireSRWLockExclusive(&_srwShapeBmp);

	Graphics tmpG(_shapeBmp.get());
	int shapeBmpWidht = _shapeBmp->GetWidth();
	int shapeBmpHeight = _shapeBmp->GetHeight();
	tmpG.FillRectangle(&SolidBrush(Color(255, 255, 255)), 0, 0, gShapeBmpWidth, gShapeBmpHeight);

	AcquireSRWLockShared(&_srwSamplesPublish);
	for each (auto iter in _samplesPublish)
	{
		DrawShape(tmpG, iter.second, false);
	}
	ReleaseSRWLockShared(&_srwSamplesPublish);

	AcquireSRWLockShared(&_srwSamplesSubscribe);
	for each (auto iter in _samplesSubscribe)
	{
		DrawShape(tmpG, iter.second, true);
	}
	ReleaseSRWLockShared(&_srwSamplesSubscribe);

	ReleaseSRWLockExclusive(&_srwShapeBmp);

	CRect rect;
	GetClientRect(rect);
	const int rectWidth = 350;
	const int rectHeight = 350;
	CRect bmpRect(rect.Width() - rectWidth, 50 + 25, rect.Width() - rectWidth + rectWidth, 50 + 25 + rectHeight);
	InvalidateRect(bmpRect, FALSE);
}

void CDDS_ShapeDemoDlg::OnPaint()
{
	CRect rect;
	GetClientRect(rect);

	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
	
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CDC memDC;
		CBitmap memBmp;
		memDC.CreateCompatibleDC(&dc);
		memBmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		CBitmap* oldBmp = memDC.SelectObject(&memBmp);

		Graphics g(memDC);
		const int rectWidth = 350;
		const int rectHeight = 350;

		g.FillRectangle(&SolidBrush(Color(255, 255, 255)), 0, 0, rect.Width(), rect.Height());
		// 图形区域边框
		g.FillRectangle(&SolidBrush(Color(128, 128, 128)), rect.Width() - rectWidth - 20, 50, rectWidth, rectHeight);

		AcquireSRWLockShared(&_srwShapeBmp);
		g.FillRectangle(&SolidBrush(Color(255, 0, 0)), rect.Width() - rectWidth - 20 + 25, 50 + 25, gShapeBmpWidth, gShapeBmpHeight);
		g.DrawImage(_shapeBmp.get(), rect.Width() - rectWidth - 20 + 25, 50 + 25, gShapeBmpWidth, gShapeBmpHeight);
		ReleaseSRWLockShared(&_srwShapeBmp);

		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(oldBmp);
		memBmp.DeleteObject();
		memDC.DeleteDC();		
	}
}

HCURSOR CDDS_ShapeDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDDS_ShapeDemoDlg::AppendMSG(const CString& msg)
{
	CString* pMsg = new CString(msg);
	PostMessage(WMSG_LOG, (WPARAM)pMsg);
}

LRESULT CDDS_ShapeDemoDlg::OnLog(WPARAM wParam, LPARAM lParam)
{
	CString* pMsg = (CString*)wParam;

	if (_editMsg.GetLineCount() > 30)
	{
		_editMsg.SetSel(0, -1);
		_editMsg.Clear();
	}

	CString curMsg;
	_editMsg.GetWindowTextW(curMsg);
	curMsg += "\r\n";
	curMsg += CTime::GetCurrentTime().Format(L"[%H:%M:%S] ");
	curMsg += *pMsg;
	_editMsg.SetWindowTextW(curMsg);
	_editMsg.LineScroll(_editMsg.GetLineCount());

	delete pMsg;
	return TRUE;
}

void CDDS_ShapeDemoDlg::OnBtnPublish()
{
	shared_ptr<ShapeInfo> shapeInfo = make_shared<ShapeInfo>();
	CDlgSetting dlgSetting(shapeInfo, true);
	if (dlgSetting.DoModal())
	{
		shared_ptr<ShapeInfo> sampleShape = make_shared<ShapeInfo>();
		*sampleShape = *shapeInfo;
		static int sampleID = 1;
		sampleShape->id = sampleID++;

		shared_ptr<SampleData> sampleData = make_shared<SampleData>();
		sampleData->shapeInfo = sampleShape;
		AcquireSRWLockExclusive(&_srwSamplesPublish);
		_samplesPublish.insert(map<int, shared_ptr<SampleData>>::value_type(sampleShape->id, sampleData));
		ReleaseSRWLockExclusive(&_srwSamplesPublish);

		shared_ptr<TaskData> taskData = make_shared<TaskData>();
		taskData->shapeInfo = shapeInfo;
		int sampleIndex = _taskPublish.size(); // 索引，递增
		_taskPublish.insert(map<int, shared_ptr<TaskData>>::value_type(sampleIndex, taskData));
		taskData->cts = make_shared<cancellation_token_source>();
		auto token = taskData->cts->get_token();
		taskData->task = make_shared<task<void>>([&, token, sampleShape, sampleData]
		{
			CString log;
			// 创建主题
			CORBA::String_var type_name = _shapeInfo_TS->get_type_name();
			Topic_var topic = _participant->create_topic(sampleShape->shapeType,
				type_name,
				TOPIC_QOS_DEFAULT,
				0,
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
			if (!topic)
			{
				AppendMSG(L"创建主题失败");
				return;
			}

			// 创建发布者
			Publisher_var publisher = _participant->create_publisher(PUBLISHER_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
			if (!publisher)
			{
				AppendMSG(L"创建发布者失败");
				return;
			}

			DataWriterQos dataWriterQos;
			publisher->get_default_datawriter_qos(dataWriterQos);
			dataWriterQos.liveliness.kind = AUTOMATIC_LIVELINESS_QOS;
			DDS::Duration_t livelinessTime = { 1, 0 };
			dataWriterQos.liveliness.lease_duration = livelinessTime;
			dataWriterQos.history.kind = KEEP_LAST_HISTORY_QOS;
			dataWriterQos.durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;

			// 创建写数据工具
			DDS::DataWriter_var writer =
					publisher->create_datawriter(topic,
					dataWriterQos, // DATAWRITER_QOS_DEFAULT
					nullptr,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);

			if (!writer)
			{
				AppendMSG(L"创建写数据工具失败");
				return;
			}

			ShapeInfoDataWriter_var message_writer = ShapeInfoDataWriter::_narrow(writer);
			if (!message_writer)
			{
				ASSERT(0);
			}

			DDS::Duration_t timeout = { 30, 0 };			
			while (!token.is_canceled())
			{
				AcquireSRWLockExclusive(&sampleData->srwShape);

				// 移动图形
				if (sampleShape->directionX)
				{
					// 正在向右移动
					sampleShape->posX += gShapeMoveInterval;
					if (sampleShape->posX + sampleShape->size > gShapeBmpWidth)
					{
						sampleShape->posX = gShapeBmpWidth - sampleShape->size - gShapeMoveInterval;
						sampleShape->directionX = 0; // 转向
					}					
				}
				else
				{
					// 正在向左移动
					sampleShape->posX -= gShapeMoveInterval;
					if (sampleShape->posX < 0)
					{
						sampleShape->posX = gShapeMoveInterval;
						sampleShape->directionX = 1; // 转向
					}
				}

				if (sampleShape->directionY)
				{
					// 正在向下移动
					sampleShape->posY += gShapeMoveInterval;
					if (sampleShape->posY + sampleShape->size > gShapeBmpHeight)
					{
						sampleShape->posY = gShapeBmpHeight - sampleShape->size - gShapeMoveInterval;
						sampleShape->directionY = 0; // 转向
					}
				}
				else
				{
					// 正在向上移动
					sampleShape->posY -= gShapeMoveInterval;
					if (sampleShape->posY < 0)
					{
						sampleShape->posY = gShapeMoveInterval;
						sampleShape->directionY = 1; // 转向
					}
				}
				/********************************************************************************************/

				DDS::InstanceHandle_t handle = message_writer->register_instance(*sampleShape);
				ReturnCode_t error = message_writer->write(*sampleShape, handle);
				if (error != DDS::RETCODE_OK)
				{
					AppendMSG(L"写数据时出错");
				}

				ReleaseSRWLockExclusive(&sampleData->srwShape);

				// 等待订阅者接收完毕
				if (message_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
				{
					ASSERT(0);
				}

				Sleep(10);
			}

			// 清理资源
			publisher->delete_datawriter(writer);
			_participant->delete_publisher(publisher);
			_participant->delete_topic(topic);

// 			log.Format(L"数据：%d已停止", sampleIndex);
// 			AppendMSG(log);
		}, token);
	}
}

void CDDS_ShapeDemoDlg::OnBtnSubscribe()
{
	shared_ptr<ShapeInfo> shapeInfo = make_shared<ShapeInfo>();
	CDlgSetting dlgSetting(shapeInfo, false);
	if (dlgSetting.DoModal())
	{
		shared_ptr<TaskData> taskData = make_shared<TaskData>();
		taskData->shapeInfo = shapeInfo;
		int sampleIndex = _taskSubscribe.size(); // 索引，递增
		_taskSubscribe.insert(map<int, shared_ptr<TaskData>>::value_type(sampleIndex, taskData));
		taskData->cts = make_shared<cancellation_token_source>();
		auto token = taskData->cts->get_token();
		taskData->task = make_shared<task<void>>([&, token, shapeInfo]
		{
			// 创建主题
			CORBA::String_var type_name = _shapeInfo_TS->get_type_name();
			DDS::Topic_var topic =
					_participant->create_topic(shapeInfo->shapeType,
					type_name,
					TOPIC_QOS_DEFAULT,
					0,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);

			if (!topic)
			{
				AppendMSG(L"创建主题失败");
				return;
			}

			// 创建订阅者
			DDS::Subscriber_var subscriber =
				_participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
					0,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);

			if (!subscriber)
			{
				AppendMSG(L"创建订阅者失败");
				return;
			}

			DataReaderQos dataReaderQos;
			subscriber->get_default_datareader_qos(dataReaderQos);
			dataReaderQos.liveliness.kind = AUTOMATIC_LIVELINESS_QOS;
			DDS::Duration_t livelinessTime = { 1, 0 };
			dataReaderQos.liveliness.lease_duration = livelinessTime;
			dataReaderQos.history.kind = KEEP_LAST_HISTORY_QOS;
			dataReaderQos.durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;

			// 创建读监听器
			CDataReaderListenerImpl* dataReaderListener = new CDataReaderListenerImpl();
			dataReaderListener->Init(this);

			// 创建过滤规则
			CStringA strFilter;
			if (shapeInfo->color == -1)
			{
				// 全部颜色
				strFilter.Format("shapeType='%s'", shapeInfo->shapeType);
			}
			else
			{
				strFilter.Format("shapeType='%s' and color=%d", shapeInfo->shapeType, shapeInfo->color);
			}

			static int filterIndex = 1;
			CStringA filterName;
			filterName.Format("filter%d", filterIndex++);
			DDS::ContentFilteredTopic_var cft =
				_participant->create_contentfilteredtopic(filterName,
					topic,
					strFilter,
					StringSeq());

			DDS::DataReader_var reader = subscriber->create_datareader(cft,
				dataReaderQos,
				dataReaderListener,
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);

			if (!reader)
			{
				AppendMSG(L"创建读监听器失败");
				return;
			}

			while (!token.is_canceled())
			{
				Sleep(100);
			}

			// 清理资源
			subscriber->delete_datareader(reader);
			_participant->delete_subscriber(subscriber);
			_participant->delete_topic(topic);
		}, token);
	}
}

void CDDS_ShapeDemoDlg::on_data_available(DDS::DataReader_ptr reader)
{
	ShapeInfoDataReader_var reader_i = ShapeInfoDataReader::_narrow(reader);
	if (!reader_i) {
		ASSERT(0);
	}

	ShapeInfoSeq shapeInfo;
	SampleInfoSeq sampleInfos;
	ReturnCode_t ret = reader_i->take(shapeInfo, sampleInfos, DDS::LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if (ret == DDS::RETCODE_OK)
	{
		CString tmpLog;
		int sampleInfoLength = sampleInfos.length();
		for (int i = 0; i < sampleInfoLength; ++i)
		{
			if (sampleInfos[i].valid_data)
			{
				AcquireSRWLockExclusive(&_srwSamplesSubscribe);
				if (_samplesSubscribe.find(shapeInfo[i].id) == _samplesSubscribe.end())
				{
					shared_ptr<SampleData> sampleData = make_shared<SampleData>();
					sampleData->shapeInfo = make_shared<ShapeInfo>();
					*sampleData->shapeInfo = shapeInfo[i];
					_samplesSubscribe.insert(map<int, shared_ptr<SampleData>>::value_type(shapeInfo[i].id, sampleData));
				}
				else
				{
					auto& sampleData = _samplesSubscribe[shapeInfo[i].id];
					AcquireSRWLockExclusive(&sampleData->srwShape);
					*sampleData->shapeInfo = shapeInfo[i];
					ReleaseSRWLockExclusive(&sampleData->srwShape);
				}
				ReleaseSRWLockExclusive(&_srwSamplesSubscribe);
			}
		}
	}
	else
	{
		ASSERT(0);
	}
}

void CDDS_ShapeDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_SHOW_SHAPE)
	{
		ShowShape();
	}

	CDialogEx::OnTimer(nIDEvent);
}
