#include "ACT_Subordinate.h"
#include "ACT_ActivitySchedule.h"

#include "LC_SubordinateAsset.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"
#include "UT_Redis.h"
#include "GameServerApp.h"
#include "GlobalSettings.h"
#include "MG_Game2Guild.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_SubordinateRewardDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int nLogCode, const vector<int>& nParams)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DoReward m_iID:%d r1", m_iID);
		return RE_FAIL;
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	ResultType uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		rkExtend.SetChoice(m_kRewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kRewardItems.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			//开服争霸榜，背包空间不足，领奖失败
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SUBORDINATE
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable, false, false, nLogCode, nParams);
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DoReward m_iID:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, uiRslt:%d r2", 
				m_iID, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), uiRslt);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}


struct SubLevelUPScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_SubordinateManager::DataEntryType>
{
	SubLevelUPScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_SubordinateManager::DataEntryType>(schedule, player, asset, act, cat)
		,iLevel(0)
	{}

	virtual bool operator ()(ACT_SubordinateManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_SubordinateManager::DataEntryType* pkRewardDataEntry = pkSchedule->GetDataEntry(iLevel);
			if (pkRewardDataEntry)
			{
				ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
				if (pkSaveEntry)
				{
					pkSaveEntry->RegFire(pkRewardDataEntry->m_iSaveIndex);
					pkSaveCatagory->UpdateDirtyFlag();
					pkSaveActivity->UpdateDirtyFlag();
					pkAsset->UpdateDirtyFlag();
					return true;
				}
			}
		}
		return false;
	}
	int32_t iLevel;
};

void ACT_SubordinateManager::OnSubUpgrade(LC_ServerPlayer* pkPlayer, int32_t iSubType, int32_t iSubLvl, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL==pkPlayer || !IS_VALID_SUBORDINATE_BASE_TYPE(iSubType))
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	Activity::ACT_ScheduleSaveCatagory*	pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iSubType);
	if (NULL==pkSaveActivity || NULL==pkSaveCatagory)
	{
		return;
	}

	SubLevelUPScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, pkSaveCatagory);
	kOper.iLevel = iSubLvl;

	ACT_SubordinateManager::CatagoryDataType* pkCatagory = GetCatagoryData(iSubType);
	if (pkCatagory)
	{
		pkCatagory->ForAll(&kOper);
	}
}
ResultType ACT_SubordinateManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& iCount)
{
	if (NULL==pkPlayer)
		return RE_FAIL;

	DataEntryType* pkRewardDataEntry = GetDataEntry(iType, iSchedule, iKey);
	if (NULL == pkRewardDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(pkPlayer->GetServerID(), iSchedule);
	if(NULL == pkSchedule)
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iType);
	if (NULL == pkSaveCatagory)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return RE_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}

	CF_ACTSubordinateRewardA::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTSubordinateRewardA>(pkRewardDataEntry->m_iID);
	if(pkData == NULL)
	{
		return RE_FAIL;
	}

	PlayerACTSubInfo info;
	bool tag = false;
	//基础竞技榜活动
	if(pkData->_bHighReward == 0)
	{		
		//判断基础竞技榜活动是否已经结束
		if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), GetGlobalSetting.ACTSubordinateScheduleID))
		{
			pkPlayer->GetActSubPlayerInfoCache(info);
			tag = true;
		}
	}
	else if(pkData->_bHighReward == 1)
	{		
		//检测所有的基础竞技榜活动是否已经全部达成
		bool tag = UT_ServerHelper::CheckDoReward(pkRewardDataEntry->m_iID, pkPlayer);
		if(!tag)
		{
			return RE_ACTIVITY_PLUS_NOT_OPEN;
		}

		//进阶竞技榜活动
		//判断进阶竞技榜活动是否已经结束
		if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), GetGlobalSetting.ACTSubordinateScheduleID_Plus))
		{
			pkPlayer->GetActSubPlusPlayerInfoCache(info);
			tag = true;
		}
	}
	
	if(tag)
	{
		uint64_t value = 0;
		switch(iType)
		{
		case Rank_Type_Level:
			value = info.controller_level;
			break;
		case Rank_Type_Battle:
			value = info.controller_combatScore;
			break;
		case Rank_Type_Vehicle:
			value = info.subvehicle_combatscore;
			break;
		case Rank_Type_Devaeye:
			value = info.subdevaeye_combatscore;
			break;
		case Rank_Type_Wing:
			value = info.subwing_combatscore;
			break;
		case Rank_Type_Pet:
			value = info.pet_combatscore;
			break;
		default:
			return RE_FAIL;
		}

		if(value < iKey)
		{
			return RE_ACTIVITY_CONDITION_FAIL;
		}	
	}
	else
	{
		//活动未结束，从玩家实体拿数据
		uint64_t result = 0;	
		switch(iType)
		{
		case Rank_Type_Level:			//等级
			result = pkPlayer->GetLevel();
			break;
		case Rank_Type_Battle:			//战斗力
			result = pkPlayer->GetControllerCombatScore(); 
			break;
		case Rank_Type_Vehicle:			//坐骑
			result = pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_VEHICLE);//坐骑战斗力
			break;
		case Rank_Type_Devaeye:			//宠物
			result = pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_DEVA_EYE);//宠物战斗力
			break;
		case Rank_Type_Wing:			//神翼
			result = pkPlayer->GetSumSubSystemCombatScore(LC_SUB_TP_MUHUN);//背饰战斗力
			break;
		case Rank_Type_Pet:
			result = pkPlayer->GetTotalPetCombatScore();//玩具总战力	
			break;
		default:
			break;
		}

		if(result < iKey)
		{
			return RE_ACTIVITY_CONDITION_FAIL;
		}
	}
	
	bool flag = false;
	int num = UT_ServerHelper::GetLimitRewardRecordByID(pkRewardDataEntry->m_iID);
	if(num < pkData->_iGetMaxCounts)
	{
		uint32_t id = pkPlayer->GetOwnerPlayerId();
		mem::map<int, mem::set<uint32_t> >::iterator findIt = UT_ServerHelper::subRewardPlayerIDs.find(pkRewardDataEntry->m_iID);
		if(findIt == UT_ServerHelper::subRewardPlayerIDs.end())
		{
			mem::set<uint32_t> myset;
			myset.insert(id);
			UT_ServerHelper::subRewardPlayerIDs.insert(std::make_pair(pkRewardDataEntry->m_iID, myset));
			flag = true;
		}
		else
		{
			mem::set<uint32_t>::iterator it = findIt->second.find(id);
			if(it == findIt->second.end())
			{
				findIt->second.insert(id);
				flag = true;
			}			
		}

		if(flag)
		{
			MG_REQ_GetLimitReward msg;
			msg.m_nEntryServerID = GetGameServerEntryID();
			msg.m_nKey = pkRewardDataEntry->m_iID;
			msg.m_iType = iType;
			msg.m_iSchedule = iSchedule;
			msg.m_iKey = iKey;
			msg.m_iChoice = rkExtend.m_iChoice;
			msg.m_iActivetyType = GetType();
			SendMsgToSociety(MGPT_GGU_REQ_GETLIMITREWARD, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		}
	}
	ResultType rslt = num >= pkData->_iGetMaxCounts ? RE_ACTIVITY_REWARD_OUT : RE_SUCCESS;
	return rslt;
}

ResultType ACT_SubordinateManager::DrawLimitReward(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, int iCount)
{
	DataEntryType* pkRewardDataEntry = GetDataEntry(iType, iSchedule, iKey);
	if (NULL == pkRewardDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}

	CF_ACTSubordinateRewardA::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTSubordinateRewardA>(pkRewardDataEntry->m_iID);
	if(pkData == NULL)
	{
		return RE_FAIL;
	}

	if(iCount > pkData->_iGetMaxCounts)
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iType);
	if (NULL == pkSaveCatagory)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return RE_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}

	ResultType rslt = pkRewardDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		uint32_t id = pkPlayer->GetOwnerPlayerId();
		mem::map<int, mem::set<uint32_t> >::iterator findIt = UT_ServerHelper::subRewardPlayerIDs.find(pkRewardDataEntry->m_iID);
		if(findIt != UT_ServerHelper::subRewardPlayerIDs.end())
		{
			findIt->second.erase(id);
		}

		pkSaveSchedule->RegReward(pkRewardDataEntry->m_iSaveIndex);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return rslt;
}

ResultType ACT_SubordinateManagerB::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& iCount)
{
	if (NULL==pkPlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d r1", iType, iSchedule, iKey);
		return RE_FAIL;
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	//背包是否满
	bool full = pkPlayer->GetPackAsset()->IsPackFull(PACK_TYPE_BASIC);
	if(true == full)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d r2", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_ITEM_PACKAGE_FULL;
	}

	DataEntryType* pkRewardDataEntry = GetDataEntry(iType, iSchedule, iKey);
	if (NULL == pkRewardDataEntry)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d r3", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_ACTIVITY_BAD_PARAMS;
	}

	CF_ACTSubordinateRewardB::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTSubordinateRewardB>(pkRewardDataEntry->m_iID);
	if(NULL == pkData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, pkRewardDataEntry->m_iID:%d r4", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkRewardDataEntry->m_iID);
		return RE_ACTIVITY_BAD_PARAMS;
	}

	bool finish = true;
	//先检查活动有效时间
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		finish = false;
	}

	//领奖，检测_iFinishSchedule时间
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkData->_iFinishSchedule))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, finish:%d r5", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), finish);
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(pkPlayer->GetServerID(), iSchedule);
	if(NULL == pkSchedule)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, finish:%d, serverid:%d r6", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), finish, pkPlayer->GetServerID());
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, finish:%d, GetType:%d r7", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), finish, GetType());
		return RE_FAIL;
	}

	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iType);
	if (NULL == pkSaveCatagory)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, finish:%d r8", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), finish);
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d r9", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iSaveIndex))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, m_iSaveIndex:%d r10", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkRewardDataEntry->m_iSaveIndex);
		return RE_ACTIVITY_REWARDED;
	}

	uint64_t result = 0;	
	if(finish)
	{
		result = pkPlayer->GetSubBSendValue(pkData->_GroupID);
	}
	else
	{
		result = pkPlayer->GetSubBValue(pkData->_GroupID);
	}

	if(result < iKey)
	{	
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, GroupID:%d, result:%d r11", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkData->_GroupID, result);
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	bool flag = false;
	int num = UT_ServerHelper::GetLimitRewardRecordBByID(pkRewardDataEntry->m_iID);
	if(num < pkData->_iGetMaxCounts)
	{
		uint32_t id = pkPlayer->GetOwnerPlayerId();
		mem::map<int, mem::set<uint32_t> >::iterator findIt = UT_ServerHelper::subBRewardPlayerIDs.find(pkRewardDataEntry->m_iID);
		if(findIt == UT_ServerHelper::subBRewardPlayerIDs.end())
		{
			mem::set<uint32_t> myset;
			myset.insert(id);
			UT_ServerHelper::subBRewardPlayerIDs.insert(std::make_pair(pkRewardDataEntry->m_iID, myset));
			flag = true;
		}
		else
		{
			mem::set<uint32_t>::iterator it = findIt->second.find(id);
			if(it == findIt->second.end())
			{
				findIt->second.insert(id);
				flag = true;
			}			
		}

		if(flag)
		{
			MG_REQ_GetLimitReward_B msg;
			msg.m_nEntryServerID = GetGameServerEntryID();
			msg.m_nKey = pkRewardDataEntry->m_iID;
			msg.m_iType = iType;
			msg.m_iSchedule = iSchedule;
			msg.m_iKey = iKey;
			msg.m_iChoice = rkExtend.m_iChoice;
			msg.m_iActivetyType = GetType();
			SendMsgToSociety(MGPT_GGU_REQ_GETLIMITREWARD_B, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
			
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, m_nEntryServerID:%d, m_nKey:%d, m_iChoice:%d, m_iActivetyType:%d", 
				iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), msg.m_nEntryServerID, pkRewardDataEntry->m_iID, msg.m_iChoice, msg.m_iActivetyType);
		}
	}
	ResultType rslt = num >= pkData->_iGetMaxCounts ? RE_ACTIVITY_REWARD_OUT : RE_SUCCESS;

	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawReward iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, flag:%d, num:%d, MaxCounts:%d, rslt:%d r12", 
		iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), flag, num, pkData->_iGetMaxCounts, rslt);
	return rslt;
}

ResultType ACT_SubordinateManagerB::DrawLimitReward(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, int iCount)
{
	if(NULL == pkPlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d r0", iType, iSchedule, iKey);
		return RE_FAIL;
	}

	const unique_id_impl& instance = pkPlayer->GetInstance();
	DataEntryType* pkRewardDataEntry = GetDataEntry(iType, iSchedule, iKey);
	if (NULL == pkRewardDataEntry)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d r1", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_ACTIVITY_BAD_PARAMS;
	}

	CF_ACTSubordinateRewardB::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTSubordinateRewardB>(pkRewardDataEntry->m_iID);
	if(pkData == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, m_iID:%d r2", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkRewardDataEntry->m_iID);
		return RE_FAIL;
	}

	if(iCount > pkData->_iGetMaxCounts)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, m_iID:%d, iCount:%d, GetMaxCounts:%d r3", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkRewardDataEntry->m_iID, iCount, pkData->_iGetMaxCounts);
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, GetType:%d r4", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), GetType());
		return RE_FAIL;
	}

	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iType);
	if (NULL == pkSaveCatagory)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d r5", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d r6", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return RE_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iSaveIndex))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d c_cid:%d, c_citizenship:%d, c_uid:%d, m_iSaveIndex:%d r7", 
			iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkRewardDataEntry->m_iSaveIndex);
		return RE_ACTIVITY_REWARDED;
	}

	//埋点
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(pkRewardDataEntry->m_iID);
	int nLogCode = 0;
	nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE1);
	/*switch (iType)
	{
	case Sub_Reward_Type_Vehicle:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE1);
			break;
		}
	case Sub_Reward_Type_Wing:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE3);
			break;
		}
	case Sub_Reward_Type_Devaeye:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE5);
			break;
		}
	case Sub_Reward_Type_CommanderTitle:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE9);
			break;
		}
	case Sub_Reward_Type_CommanderLevel:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE11);
			break;
		}
	case Sub_Reward_Type_AllRuneItemCombatScore:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE13);
			break;
		}
	case Sub_Reward_Type_RuneItemCombatScore:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY18_OPERATE, OPERATE_LOGTYPE7);
			break;
		}
	}*/
	ResultType rslt = pkRewardDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend, nLogCode, nParams);
	if (RE_SUCCESS == rslt)
	{
		uint32_t id = pkPlayer->GetOwnerPlayerId();
		mem::map<int, mem::set<uint32_t> >::iterator findIt = UT_ServerHelper::subBRewardPlayerIDs.find(pkRewardDataEntry->m_iID);
		if(findIt != UT_ServerHelper::subBRewardPlayerIDs.end())
		{
			findIt->second.erase(id);
		}

		pkSaveSchedule->RegReward(pkRewardDataEntry->m_iSaveIndex);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}

	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DrawLimitReward, iType:%d, iSchedule:%d, iKey:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, rslt:%d r8", 
		iType, iSchedule, iKey, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), rslt);
	return rslt;
}