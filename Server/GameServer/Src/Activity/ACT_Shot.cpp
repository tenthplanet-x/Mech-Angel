#include "ACT_Shot.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ShotActivity.h"

#include "UT_ServerHelper.h"

#include "GameServerApp.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;


int32_t ACT_ShotDataEntry::Catagory(void) const 
{
	return m_iRankType;
}

int32_t ACT_ShotDataEntry::Schedule(void) const 
{
	return m_iSchdeule;
}

int32_t ACT_ShotDataEntry::Key(void) const 
{
	return m_iKey;
}


ResultType ACT_ShotDataEntry::CheckReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		if (!m_bMailEnable)
		{
			/*
			//奖励全部领取，先屏掉这段
			rkExtend.SetChoice(m_kRewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
			LC_RewardItemsEntry::PackSizeMap kPackSizeMap;
			m_kRewardItems.PackSize(pkPlayer, kPackSizeMap, PACK_TYPE_UNDEFINE, &rkExtend);
			mem::map<int32_t, int32_t>::iterator it = kPackSizeMap.begin();
			for (; it != kPackSizeMap.end(); ++it)
			{
				if (pkPlayer->GetPackAsset()->GetPackEmptyEntryCount(it->first) < it->second)
				{
					uiRslt = RE_ITEM_REWARD_PACKAGE_FULL;
					break;
				}
			}
			*/
		}
	}while(0);

	return uiRslt;
}
ResultType ACT_ShotDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	/*
	do
	{
		//奖励全部领取，先屏掉这段
		Utility::UT_SIMDataList kItems;
		rkExtend.SetChoice(m_kRewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kRewardItems.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SHOT
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);
	*/

	return uiRslt;
}
void ACT_ShotManager::GetValidScheduleIDs(int32_t iServerID, int32_t iCatagory, GameLogic::LC_IDVector& rkScheduleIDs)
{
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	ACT_ShotManager::CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
	if (pkCatagory)
	{
		ACT_ScheduleValidPeekOper<ACT_ShotManager::DataEntryType> kOper(pkScheduleMgr, iServerID, rkScheduleIDs);
		pkCatagory->ForAll(&kOper);
	}
}
ResultType ACT_ShotManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t, int32_t, int32_t iShotID, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	//ACT_SHOT由服务器发奖，客户端的所有申请领取都无效
	return RE_FAIL;
}
