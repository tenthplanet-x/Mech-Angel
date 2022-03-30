#include "ACT_Shop.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"
#include "oss_define.h"
#include "oss_360_define.h"
#include "LC_ServerGuildManager.h"
#include "LC_GuildLadderManager.h"
#include "UT_OperateLog.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;
using namespace OPERATELOG;

int32_t ACT_ShopDataEntry::Catagory(void) const 
{
	return m_iType;
}

int32_t ACT_ShopDataEntry::Schedule(void) const 
{
	return m_iSchdeule;
}

int32_t ACT_ShopDataEntry::Key(void) const 
{
	return m_iKey;
}

ResultType ACT_ShopDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iDrawCount, uint32_t& nLogItemId, int32_t& nLogItemCount, int nLogCode,const vector<int>& nParams)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	Utility::UT_SIMDataList kItems;
	do
	{
		rkExtend.SetChoice(m_kItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kItems.Resolve(pkPlayer, kItems, &rkExtend);
		Utility::UT_SIMDataList::iterator it = kItems.begin();
		for (; it != kItems.end(); ++it)
		{
			Utility::UT_SIMDataInfo& DataEntry = *it;
			DataEntry.ItemCount(DataEntry.ItemCount() * iDrawCount);
			nLogItemId = DataEntry.GetItemId();
			nLogItemCount = DataEntry.ItemCount();
		}
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SHOP
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable, false,false, nLogCode,nParams);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL!=pOwnerPlayer && RE_SUCCESS == uiRslt && m_kCurrentPrices.size()>0 && kItems.size()>0)
	{
		//m_iType=126是兑换活动，要发公告
		if(m_iType == 126)
		{
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID(31);
			noticeFmort.AddPlayerInfo(pOwnerPlayer->GetInstanceUniqueID(), pkPlayer->GetOwnerCharName());
			noticeFmort.AddItemName(m_kCurrentPrices.begin()->first);
			noticeFmort.AddNumber(m_kCurrentPrices.begin()->second);
			noticeFmort.AddItemName(kItems[0].IID());
			noticeFmort.AddNumber(kItems[0].ItemCount());
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
		}
	}

	return uiRslt;
}

ResultType ACT_ShopDataEntry::CheckConditions(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkSaveSchedule)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;

	bool tag = pkPlayer->CheckShopVaild(m_iType, m_iID);
	if(!tag)
	{
		return RE_SHOP_CONDITION_BE_LIMIT;
	}

	if(m_nGuildLevel != 0)
	{
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		if (pkGuildManager == NULL)
		{
			return RE_FAIL;
		}
		LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
		if (guild == NULL)
		{
			return RE_FAIL;
		}
		LC_GuildLadderManager* pkLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
		if (NULL == pkLadderMgr)
		{
			return RE_FAIL;
		}

		if(guild->GetLevel() <m_nGuildLevel)
		{
			return RE_GUILD_LEVEL_NOTENOUGH;
		}
	}

	Utility::UT_SIMDataList::iterator it = m_kBuyConditions.begin();
	for (; it != m_kBuyConditions.end(); ++it)
	{
		switch (it->ID())
		{
			case ACT_SHOP_COND_DAILY_CHARGE:
				if ((int32_t)pkPlayer->GetCommonStatAsset().GetCommonStatBasic(LC_ST_TP_DAILY)->GetRecharge() < it->ParamA())
				{
					return RE_ACTIVITY_CONDITION_FAIL;
				}
				break;
			case ACT_SHOP_COND_VIP:
				if ((int32_t)pkPlayer->GetVIPLevel() < it->ParamA())
				{
					return RE_ACTIVITY_CONDITION_FAIL;
				}
				break;
			case ACT_SHOP_COND_ARNENA_SEC:
				{
					LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->GetArenaEntry(LC_ARENA_TP_SOLO);
					if(NULL == pkArenaEntry)
					{
						return RE_ACTIVITY_CONDITION_FAIL;	
					}
					
					CSVFile::CF_ArenaSection::DataEntry* pkDataEntry = GetArenaSectionEntry(pkArenaEntry->GetScore());
					if (NULL==pkDataEntry || pkDataEntry->_iID < it->ParamA())
					{
						return RE_ACTIVITY_CONDITION_FAIL;	
					}
				}
				break;
			case ACT_SHOP_COND_CHARGE:
				{
					if (NULL == pkSaveSchedule)
					{
						return RE_FAIL;
					}
					if((int32_t)pkSaveSchedule->GetCount() < it->ParamA())
					{
						return RE_ACTIVITY_CONDITION_FAIL;
					}
				}
				break;
			case ACT_SHOP_COND_CONTROLLER_LEVEL:
				{
					int controllerLevel = pkPlayer->GetLevel();
					if(controllerLevel < it->ParamA())
					{
						return RE_SHOP_CONTROLLER_LEVEL_TOO_LOWER;
					}
				}
				break;
			case ACT_SHOP_COND_CHARGE_UNBINDYUANBAO_NUM:
				{
					int chargeYuanBao = pkPlayer->GetVIPAsset().GetCredit();
					if(chargeYuanBao < it->ParamA())
					{
						return RE_SHOP_CHARGE_YUANBAO_COUNT_TOO_LOWER;
					}
				}
				break;
			case ACT_SHOP_COND_PRETASK:
				{
					int taskID = it->ParamA();
					bool tag = pkPlayer->IsInFinishedTaskMap(taskID);
					if(!tag)
					{
						return RE_SHOP_PRE_TASK_NOT_FINISH;
					}
				}
				break;
			default:
				return RE_FAIL;
		}
	}
	return RE_SUCCESS;
}

StringType ACT_ShopDataEntry::FormatCond2String()
{
	StringStreamType ss;
	Utility::UT_SIMDataList::iterator it = m_kBuyConditions.begin();
	for (; it != m_kBuyConditions.end(); ++it)
	{
		ss << it->ID() << ":" << it->ParamA() << ";";
	}
	StringType str = ss.str();
	if (str.size() > 0)
	{
		str.resize(str.size() - 1);
	}
	else
	{
		str = "0";
	}
	return str;
}

struct ShopChargeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_ShopManager::DataEntryType>
{
	ShopChargeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_ShopManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}
	virtual bool operator ()(ACT_ShopManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (pkSaveEntry)
			{
				pkSaveEntry->AddCount(iValue);	
				pkSaveCatagory->UpdateDirtyFlag();
				pkSaveActivity->UpdateDirtyFlag();
				pkAsset->UpdateDirtyFlag();
				return true;
			}
		}
		return false;
	}
	int32_t iValue;
};

void ACT_ShopManager::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
{
	if (NULL==pkPlayer || !val)
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}

	ShopChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;

	for (size_t i = 0; i < kCatagorys.size(); ++i)
	{
		int32_t iCatagory = kCatagorys.at(i);
		CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
		if (NULL == pkCatagory)
		{
			continue;
		}

		kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iCatagory);
		if (kOper.pkSaveCatagory)
		{
			pkCatagory->ForAll(&kOper);
		}
	}
}

void ACT_ShopManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL==pkPlayer || !val)
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}

	ShopChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;

	if (NULL==pkCatagorys)
	{
		CatagoryDataMap::iterator it = CatagoryDatas().begin();
		for (; it!=CatagoryDatas().end(); ++it)
		{
			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(it->first);
			if (kOper.pkSaveCatagory)
			{
				it->second.ForAll(&kOper);
			}
		}
	}
	else
	{
		LC_IDSet::const_iterator cit = pkCatagorys->begin();
		for (; cit!=pkCatagorys->end(); ++cit)
		{
			CatagoryDataType* pkCatagory = GetCatagoryData(*cit);
			if (NULL == pkCatagory)
			{
				continue;
			}

			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(*cit);
			if (kOper.pkSaveCatagory)
			{
				pkCatagory->ForAll(&kOper);
			}
		}
	}
}

ResultType ACT_ShopManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;

	DataEntryType* pkDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}
 	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iCatagory);
	if (NULL == pkSaveCatagory)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}

	if (NULL==pkExtraParams || pkExtraParams->size() < ACT_EXTRA_PARAM_MAX_COUNT)
	{
		return RE_FAIL;
	}
	//int32_t iDrawCount = pkExtraParams->at(ACT_EXTRA_PARAM_INDEX_1);
	int32_t iDrawCount = pkExtraParams->at(ACT_COMMON_OP_COUINT);	
	iDrawCount = iDrawCount < 1 ? 1 : iDrawCount;
	count = iDrawCount;

	{
		//check limit
		bool  bValid = true;
		Utility::UT_SIMDataMap::iterator it = pkDataEntry->m_kLimits.begin();
		for (; it!=pkDataEntry->m_kLimits.end(); ++it)
		{
			Utility::UT_SIMDataInfo& rkLimit = it->second;
			if(!GameLogic::IS_VALID_STAT_TYPE(rkLimit.StatType()))
			{
				continue;
			}

			int32_t iStatKey = GET_SCHEDULE_KEY(rkLimit.StatType(), ACT_SCHEDULE_DETAIL_COUNT);
			if ((int32_t)pkSaveDetail->GetValue(iStatKey) + iDrawCount > rkLimit.StatValue())
			{
				bValid = false;
				break;
			}
		}
		if (!bValid)
		{
			return RE_ACTIVITY_TIMES_OUT;
		}
	}

	ResultType rslt = pkDataEntry->CheckConditions(pkPlayer, pkSaveSchedule);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}

	rslt = LC_Helper::CheckCashMap(pkPlayer, pkDataEntry->m_kCurrentPrices, iDrawCount, pkDataEntry->m_bForbidExpire);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}
	rslt = UT_ServerHelper::CheckActScore(pkPlayer, pkDataEntry->m_kActScorePrices, iSchedule);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}
	uint32_t nLogItemId = 0;
	int32_t nLogItemCount = 0;
	//埋点日志
	int nEnumId = 0;
	int nLogCode = 0;
	switch(iCatagory)
	{
	case SHOP_TYPE_UNBINDYUANBAO: 
		{
			nEnumId = DIAMOND_SHOP;
		}break;
	case SHOP_TYPE_MONEY_COURAGE: 
		{
			nEnumId = BRAVERY_SHOP;
		}break;
	case SHOP_TYPE_GUILD: 
		{
			nEnumId = GUILD_SHOP;
		}break;
	case SHOP_TYPE_BOSS_JP: 
		{
			nEnumId = JP_SHOP;
		}break;
	case SHOP_TYPE_LIMIT: 
		{
			nEnumId = LIMITS_SHOP;
		}break;
	case SHOP_TYPE_SKIN: 
		{
			nEnumId = SKIN_SHOP;
		}break;	
	case SHOP_TYPE_MRTH: 
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY12_OPERATE, OPERATE_LOGTYPE1);
		}break;
	case SHOP_TYPE_CZLB: 
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY10_OPERATE, OPERATE_LOGTYPE1);
		}break;	
	case SHOP_TYPE_JPDH: 
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY22_OPERATE, OPERATE_LOGTYPE1);
		}break;	
	case SHOP_TYPE_LLKZ: 
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY21_OPERATE, OPERATE_LOGTYPE1);
		}break;
	};
	
	if(nEnumId > 0)
	{
		nLogCode = LC_Helper::SerializeOperateLogCode(SHOP_LOGTYPE, nEnumId, OPERATE_LOGTYPE1);
	}
	vector<int> nParams;
	nParams.clear();
	//nParams.push_back(iCatagory);
	//nParams.push_back(iCatagory);
	nParams.push_back(pkDataEntry->m_iID);
	nParams.push_back(iDrawCount);

	rslt = pkDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend, iDrawCount, nLogItemId, nLogItemCount,nLogCode,nParams);
	if (RE_SUCCESS == rslt)
	{
		StringType sRewards;
		//for ossLog
		if (pkDataEntry->m_bOssLog)
		{
			LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
			if (NULL != pkPackAsset)
			{
				StringStreamType ss;
				PackOPRecordSet* record_set = pkPackAsset->GetRecordSet() ;
				for(PackOPRecordSet::PackOPRecords::iterator it = record_set->m_Add_Items.begin(); it != record_set->m_Add_Items.end(); ++it)
				{
					PackEntryOPRecord& rd = *it;
					if (NULL == rd.m_Entry)
					{
						continue;
					}
					ss << rd.m_Entry->GetBackPackValueByType(LPSFET_TYPE_ID) << "," << rd.m_lNum << ";";
				}
				sRewards = ss.str();
				if (sRewards.size() > 0)
				{
					sRewards.resize(sRewards.size() - 1);
				}
			}
		}
		//reg stat
		//reduce price
		LC_Helper::PayCashMap(pkPlayer, pkDataEntry->m_kCurrentPrices, SYS_FUNC_TP_ACTIVITY_SHOP, pkDataEntry->m_iID, iDrawCount, pkDataEntry->m_bForbidExpire, nLogCode,nParams);
		UT_ServerHelper::PayActScore(pkPlayer, pkDataEntry->m_kActScorePrices, iSchedule);

		LC_CashMap::iterator itCash = pkDataEntry->m_kCurrentPrices.begin();
		for ( ; itCash != pkDataEntry->m_kCurrentPrices.end(); ++itCash)
		{
			int nMoneyType = itCash->first;
			MoneyType nMoneyValue = itCash->second;
			int nBanance = pkPlayer->GetCashNumByCashType(static_cast<CashType>(nMoneyType));
			int nRemainCount = pkPlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, nLogItemId);
			switch ( nMoneyType )
			{
			case CT_PENTACLE:
				{
					oss_360_log_giftmoney_buy( nBanance, nLogItemId, nLogItemCount, nMoneyValue, pkPlayer );
				}
				break;
			case CT_UNBIND_YUANBAO:
				{
					oss_360_log_shop_buy(nBanance, nLogItemId, nLogItemCount, nMoneyValue, nRemainCount, 0, pkPlayer);
				}
				break;
			}
			//记录用户在游戏内金币消耗行为
			//备注：当该消耗行为是与NPC 发生的交易时，下列字段itemid及amount不能为空，当该消耗行为是其他行为时，itemid及amount可为空，根据
			//具体情况而定
			//字段名称 类型 含义
			//balance int 金币余额，游戏内金币消耗行为之后的金币余额
			//itemid varchar(255) 物品ID(道具物品的模板ID)
			//amount int 购买数量
			//money int 本次消耗的金币数量
			//level int 当前等级
			//map_id varchar(255) 地图id
			// todo
			oss_360_log_game_buy(nBanance, nLogItemId, nLogItemCount, nMoneyValue, pkPlayer);
		}

		Utility::UT_SIMDataMap::iterator it = pkDataEntry->m_kLimits.begin();
		for (; it!=pkDataEntry->m_kLimits.end(); ++it)
		{
			Utility::UT_SIMDataInfo& rkLimit = it->second;
			if(!GameLogic::IS_VALID_STAT_TYPE(rkLimit.StatType()))
			{
				continue;
			}

			int32_t iStatKey = GET_SCHEDULE_KEY(rkLimit.StatType(), ACT_SCHEDULE_DETAIL_COUNT);
			pkSaveDetail->AddValue(iStatKey, iDrawCount);
		}

		if (pkDataEntry->m_iScore > 0)
		{
			static ACT_ScheduleDataManager* pkDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
			pkDataMgr->OnShop(pkPlayer, pkDataEntry->m_iScore * iDrawCount);
		}
		pkSaveSchedule->UpdateDirtyFlag();
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
		if (pkDataEntry->m_bOssLog)
		{
			StringType sConsumes = FormatComMap2String(pkDataEntry->m_kCurrentPrices, iDrawCount);
			oss_act_shop_buy(pkPlayer, iCatagory, iKey, sRewards, sConsumes, pkDataEntry->FormatCond2String(), iDrawCount);
		}
	}
	return rslt;
}
