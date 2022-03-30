#include "MG_SyncProtocol.h"
#include "UT_Singleton.h"

#include "LC_Define.h"
#include "LC_ServerPlayerMapInfo.h"
#include "LC_ServerPlayer.h"
#include "LC_WildPrincesManager.h"

CPlayerMapInfo::CPlayerMapInfo()
{
	m_vServerPlayerBossFlagInfo.clear();
}

CPlayerMapInfo::~CPlayerMapInfo()
{
	m_vServerPlayerBossFlagInfo.clear();
}

void CPlayerMapInfo::SaveData(char_data_define::PBPlayerMapInfo* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	for(int i=0; i<m_vServerPlayerBossFlagInfo.size(); ++i)
	{
		::char_data_define::PBServerPlayerBossFlagData* pkInfo = pkData->add_serverplayerbossflagdata();
		if(NULL != pkInfo)
		{
			pkInfo->set_mapid(m_vServerPlayerBossFlagInfo[i].m_nMapID);
			pkInfo->set_chartype(m_vServerPlayerBossFlagInfo[i].m_nCharTypeID);
			pkInfo->set_flag(m_vServerPlayerBossFlagInfo[i].m_bFlag);
		}
	}
}

void CPlayerMapInfo::LoadData(char_data_define::PBPlayerMapInfo data)
{
	m_vServerPlayerBossFlagInfo.clear();
	m_vServerPlayerBossFlagInfo.reserve(data.serverplayerbossflagdata_size());
	for(int i=0; i<data.serverplayerbossflagdata_size(); ++i)
	{
		PB_ServerPlayerBossFlagData info;
		info.m_nMapID = data.serverplayerbossflagdata(i).mapid();
		info.m_nCharTypeID = data.serverplayerbossflagdata(i).chartype();
		info.m_bFlag = data.serverplayerbossflagdata(i).flag();
		m_vServerPlayerBossFlagInfo.push_back(info);
	}
}

void CPlayerMapInfo::SetMapBossFlag(LC_ServerPlayer* pkOwnerPlayer, PB_ServerPlayerBossFlagData& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetMapBossFlag r1!");
		return;
	}

	//¼ì²é²ÎÊý
	MG_RLT_SetMapBossFlag rltMsg;
	rltMsg.m_serverPlayerBossFlagData.m_nMapID = data.m_nMapID;
	rltMsg.m_serverPlayerBossFlagData.m_nCharTypeID = data.m_nCharTypeID;
	rltMsg.m_serverPlayerBossFlagData.m_bFlag = data.m_bFlag;

	CWildPrincesManager* pkMgr = CWildPrincesManager::GetSingletonPtr();
	rltMsg.m_nResult = pkMgr->CheckMapBossIDVaild(TYPE_GOLD_BOSS, data.m_nMapID, data.m_nCharTypeID);
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_SETMAPBOSSFLAG, &rltMsg);
		return;
	}
	
	for(int i=0; i<m_vServerPlayerBossFlagInfo.size(); ++i)
	{
		if(m_vServerPlayerBossFlagInfo[i].m_nMapID==data.m_nMapID && m_vServerPlayerBossFlagInfo[i].m_nCharTypeID==data.m_nCharTypeID)
		{
			m_vServerPlayerBossFlagInfo[i].m_bFlag = data.m_bFlag;
			pkOwnerPlayer->SendMsgToClient(MGPT_RLT_SETMAPBOSSFLAG, &rltMsg);
			return;
		}
	}

	m_vServerPlayerBossFlagInfo.push_back(data);
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_SETMAPBOSSFLAG, &rltMsg);
}

void CPlayerMapInfo::SyncMapBossFlagInfo(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SyncMapBossFlagInfo r1!");
		return;
	}

	MG_SyncMapBossFlagInfo msg;
	msg.m_vServerPlayerBossFlagInfo.reserve(m_vServerPlayerBossFlagInfo.size());
	for(int i=0; i<m_vServerPlayerBossFlagInfo.size(); ++i)
	{
		msg.m_vServerPlayerBossFlagInfo.push_back(m_vServerPlayerBossFlagInfo[i]);
	}
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_SETMAPBOSSFLAG, &msg);
}