#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "CF_ItemList.h"
#include "CF_LogicAreaFunc.h"
#include "CF_OpenBag.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "TS_TranManager.h"
#include "TS_TranType.h"
#include "MG_Common.h"
#include "LC_GameEventManager.h"
#include "oss_define.h"
#include "str2val.h"
#include "LC_Helper.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ServerItem.h"
#include "LC_GlobalKeyManager.h"
#include "LC_GemReinforceAsset.h"
#include "../Buddy/BuddyHelper.h"
#include "GlobalSettings.h"
#include "LC_ServerWorldManager.h"
#include "../Buddy/BuddyInstance.h"
#include "LC_Define.h"
#include "../Utility/UT_OperateLog.h"
using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;
using namespace Activity;
using namespace OPERATELOG;
namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
struct UseItemInPackByIndexOP
{
	UseItemInPackByIndexOP(void)
		: actor_id(0)
		, pack_type(0)
		, entry_index(0)
		, item_id(0)
		, item_count(0)
		, param(0)
	{}

	void operator()(int32_t iRet, const std::string& val)
	{
		if (RE_SUCCESS != iRet)
		{
			return;
		}
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(actor_id);
		if (NULL == pkPlayer)
		{
			return;
		}

		bool bCheckResult = LC_Helper::CheckItemReqInPackage(pkPlayer, actor_id, pack_type, entry_index, item_id);
		if (bCheckResult == false)
		{
			return;
		}
		//背包使用--埋点日志
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(item_id);
		nParams.push_back(item_count);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, USERPACK_OPERATE, OPERATE_LOGTYPE1);
		ResultType iUseResult = pkPlayer->UseItemInPackByIndex(pack_type, entry_index, item_count, param, ext, buddy_id,nLogCode, nParams);
		if (iUseResult == RE_ITEM_USE_SUCCESS)
		{
			MG_RLT_UseItemInPackage sucMsg;
			sucMsg.m_lItemID = item_id;
			sucMsg.m_iParam = item_count;
			pkPlayer->SendMsgToClient(MGPT_RLT_USE_ITEM_IN_PACKAGE, &sucMsg);
			pkPlayer->SyncBackPackToClient(pack_type);
		}
		else
		{
			MG_Message errorMsg;
			errorMsg.m_iMessageID = iUseResult;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		}
	}

	object_id_type actor_id;
	int32_t pack_type;
	int32_t entry_index;
	int32_t item_id;
	int32_t item_count;
	int32_t param;
	int32_t buddy_id;
	StringType ext;
};

ON_HANDLE(MGPT_REQ_USE_ITEM_IN_PACKAGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	MG_REQ_UseItemInPackage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pUsePlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(reqMsg.m_lPlayerID);
	if ( pUsePlayer == NULL )
		return;

	if ( pUsePlayer->GetOwnerPlayerId() != pkOwnerPlayer->GetID() )
		return;

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ServerItem* pkItem = (LC_ServerItem*)pkItemFactory->RequestItem(reqMsg.m_lItemID);
	if (NULL == pkItem || reqMsg.m_lItemCount <= 0)
	{
		return;
	}

	//只有经验丹和卡牌碎片可以给未出战的buddy使用
	//经验丹和卡牌碎片特殊逻辑判断
	if(!pkItem->CanUsedForNotReadyBuddy())
	{
		pkPlayer = pkPlayer->GetActivateBuddyByID(reqMsg.m_nBuddyID);
		if(NULL == pkPlayer)
			return;
	}

	UseItemInPackByIndexOP kOP;
	//kOP.actor_id = pkPlayer->GetID();
	//kOP.actor_id = pkOwnerPlayer->GetID();
	kOP.actor_id = reqMsg.m_lPlayerID;
	kOP.pack_type = reqMsg.m_iPackType;
	kOP.entry_index = reqMsg.m_iPackageEntryIndex;
	kOP.item_id = reqMsg.m_lItemID;
	kOP.item_count = UT_MathBase::MinInt(reqMsg.m_lItemCount, 9999);
	kOP.param = reqMsg.m_iParam;
	kOP.ext = reqMsg.m_ext.c_str();
	kOP.buddy_id = reqMsg.m_nBuddyID;
	{
		std::string val;
		kOP(RE_SUCCESS, val);
	}
}

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_EQUIP_SLOT_LV, this_source_type, false)
{
	//TODO
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if (pOwnerPlayer == NULL )
	{
		return;
	}

	MGPT_REQ_EquipSlotLv reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		object_id_type lPlayerID = pOwnerPlayer->GetID();
		int32_t nBuddyId = reqMsg.m_nBuddyId;
		
		BuddyInstance* pInstance = pOwnerPlayer->GetBuddyInstanceByID(nBuddyId);
		MGPT_RLT_EquipSlotLv equipGradeMsg;
		// 检查 英雄装备 所有槽位是否已具备升阶条件
		// 检查英雄等级是否允许 进阶
		if(pInstance->CheckEquipUpgradeConfig())
		{
			pInstance->EquipUpgrade();
			
			equipGradeMsg.m_nResult = RE_EQUIP_UPGRADE_SUCCESS;
			pkPlayer->SendMsgToClient(MGPT_RLT_EQUIP_SLOT_LV, &equipGradeMsg);
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		}
		else
		{
			equipGradeMsg.m_nResult = RE_EQUIP_UPGRADE_FAIL;
			pkPlayer->SendMsgToClient(MGPT_RLT_EQUIP_SLOT_LV, &equipGradeMsg);
		}
	}
}

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_RECEIVE_DAILY_REWARD, this_source_type, false)
{
	//TODO 每日领奖
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
	{
		return;
	}

	MGPT_REQ_Daily_Reward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	 pkPlayer->ReceiveDailyReward_RongGuangYuanZheng();
	 pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_COLLECT_PLAYER_EQUIP_SUIT, this_source_type, false)
{
	// 装备收集
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
	{
		return;
	}

	MGPT_REQ_Collect_Player_Equip_Suit reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;
	MGPT_RESP_Collect_Player_Equip_Suit resMsg;
	resMsg.m_nResult = RE_SUCCESS;
	resMsg.m_uOpType = reqMsg.m_uOpType;
	//if(reqMsg.m_uOpType == CET_MsgSuitInfo::CET_MsgSuitInfo)
	if(reqMsg.m_uOpType == CollectEquipType::CET_MsgReward)
		resMsg.m_nResult = pkPlayer->ReceiveRewardBySuitCollection(reqMsg.m_uStepLevel,reqMsg.m_uType);
	else if(reqMsg.m_uOpType == CollectEquipType::CET_MsgActivate)
		resMsg.m_nResult = pkPlayer->activateSuitCollection(reqMsg.m_uStepLevel);
	
	pkPlayer->SendMsgToClient(MGPT_RESP_COLLECT_PLAYER_EQUIP_SUIT, &resMsg);
	pkPlayer->NotifyCollectEquipInfo();
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_PACK_SWITCH_ITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_PACK_SwitchItem reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		/*
		object_id_type lPlayerID	= reqMsg.m_lPlayerID;
		if (lPlayerID != pkPlayer->GetID())
		{
			return;
		}
		*/

		MG_RLT_PACK_SwitchItem sucMsg;
		sucMsg.m_lPlayerID = pkPlayer->GetOwnerObjectID();
		sucMsg.m_iSrcPackType = reqMsg.m_iSrcPackType;
		sucMsg.m_iSrcIndex = reqMsg.m_iSrcIndex;
		sucMsg.m_iTargetPackType = reqMsg.m_iTargetPackType;
		sucMsg.m_iTargetIndex = reqMsg.m_iTargetIndex;
		sucMsg.m_Result = RE_SUCCESS;

		//安全系统检查
		if (IS_WAREHOUSE_PACK(reqMsg.m_iSrcPackType) || IS_WAREHOUSE_PACK(reqMsg.m_iTargetPackType))
		{
			//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
			//if (NULL == pkSafeSystemAsset)
			//{
			//	return;
			//}
			//sucMsg.m_Result = pkSafeSystemAsset->CheckOperateWithPassword();
			LC_PackAsset* pack = pkPlayer->GetPackAsset();
			if (sucMsg.m_Result == RE_SUCCESS)
			{
				LC_BackPackEntry* item1 = pack->GetBackPackEntry(reqMsg.m_iSrcPackType, reqMsg.m_iSrcIndex);
				if (item1)
				{
					if (reqMsg.m_iSrcPackType != reqMsg.m_iTargetPackType)
					{
						if (IS_WAREHOUSE_PACK(reqMsg.m_iSrcPackType))
						{
							oss_role_store(pkPlayer, item1);
						}
						else
						{
							oss_role_takeout(pkPlayer, item1);
						}
					}
				}
				else
				{
					sucMsg.m_Result = RE_ITEM_SYSTEM_ERROR;
				}
			}

			if (sucMsg.m_Result == RE_SUCCESS)
			{
				LC_BackPackEntry* item2 = pack->GetBackPackEntry(reqMsg.m_iTargetPackType, reqMsg.m_iTargetIndex);
				if (item2 == NULL || (reqMsg.m_bNeedEmpty == true && item2->GetBackPackValueByType(LPSFET_VALID) == true))
				{
					//再找一个合适的位置
					item2 = pack->GetEmptyPackEntry(reqMsg.m_iTargetPackType, reqMsg.m_iTargetIndex);
					if (item2 == NULL)
					{
						//找寻不到合适位置
						sucMsg.m_Result = RE_ITEM_SYSTEM_ERROR;
					}
				}
				else if (item2->GetBackPackValueByType(LPSFET_VALID) == true)
				{
					if (reqMsg.m_iSrcPackType != reqMsg.m_iTargetPackType)
					{
						if (IS_WAREHOUSE_PACK(reqMsg.m_iSrcPackType))
						{
							oss_role_store(pkPlayer, item2);
						}
						else
						{
							oss_role_takeout(pkPlayer, item2);
						}
					}
				}
			}
		}
		if (sucMsg.m_Result == RE_SUCCESS)
		{
			sucMsg.m_Result = pkPlayer->ExchangeItemsInPackage(reqMsg.m_iSrcPackType, reqMsg.m_iSrcIndex, reqMsg.m_iTargetPackType, reqMsg.m_iTargetIndex);
			pkPlayer->SyncBackPackToClient(reqMsg.m_iSrcPackType);
			if(reqMsg.m_iSrcPackType != reqMsg.m_iTargetPackType)
			{
				pkPlayer->SyncBackPackToClient(reqMsg.m_iTargetPackType);
			}
		}
			
		pkPlayer->SendMsgToClient(MGPT_RLT_PACK_SWITCH_ITEM, &sucMsg);
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_PACK_SPLIT_ITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_PACK_SplitItem reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		if (reqMsg.m_iSplitNum <= 0)
		{
			return;
		}
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		if (pkPackAsset == NULL)
		{
			return;
		}

		ResultType iResult = pkPlayer->SplitItemInPackageToAssignEntry(reqMsg.m_iPackType, reqMsg.m_iSrcIndex, reqMsg.m_iTargetIndex, reqMsg.m_iSplitNum);

		if (iResult == RE_SUCCESS)
		{
			MG_RLT_PACK_SplitItem sucMsg;
			LC_BackPackEntry* pkEntryTemp = pkPackAsset->GetBackPackEntry(reqMsg.m_iPackType, reqMsg.m_iSrcIndex);
			if (pkEntryTemp && pkEntryTemp->GetBackPackValueByType(LPSFET_VALID))
			{
				sucMsg.m_lItemID = pkEntryTemp->GetBackPackValueByType(LPSFET_TYPE_ID);
			}
			pkPlayer->SendMsgToClient(MGPT_RLT_PACK_SPLIT_ITEM, &sucMsg);
			pkPlayer->SyncBackPackToClient(reqMsg.m_iPackType);
		}
		else
		{
			MG_Message errorMsg;
			errorMsg.m_iMessageID = iResult;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		}
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_MERGE_ITEM_IN_PACKAGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_MergeItemInPackage reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		//object_id_type lPlayerID = reqMsg.m_lPlayerID;
		object_id_type lPlayerID = pkPlayer->GetOwnerObjectID();
		int	 iDstEntryIndex = reqMsg.m_iDstEntryIndex;
		int  iSrcEntryIndex = reqMsg.m_iSrcEntryIndex;
		int  lItemID = reqMsg.m_lItemID;

		bool bCheckResult = LC_Helper::CheckItemReqInPackage(pkPlayer, lPlayerID, reqMsg.m_iDstPackType, iDstEntryIndex, lItemID);
		if (bCheckResult == false)
		{
			return;
		}

		bCheckResult = LC_Helper::CheckItemReqInPackage(pkPlayer, lPlayerID, reqMsg.m_iSrcPackType, iSrcEntryIndex, lItemID);
		if (bCheckResult == false)
		{
			return;
		}

		ResultType iResult = pkPlayer->MergeItemsInPackage(reqMsg.m_iSrcPackType, iSrcEntryIndex, reqMsg.m_iDstPackType, iDstEntryIndex);
		if (iResult == RE_SUCCESS)
		{
			MG_RLT_MergeItemInPackage sucMsg;
			sucMsg.m_lItemID   = lItemID;
			pkPlayer->SendMsgToClient(MGPT_RLT_MERGE_ITEM_IN_PACKAGE, &sucMsg);
			pkPlayer->SyncBackPackToClient(reqMsg.m_iDstPackType);
			if(reqMsg.m_iSrcPackType != reqMsg.m_iDstPackType)
			{
				pkPlayer->SyncBackPackToClient(reqMsg.m_iSrcPackType);
			}

		}
		else
		{
			MG_Message errorMsg;
			errorMsg.m_iMessageID = iResult;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		}
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TIDY_ITEM_IN_PACKAGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_PACK_Tidy reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		ResultType iUseResult = pkPlayer->TidyItemsInPackage(reqMsg.m_iPackType);
		if (iUseResult == RE_SUCCESS)
		{
			MG_RLT_PACK_Tidy sucMsg;
			pkPlayer->SendMsgToClient(MGPT_RLT_TIDY_ITEM_IN_PACKAGE, &sucMsg);
			pkPlayer->SyncBackPackToClient(reqMsg.m_iPackType, true);
		}
		else
		{
			MG_Message errorMsg;
			errorMsg.m_iMessageID = iUseResult;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		}
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DESTROY_ITEM_IN_PACKAGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DestroyItemInPackage reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		//object_id_type lPlayerID = reqMsg.m_lPlayerID;
		object_id_type lPlayerID = pkPlayer->GetOwnerObjectID();
		int  iEntryIdx = reqMsg.m_iEntryIndex;
		int lItemID = reqMsg.m_lItemID;

		bool bCheckResult = LC_Helper::CheckItemReqInPackage(pkPlayer, lPlayerID, reqMsg.m_iPackType, iEntryIdx, lItemID);
		if (bCheckResult == false)
		{
			return;
		}
		// 为写出OSS，获取数量
		LC_BackPackEntry* pkEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(reqMsg.m_iPackType, iEntryIdx);
		int lCount = 0;
		LC_BackPackEntry kOssEntry;
		if (pkEntry && pkEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			lCount = pkEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
			kOssEntry.Clone(pkEntry);
		}
		//背包销毁--埋点日志
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(lItemID);
		nParams.push_back(lCount);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, USERPACK_OPERATE, OPERATE_LOGTYPE1);
		ResultType iUseResult = pkPlayer->DestroyItemInPackage(reqMsg.m_iPackType, iEntryIdx, SYS_FUNC_TP_DESTROY, lPlayerID,nLogCode);
		if (iUseResult == RE_SUCCESS)
		{
			MG_RLT_DestroyItemInPackage sucMsg;
			sucMsg.m_lItemID = lItemID;
			pkPlayer->SendMsgToClient(MGPT_RLT_DESTROY_ITEM_IN_PACKAGE, &sucMsg);
			oss_role_del_item(pkPlayer, &kOssEntry);
			pkPlayer->SyncBackPackToClient(reqMsg.m_iPackType);
		}
		else
		{
			MG_Message errorMsg;
			errorMsg.m_iMessageID = iUseResult;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		}
		
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_IDENTIFY_ITEM_IN_PACKAGE, this_source_type, false)
{

}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEMENHANCE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_EnhanceItemInPackage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//object_id_type lPlayerID = reqMsg.m_lPlayerID;
	object_id_type lPlayerID = pkPlayer->GetOwnerObjectID();
	//	int  iPackType = reqMsg.m_iPackType;
	int  iEntryIndex = reqMsg.m_iPackageEntryIndex;
	int lItemID = reqMsg.m_lItemID;

	bool bCheckResult = LC_Helper::CheckItemReqInPackage(pkPlayer, lPlayerID, reqMsg.m_iPackType, iEntryIndex, lItemID);
	if (bCheckResult == false)
	{
		return;
	}

	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_ITEM, LAF_ITEM_IMMEDIATE_ITEM, lItemID))
	{
		return;
	}

	ResultType iUseResult = pkPlayer->EnhanceItem(reqMsg.m_iPackType, iEntryIndex , reqMsg.m_AttribEnhanceInfos);
	MG_RLT_EnhanceItemInPackage sucMsg;
	sucMsg.m_Result = iUseResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEMEHNANCE, &sucMsg);
	pkPlayer->SyncBackPackToClient(reqMsg.m_iPackType);
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_EXTERN_BASICBAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_Common reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	int64_t value = 0;
	int ret = reqMsg.get("play_id", value);
	if (!ret)
	{
		return;
	}
	// 判断是否使用物品扩充
	int64_t use_item_value = 0;
	int32_t use_item = 0;
	ret = reqMsg.get("use_item", use_item_value);
	if (ret)
	{
		// 使用物品扩充
		int64_t k_use_item = boost::strval::str_to_val("use_item");
		use_item = get_value(reqMsg, k_use_item, 0);
	}

	static const int64_t k_play_id = boost::strval::str_to_val("play_id");
	static const int64_t k_slot = boost::strval::str_to_val("bag_slot");
	object_id_type lPlayerID = get_value(reqMsg, k_play_id, 0);
	int slot = get_value(reqMsg, k_slot, 0);

	if (!(lPlayerID))
	{
		return;
	}

	ResultType res = RE_SUCCESS;
	if (use_item)
	{
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		LC_BasicPack& pack = pkPackAsset->GetBasicPack();
		// 使用物品扩充
		do
		{
			typedef mem::map<int32_t, int32_t> ItemCountCacheType;
			ItemCountCacheType kItemCache;
			ItemCountCacheType kItemUsed;
			LC_ShopIDMap kShopIDMap;
			for (int i = pack.m_iSize; i <= slot; i++)
			{
				// 取得CSV配置行
				NS_CF_OpenBag::DataEntry* csv_entry = SafeGetCSVFileDataEntryPtr<CF_OpenBag>(i);
				if (!csv_entry)
				{
					// 未找到配置行
					res = RE_FAIL;
					MG_Common sucMsg;
					sucMsg.set("result", res);
					pkPlayer->SendMsgToClient(MGPT_RLT_EXTERN_BASICBAG, &sucMsg);
					return;
				}

				int32_t iItemID = csv_entry->_ItemID;
				ItemCountCacheType::iterator it = kItemCache.find(iItemID);
				if (it == kItemCache.end())
				{
					kItemCache[iItemID] = pkPackAsset->GetCountCanDeleteInPack(iItemID);
				}

				if (kItemCache[iItemID] >= csv_entry->_ItemCount)
				{
					kItemUsed[iItemID] += csv_entry->_ItemCount;
					kItemCache[iItemID] -= csv_entry->_ItemCount;
				}
				else
				{
					if (csv_entry->_ShopID <= 0)
					{
						return;
					}
					kItemUsed[iItemID] += kItemCache[iItemID];
					kShopIDMap[csv_entry->_ShopID] += csv_entry->_ItemCount - kItemCache[iItemID];
					kItemCache[iItemID] = 0;
				}
			}
			Utility::UT_SIMDataList dataInfoList;
			ItemCountCacheType::iterator it = kItemUsed.begin();
			for (; it != kItemUsed.end(); ++it)
			{
				Utility::UT_SIMDataInfo dataInfo;
				// 设置需要消耗的物品ID，物品数量
				dataInfo.SetItemInfo(it->first, it->second, 0);
				dataInfoList.push_back(dataInfo);
			}
			PackOperateInfoList kConsumeOPs;
			LC_Helper::BuildConsumeItemOPs(dataInfoList, kConsumeOPs);
			// 检查物品数量
			res = LC_Helper::CheckItemOPs(pkPlayer->GetPackAsset(), kConsumeOPs);
			if (RE_SUCCESS != res)
			{
				res = RE_ITEM_OP_ITEM_LCAK;
				break;
			}
			//检查消耗钱
			if (!kShopIDMap.empty())
			{
				res = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
				if (RE_SUCCESS != res)
				{
					break;
				}
			}
			//背包扩充--埋点日志
			vector<int> nParams;
			nParams.clear();
			nParams.push_back(slot+1);
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, USERPACK_OPERATE,OPERATE_LOGTYPE2);

			// 使用物品扩充
			res = LC_Helper::DoItemOPs(pkPlayer, kConsumeOPs, SYS_FUNC_TP_EXTERN_BAG, PACK_TYPE_BASIC, true, true,nLogCode);
			if (RE_SUCCESS != res)
			{
				res = RE_ITEM_OP_ITEM_LCAK;
				break;
			}
			//使用元宝
			res = LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_EXTERN_BAG, PACK_TYPE_BASIC,NULL, nLogCode);
			if (RE_SUCCESS != res)
			{
				break;
			}
			// 扩充背包
			for (int i = pack.m_iSize; i <= slot; i++)
			{
				res = pkPlayer->ExternBasicBag(i);
				if (RE_SUCCESS != res)
				{
					MG_Common sucMsg;
					sucMsg.set("result", res);
					pkPlayer->SendMsgToClient(MGPT_RLT_EXTERN_BASICBAG, &sucMsg);
					return;
				}
			}
			AchievementEvent kEvent;
			kEvent.SetNum(pack.m_iSize);
			pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_OPENBAGCOUNT, kEvent);
		} while (0);
	}
	else
	{
		do
		{
			// 使用元宝扩充
			LC_ShopIDMap kShopIDMap;
			res = pkPlayer->CheckExternBasicBag(slot, kShopIDMap);
			if (RE_SUCCESS != res)
			{
				break;
			}
			res = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
			if (RE_SUCCESS != res)
			{
				break;
			}
			res = pkPlayer->ExternBasicBag(slot);
			if (RE_SUCCESS != res)
			{
				break;
			}
			//背包扩充--埋点日志
			vector<int> nParams;
			nParams.clear();
			nParams.push_back(slot+1);
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, USERPACK_OPERATE,OPERATE_LOGTYPE2);
			if (!kShopIDMap.empty())
			{
				LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_EXTERN_BAG, PACK_TYPE_BASIC, NULL, nLogCode, nParams);
			}

			LC_BasicPack& pack = pkPlayer->GetPackAsset()->GetBasicPack();
			AchievementEvent kEvent;
			kEvent.SetNum(pack.m_iSize);
			pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_OPENBAGCOUNT, kEvent);
			/*pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);*/
		} while (0);
	}
	{
		MG_Common sucMsg;
		sucMsg.set("result", res);
		pkPlayer->SendMsgToClient(MGPT_RLT_EXTERN_BASICBAG, &sucMsg);
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEMDECOMPOSIT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DecompositItemInPackage reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	/*
	if (pkPlayer->GetID() != reqMsg.m_lPlayerID)
	{
		return;
	}
	*/

	MG_RLT_DecompositItemInPackage sucMsg;
	ResultType iUseResult = pkPlayer->DecompositItem(reqMsg.m_Items, sucMsg.m_DecompositItems);
	if (iUseResult == RE_SUCCESS)
	{
		sucMsg.m_Result = iUseResult;
		pkPlayer->SendMsgToClient(MGPT_RLT_ITEMDECOMPOSIT, &sucMsg);
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
	}
	else
	{
		MG_Message errorMsg;
		errorMsg.m_iMessageID = iUseResult;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
	}
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_EXPAND_WAREHOUSE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ExternWarehouse reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	ResultType iResult = pkPlayer->ExternWarehouse(reqMsg.m_iRowNum);

	MG_RLT_ExternWarehouse rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetOwnerObjectID();
	rltMsg.m_iResult = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_EXPAND_WAREHOUSE, &rltMsg);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_WAREHOUSE);
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SAVECASH_TO_WAREHOUSE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_SaveCashToWarehouse reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (pkPlayer->GetID() != reqMsg.m_lPlayerID)
	{
		return;
	}
	*/

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	if (!IS_VALID_CHAR_MONEY(reqMsg.m_llCash))
	{
		return;
	}
	ResultType iResult = pkPlayer->SaveCashToWarehouse(reqMsg.m_llCash);

	MG_RLT_SaveCashToWarehouse rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetOwnerObjectID();
	rltMsg.m_iResult = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_SAVECASH_TO_WAREHOUSE, &rltMsg);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}

//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GETCASH_FROM_WAREHOUSE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetCashFromWarehouse reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	/*
	if (pkPlayer->GetID() != reqMsg.m_lPlayerID)
	{
		return;
	}
	*/

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	if (!IS_VALID_CHAR_MONEY(reqMsg.m_llCash))
	{
		return;
	}
	ResultType iResult = pkPlayer->GetCashFromWarehouse(reqMsg.m_llCash);

	MG_RLT_GetCashFromWarehouse rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetOwnerObjectID();
	rltMsg.m_iResult = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_GETCASH_FROM_WAREHOUSE, &rltMsg);
}
//纹章雕刻
ON_HANDLE(MGPT_REQ_REINFORCE_EQUIPSLOT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	if (!pkPlayer->IsInGame())
	{
		return;
	}

	MG_REG_Reinforce_EquipSlot reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->ReinforceEquipSlot(reqMsg.m_iReinforeType, reqMsg.m_iBuddyId, reqMsg.m_iSlotID);	
}

ON_HANDLE(MGPT_REQ_RUNE_UPGRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RuneUpgrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->RuneUpgrade_New(reqMsg.m_iBuddyID, reqMsg.m_iSlotIndex, reqMsg.m_iTargetLevel);
}

ON_HANDLE(MGPT_REQ_RUNE_STAR, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RuneStar reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->RuneStar_New(reqMsg.m_iBuddyID, reqMsg.m_iSlotIndex);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}

ON_HANDLE(MGPT_REQ_RESET_RUNE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ResetRune reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->ResetRune_New(reqMsg.m_iBuddyID, reqMsg.m_iSlotIndex);
}

ON_HANDLE(MGPT_REQ_EQUIP_RUNE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_EquipRune reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->EquipRuneToBuddy(reqMsg.m_iSrcBuddyID, reqMsg.m_iSrcIndex, reqMsg.m_iDestBuddyID, reqMsg.m_iDestIndex);
}

ON_HANDLE(MGPT_REQ_UNEQUIP_RUNE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_UnEquipRune reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->UnEquipRuneFromBuddy(reqMsg.m_iBuddyID, reqMsg.m_iSrcSlotIndex);
}

ON_HANDLE(MGPT_REQ_ONEKEY_EQUIP_RUNE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_OneKeyEquipRune reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->OneKeyEquipRuneToBuddy(reqMsg.m_iBuddyID);
}

ON_HANDLE(MGPT_REQ_ONEKEY_UNEQUIP_RUNE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_OneKeyUnEquipRune reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->OneKeyUnEquipRuneFromBuddy(reqMsg.m_iBuddyID);
}

ON_HANDLE(MGPT_REQ_EQUIPSLOT_STAR, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	if (!pkPlayer->IsInGame())
	{
		return;
	}

	MG_REG_EquipSlot_Star reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}	

	pkPlayer->EquipSlotStar(reqMsg.m_iBuddyId, reqMsg.m_iEquipSlotIndex);
}
//分解
ON_HANDLE(MGPT_REQ_DECOMPOSITION_ITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DecompositionItem reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_DecompositionItem respMsg;
	respMsg.m_nResult = pkPlayer->DecompositionEquip(reqMsg.m_DecompositionItemIndexs, respMsg.m_nDecompositionResult);
	if(RE_SUCCESS == respMsg.m_nResult)
	{
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_DECOMPOSITION_ITEM, &respMsg);
}

ON_HANDLE(MGPT_REQ_DECOMPOSITION_SETTING, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return;

	MG_REQ_DecompositionSetting reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	pkPlayer->SetDecompositionSetting(reqMsg);
	pkPlayer->SyncDecompositionSetting();
}

//装备指挥官装备
ON_HANDLE(MGPT_CS_EquipCommand_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_EquipCommand_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	player->EquipCommandEquip( msg.m_PackIndex );
	player->SyncBackPackToClient(PACK_TYPE_BASIC);
}

//指挥官装备强化
ON_HANDLE(MGPT_CS_IntensifyCommandEquip_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_IntensifyCommandEquip_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int type = msg.m_Type;

	if( type == 0 )
	{
		player->IntensifyCommandEquip( msg.m_EquipIndex );
	}
	else
	{
		player->GetCommandEquipPack().RepeatedlyIntensifyCommandEquip( player, msg.m_EquipIndex );
	}
	player->SyncBackPackToClient(PACK_TYPE_BASIC);
}

//指挥官装备精炼
ON_HANDLE(MGPT_CS_RefineCommandEquip_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_RefineCommandEquip_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	player->RefineCommandEquip( msg.m_EquipIndex );
	player->SyncBackPackToClient(PACK_TYPE_BASIC);
}

//指挥官装备铸魂
ON_HANDLE(MGPT_CS_SoulCommandEquip_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_SoulCommandEquip_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	player->SoulCommandEquip( msg.m_EquipIndex );
	player->SyncBackPackToClient(PACK_TYPE_BASIC);
}

ON_HANDLE(MGPT_REQ_EQUIPSUBITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(pkPlayer == NULL)
	{
		return;
	}

	MG_Req_EquipSubItem msg;
	if(!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->EquipSubItem(msg.m_nType, msg.m_nSrcSlotIndex);
}

ON_HANDLE(MGPT_REQ_UNEQUIPSUBITEM, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(pkPlayer == NULL)
	{
		return;
	}

	MG_Req_UnEquipSubItem msg;
	if(!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->UnEquipSubItem(msg.m_nType, msg.m_nSrcSlotIndex);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}

//装备进阶
ON_HANDLE(MGPT_CS_EquipAdvance_Req, this_source_type, false)
{
	LC_ServerPlayer* player = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if( player == NULL )
	{
		return;
	}

	MG_CS_EquipAdvance_Req msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	player->EquipAdvance( msg.m_Type, msg.m_Index );
	player->SyncBackPackToClient(PACK_TYPE_BASIC);
}


ON_HANDLE(MGPT_REQ_SUBEQUIPADVANCE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SubEquipAdvance msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SubEquipAdvance(msg.m_nSubType, msg.m_nSlotIndex);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}
}
