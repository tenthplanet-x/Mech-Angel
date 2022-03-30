#include "ClientPCH.h"
#include "LC_ClientHelp.h"

#include "LC_PlayerBase.h"
#include "CF_WorldMapList.h"

#include "LC_ClientGamePlayer.h"

#include "NW_ClientSessionManager.h"
#include "SC_ClientScriptInterface.h"
#include "SC_ClientSkillInterface.h"

using namespace GameLogic;
using namespace CSVFile;

int LC_ClientHelp::CanEnterMap(int iMapResID, LC_ClientGamePlayer* pkPlayer, int& iRetValue)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (NULL == pkData)
	{
		return RE_FAIL;
	}

	//进入副本所需道具是否足够
	CF_TranscriptionRule::DataEntry* pkRuleData = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(iMapResID);
	if(pkRuleData != NULL)
	{
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		UT_SIMDataList::iterator iter = pkRuleData->_kConsumeItems.begin();
		for(; iter != pkRuleData->_kConsumeItems.end(); ++iter)
		{
			const UT_SIMDataInfo& info = *iter;
			if(pkPackAsset->GetCountCanDeleteInPack(info.ID()) < info.ParamA())
				return RE_TRANSCRIPTION_ITEM_LACK;
		}
	}

	ResultType result = RE_FAIL;

	bool bRelation = pkData->_needRelation;			//false所有都要满足 true满足任意条件
	
	int conditionCount = 0;
	int count = 0;

	//玩家等级需求
	if ( pkData->_iNeedLevel > 0 )
	{
		++conditionCount;

		if(pkPlayer->GetCommanderLevel() < pkData->_iNeedLevel)
		{
			iRetValue = pkData->_iNeedLevel;
			result = RE_TRANS_LEVEL_LOW;
		}
		else
		{
			++count;
		}
	}
	
	//前置任务
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (NULL != pkTaskMap && pkData->iPreTaskID != INVALID_TASK_ID)
	{
		++conditionCount;

		if (!pkTaskMap->IsInFinishedTaskMap(pkData->iPreTaskID))
		{
			iRetValue = pkData->iPreTaskID;
			result = RE_CANNOT_TRANSFERMAP_TASK_FAIL;
		}
		else
		{
			++count;
		}
	}

	//vip等级
	if ( pkData->_iVIPLevel > 0)
	{
		++conditionCount;

		if( pkPlayer->GetVIPLevel() < pkData->_iVIPLevel)
		{
			iRetValue = pkData->_iVIPLevel;
			result = RE_TRANS_VIP_LEVEL_LOW;
		}
		else
		{
			++count;
		}
	}

	//newVip等级
	if( pkData->_needNewVipLevel > 0 )
	{
		++conditionCount;

		if(pkPlayer->GetVipNewLevel() < pkData->_needNewVipLevel)
		{
			iRetValue = pkData->_needNewVipLevel;
			result = RE_TRANSCRIPTION_NEWVIPLEVEL_NOT_ENOUGH;
		}
		else
		{
			++count;
		}
	}

	//前置历练
	bool bAch = true;
	for(int i=0; i<pkData->_needAchievement.size(); ++i)
	{
		++conditionCount;

		int achievementID = pkData->_needAchievement[i];

		bool bFinish = pkPlayer->IsInFinishAchievementMap(achievementID)
			|| RE_SUCCESS == pkPlayer->CheckAchievementCanFinish(achievementID);
		
		if(bFinish)
			++count;
		else
			bAch = false;
	}

	if(!bAch)
		result = RE_TRANSCRIPTION_ACHIEVEMENT_NOT_ENOUGH;
	
	//检测条件和完成条件数量相等，一定符合条件
	if(conditionCount == count || (bRelation && count > 0))
	{
		return RE_SUCCESS;
	}
	//超过1个条件未符合显示特殊错误码
	else if((conditionCount - count) > 1)
	{
		return RE_TRANSPORT_MAP_CANT_TRANSPORT;
	}
	else
	{
		return result;
	}
}

bool GameLogic::LC_ClientHelp::CheckCronTimeStamp(const StringType& cronStr, int timeLen, uint32_t& uiBeginTime, uint32_t& uiEndTime)
{
	if (cronStr.empty())
	{
		return false;
	}
	Utility::UT_CronData kCron;
	kCron.ParseCronStr(cronStr);
	if(kCron.IsValid())
	{
		uint64_t iSystemTime = GET_SERVER_SYSTEM_TIME;
		time_t cur = (time_t)(iSystemTime);
		time_t p = cur;

		p -= timeLen*60;
		p++;
		if (p < 0)
		{
			return false;
		}
		time_t t = kCron.GetNextTime(p);

		uiEndTime = t + timeLen*60;
		uiBeginTime = t;

		if ((uint32_t)cur >= uiBeginTime && (uint32_t)cur < uiEndTime )
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------
StringType GameLogic::LC_ClientHelp::GetWeaponedResName(GameLogic::WeaponType eWeaponType, const char* pResName, bool bDefaultAsHand)
{
	return pResName;
}

std::string GameLogic::LC_ClientHelp::FilterGBKPlayerName(const std::string& inputStr)
{
	size_t iSize = SAFE_TRUNCATE_STRING<std::string::const_iterator>(inputStr.begin(), inputStr.end());

	std::string outputStr = inputStr;
	outputStr.resize(iSize);
	return outputStr;
}

bool GameLogic::LC_ClientHelp::IsCrossServerPlayer(LC_PlayerBase* pkPlayer)
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();

	int iGSEntryID = pkSessionGroup->GetCurGameServer();
	return iGSEntryID != pkPlayer->GetServerID();
}

bool LC_ClientHelpDiff::CrossSafe(int32_t iIndex, int32_t sServerID)
{							   
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();

	int iCurrentGameServerID = pkSessionGroup->GetCurGameServer();
	return sServerID == pkSessionManager->GetGameServerInfoIndex(iCurrentGameServerID, iIndex);
}

float LC_ClientHelpDiff::CalMasterBuffState(int32_t serverID , int64_t iScore, int32_t worldLevel)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if ( pkGamePlayer )
	{
		SC_ClientScriptInterface* csi = SC_ClientScriptInterface::GetSingletonPtr();
		return csi->GetSkillInterface()->CalcuteMultiply(worldLevel, iScore);
	}
	return 0;
}

bool LC_ClientHelpDiff::IsCrossPlayer(LC_PlayerBase* pkPlayer)
{
	return LC_ClientHelp::IsCrossServerPlayer(pkPlayer);
}