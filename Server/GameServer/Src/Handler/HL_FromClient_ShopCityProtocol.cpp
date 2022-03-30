#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "LC_SequenceManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_VIPAsset.h"
#include "LC_ServerWorldManager.h"
#include "LC_RumorManager.h"

#include "oss_define.h"
#include "oss_360_define.h"
#include "gsnode.h"

#include "TS_TranManager.h"
#include "TS_TranType.h"

#include "UT_ServerHelper.h"
#include "LC_ServerShopAsset.h"
#include "CF_WorldMapList.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

ON_HANDLE(MGPT_REQ_BUY_ITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Buy_Item kReqBody;
	if (!kReqBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_CommonShopManager* pkCmnShopMgr = LC_ServerShopManager::GetSingletonPtr();
	LC_ShopIDMap kShopIDMap;

	int32_t iItemID = 0;
	int32_t iItemCount = 0;

	std::map<int32_t, int32_t>::iterator it = kReqBody.m_kShopIDMap.begin();
	for (; it != kReqBody.m_kShopIDMap.end(); ++it)
	{
		if (it->second <= 0)
		{
			continue;
		}
		LC_ShopDataEntry* pkEntry = pkCmnShopMgr->GetShopDataEntry(it->first);
		if (NULL == pkEntry || !pkEntry->m_iSaleState)
		{
			continue;
		}
		kShopIDMap[it->first] = it->second;

		iItemID = pkEntry->m_iItemID;
		iItemCount = pkEntry->m_iItemCount * it->second;
	}

	ResultType rslt = RE_SUCCESS;
	int32_t iShopID = 0;
	if (!kShopIDMap.empty())
	{
		iShopID = kShopIDMap.begin()->first;
		rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
	}
	if (RE_SUCCESS == rslt)
	{
		//商城购买--埋点日志
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(iShopID);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SHOP_LOGTYPE, NORMAL_SHOP, OPERATE_LOGTYPE1);
		LC_ShopCostRecord kShopCost;
		rslt = LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_CITY_SHOP, iShopID, &kShopCost, nLogCode,nParams);
		if (RE_SUCCESS == rslt)
		{
			// Send message immediatly
			if (pkPlayer->GetDirtyFlag(DIRTY_FLAG_COMMON_STAT_INFO))
			{
				bool bDirty = false;
				pkPlayer->SyncCommonStatInfo(bDirty);
				if (!bDirty)
				{
					pkPlayer->SetDirtyFlag(DIRTY_FLAG_COMMON_STAT_INFO, false);
				}
			}
			oss_shop_buy(pkPlayer, kShopCost.Cash(CT_UNBIND_YUANBAO), kShopCost.Cash(CT_BIND_YUANBAO), iItemID, iItemCount);

			int nRemainCount = pkPlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, iItemID);
			int nBanance = pkPlayer->GetCashNumByCashType(CT_UNBIND_YUANBAO);
			oss_360_log_shop_buy(nBanance, iItemID, iItemCount, kShopCost.Cash(CT_UNBIND_YUANBAO), nRemainCount, 0, pkPlayer);
		
			GameLogic::LC_RumorManager* pkRumorManager = GameLogic::LC_RumorManager::GetSingletonPtr();
			if (pkRumorManager)
			{
				GameLogic::LC_Rumor* pkRumor = pkRumorManager->GetRumorByType(GameLogic::ShopRumor);
				const GameLogic::LC_RumorDataEntry* pkRumorEntry = NULL;
				if (pkRumor && (pkRumorEntry = pkRumor->GetRumerEntry(iShopID)))
				{
					int noticeID = pkRumor->GetRumorID(pkRumorEntry->kKey);
					UT_ServerHelper::_GameNoticeFmort noticeFmort;
					noticeFmort.SetChannelType(pkRumor->GetChatChannel());
					noticeFmort.SetMsgID(noticeID);
					noticeFmort.AddPlayerInfo(pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName());
					UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
				}
			}		
		}
	}

	MG_RLT_Buy_Item kRltMsg;
	kRltMsg.m_Result = rslt;
	kRltMsg.m_kShopIDMap.insert(kShopIDMap.begin(), kShopIDMap.end());
	pkPlayer->SendMsgToClient(MGPT_RLT_BUY_ITEM, &kRltMsg);
}

ON_HANDLE(MGPT_REQ_DRAW_VIP_ONEOFF_GIFT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Draw_VIP_Oneoff_Gift kReqBody;
	if (!kReqBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//特权礼包--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(kReqBody.m_uiLevel);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, PRIVILEGE_OPERATE, OPERATE_LOGTYPE1);
	MG_RLT_Draw_VIP_Oneoff_Gift rltMsg;
	rltMsg.m_uiLevel = kReqBody.m_uiLevel;
	rltMsg.m_uiRslt = pkPlayer->GetVIPAsset().DrawGiftVip(kReqBody.m_uiLevel, nLogCode,nParams);
	pkPlayer->SendMsgToClient(MGPT_RLT_DRAW_VIP_ONEOFF_GIFT, &rltMsg);
}

ON_HANDLE(MGPT_REQ_DRAW_VIP_TIME_GIFT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Draw_VIP_Time_Gift kReqBody;
	if (!kReqBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_RLT_Draw_VIP_Time_Gift rltMsg;
	rltMsg.m_uiType = kReqBody.m_uiType;
	rltMsg.m_uiLevel = kReqBody.m_uiLevel;
	//特权每日--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(kReqBody.m_uiLevel);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, PRIVILEGE_OPERATE, OPERATE_LOGTYPE2);
	rltMsg.m_uiRslt = pkPlayer->GetVIPAsset().DrawGiftDaily(kReqBody.m_uiLevel, nLogCode,nParams);
	pkPlayer->SendMsgToClient(MGPT_RLT_DRAW_VIP_TIME_GIFT, &rltMsg);
}

ON_HANDLE(MGPT_REQ_DRAW_FDEPOSIT_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Draw_FDepositReward kReqBody;
	if (!kReqBody.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_VIPAsset& kVIPAsset = pkPlayer->GetVIPAsset();
	uint32_t uiFDLvl = kVIPAsset.GetFDepositLevel();
	LC_VIPFDepositEntry* pkEntry = LC_VIPCmnManager::GetSingletonPtr()->GetVIPFDepositDataEntry(kReqBody.m_uiLevel);		
	if(NULL == pkEntry)
	{
		return;
	}
	
	MG_RLT_Draw_FDepositReward rltMsg;
	rltMsg.m_uiLevel = kReqBody.m_uiLevel;
	rltMsg.m_uiRslt = RE_FAIL;
	do
	{
		if (kReqBody.m_uiLevel > uiFDLvl)
		{
			break;
		}
		if (/*NULL == pkEntry ||*/ kVIPAsset.IsFDLVLDrawed(kReqBody.m_uiLevel))
		{
			break;
		}
		if (pkEntry->GetItemsCount() > 0)
		{
			StringType title;
			StringType desc;
			rltMsg.m_uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkEntry->GetItemList(), pkPlayer, SYS_FUNC_TP_VIP_FDEPOSIT, kReqBody.m_uiLevel, title, desc, false);
			if (RE_SUCCESS != rltMsg.m_uiRslt)
			{
				break;
			}
		}
		attr_value_type iExp = pkEntry->GetExp();
		if (iExp)
		{
			pkPlayer->AddExpWithGamePlay(iExp, SYS_FUNC_TP_VIP_FDEPOSIT, kReqBody.m_uiLevel);
		}
		MoneyType iCash = pkEntry->GetCash();
		if (iCash)
		{
			pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_VIP_FDEPOSIT, kReqBody.m_uiLevel);
		}
		attr_value_type iSp = pkEntry->GetSP();
		if (iSp)
		{
			pkPlayer->AddSkillLevelupPoints(iSp, SYS_FUNC_TP_VIP_FDEPOSIT, kReqBody.m_uiLevel);
		}
		kVIPAsset.RegFDLVLDrawed(kReqBody.m_uiLevel);
		GameLogic::LC_RumorManager* pkRumorManager = GameLogic::LC_RumorManager::GetSingletonPtr();
		if (pkRumorManager)
		{
			GameLogic::LC_Rumor* pkRumor = pkRumorManager->GetRumorByType(GameLogic::FirstDeposit);
			const GameLogic::LC_RumorDataEntry* pkRumorEntry = NULL;
			if (pkRumor && (pkRumorEntry = pkRumor->GetRumerEntry(kReqBody.m_uiLevel)))
			{
				if (pkEntry->GetItemsCount() > 0)
				{
					int noticeID = pkRumor->GetRumorID(pkRumorEntry->kKey);
					UT_ServerHelper::_GameNoticeFmort noticeFmort;
					noticeFmort.SetChannelType(pkRumor->GetChatChannel());
					noticeFmort.SetMsgID(noticeID);
					noticeFmort.AddPlayerInfo(pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName());
					noticeFmort.AddItemName(pkEntry->GetItemIDByIndex(0));
					UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
				}
			}
		}
		rltMsg.m_uiRslt = RE_SUCCESS;
	} while (0);
	pkPlayer->SendMsgToClient(MGPT_RLT_DRAW_FDEPOSIT_REWARD, &rltMsg);
}

ON_HANDLE(MGPT_REQ_TRANSFER_TO_LOCATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_Req_TransferToLocation kmsg;
	if (!kmsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	float time = GET_PROCESS_TIME;
	const unique_id_impl& playerInstance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_REQ_TRANSFER_TO_LOCATION  transfer c_cid:[%d], c_citizenship:[%d], c_uid:[%d], mapID:[%d], time:[%f]", 
		playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID(), kmsg.m_iMapResID, time);

	MG_Rlt_TransferToLocation rltMsg;
	rltMsg.m_iMapResID = kmsg.m_iMapResID;
	rltMsg.m_iMapLogicID = kmsg.m_iMapLogicID;
	rltMsg.m_iLineNum= kmsg.m_iLineNum;
	UT_Vec3Int kPos;
	rltMsg.m_iLocX = kPos.x = kmsg.m_iLocX;
	rltMsg.m_iLocY = kPos.y = kmsg.m_iLocY;
	rltMsg.m_iLocZ = kPos.z = kmsg.m_iLocZ;
	rltMsg.m_uiRslt = RE_SUCCESS;
	rltMsg.m_iServerID = kmsg.m_iServerID;
	int32_t nTargetServer = 0;
	if(kmsg.m_iServerID > 0)
	{
		//nTargetServer = UT_ServerHelper::GetActiveId(kmsg.m_iServerID);
		nTargetServer = GSNodeNetwork::Instance().getEntryIdByMergeId(kmsg.m_iServerID);		
	}
	if(nTargetServer > 0 && nTargetServer == UT_ServerHelper::GetServerId())
	{
		nTargetServer = 0;
	}
	//if(nTargetServer > 0 && nTargetServer != UT_ServerHelper::GetActiveId(pkPlayer))
	if(nTargetServer > 0 && nTargetServer != GSNodeNetwork::Instance().getEntryIdByMergeId(pkPlayer->GetCitizenship()))
	{
		rltMsg.m_uiRslt = RE_CANNOT_TRANS_TO_NO_SELF_MAP;
	}

	if(rltMsg.m_uiRslt == RE_SUCCESS)
	{
		//检测是否可以传送
		bool tag = pkPlayer->CheckTripMap_PVPTime();
		if(!tag)
		{
			rltMsg.m_uiRslt = RE_TRANSFER_ERROR;
		}
		else
		{
			if(nTargetServer > 0)		//目前只支持返回自己的服务器
			{
				LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
				rltMsg.m_uiRslt = enterLeaveManager->RequstCrossChangeLocation(pkPlayer, nTargetServer, kmsg.m_iMapResID, kPos);
			}
			else
			{
				CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(kmsg.m_iMapResID);
				if (pkMapEntry == NULL || (pkMapEntry->_CanChgLocation <= 0 && UT_ServerHelper::IsInCross(pkPlayer) == true))
				{
					rltMsg.m_uiRslt = RE_CANNOT_TRANS_TO_NORMAL_MAP;
				}
				else
				{
					pkPlayer->GetWorldLocationAsset()->SetForceLocation(true);
					LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
					rltMsg.m_uiRslt = pkServerWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, kmsg.m_iMapLogicID, kmsg.m_iMapResID, kPos, kmsg.m_iLineNum);
				}
			}
		}

		if (RE_SUCCESS == rltMsg.m_uiRslt)
		{
			pkPlayer->ActiveSkillState(31000002, 1, 0);
			pkPlayer->SyncPropertyToClient(false);
		}
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_TRANSFER_TO_LOCATION, &rltMsg);
}

class BuyVigorCBArg : public TS_PostCommit
{
public:
	uint32_t				m_uiCharID;
	LC_VIPProfitDataEntry*	m_pkDataEntry;

	BuyVigorCBArg(uint32_t uiCharID, LC_VIPProfitDataEntry* pkDataEntry): m_uiCharID(uiCharID), m_pkDataEntry(pkDataEntry) {}

	virtual void operator()(void)
	{
		if (NULL == m_pkDataEntry)
		{
			return;
		}
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(m_uiCharID);
		if (NULL == pkPlayer)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "BuyVigorCBArg player is null !CharID:%u Profit:%d", m_uiCharID, m_pkDataEntry->GetID());
			return;
		}
		const LC_VIPProfitDefine& kCommonDefine = m_pkDataEntry->GetCommonDefine();

		MG_Rlt_BuyVigor rltMsg;
		rltMsg.m_iDelta = kCommonDefine.m_iValue;

		pkPlayer->AddVigor(rltMsg.m_iDelta);
		/*health(pkPlayer,5,rltMsg.m_iDelta,pkPlayer->GetVigor() - rltMsg.m_iDelta);*/
		LC_VIPAsset& kVipAsset = pkPlayer->GetVIPAsset();

		kVipAsset.SetProfitStatValue(m_pkDataEntry->GetID(), kVipAsset.GetProfitStatValue(m_pkDataEntry->GetID()) + 1);
		pkPlayer->SendMsgToClient(MGPT_RLT_BUY_VIGOR, &rltMsg);
	}
};

ON_HANDLE(MGPT_REQ_BUY_VIGOR, this_source_type, false)
{
	//RGame 去掉VIP体力购买次数
	/*
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_BuyVigor kmsg;
	if (!kmsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	
	LC_VIPCmnManager* pkVIPMgr = LC_VIPCmnManager::GetSingletonPtr();
	LC_VIPProfitDataEntryPtrs& kProfitEntryPtrs = pkVIPMgr->GetVIPProfitDataEntryPtrs(LC_VIP_PROF_TP_COMMON, LC_VIP_PROF_KEY_CMN_VIGOR_BUY);

	LC_VIPProfitDataEntry* pkProfDataEntry = NULL;
	LC_VIPProfitDataEntryPtrs::iterator itProf = kProfitEntryPtrs.begin();
	for (; itProf != kProfitEntryPtrs.end(); ++itProf)
	{
		pkProfDataEntry = *itProf;
		if (pkProfDataEntry)
		{
			break;
		}
	}

	if (NULL == pkProfDataEntry)
	{
		return;
	}

	LC_VIPAsset& kVipAsset = pkPlayer->GetVIPAsset();
	const LC_VIPProfitDefine& kCommonDefine = pkProfDataEntry->GetCommonDefine();

	MG_Rlt_BuyVigor rltMsg;

	do
	{
		//int32_t iCurCount = kVipAsset.GetProfitStatValue(pkProfDataEntry->GetID());
		inT32_t iCurCount = limitInfo.IID();
		int32_t iCountLimit = pkProfDataEntry->GetValue(pkPlayer->GetVIPLevel(), pkPlayer->GetVIPAsset().IsFlagSet(LC_VIP_FG_FAKE));
		if (iCurCount >= iCountLimit)
		{
			rltMsg.m_uiRslt = RE_VIGOR_BUY_TIMES_LIMIT;
			break;
		}

		++iCurCount;
		LC_ShopIDMap kShopIDMap;
		Utility::UT_SIMDataList::const_iterator cit = kCommonDefine.m_kExtras.begin();
		for (; cit != kCommonDefine.m_kExtras.end(); ++cit)
		{
			if (iCurCount > (int32_t)cit->ItemCount())
			{
				kShopIDMap[cit->ID()] += (int32_t)cit->ItemCount();
				iCurCount -= (int32_t)cit->ItemCount();
			}
			else
			{
				kShopIDMap[cit->ID()] += (int32_t)iCurCount;
				break;
			}
		}

		if (!kShopIDMap.empty())
		{
			rltMsg.m_uiRslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
			if (RE_SUCCESS != rltMsg.m_uiRslt)
			{
				break;
			}

			LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_VIGOR, pkProfDataEntry->GetID());
		}

		{
			BuyVigorCBArg kBuyVigorOP(pkPlayer->GetID(), pkProfDataEntry);
			kBuyVigorOP();
		}

		return;
	} while (0);

	pkPlayer->SendMsgToClient(MGPT_RLT_BUY_VIGOR, &rltMsg);
	*/
}
}
