#include "stdafx.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl.h"
#include "DDS_IDL/ShapeInfoTypeSupportC.h"
#include "DDS_IDL/ShapeInfoTypeSupportImpl.h"

#include "DDS_ShapeDemoDlg.h"


void CDataReaderListenerImpl::Init(CDDS_ShapeDemoDlg* dlg)
{
	_dlg = dlg;
}

void CDataReaderListenerImpl::on_requested_deadline_missed(
	DDS::DataReader_ptr reader,
	const DDS::RequestedDeadlineMissedStatus& status)
{
}

void CDataReaderListenerImpl::on_requested_incompatible_qos(
	DDS::DataReader_ptr reader,
	const DDS::RequestedIncompatibleQosStatus& status)
{
}

void CDataReaderListenerImpl::on_sample_rejected(
	DDS::DataReader_ptr reader,
	const DDS::SampleRejectedStatus& status)
{
}

void CDataReaderListenerImpl::on_liveliness_changed(
	DDS::DataReader_ptr reader,
	const DDS::LivelinessChangedStatus& status)
{
}

void CDataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
	_dlg->on_data_available(reader);
}

void CDataReaderListenerImpl::on_subscription_matched(
	DDS::DataReader_ptr reader,
	const DDS::SubscriptionMatchedStatus& status)
{
}

void CDataReaderListenerImpl::on_sample_lost(
	DDS::DataReader_ptr reader,
	const DDS::SampleLostStatus& status)
{
}
