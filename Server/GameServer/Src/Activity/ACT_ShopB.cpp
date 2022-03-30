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


ResultType ACT_ShopManagerB::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;

	DataEntryType* pkDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}

	static CActivityTimeMgr* pkTimeMgr = CActivityTimeMgr::GetSingletonPtr();
	bool tag = pkTimeMgr->CheckTimeActived(pkPlayer, iSchedule);
	if (!tag)
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
	case SHOP_TYPE_XSLB: 
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY11_OPERATE, OPERATE_LOGTYPE1);
		}
		break;
	};

	if(nEnumId > 0)
	{
		nLogCode = LC_Helper::SerializeOperateLogCode(SHOP_LOGTYPE, nEnumId, OPERATE_LOGTYPE1);
	}
	vector<int> nParams;
	nParams.clear();
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
