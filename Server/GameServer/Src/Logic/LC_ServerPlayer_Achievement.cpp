#include "LC_ServerPlayer.h"
#include "SystemError.h"
#include "LC_AchievementAsset_New.h"
#include "MG_SyncProtocol.h"
#include "GlobalSettings.h"
#include "LC_ServerGuildManager.h"
#include "LC_GameEventManager.h"
#include "UT_OperateLog.h"
#include "ACT_ActivityDataManager.h"
using namespace  OPERATELOG;

ResultType LC_ServerPlayer::AddAchievementReward(int32_t iAchievementID)
{
	//参数检测
	CAchievementManager* pkMgr = CAchievementManager::GetSingletonPtr();
	const LC_AchievementDataEntry_New* pkEntry = pkMgr->GetAchievementEntryByID(iAchievementID);
	if(NULL == pkEntry)
	{
		return RE_FAIL;
	}

	//活动是否有效
	switch(pkEntry->_iAchievementType)
	{
	case ACHIEVEMENT_TRIAL:
		{
			//check活动是否开启
			if(!CheckSevenDayTime2Vaild())
			{
				return RE_ACTIVITY_NOT_VALID;
			}
		}
		break;
	case ACHIEVEMENT_INVESTMENT:
		{
			bool tag = false;
			CF_InvestmentPlan::DataEntry* pkData = NULL;
			CF_InvestmentPlan::DataEntryMapExternIterator kIter = CF_InvestmentPlan::GetSingletonPtr()->GetIterator();
			for(; kIter.HasMoreElements(); kIter.MoveNext())
			{
				pkData = kIter.PeekNextValuePtr();
				const Utility::UT_SIMDataList& achievementID = pkData->_sAchievementID;
				for(int i=0; i<achievementID.size(); ++i)
				{
					if(achievementID[i].IID() == iAchievementID)
					{
						tag = true;
						break;
					}
				}

				if(tag)
				{
					break;
				}
			}

			if(!tag || NULL==pkData)
			{
				return RE_XLSX_ERROR;
			}

			//check活动是否开启
			if(!UT_ServerHelper::CheckActivityVaild(pkData->_iSchedule))
			{
				return RE_ACTIVITY_NOT_VALID;
			}
		}
		break;
	case ACHIEVEMENT_BESTGUILD:
		{
			//check是否在领奖时间内
			if(!UT_ServerHelper::CheckActivityVaild(pkEntry->_iScheduleID))
			{
				return RE_ACTIVITY_NOT_VALID;
			}
		}
		break;
	default:
		break;
	}

	//是否可以完成
	ResultType res = CheckAchievementCanFinish(iAchievementID);
	if(RE_SUCCESS != res)
	{
		return res;
	}
	//成就奖励--埋点日志
	int nLogCode = 0;
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(iAchievementID);
	switch(pkEntry->_iAchievementType)
	{
	case ACHIEVEMENT_NORMAL://普通成就
		{
		}break;
	case ACHIEVEMENT_BIOGRAPHY://传记成就
		{

		}break;
	case ACHIEVEMENT_TASK://任务成就
		{

		}break;
	case ACHIEVEMENT_TRIAL://七日试炼
		{

		}break;
	case ACHIEVEMENT_COMMANDER_TITLE://头衔类
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE,TITLES_OPERATE, OPERATE_LOGTYPE1);
		}
		break;
	case ACHIEVEMENT_INVESTMENT://投资计划类
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE,ACTIVITY14_OPERATE, OPERATE_LOGTYPE2);
		}
		break;
	case ACHIEVEMENT_SPECIALTRAINING://指挥官特训
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE,SPECIALTRAIN_OPERATE, OPERATE_LOGTYPE1);
		}
		break;
	case ACHIEVEMENT_BESTGUILD://最强公会历练(荣誉挑战)
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, HONOURCHALLENG_OPERATE, OPERATE_LOGTYPE1);
		}
		break;
	case ACHIEVEMENT_NEW_ACHIEVEMENT://新成就系统类
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE,ACHIEVEMENT_OPERATE, OPERATE_LOGTYPE1);
		}
		break;
	default:
		break;
	}

	//发奖
	if(!pkEntry->_sRewardItems.empty())
	{
		res = LC_Helper::AddItemByString(GetPackAsset(), pkEntry->_sRewardItems, CIET_NONE, true, nLogCode, nParams);
	}
	
	//更新已完成记录
	if(RE_SUCCESS == res)
	{
		GetAchievementAsset().UpdateAchievementID(pkEntry);

		Activity::ACT_ScheduleDataManager* pkACTDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
		if (NULL != pkACTDataMgr && pkEntry->_iScheduleID)
		{
			pkACTDataMgr->OnFinishTaskAndAchievement(this, 2, iAchievementID);
		}
	}
	

	return res;
}

ResultType LC_ServerPlayer::AddAchievementPointReward(int32_t rewardID)
{
	return GetAchievementAsset().AddAchievementPointReward(rewardID);
}

ResultType LC_ServerPlayer::AddSevenDayPointReward(int32_t rewardID)
{
	//check活动是否开启
	if(!CheckSevenDayTime2Vaild())
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	return GetAchievementAsset().AddSevenDayPointReward(rewardID);
}

void LC_ServerPlayer::PostGetAchievement(const LC_AchievementDataEntry_New* pkEntry)
{
	if (NULL == pkEntry)
	{
		return;
	}
	
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pkEntry->_iShow == 0 && pOwnerPlayer) 
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(44);
		noticeFmort.AddPlayerInfo(pOwnerPlayer->GetInstanceUniqueID(), GetOwnerCharName());
		noticeFmort.AddAchievementName(pkEntry->_lID);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
	}

	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	{
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent != NULL)
		{
			pkGameEvent->SetEventType(GLET_ADD_ACHIEVEMENT);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
			pkGameEvent->SetEventParam(EP_ADD_ACHIEVEMENT_PLAYER_ID, GetID());
			pkGameEvent->SetEventParam(EP_ADD_ACHIEVEMENT_ID, pkEntry->_lID);
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
}

void LC_ServerPlayer::UpdateAchievementRecord(int32_t iAchievementtype, AchievementEvent& kEvent)
{
	if(iAchievementtype <= LC_ACHIEVEMENT_INDEX_DEFAULT || iAchievementtype>LC_ACHIEVEMENT_TYPE_MAX_COUNT)
	{
		return;
	}

	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return;

	if(!IsMainController())
	{
		return m_pOwnerPlayer->UpdateAchievementRecord(iAchievementtype, kEvent);
	}

	GetAchievementAsset().UpdateAchievementRecord(iAchievementtype, kEvent);
}

void LC_ServerPlayer::SyncFinishAchievement(int32_t id)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	if(!IsMainController())
	{
		return pkOwnerPlayer->SyncFinishAchievement(id);
	}

	MG_SyncFinishAchievement msg;
	msg.m_nAchievementID = id;
	SendMsgToClient(MGPT_SYNCFINISHACHIEVEMENT, &msg);
}

void LC_ServerPlayer::SyncAchievementRecord(int32_t groupID, uint64_t value)
{
	if(0 == value)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	if(!IsMainController())
	{
		return pkOwnerPlayer->SyncAchievementRecord(groupID, value);
	}

	MG_SyncAchievementRecord msg;
	msg.m_nGroupID = groupID;
	msg.m_nValue = value;
	SendMsgToClient(MGPT_SYNCACHIEVEMENTRECORD, &msg);	
}