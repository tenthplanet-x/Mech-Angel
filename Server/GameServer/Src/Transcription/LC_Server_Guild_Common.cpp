#include "LC_Server_Guild_Common.h"
#include "UT_Timer.h"
#include "NW_Helper.h"
#include "UT_Redis.h"
#include "GlobalSettings.h"
#include "UT_ServerHelper.h"
#include "CF_BestGuildBossInfo.h"
#include "MG_Game2Guild.h"
#include "LC_ServerGuildManager.h"
#include "LC_Helper.h"
#include "CF_BestGuildBossRankRewards.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;
WatchTowerGuildInfo::WatchTowerGuildInfo(int nStoryID,int nTowerId )
{
	init();
	m_Id =  LC_Helper::EncodeScore(nStoryID,nTowerId);
	m_nState = WatchTowerType::WatchTower_Protocted;
}

void WatchTowerGuildInfo::init()
{
	m_Id = 0;
	m_GuildId = 0;
	m_MonsterId = 0;
	m_nMonsterHP = 0;
	m_nState = WatchTower_Protocted;

	m_Top3DamagePlayerName.clear();

	m_nPlayerName.clear();
	m_nLevel = 0;
	m_nTitleID = 0;
	m_nProfilePictureID = 0;
	m_nCommanderTitleFrameID = 0;

	m_sGuildName.clear();
	m_nDamagePlayerID.clear();
	m_nEndTimerID = 0;
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_MapId = 0;
	m_NextTimeStamp = 0;
	m_RewardTimeStamp = 0;
}

void WatchTowerGuildInfo::Clear()
{
	//m_Id = 0;
	m_GuildId = 0;
	//m_SafeTimeStamp = 0;
	//m_TempTimeStamp = 0;
	m_RewardTimeStamp = 0;
	m_nEndTimerID = 0;
	//m_sGuildLeaderName.clear();
	m_Top3DamagePlayerName.clear();
	//m_nGuildLeaderInfo.clear();
	m_nPlayerName.clear();
	m_nLevel = 0;
	m_nTitleID = 0;
	m_nProfilePictureID = 0;
	m_nCommanderTitleFrameID = 0;
	m_sGuildName.clear();
	m_nDamagePlayerID.clear();
	m_nState = WatchTowerType::WatchTower_Protocted;
}
void WatchTowerGuildInfo::CheckSendOwerReward()
{
	uint32_t CurrentTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(WatchTowerType::WatchTower_Owner != m_nState
	|| m_RewardTimeStamp > CurrentTimeStamp  || m_GuildId == 0)
		return;
	// 发送奖励
	//SendSingleReward();
	UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_SCORE, GetTowerID(), GetGuildID());
	m_RewardTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS() + GetGlobalSetting.BestGuildWatchTowerOwerRewardTime;
}
void WatchTowerGuildInfo::SetState(int nState,int nMapId,int nMonsterId,uint32_t uNextTimeStamp)
{
	int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(currentTime < m_NextTimeStamp)
	{
		return;
	}

	if(WatchTowerType::WatchTower_AllKilled == nState||WatchTowerType::WatchTower_Protocted == nState)
	{
		// 清除 towerid
		if(GetGuildID() > 0)
		{
			UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_BUFF,GetTowerID(), GetGuildID(), 1,m_NextTimeStamp,false);
		}
		Clear();
	}	
	
	m_MapId = nMapId;
	m_nState = nState;
	m_MonsterId	= nMonsterId;

	//uNextTimeStamp==-1,开始争夺，没有时间限制
	if(uNextTimeStamp == -1)
	{
		m_NextTimeStamp = 0;
	}
	else
	{
		m_NextTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS() + uNextTimeStamp;
	}

	if(WatchTowerType::WatchTower_Owner == nState)
	{
		if(GetGuildID() > 0)
		{
			m_RewardTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS() + GetGlobalSetting.BestGuildWatchTowerOwerRewardTime;
			UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_SCORE,GetTowerID(), GetGuildID());
			// 补发 towerid
			UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_BUFF,GetTowerID(), GetGuildID(), 2,m_NextTimeStamp,true);
		}
	}

	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "WatchTowerGuildInfo SetState nState:%d, nMapId:%d, nMonsterId:%d, uNextTimeStamp:%d, guildID:%lld, towerID:%d, m_NextTimeStamp:%d, m_RewardTimeStamp:%d", 
		nState, nMapId, nMonsterId, uNextTimeStamp, GetGuildID(), GetTowerID(), m_NextTimeStamp, m_RewardTimeStamp);
}

uint32_t WatchTowerGuildInfo::GetTowerID()
{
	return (uint32_t)(0x00000000FFFFFFFF&m_Id);
}

uint32_t WatchTowerGuildInfo::GetStoryID()
{
	return (uint32_t)(m_Id>>32);
}

void WatchTowerGuildInfo::EntryTempOwer(uint64_t GuildId,std::string & sGuildName)
{
	m_GuildId = GuildId;
	//m_sGuildLeaderName = sGuildLeaderName;
	m_sGuildName = sGuildName;

	//m_nState = WatchTowerType::WatchTower_Temp;
	//m_TempTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS() + GetGlobalSetting.BestGuildWatchTowerTempTime;

}

void WatchTowerGuildInfo::SetGuildLeaderInfo(GuildServerMember* pkMaster)
{
	if(NULL == pkMaster)
	{
		return;
	}

	m_nLevel = pkMaster->m_Level;
	m_nPlayerName = TPSTR2STDSTR(pkMaster->m_CharName);
	m_nProfilePictureID = pkMaster->m_nProfilePictureID==0 ? pkMaster->m_Career : pkMaster->m_nProfilePictureID;
	m_nTitleID = pkMaster->m_nTitleID==0 ? 1 : pkMaster->m_nTitleID;
	m_nCommanderTitleFrameID = pkMaster->m_nCommanderTitleFrameID==0 ? 1 : pkMaster->m_nCommanderTitleFrameID;
}

void WatchTowerGuildInfo::UpdateGuildDamagePlayerInfo(GuildServerMember* pkMaster)
{
	if(NULL == pkMaster)
	{
		return;
	}

	m_Top3DamagePlayerName.push_back(TPSTR2STDSTR(pkMaster->m_CharName));
}

void WatchTowerGuildInfo::SetMonsterHP(int64_t hp)
{
	m_nMonsterHP = hp;
}

void WatchTowerGuildInfo::SendOfflineReward(int vaildTime, bool updateTime)
{
	if(0 == GetGlobalSetting.BestGuildWatchTowerOwerRewardTime)
	{
		return;
	}

	int t = vaildTime / GetGlobalSetting.BestGuildWatchTowerOwerRewardTime;
	if(t < 1)
	{
		return;
	}

	GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_INFO, "SendOfflineReward times:%d. guildID:%lld, towerID:%d", t, GetGuildID(), GetTowerID());
	UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_SCORE, GetTowerID(), GetGuildID(), t);
	
	if(updateTime)
	{
		if(0 == m_RewardTimeStamp)
		{
			m_RewardTimeStamp = m_NextTimeStamp+ (t+1)*GetGlobalSetting.BestGuildWatchTowerOwerRewardTime;
		}
		else
		{
			m_RewardTimeStamp += (t+1)*GetGlobalSetting.BestGuildWatchTowerOwerRewardTime;
		}
	}
}

void WatchTowerGuildInfo::ClearDamageInfo()
{
	m_Top3DamagePlayerName.clear();
	m_nDamagePlayerID.clear();
}

void WatchTowerGuildInfo::UpdateDamagePlayerID(uint64_t playerID)
{
	m_nDamagePlayerID.push_back(playerID);
}

const std::vector<uint64_t>& WatchTowerGuildInfo::GetDamagePlayerVector()
{
	return m_nDamagePlayerID;
}
void WatchTowerGuildInfo::PackTowerInfo(ST_Single_Tower_info & info)
{
	info.m_uTowerID = GetTowerID();
	info.m_GuildId = m_GuildId;
	info.m_nState = m_nState;

	info.m_nGuildLeaderInfo.m_nCommanderTitleFrameID = m_nCommanderTitleFrameID;
	info.m_nGuildLeaderInfo.m_nLevel = m_nLevel;
	info.m_nGuildLeaderInfo.m_nPlayerName = m_nPlayerName;
	info.m_nGuildLeaderInfo.m_nProfilePictureID = m_nProfilePictureID;
	info.m_nGuildLeaderInfo.m_nTitleID = m_nTitleID;

	for(int i=0; i<m_Top3DamagePlayerName.size(); ++i)
	{
		info.m_Top3DamagePlayerName.push_back(m_Top3DamagePlayerName[i]);
	}

	info.m_MonsterHP = m_nMonsterHP;

	info.m_sGuildName = m_sGuildName;
	info.m_NextTimeStamp = m_NextTimeStamp;
	info.m_MonsterId = m_MonsterId;
	info.m_MapId	 = m_MapId;
	info.m_x = m_x;
	info.m_y = m_y;
	info.m_z = m_z;
}


int WatchTowerGuildInfo::GetTimerID()
{
	return m_nEndTimerID;
}

void WatchTowerGuildInfo::SetTimerID(int timerID)
{
	m_nEndTimerID = timerID;
}


void WatchTowerGuildInfo::SendDamageReward()
{
	const std::vector<uint64_t>& rkList = GetDamagePlayerVector();
	for(int i=0; i<rkList.size(); ++i)
	{
		uint64_t playerID = rkList[i];

		int towerID = GetTowerID();
		CF_CharType::DataEntry* pkCharDataEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(towerID);
		CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(m_MonsterId);
		if (NULL==pkCharDataEntry || NULL==pkEntry)
		{
			return;
		}
		int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE2);
		CF_BestGuildBossRankRewards::DataEntryMapExternIterator kIter = CF_BestGuildBossRankRewards::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_BestGuildBossRankRewards::DataEntry* pkData = kIter.PeekNextValuePtr();
			if(pkData->_iType==pkEntry->_iType && pkData->_iRankStartIndex<=i && i<pkData->_iRankEndIndex)
			{
				stringstream ss;
				ss<< towerID << ";";
				
				stringstream index;
				index << (i+1) << ";";

				StringType mailTitle = pkData->_iMailTitle + STDSTR2TPSTR(ss.str());
				StringType mailDesc = pkData->_iMailDesc + STDSTR2TPSTR(ss.str()) + STDSTR2TPSTR(index.str());

				int nExtraParam = pkData->_iType * 1000 + i+1;				
				ResultType res =  UT_ServerHelper::SendToMail(mailTitle, mailDesc, playerID, "", 0, pkData->_kReward, "", nLogCode,nExtraParam);
				if(RE_SUCCESS != res)
				{
					MailInfo info;
					info.SetMailInfo(mailTitle, mailDesc, playerID, pkData->_kReward);
					info.nLogCode = nLogCode;
					info.nExtraParam = nExtraParam;
					UT_ServerHelper::AddMail(info);
					continue;
				}
				break;
			}
		}		
	}
}