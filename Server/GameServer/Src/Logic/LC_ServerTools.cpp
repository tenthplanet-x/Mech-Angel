#include "LC_ServerTools.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_RankManager.h"
#include "LC_RumorManager.h"
#include "oss_define.h"
#include "LC_GameEventManager.h"
#include "LC_ItemFactoryBase.h"
#include "LC_PackEntryFactory.h"
#include "LC_GameStory_Manager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void on_common_stat_op_successfully(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iKey, uint32_t opFlag, int32_t opPara)
{
	if (pkPlayer == NULL)
		return;

	oss_role_common_stat(pkPlayer, iType, iKey, opFlag, opPara);
	//do event dispatch
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent)
	{
		pkGameEvent->SetEventType(GLET_COMMON_STAT_OP);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
		pkGameEvent->SetEventParam(EP_CSOP_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_CSOP_TYPE, iType);
		pkGameEvent->SetEventParam(EP_CSOP_KEY, iKey);
		pkGameEvent->SetEventParam(EP_CSOP_OP_FLAG, opFlag);
		pkGameEvent->SetEventParam(EP_CSOP_MAP_ID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t func_CommonStatDraw(LC_ServerPlayer* pkPlayer, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, LC_CommonStatEntry* pkCmnStatEntry, 
							LC_CommonStatAsset& kCmnStatAsset, int32_t iType, float fRewardExpo = 1.0, int nLogCode = 0, const vector<int> nParams = vector<int>())
{
	if (NULL == pkPlayer || NULL == pkCSRewardEntry)
	{
		return RE_FAIL;
	}
	int32_t uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		pkCSRewardEntry->m_kRewardItems.Resolve(pkPlayer, kItems, NULL);
		if (!kItems.empty())
		{
			Utility::UT_SIMDataList::iterator itItem = kItems.begin();
			for (; itItem!=kItems.end(); ++itItem)
			{
				itItem->ItemCount(int32_t(itItem->ItemCount()*fRewardExpo));
			}
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_CMN_STAT_REWARD
				, pkCSRewardEntry->m_iID, pkCSRewardEntry->MailTitle(), pkCSRewardEntry->MailDesc(), pkCSRewardEntry->IsMailEnable(), false, false, nLogCode, nParams);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
		attr_value_type iExp = attr_value_type(pkCmnStatEntry->ResolveExp(pkPlayer, pkCSRewardEntry) * fRewardExpo);
		MoneyType iGold = MoneyType(pkCmnStatEntry->ResolveGold(pkPlayer, pkCSRewardEntry) * fRewardExpo);
		MoneyType iLijin = MoneyType(pkCmnStatEntry->ResolveLijin(pkPlayer, pkCSRewardEntry) * fRewardExpo);
		MoneyType iIngot = MoneyType(pkCmnStatEntry->ResolveIngot(pkPlayer, pkCSRewardEntry) * fRewardExpo);
		attr_value_type iSP = attr_value_type(pkCmnStatEntry->ResolveSP(pkPlayer, pkCSRewardEntry) * fRewardExpo);
		MoneyType iGuild = MoneyType(pkCmnStatEntry->ResolveGuild(pkPlayer, pkCSRewardEntry) * fRewardExpo);
		int32_t iGSScore = int32_t(pkCmnStatEntry->ResolveGSScore(pkPlayer, pkCSRewardEntry) * fRewardExpo);

		if (iExp)
		{
			pkPlayer->AddEXP(iExp, SYS_FUNC_TP_CMN_STAT_REWARD, pkCSRewardEntry->m_iID);
		}

		if (iGold)
		{
			pkPlayer->AddCash(iGold, CT_UNBIND_CASH, SYS_FUNC_TP_CMN_STAT_REWARD, pkCSRewardEntry->m_iID, 0, true, NULL, nLogCode, nParams);
		}
		
		if (iLijin)
		{
			pkPlayer->AddCash(iLijin, CT_BIND_YUANBAO, SYS_FUNC_TP_CMN_STAT_REWARD, pkCSRewardEntry->m_iID, 0, true, NULL, nLogCode, nParams);
		}
		
		if (iIngot)
		{
			pkPlayer->AddCash(iIngot, CT_UNBIND_YUANBAO, SYS_FUNC_TP_CMN_STAT_REWARD, pkCSRewardEntry->m_iID, 0, true, NULL, nLogCode, nParams);
		}

		if (iSP)
		{
			pkPlayer->AddSkillLevelupPoints(iSP, SYS_FUNC_TP_CMN_STAT_REWARD, pkCSRewardEntry->m_iID);
		}
		
		if (iGuild)
		{
			pkPlayer->AddGuildMoney(iGuild, SYS_FUNC_TP_CMN_STAT_REWARD, pkCSRewardEntry->m_iID);
		}

		if (iGSScore)
		{
			pkPlayer->GetGameStoryAsset().SetScore(pkCSRewardEntry->m_iGSScoreType, pkPlayer->GetGameStoryAsset().GetScore(pkCSRewardEntry->m_iGSScoreType) + iGSScore);
		}

		pkCmnStatEntry->RegReward(pkCSRewardEntry->m_iSaveIndex);
		if (pkCSConfigCEntry && pkCSConfigCEntry->m_iStoryID)
		{
			pkCmnStatEntry->RegStoryReward(pkCSRewardEntry->m_iSaveIndex);
		}
		kCmnStatAsset.UpdateDirtyFlag();

		uiRslt = RE_SUCCESS;
	}while (0);
	return uiRslt;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t gain_CommonStatDraw(LC_ServerPlayer* pkPlayer, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, int32_t iType, float fRewardExpo)
{
	if(pkPlayer == NULL || pkCSRewardEntry == NULL)
	{
		return RE_FAIL;
	}
	if(pkCSRewardEntry->m_iForceMail <= 0)
	{
		LC_CommonStatAsset& kCmnStatAsset = pkPlayer->GetCommonStatAsset();
		return func_CommonStatDraw(pkPlayer, pkCSConfigCEntry, pkCSRewardEntry, kCmnStatAsset.GetCommonStatEntry(iType), kCmnStatAsset, iType, fRewardExpo);
	}
	else
	{
		unique_id_list idList;
		idList.push_back(pkPlayer->GetInstance());
		return gain_CommonStatDraw(idList, pkCSConfigCEntry, pkCSRewardEntry, iType);
	}
}

int32_t gain_CommonStatDraw(mem::list<LC_ServerPlayer*>& rPlayerList, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, int32_t iType)
{
	if(pkCSRewardEntry == NULL)
	{
		return RE_FAIL;
	}
	unique_id_list idList;
	for(mem::list<LC_ServerPlayer*>::iterator iter = rPlayerList.begin(); iter != rPlayerList.end(); ++iter)
	{
		LC_ServerPlayer* pkPlayer = *iter;
		if(pkPlayer == NULL)
		{
			continue;
		}
		if(pkCSRewardEntry->m_iForceMail <= 0)
		{
			if(gain_CommonStatDraw(*iter, pkCSConfigCEntry, pkCSRewardEntry, iType) == RE_SUCCESS)
			{
				on_common_stat_op_successfully(*iter, iType, 0, 0, 0);
			}
		}
		else
		{
			idList.push_back(pkPlayer->GetInstance());
		}
	}
	if(idList.empty() == false)
	{
		return gain_CommonStatDraw(idList, pkCSConfigCEntry, pkCSRewardEntry, iType);
	}
	return RE_SUCCESS;
}

int32_t gain_CommonStatDraw(const unique_id_list& rCharList, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, int32_t iType)
{
	if (NULL == pkCSRewardEntry)
	{
		return RE_FAIL;
	}
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (NULL == pkItemFactory)
	{
		return RE_FAIL;
	}
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return RE_FAIL;
	}

	LC_BackPackEntryList kEntryList;
	int32_t iItemTypeID = 0;
	int32_t iItemCount = 0;
	int32_t iGold = (int32_t)pkCSRewardEntry->GetGold();
	if (ResolveExpItem((int32_t)pkCSRewardEntry->GetExp(), iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveSPItem((int32_t)pkCSRewardEntry->GetSp(), iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveLijinItem((int32_t)pkCSRewardEntry->GetLijin(), iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveYuanbaoItem((int32_t)pkCSRewardEntry->GetIngot(), iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}

	Utility::UT_SIMDataList kRewordItems;
	pkCSRewardEntry->m_kRewardItems.Resolve(WT_NONE, kRewordItems, NULL);
	for (Utility::UT_SIMDataList::const_iterator cit = kRewordItems.begin(); cit != kRewordItems.end(); ++cit)
	{
		pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList);
	}

	for(unique_id_list::const_iterator iter = rCharList.begin(); iter != rCharList.end(); ++iter)
	{
		UT_ServerHelper::SendToMail(pkCSRewardEntry->MailTitle(), pkCSRewardEntry->MailDesc(), *iter, "", iGold,  kEntryList);
	}
	return RE_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CommonStatOPCBArg::CommonStatOPCBArg(uint32_t uiCharID, int32_t iType, int32_t iKey, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry,
									LC_CommonStatReplevyDataEntry* pkCSReplevyEntry, uint32_t uiOPFlag, int32_t iOPPara)
	: m_uiRslt(RE_FAIL)
	, m_uiCharID(uiCharID)
	, m_iType(iType)
	, m_iKey(iKey)
	, m_pkCSConfigCEntry(pkCSConfigCEntry)
	, m_pkCSRewardEntry(pkCSRewardEntry)
	, m_pkCSReplevyEntry(pkCSReplevyEntry)
	, m_uiOPFlag(uiOPFlag)
	, m_iOPPara(iOPPara)
	, m_fDiscountExpo(1.0f)
	, m_fRewardExpo(1.0f)
{
	nLogCode = 0;
	nLogParams.clear();
}

void CommonStatOPCBArg::operator()(void)
{
	if (NULL == m_pkCSConfigCEntry)
	{
		return;
	}
	int iStoryID = m_pkCSConfigCEntry->m_iStoryID;
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(m_uiCharID);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_CommonStatAsset& kCmnStatAsset = pkPlayer->GetCommonStatAsset();
	LC_CommonStatEntry* pkCmnStatEntry = kCmnStatAsset.GetCommonStatEntry(m_iType);
	if (NULL == pkCmnStatEntry)
	{
		return;
	}
	if (m_uiOPFlag & (1 << LC_CMN_STAT_OP_FIRE))
	{
		if (NULL == m_pkCSRewardEntry)
		{
			return;
		}
		int iGold = Utility::Round2Int(LC_Helper::BaseLevelCash(pkPlayer->GetLevel()) * m_pkCSRewardEntry->m_fFireGold);
		if (iGold > 0)
		{
			pkPlayer->ReduceCash(iGold, CT_UNBIND_CASH, SYS_FUNC_TP_CMN_STAT_FIRE, m_pkCSRewardEntry->m_iID);
		}
		if (!m_pkCSRewardEntry->m_kFireShopIDMap.empty())
		{
			LC_Helper::PayForShop(pkPlayer, m_pkCSRewardEntry->m_kFireShopIDMap, SYS_FUNC_TP_CMN_STAT_FIRE, m_pkCSRewardEntry->m_iID);
		}
		pkCmnStatEntry->RegFire(m_pkCSRewardEntry->m_iSaveIndex);
		if (iStoryID > 0)
		{
			pkCmnStatEntry->AddAccumulateStoryCount(1);
		}
		pkCmnStatEntry->AddAccumulateCnt(1);
		m_uiRslt = RE_SUCCESS;
	}

	if (m_uiOPFlag & (1 << LC_CMN_STAT_OP_DRAW))
	{
		m_uiRslt = func_CommonStatDraw(pkPlayer, m_pkCSConfigCEntry, m_pkCSRewardEntry, pkCmnStatEntry, kCmnStatAsset, m_iType, m_fRewardExpo, nLogCode, nLogParams);
	}
	if (m_uiOPFlag & (1 << LC_CMN_STAT_OP_REPLEVY))
	{
		do
		{
			if (NULL == m_pkCSReplevyEntry)
			{
				m_uiRslt = RE_FAIL;
				break;
			}
			int32_t iPro = 10000;
			if (0 == m_iOPPara)
			{
				int iGold = 0;
				iGold = Utility::Round2Int(LC_Helper::BaseLevelCash(pkPlayer->GetLevel()) * m_pkCSReplevyEntry->m_fGold * m_fDiscountExpo);
				if (iGold > 0)
				{
					pkPlayer->ReduceCash(iGold, CT_UNBIND_CASH, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID, nLogCode, nLogParams);
				}
				iPro = m_pkCSConfigCEntry->m_iGoldReplevyPro;
			}
			else if(!m_pkCSReplevyEntry->m_kShopIDMap.empty())
			{
				LC_ShopIDMap kFinalShops;
				LC_ShopIDMap::iterator itShop = m_pkCSReplevyEntry->m_kShopIDMap.begin();
				for (; itShop!=m_pkCSReplevyEntry->m_kShopIDMap.end(); ++itShop)
				{
					kFinalShops[itShop->first] += Utility::Round2Int(itShop->second * m_iKey);
				}
				LC_Helper::PayForShop(pkPlayer, kFinalShops, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID, NULL, nLogCode, nLogParams);
			}

			double dExpo = iPro * 0.0001;
			int nKeyValue = pkPlayer->GetCommanderLevel();
			// nKeyValue == 0可以理解为，不符合条件
			if (nKeyValue == 0)
			{
				return;
			}
			//计算后AddEXP，就不会导致Getlevel获得的经验不一致
			attr_value_type iExp = attr_value_type(pkCmnStatEntry->ResolveReplevyExp(pkPlayer, m_pkCSReplevyEntry, nKeyValue)* dExpo) * m_iKey;
			MoneyType iGold = MoneyType(pkCmnStatEntry->ResolveReplevyGold(pkPlayer, m_pkCSReplevyEntry, nKeyValue) * dExpo) * m_iKey;
			attr_value_type iSP = attr_value_type(pkCmnStatEntry->ResolveReplevySP(pkPlayer, m_pkCSReplevyEntry, nKeyValue) * dExpo) * m_iKey;
			MoneyType iGuild = MoneyType(pkCmnStatEntry->ResolveReplevyGuild(pkPlayer, m_pkCSReplevyEntry, nKeyValue) * dExpo) * m_iKey;
			int iGSScore = int(pkCmnStatEntry->ResolveReplevyGSScore(pkPlayer, m_pkCSReplevyEntry, nKeyValue) * dExpo) * m_iKey;
			int iCourage = int(pkCmnStatEntry->ResolveReplevyCourage(pkPlayer, m_pkCSReplevyEntry, nKeyValue) * dExpo) * m_iKey;
			// 物品奖励
			Utility::UT_SIMDataInfo kRewards;
			bool bRet = pkCmnStatEntry->ResolveReplevyItems(pkPlayer, m_pkCSReplevyEntry, kRewards, nKeyValue);
			if (bRet)
			{
				pkPlayer->AddItem(kRewards.X(), kRewards.Y() * m_iKey* dExpo,false, 0, 0, nLogCode, nLogParams);
			}
			else
			{
				const unique_id_impl& instance = pkPlayer->GetInstance();
				GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CommonStatOPCBArg ResolveReplevyItems err! nKeyValue:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d",
					nKeyValue, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkPlayer->GetID());
			}

			if (iExp)
			{
				pkPlayer->AddEXP(iExp, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID);
			}
			
			if (iGold)
			{
				pkPlayer->AddCash(iGold, CT_UNBIND_CASH, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID, 0,true,NULL,nLogCode, nLogParams);
			}
			
			if (iSP)
			{
				pkPlayer->AddSkillLevelupPoints(iSP, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID);
			}
			
			if (iGuild)
			{
				pkPlayer->AddGuildMoney(iGuild, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID);
			}
			
			if (iGSScore)
			{
				pkPlayer->GetGameStoryAsset().SetScore(m_pkCSReplevyEntry->m_iGSScoreType, pkPlayer->GetGameStoryAsset().GetScore(m_pkCSReplevyEntry->m_iGSScoreType) + iGSScore);
			}
			
			if (iCourage)
			{
				pkPlayer->AddCouragePoint(iCourage, SYS_FUNC_TP_CMN_STAT_REPLEVY, m_pkCSReplevyEntry->m_iID);
			}
			int nOrigCount = pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_REPLEVY);
			nOrigCount++;
			pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_REPLEVY, nOrigCount);
			kCmnStatAsset.UpdateDirtyFlag();
			m_uiRslt = RE_SUCCESS;
		}while (0);
	}

	if (RE_SUCCESS == m_uiRslt)
	{
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		on_common_stat_op_successfully(pkPlayer, m_iType, m_iKey, m_uiOPFlag, m_iOPPara);
	}
}