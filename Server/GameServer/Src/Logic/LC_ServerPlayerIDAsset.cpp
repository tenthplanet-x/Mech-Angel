#include "LC_ServerPlayerIDAsset.h"
#include "LC_ServerPlayer.h"
#include "SystemDefine.h"

CIDAsset::CIDAsset()
{

}

CIDAsset::~CIDAsset()
{

}

void CIDAsset::LoadData(const char_data_define::PBIDAsset& data)
{
	m_IDInfoMap.clear();
	for(int i=0; i<data.iddata_size(); ++i)
	{
		mem::set<int32_t> idInfo;
		for(int j=0; j<data.iddata(i).idvalue_size(); ++j)
		{
			idInfo.insert(data.iddata(i).idvalue(j));
		}
		m_IDInfoMap[data.iddata(i).type()] = idInfo;
	}
}

void CIDAsset::SaveData(char_data_define::PBIDAsset* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	for(mem::map<int32_t, mem::set<int32_t> >::iterator it=m_IDInfoMap.begin(); it!=m_IDInfoMap.end(); ++it)
	{
		::char_data_define::PBIDData* pkInfo = pkData->add_iddata();
		if(NULL != pkInfo)
		{
			pkInfo->set_type(it->first);
			for(mem::set<int32_t>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
			{
				pkInfo->add_idvalue(*kIt);
			}
		}
	}
}

void CIDAsset::SyncIDAsset(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_SyncIDAsset syncMsg;
	syncMsg.m_IDVec.reserve(m_IDInfoMap.size());
	for(mem::map<int32_t, mem::set<int32_t> >::iterator it=m_IDInfoMap.begin(); it!=m_IDInfoMap.end(); ++it)
	{
		PB_KeyValueVec data;
		data.m_nKey = it->first;
		data.m_nValue.reserve(it->second.size());
		for(mem::set<int32_t>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
		{
			data.m_nValue.push_back(*kIt);
		}
		syncMsg.m_IDVec.push_back(data);
	}
		
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_IDASSET, &syncMsg);
	
}

void CIDAsset::AddIDByType(LC_ServerPlayer* pkOwnerPlayer, int32_t type, int32_t value)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	bool valid = CheckIDTypeVaild(type);
	if(!valid)
	{
		return;
	}

	MG_RESP_AddID rltMsg;
	rltMsg.m_nResult = RE_SUCCESS;
	rltMsg.m_nType = type;
	rltMsg.m_nValue = value;
	mem::map<int32_t, mem::set<int32_t> >::iterator findIt = m_IDInfoMap.find(type);
	if(findIt == m_IDInfoMap.end())
	{
		mem::set<int32_t> data;
		data.insert(value);
		m_IDInfoMap[type] = data;
	}
	else
	{
		if(!findIt->second.insert(value).second)
		{
			rltMsg.m_nResult = RE_FAIL;
		}
	}

	pkOwnerPlayer->SendMsgToClient(MGPT_RESP_ADDID, &rltMsg);
}

bool CIDAsset::CheckIDTypeVaild(int32_t type)
{
	return IDType_None<type && type<IDType_End;
}