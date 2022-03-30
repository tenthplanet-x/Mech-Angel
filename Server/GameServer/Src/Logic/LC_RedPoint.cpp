#include "LC_RedPoint.h"
#include "LC_ServerPlayer.h"
#include "MG_SyncProtocol.h"

CRedPoint::CRedPoint()
{
	m_RedPointRecord.clear();
	m_RedPointRecord.reserve(RedPoint_Max);
}

CRedPoint::~CRedPoint()
{
	m_RedPointRecord.clear();
}

void CRedPoint::SaveData(char_data_define::PBRedPoint* pkData)
{
	if(NULL != pkData)
	{
		for(int i=0; i<m_RedPointRecord.size(); ++i)
		{
			char_data_define::mapKey_Value_bool* pkInfo = pkData->add_redpointdata();
			if(NULL != pkInfo)
			{
				pkInfo->set_key(m_RedPointRecord[i].key);
				pkInfo->set_value(m_RedPointRecord[i].value);
			}
		}
	}	
}

void CRedPoint::LoadData(const char_data_define::PBRedPoint& data)
{
	m_RedPointRecord.reserve(data.redpointdata_size());
	for(int i=0; i<data.redpointdata_size(); ++i)
	{
		Protocol::KeyValue_Base_Bool info;
		info.key = data.redpointdata(i).key();
		info.value = data.redpointdata(i).value();
		m_RedPointRecord.push_back(info);
	}
}

void CRedPoint::SetRedPointFlag(LC_ServerPlayer* pkPlayer, int32_t param, bool flag)
{
	if(NULL == pkPlayer || !pkPlayer->IsMainController())
	{
		return;
	}

	//²ÎÊý¼ì²â
	if(param<RedPoint_Begin || param>=RedPoint_Max)
	{
		return;
	}

	bool tag = false;
	for(int i=0; i<m_RedPointRecord.size(); ++i)
	{
		if(m_RedPointRecord[i].key == param)
		{
			m_RedPointRecord[i].value = flag;
			tag = true;
			break;
		}
	}

	if(tag == false)
	{
		Protocol::KeyValue_Base_Bool data;
		data.key = param;
		data.value = flag;
		m_RedPointRecord.push_back(data);

		tag = true;
	}

	if(tag)
	{
		SyncRedPointFlag(pkPlayer, param, flag);
	}
}

void CRedPoint::ResetRedPointFlag(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer || !pkPlayer->IsMainController())
	{
		return;
	}

	for(int i=0; i<m_RedPointRecord.size(); ++i)
	{
		int32_t key = m_RedPointRecord[i].key;
		switch(key)
		{
		case RedPoint_PentacleShopDaily:
			m_RedPointRecord[i].value = false;
			break;
		case RedPoint_MicroLoginIn:
		case RedPoint_SoloDuleRobotLockScheme:
			break;
		}
	}

	SyncAllRedPointFlag(pkPlayer);
}

void CRedPoint::SyncRedPointFlag(LC_ServerPlayer* pkPlayer, int32_t param, bool flag)
{
	if(NULL == pkPlayer || !pkPlayer->IsMainController())
	{
		return;
	}

	MG_SyncRedPointFlag msg;
	msg.m_nSyncType = SyncType_Delta;

	Protocol::KeyValue_Base_Bool data;
	data.key = param;
	data.value = flag;
	msg.m_RedPoint.push_back(data);
	pkPlayer->SendMsgToClient(MGPT_SYNCREDPOINTFLAG, &msg);
}

void CRedPoint::SyncAllRedPointFlag(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer || !pkPlayer->IsMainController())
	{
		return;
	}

	MG_SyncRedPointFlag msg;
	msg.m_nSyncType = SyncType_All;
	msg.m_RedPoint.reserve(m_RedPointRecord.size());
	for(int i=0; i<m_RedPointRecord.size(); ++i)
	{
		Protocol::KeyValue_Base_Bool data;
		data.key = m_RedPointRecord[i].key;
		data.value = m_RedPointRecord[i].value;
		msg.m_RedPoint.push_back(data);
	}
	pkPlayer->SendMsgToClient(MGPT_SYNCREDPOINTFLAG, &msg);
}