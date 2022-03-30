#include "LM_LocaleManager.h"
#include "GameServerApp.h"

#include "CF_ItemList.h"
#include "CF_SocketItemFunc.h"
#include "CF_WarehouseCost.h"
#include "CF_TranscriptionRule.h"
#include "CF_TranscriptionSettleGrade.h"
#include "CF_ItemGen.h"
#include "CF_WorldMapList.h"
#include "CF_ItemEquipLimitFunc.h"
#include "CF_ItemAttrActive.h"
#include "CF_GemExp.h"
#include "CF_GemReinforceLevel.h"
#include "CF_Helper.h"
#include "CF_ItemEffect.h"
#include "CF_ItemUseLimitFunc.h"

#include "LC_ItemBase.h"
#include "LC_ServerPlayer.h"
#include "LC_RumorManager.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_ServerPackEntryFactory.h"
#include "UT_ServerHelper.h"
#include "LC_ItemFactoryBase.h"
#include "LC_GameEventManager.h"
#include "LC_ItemSuitManager.h"
#include "LC_ServerItem.h"
#include "LC_ServerLogicManager.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_GameStory_Manager.h"
#include "LC_MillionWeaponAsset.h"

#include "SC_ScriptAction.h"

#include "msg.pb.h"
#include "oss_define.h"
#include "oss_360_define.h"
#include "../Buddy/BuddyInstance.h"
#include "GlobalSettings.h"

#include "CF_RuneLevelUp.h"
#include "CF_RuneStar.h"
#include "CF_ItemDecompositionFunc.h"
#include "CF_EquipSlotStar.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerGuildManager.h"
#include "ItemConfigManager.h"
#include "UT_OperateLog.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Utility;
using namespace Protocol;
using namespace OPERATELOG;
class BuddyInstance;


ResultType LC_ServerPlayer::EnhanceItem(int iPackType,int iEntryIndex , std::vector<Protocol::EnhanceItemInfo>& infos)
{
	if(!IsInstance())
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	return RE_ENHANCE_SUCCESS;
}

void LC_ServerPlayer::HandlePackOPRecord(GameLogic::PackOPRecordSet* pkPackOpRecords, int32_t iReasonType, uint64_t iReasonDetail)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return UT_ServerHelper::HandlePackOPRecord(this, pkPackOpRecords, iReasonType, iReasonDetail);;

	if(!IsMainController())
	{
		return UT_ServerHelper::HandlePackOPRecord(m_pOwnerPlayer, pkPackOpRecords, iReasonType, iReasonDetail);
	}
	return UT_ServerHelper::HandlePackOPRecord(this, pkPackOpRecords, iReasonType, iReasonDetail);
}

ResultType LC_ServerPlayer::DoCommonItemOperation(LC_BackPackEntry* pkPackEntry, LC_ItemOperationDataEntry* pkOperation, bool bLock, int32_t iCustomProb, PackOperateInfoList* pkCustomOP, int32_t iPay)
{
	if (NULL==pkPackEntry || NULL==pkOperation)
	{
		return RE_FAIL;
	}

	ResultType rslt = RE_SUCCESS;
	const LC_ShopIDMap* kLockFinalShopIDMap = &pkOperation->_kLockFinalShopIDs;
	const LC_ShopIDMap* kConsumeShopIDMap = &pkOperation->_kConsumeShopIDs;
	do 
	{
		switch (iPay)
		{
		case LC_PAY_WITH_ITEM:
			{
				const UT_SIMDataList& _kConsumeItems = pkOperation->GetConsumeItems();
				if (!_kConsumeItems.empty() || (pkCustomOP&&!pkCustomOP->empty()))
				{
					PackOperateInfoList kConsumeOPs;
					LC_Helper::BuildConsumeItemOPs(_kConsumeItems, kConsumeOPs);
					if (pkCustomOP)
					{
						kConsumeOPs.insert(kConsumeOPs.end(), pkCustomOP->begin(), pkCustomOP->end());
					}
					rslt = LC_Helper::DoItemOPs(this, kConsumeOPs, SYS_FUNC_TP_ITEM_OPERATION, pkOperation->_iType);
					if (RE_SUCCESS != rslt)
					{
						rslt = RE_ITEM_OP_ITEM_LCAK;
						break;
					}
				}
			}break;
		case LC_PAY_WITH_BIND_YUANBAO:
			{
				kConsumeShopIDMap = &pkOperation->_kReplaceShopIDs1;
				kLockFinalShopIDMap = &pkOperation->_kLockFinalShopIDs1;
			}break;
		case LC_PAY_WITH_UNBIND_YUANBAO:
			{
				kConsumeShopIDMap = &pkOperation->_kReplaceShopIDs2;
				kLockFinalShopIDMap = &pkOperation->_kLockFinalShopIDs2;
			}break;
		default:
			rslt = RE_FAIL;
			break;
		}
		if (RE_SUCCESS != rslt)
			break;


		//元宝锁定成功
		if (bLock)
		{
			if (kLockFinalShopIDMap && !kLockFinalShopIDMap->empty())
			{
				rslt = LC_Helper::PayForShop(this, *kLockFinalShopIDMap, SYS_FUNC_TP_ITEM_OP_LOCK, pkOperation->_iKey);
				if (RE_SUCCESS != rslt)
				{
					break;
				}
			}
		}
		else
		{
			if (kConsumeShopIDMap && !kConsumeShopIDMap->empty())
			{
				rslt = LC_Helper::PayForShop(this, *kConsumeShopIDMap, SYS_FUNC_TP_ITEM_OPERATION, pkOperation->_iType);
				if (RE_SUCCESS != rslt)
				{
					break;
				}
			}

			int32_t iFinalProb = iCustomProb>0 ? iCustomProb :pkOperation->_iProbability;
			if (UT_MathBase::RandInRangeInt(1, MAX_ITEM_OPERATION_PROB_UP) > iFinalProb)
			{
				rslt = RE_ITEM_OP_FAIL;
				break;
			}
		}
		break;
	}while(1);
	return rslt;
}

#define REFINE_ITEM_ID 11114009

ResultType LC_ServerPlayer::CheckExternBasicBag(int slot, GameLogic::LC_ShopIDMap& shop_info)
{
	if(slot < 0 || slot >= MAX_BASIC_PACK_COUNT )
	{
		return RE_FAIL;
	}
	LC_BasicPack& pack = GetPackAsset()->GetBasicPack();

	int now_size = pack.GetSize();
	if( slot < now_size )
	{
		return RE_FAIL;
	}

	LC_OpenBagAsset& asset = GetOpenBagAssert();
	OpenAssetParam * param = asset.GetParam();
	int TotalYB = 0;
	for( int i = now_size; i <= slot; ++i )
	{
		OpenAssetParam::open_bag_cost_map_type::iterator pos = param->cost_map.find(i);
		if( pos != param->cost_map.end() )
		{
			open_bag_cost& cost = pos->second;
			if(cost.yb && cost.shop_id && cost.shop_count)
			{
				TotalYB += cost.yb;
				shop_info[cost.shop_id] += cost.shop_count;

			}
		}
	}

	if(TotalYB > 0)
	{
		if(!IS_VALID_CHAR_MONEY(TotalYB))
		{
			return RE_YUANBAO_NOT_ENOUGH;
		}
		if (!IsCashEnough(TotalYB,CT_BIND_YUANBAO))
		{
			return RE_YUANBAO_NOT_ENOUGH;
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ServerPlayer::ExternBasicBag(int slot)
{
	++slot;

	LC_BasicPack& pack = GetPackAsset()->GetBasicPack();
	LC_OpenBagAsset& asset = GetOpenBagAssert();
	pack.SetSize(slot);

	SyncBackPackToClient(PACK_TYPE_BASIC);
	asset.UpdateParam(slot);
	return RE_SUCCESS;
}

ResultType LC_ServerPlayer::DecompositItem(std::vector<Protocol::DecompositItemInfo>& items , std::vector<Protocol::DecompositItemResult>& resitems)
{
	if(!IsInstance())
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ServerItemFlopManager* pkServerItemFlopManager = LC_ServerItemFlopManager::GetSingletonPtr();
	LC_PackAsset* pkPackAsset = GetPackAsset();
	if(NULL == pkPackAsset)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}
	LC_ItemFlopInfoList kItemFlopList;
	PackOperateInfoList objOPList;
	PackOperateInfo objData;
	LC_ItemBase* aItem = NULL;
	for(std::vector<DecompositItemInfo>::iterator i = items.begin(); i != items.end(); ++i)
	{
		DecompositItemInfo& info = *i;
		DecompositItemResult result;
		result.m_iPackType = info.m_iPackType;
		result.m_iPackageEntryIndex = info.m_iPackageEntryIndex;
		result.m_Result = RE_FAIL;
		LC_BackPackEntry* pkEquipEntry = GetPackAsset()->GetBackPackEntry(info.m_iPackType, info.m_iPackageEntryIndex);
		if(NULL == pkEquipEntry)
		{
			result.m_Result = RE_ITEM_NOT_HAVE;
			resitems.push_back(result);
			continue;
		}
		if(pkEquipEntry->GetBackPackValueByType(LPSFET_VALID) == false)
		{
			result.m_Result = RE_ITEM_NOT_HAVE;
			resitems.push_back(result);
			continue;
		}
		if(pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID) != (int)info.m_lItemID)
		{
			result.m_Result = RE_ITEM_SYSTEM_ERROR;
			resitems.push_back(result);
			continue;
		}
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID));
		if(NULL == pkItem)
		{
			result.m_Result = RE_ITEM_SYSTEM_ERROR;
			resitems.push_back(result);
			continue;
		}
		aItem = pkItem;
		result.m_Result = pkItem->CheckDecompositionItem(this,pkEquipEntry);
		if(result.m_Result != RE_SUCCESS)
		{
			resitems.push_back(result);
			continue;
		}
		int funcid = pkItem->GetDecompositionFuncID();
		if(funcid <= 0)
		{
			result.m_Result = RE_ITEM_DECOMPOSITION_FAIL_CANT;
			resitems.push_back(result);
			continue;
		}

		pkServerItemFlopManager->GenerateItemFlopInfoOnDecomposition(kItemFlopList, this, funcid, NULL);
		objData.Reset();
		objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
		objData.m_bOPEntry = false;
		objData.m_iPackType  = info.m_iPackType;
		objData.m_iIndex   = info.m_iPackageEntryIndex;
		objData.m_lCount   = pkEquipEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
		objOPList.push_back(objData);
		result.m_Result = RE_ITEM_DECOMPOSITION_SUCCESS;
		resitems.push_back(result);
	}
	if(objOPList.empty())
	{
		return RE_ITEM_DECOMPOSITION_SUCCESS;
	}
	LC_ItemFlopInfo* pkItemFlopInfo = kItemFlopList.GetItemFlopInfoByIndex(0);
	LC_BackPackEntryList& rkBackEntryList = pkItemFlopInfo->GetBackPackEntryList();
	for(LC_BackPackEntryList::iterator i = rkBackEntryList.begin(); i != rkBackEntryList.end(); ++i)
	{
		LC_BackPackEntry& rkBackPackEntry = *i;
		objData.Reset();
		int iPackType = GetWhichPackAutoPut(rkBackPackEntry.GetBackPackValueByType(LPSFET_TYPE_ID));
		objData.m_iOPType = OPT_ADD_ITEM;
		objData.m_bOPEntry = true;
		objData.m_bMerger  = true;
		objData.m_iPackType  = iPackType;
		objData.m_kPackEntry.Clone(rkBackPackEntry);
		objOPList.push_back(objData);
	}
	ResultType iResult = pkPackAsset->ExecutePackOP(objOPList,true);
	if(iResult == RE_SUCCESS)
	{
		LC_BackPackEntryPtrList nitems;
		make_new_item_info( nitems , this );
		PackOPRecordSet * record_set = pkPackAsset->GetRecordSet();
		oss_role_decompund_equip(this, nitems ,record_set->m_Delete_Items);

		SyncBackPackToClient(PACK_TYPE_BASIC);
		HandlePackOPRecord(record_set, SYS_FUNC_TP_ITEM_DECOMPOSITE, 0);
	}
	return iResult;
}

//--------------------------------------------------------------------------------------

ResultType LC_ServerPlayer::TryDeleteItem(int lItemID, int lItemCount)
{
	return LC_Helper::CanDeleteItem(&m_kPackAsset, lItemID, lItemCount);
}

int LC_ServerPlayer::OnUseItem(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  uint32_t type)
{
	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if ( pOwner == NULL )
		return false;

	if (!IsMainController())
		return pOwner->OnUseItem( itemid, amount, reason, mode, remain,  type );

	oss_360_log_item_use( itemid, amount, reason, mode, remain,  type,  this);

	return true;
}

ResultType LC_ServerPlayer::NotifyAddItem(PackOperateInfoList& kItemList, mem::vector<int32_t>& slotIndex, bool isItemAdd)
{
	MG_SyncAddRune runeInfoMsg;
	if(kItemList.size() != slotIndex.size())
	{
		return RE_FAIL;
	}

	int32_t i=0;
	for(PackOperateInfoList::iterator it=kItemList.begin(); it!=kItemList.end()&&i<slotIndex.size(); ++it,++i)
	{
		PackOperateInfo kItem = *it;
		int lItemID = kItem.m_kPackEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
		int lItemCount = kItem.m_kPackEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
		CF_ItemList::DataEntry* pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID);
		if  (pkItemData == NULL)
		{
			return RE_FAIL;
		}

		if(LC_Helper::CheckIsRune(lItemID))
		{
			//分解宝具
			bool tag = DecompositionRune(lItemID, slotIndex[i], runeInfoMsg.m_runeInfo);
			if(!tag)
			{
				AchievementEvent kEvent1;
				kEvent1.SetConditionInfo(pkItemData->_lRuneLevel);
				kEvent1.SetNum(1);
				UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_RUNE_LEVEL, kEvent1);

				AchievementEvent kEvent2;
				kEvent2.SetConditionInfo(pkItemData->_InitStar);
				kEvent2.SetNum(1);
				UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_RUNE_STARLEVEL, kEvent2);

				UpdateRankInfoToRedis(Sub_Reward_Type_RuneItemCombatScore,  false, lItemID);
				UpdateRankInfoToRedis(Sub_Reward_Type_AllRuneItemCombatScore,  false, lItemID);
			}
		}

		//添加圣契，更新圣契收集记录
		UpdateHolyDeedRecord(lItemID, lItemCount);
		m_CollectEquipRecord.activateEquipCollection(this, lItemID);
		NotifyCollectEquipInfo();
	}

	if(!runeInfoMsg.m_runeInfo.empty())
	{
		runeInfoMsg.m_bIsItemAdd = isItemAdd;
		SendMsgToClient(MGPT_SYNC_ADDRUNE, &runeInfoMsg);		

		for(int i=0; i<runeInfoMsg.m_runeInfo.size(); ++i)
		{
			if(runeInfoMsg.m_runeInfo[i].m_bFirstAdd)
			{
				UpdateRuneRewardRecord(runeInfoMsg.m_runeInfo[i].m_nRuneID);
			}
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ServerPlayer::AddItem(int lItemID, int lItemCount, bool bAutoUsed, int goalID, int storyID,int nLogCode ,const vector<int> &Params)
{
	if(lItemCount == 0)
	{
		return RE_SUCCESS;
	}

	CF_ItemList::DataEntry* pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID);
	if  (pkItemData == NULL)
	{
		return RE_FAIL;
	}
	
	LC_BackPackEntryList kEntryList;
	kEntryList.clear();
	LC_PackEntryFactory::GetSingletonPtr()->CreateItemEntry(lItemID, lItemCount, CIET_NONE, kEntryList, -1, goalID, storyID);
	PackOperateInfoList objOPList;
	for (int i = 0 ; i < (int)kEntryList.size() ; ++i)
	{
		PackOperateInfo objData;
		objData.Reset();

		objData.m_iOPType = OPT_ADD_ITEM;
		objData.m_bOPEntry = true;
		objData.m_bMerger  = true;
		objData.m_kPackEntry.Clone(kEntryList[i]);
		objOPList.push_back(objData);
	}

	TryInsertItemToRecordSet(lItemID);

	oss_360_log_item_log(lItemID, SYS_FUNC_TP_ITEM_BEGIN, lItemCount, this);

	ResultType res = m_kPackAsset.ExecutePackOP(objOPList, true, NULL, nLogCode,Params);
	if (res == RE_SUCCESS)
	{
		PackOPRecordSet* record_set = m_kPackAsset.GetRecordSet();
		HandlePackOPRecord(record_set, SYS_FUNC_TP_GM, 0);

		CF_ItemUseLimitFunc::DataEntry* pUseItem = SafeGetCSVFileDataEntryPtr<CF_ItemUseLimitFunc>(pkItemData->_lUseLimitFuncID);
		if (NULL!=pUseItem && pUseItem->_lAutoUse && bAutoUsed && record_set->m_Add_Items.size() > 0 )
		{
			// 触发服务端自动使用
			//PackOPRecordSet::PackOPRecords::iterator it = record_set->m_Add_Items.front();
			PackEntryOPRecord& record = record_set->m_Add_Items.front();
			//if (it != record_set->m_Add_Items.end())
			UT_ServerHelper::TryAutoUseItem( this, GetCurrentActivateBuddyID(), lItemID, record.m_lPackType, record.m_lPackIndex, 1 );
		}
	}

	return res;
}

ResultType LC_ServerPlayer::DeleteItem(int lItemID, int lItemCount,int nLogCode ,const vector<int> &Params)
{
	PackOperateInfoList objOPList;

	PackOperateInfo objData;
	objData.Reset();
	objData.m_iOPType = OPT_DELETE_ITEM_BY_ID;
	objData.m_bOPEntry = false;
	objData.m_lItemID  = lItemID;
	objData.m_lCount   = lItemCount;
	objData.m_bMerger  = false;
	objOPList.push_back(objData);
	oss_360_log_item_log(lItemID, SYS_FUNC_TP_DESTROY, -lItemCount, this);

	ResultType res = m_kPackAsset.ExecutePackOP(objOPList, true, NULL, nLogCode, Params);
	if(RE_SUCCESS == res)
	{
		SyncBackPackToClient(PACK_TYPE_BASIC);
	}
	return res;
}

ResultType LC_ServerPlayer::DeleteItemByIndex(int iPackType, int nIndex, int lItemCount,int nLogCode ,const vector<int> &Params)
{
	PackOperateInfoList objOPList;

	PackOperateInfo objData;
	objData.Reset();
	objData.m_iPackType = iPackType;
	objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
	objData.m_bOPEntry = false;
	objData.m_iIndex  = nIndex;
	objData.m_lCount   = lItemCount;
	objOPList.push_back(objData);
	ResultType res = m_kPackAsset.ExecutePackOP(objOPList, true, NULL, nLogCode, Params);
	if(RE_SUCCESS == res)
	{
		SyncBackPackToClient(PACK_TYPE_BASIC);
	}
	return res;
}

ResultType LC_ServerPlayer::DeleteItemInPackage(int iPackType,int iEntryIndex,int nLogCode ,const vector<int> &Params)
{
	if(!IsInstance())
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_BackPackEntry* pkEntry = m_kPackAsset.GetBackPackEntry(iPackType, iEntryIndex);
	if(!pkEntry)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	ResultType iResult = RE_SUCCESS;
	PackOperateInfoList objOPList;
	PackOperateInfo objData;
	objData.Reset();

	objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
	objData.m_bOPEntry = false;
	objData.m_iPackType  = iPackType;
	objData.m_iIndex   = iEntryIndex;
	objData.m_lCount   = pkEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
	objOPList.push_back(objData);

	iResult = m_kPackAsset.ExecutePackOP(objOPList,true,NULL, nLogCode, Params);

	if (iResult != RE_SUCCESS)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	SyncBackPackToClient(PACK_TYPE_BASIC);
	return iResult;
}

void LC_ServerPlayer::EquipAllBuddyItem(bool login)
{
	for(mem::map<int32_t, BuddyInstance*>::iterator it= m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			pkBuddy->EquipAllItem(login);
		}
	}
}

bool LC_ServerPlayer::UpdateGuildMemberInfo(bool sendToClient, bool force)
{
	if(!IsMainController())
	{
		return false;
	}

	m_updateToGroupFlag = true;
	uint32_t now = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if (force || now  >= m_UpdateUserInfoTimeStamp)
	{
		//5分钟更新一次
		m_UpdateUserInfoTimeStamp = now+5*60;
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		LC_ServerGuildInfo* guild = pkGuildManager->GetPlayerGuild(GetInstance());
		if (guild != NULL)
		{
			guild->UpdateMemberInfo(this, sendToClient);
			return true;
		}
	}

	return false;
}

void LC_ServerPlayer::EquipSlotStar(int32_t buddyId, int32_t equipIndex)
{
	MG_RLT_EquipSlot_Star respMsg;
	respMsg.m_iBuddyId = buddyId;
	respMsg.m_iEquipSlotIndex = equipIndex;
	respMsg.m_Result = DoEquipSlotStar(buddyId, equipIndex, respMsg.m_iNewStarLevel);
	SendMsgToClient(MGPT_RLT_EQUIPSLOT_STAR, &respMsg);
}

ResultType LC_ServerPlayer::DoEquipSlotStar(int32_t buddyId, int32_t equipIndex, int32_t& newStarLevel)
{
	ResultType res = RE_FAIL;
	if(ET_HEAD!=equipIndex && ET_BODY!=equipIndex && ET_FOOT!=equipIndex && ET_NECKLACE!=equipIndex)
	{
		return res;
	}

	mem::map<int32_t, BuddyInstance*>::iterator it =  m_ppUsableBuddyInstance.find(buddyId);
	if(it != m_ppUsableBuddyInstance.end())
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			return pkBuddy->EquipSlotStar(equipIndex, newStarLevel);
		}
	}
	
	return res;
}

void LC_ServerPlayer::ReinforceEquipSlot(int reinforceType, int buddyID, int equipSlotIndex)
{
	MG_RLT_Reinforce_EquipSlot respMsg;
	respMsg.m_iReinforeType = reinforceType;
	respMsg.m_iBuddyId = buddyID;
	respMsg.m_iSlotID = equipSlotIndex;
	if(reinforceType == 0)
	{
		respMsg.m_Result = DoReinforceEquipSlot(buddyID, equipSlotIndex, respMsg.m_iNewLevel);	
	}
	else
	{
		respMsg.m_Result = DoOneKeyReinforceEquipSlot(buddyID, equipSlotIndex, respMsg.m_iNewLevel);	
	}
	if(RE_SUCCESS == respMsg.m_Result)
	{
		SyncBackPackToClient(PACK_TYPE_BASIC);
		SetDirtyFlag(PACK_TYPE_BASIC, false);
	}
	SendMsgToClient(MGPT_RLT_REINFORCE_EQUIPSLOT, &respMsg);
}

ResultType LC_ServerPlayer::DoReinforceEquipSlot(int buddyID, int equipSlotIndex, int32_t& newLevel)
{
	if(ET_HEAD!=equipSlotIndex && ET_BODY!=equipSlotIndex && ET_FOOT!=equipSlotIndex && ET_NECKLACE!=equipSlotIndex)
	{
		return RE_FAIL;
	}
	
	mem::map<int32_t, BuddyInstance*>::iterator it =  m_ppUsableBuddyInstance.find(buddyID);
	if(it != m_ppUsableBuddyInstance.end())
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			return pkBuddy->ReinforceEquipSlot(equipSlotIndex, newLevel);
		}
	}
	return RE_FAIL;
}

ResultType LC_ServerPlayer::DoOneKeyReinforceEquipSlot(int buddyID, int equipSlotIndex, int32_t& newLevel)
{
	if(ET_HEAD!=equipSlotIndex && ET_BODY!=equipSlotIndex && ET_FOOT!=equipSlotIndex && ET_NECKLACE!=equipSlotIndex)
	{
		return RE_FAIL;
	}

	mem::map<int32_t, BuddyInstance*>::iterator it =  m_ppUsableBuddyInstance.find(buddyID);
	if(it != m_ppUsableBuddyInstance.end())
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			return pkBuddy->OneKeyReinforceEquipSlot(equipSlotIndex, newLevel);
		}
	}
	return RE_FAIL;
}

ResultType LC_ServerPlayer::DecompositionEquip(const std::vector<int32_t>& EquipIndexs, std::vector<PS_ItemInfo>& DecompositionResult, bool HandlePackOP, bool eMail)
{
	if(EquipIndexs.size() < 1)
	{
		return RE_ERROR_PARAM;
	}

	//查重
	mem::set<int32_t> indexSet;
	for(int i=0; i<EquipIndexs.size(); ++i)
	{
		if(indexSet.insert(EquipIndexs[i]).second == false)
		{
			return RE_ERROR_PARAM;
		}
	}

	Utility::LC_ComMap	consumeCashMap;
	ResultType res = CheckDecompositionEquip(EquipIndexs, consumeCashMap);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	res = ExecuteDecompositionEquip(EquipIndexs, consumeCashMap, DecompositionResult, HandlePackOP, eMail);
	return res;
}

ResultType LC_ServerPlayer::CheckDecompositionEquip(const std::vector<int32_t>& EquipIndexs, Utility::LC_ComMap& consumeCashMap)
{
	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_PackAsset* pkPackAsset = pkOwnerPlayer->GetPackAsset();
	if(NULL==pkItemFactory || NULL==pkPackAsset)
	{
		return res;
	}

	for(int i=0; i<EquipIndexs.size(); ++i)
	{
		LC_BackPackEntry* pkEquipEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, EquipIndexs[i]);
		if(NULL == pkEquipEntry || !pkEquipEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			return res;
		}

		LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID));
		if(NULL == pkItem)
		{
			return res;
		}

		int decompositionFuncID = pkItem->GetDecompositionFuncID();
		CF_ItemDecompositionFunc::DataEntry* pkFuncData = SafeGetCSVFileDataEntryPtr<CF_ItemDecompositionFunc>(decompositionFuncID);
		if(NULL == pkFuncData)
		{
			return RE_ITEM_DECOMPOSITION_FAIL_CANT;
		}

		//把分解所有道具要消耗的货币全部汇总起来，一起check
		Utility::LC_ComMap::iterator findIter = consumeCashMap.begin();
		if (!pkFuncData->_kConsumeCashMap.empty())
		{
			for(Utility::LC_ComMap::iterator it=pkFuncData->_kConsumeCashMap.begin(); it!=pkFuncData->_kConsumeCashMap.end(); ++it)
			{
				findIter = consumeCashMap.find(it->first);
				if(findIter == consumeCashMap.end())
				{
					consumeCashMap.insert(std::make_pair(it->first, it->second));
				}
				else
				{
					findIter->second += it->second;
				}
			}
		}

		res = pkItem->CheckDecompositionItem(pkOwnerPlayer,pkEquipEntry);
		if(res != RE_SUCCESS)
		{
			return res;
		}
	}

	if(!consumeCashMap.empty())
	{
		return LC_Helper::CheckCashMap(pkOwnerPlayer, consumeCashMap);
	}

	return RE_SUCCESS;
}

ResultType LC_ServerPlayer::ExecuteDecompositionEquip(const std::vector<int32_t>& EquipIndexs, const Utility::LC_ComMap& consumeCashMap, std::vector<PS_ItemInfo>& DecompositionResult, bool HandlePackOP, bool eMail)
{
	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}

	LC_PackAsset* pkPackAsset = pkOwnerPlayer->GetPackAsset();
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL==pkPackAsset || NULL==pkPackEntryFactory || NULL==pkItemFactory)
	{
		return res;
	}

	PackOperateInfoList objOPList;
	LC_BackPackEntry* pkEquipEntry = NULL;

	vector<int> nParams;
	nParams.clear();

	for(int i=0; i<EquipIndexs.size(); ++i)
	{
		int equipIndex = EquipIndexs[i];
		//找到装备数据
		pkEquipEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, equipIndex);
		if(!pkEquipEntry || !pkEquipEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			return res;
		}

		//找到物品
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID));
		if(NULL == pkItem)
		{
			return res;
		}

		CF_ItemDecompositionFunc::DataEntry* pkFuncData = SafeGetCSVFileDataEntryPtr<CF_ItemDecompositionFunc>(pkItem->GetDecompositionFuncID());
		if(NULL == pkFuncData)
		{
			res = RE_ITEM_DECOMPOSITION_FAIL_CANT;
			return res;
		}

		//分解物品
		res = pkItem->DecompositionItem(pkOwnerPlayer, pkEquipEntry, objOPList, GET_PROCESS_TIME);
		if (res != RE_ITEM_DECOMPOSITION_SUCCESS)
		{
			return res;
		}
	}

	Utility::UT_SIMDataList addList;
	for(int i=0; i<objOPList.size(); ++i)
	{
		const PackOperateInfo& ItemData = objOPList[i];
		PS_ItemInfo data;
		data.m_nItemID = ItemData.m_kPackEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
		data.m_nItemNum = ItemData.m_kPackEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
		data.m_nBindMode = ItemData.m_kPackEntry.GetBackPackValueByType(LPSFET_BINDMODE);
		DecompositionResult.push_back(data);

		UT_SIMDataInfo item;
		item.SetItemInfo(data.m_nItemID, data.m_nItemNum, data.m_nBindMode);
		addList.push_back(item);
	}	

	PackOperateInfoList removeList;
	for(int i=0; i<EquipIndexs.size(); ++i)
	{
		PackOperateInfo objData;
		objData.Reset();

		objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
		objData.m_bOPEntry = false;
		objData.m_iPackType = PACK_TYPE_BASIC;
		objData.m_iIndex = EquipIndexs[i];
		objData.m_lCount = 1;
		objOPList.push_back(objData);

		removeList.push_back(objData);
	}

	//埋点日志
	
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ITEMFORGE_OPERATE, OPERATE_LOGTYPE9); 
	res = LC_Helper::CheckItemOPs(pkPackAsset, objOPList);
	if (RE_SUCCESS == res)
	{
		res = LC_Helper::DoItemOPs(pkOwnerPlayer, objOPList, SYS_FUNC_TP_ITEM_DECOMPOSITE, pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID), true, HandlePackOP,nLogCode, nParams);	
		if (RE_SUCCESS == res && !consumeCashMap.empty())
		{
			res = LC_Helper::PayCashMap(pkOwnerPlayer, consumeCashMap, SYS_FUNC_TP_ITEM_DECOMPOSITE, pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID),1,false,nLogCode,nParams);
		}
	}
	else if(RE_ITEM_PACKAGE_FULL==res && eMail)
	{
		LC_Helper::DoItemOPs(pkOwnerPlayer, removeList, SYS_FUNC_TP_ITEM_DECOMPOSITE, pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID), true, HandlePackOP,nLogCode, nParams);
		res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, addList, pkOwnerPlayer, SYS_FUNC_TP_ITEM_DECOMPOSITE, pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID), "79;", "80;", true, false,false, nLogCode, nParams);
	}

	if(RE_SUCCESS != res)
	{
		DecompositionResult.clear();
	}

	return res;
}

void LC_ServerPlayer::GetRuneConsume(const LC_BackPackEntry* pkRuneData, Utility::UT_SIMDataList& consumeItem, Utility::LC_ComMap& moneyAfter)
{
	if(NULL == pkRuneData)
	{
		return;
	}
	int32_t runeID = pkRuneData->GetBackPackValueByType(LPSFET_TYPE_ID);
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(runeID);
	if (NULL == pkItemData)
	{
		return;
	}

	int initLevel = pkItemData->_lRuneLevel;
	int currentLevel = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL);
	CF_RuneLevelUp::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(initLevel);
	if(NULL == pkData)
	{
		return;
	}

	Utility::LC_ComMap money;
	Utility::LC_ComMap::iterator iter = money.begin();
	//从当前等级重置回initLevel，需要返还的货币
	for(int i=initLevel+1; i<=currentLevel; ++i)
	{
		pkData = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(i);
		if(NULL == pkData)
		{
			return;
		}

		for(Utility::LC_ComMap::iterator it=pkData->_ConsumeMoney.begin(); it!=pkData->_ConsumeMoney.end(); ++it)
		{
			iter = money.find(it->first);
			if(iter != money.end())
			{
				iter->second += it->second;
			}
			else
			{
				money.insert(std::make_pair(it->first, it->second));
			}

			iter = moneyAfter.find(it->first);
			if(iter != moneyAfter.end())
			{
				iter->second += it->second;
			}
			else
			{
				moneyAfter.insert(std::make_pair(it->first, it->second));
			}
		}
	}
}

ResultType LC_ServerPlayer::RestituteRuneConsume(LC_ServerPlayer* pkPlayer, LC_BackPackEntry* pkRuneData, Utility::UT_SIMDataList& consumeItem, const Utility::LC_ComMap& moneyAfter)
{
	if(NULL==pkPlayer || NULL==pkRuneData)
	{
		return RE_FAIL;
	}

	int runeID = pkRuneData->GetBackPackValueByType(LPSFET_TYPE_ID);
	Utility::LC_ComMap::const_iterator iter=moneyAfter.begin();
	//检测加上返还的货币，会不会超过上限
	for( ; iter!=moneyAfter.end(); ++iter)
	{
		CashType type = (CashType)(iter->first);
		MoneyType moneyTotalNum = iter->second + GetCashNumByCashType((CashType)(iter->first));
		if(!LC_Helper::CheckValidTradeCash(type, moneyTotalNum))
		{
			return RE_RUNE_RESET_FAIL_MONEY_BE_LIMIT;
		}
	}
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(runeID);
	nParams.push_back(pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL));
	nParams.push_back(pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_STAR));
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BAOJU_OPERATE, OPERATE_LOGTYPE4);
	if(!consumeItem.empty())
	{
		//背包能否装得下返还的所有道具
		ResultType res = LC_Helper::CanAddItemEx(pkPlayer, consumeItem, CIET_NONE);
		if(RE_SUCCESS != res)
		{
			return res;
		}

		//返回道具
		for(Utility::UT_SIMDataList::iterator it=consumeItem.begin(); it!=consumeItem.end(); ++it)
		{
			LC_Helper::AddItem(pkPlayer->GetPackAsset(), (int)(it->IID()),(int)(it->ParamA()), CIET_NONE, false, -1, nLogCode, nParams);
		}
	}
	
	//返还货币
	for(iter=moneyAfter.begin(); iter!=moneyAfter.end(); ++iter)
	{
		CashType type = (CashType)(iter->first);
		MoneyType moneyNum = iter->second;
		pkPlayer->AddCash(moneyNum, type, SYS_FUNC_TP_RESET_RUNE, runeID,0,true,NULL,nLogCode, nParams);
	}

	SyncBackPackToClient(PACK_TYPE_BASIC);
	return RE_SUCCESS;
}

//--------------------------------------------------------
ResultType LC_ServerPlayer::ExternWarehouse(int32_t rowNum)
{	
	ResultType iResult = CanUseWarehouse();
	if (RE_SUCCESS != iResult)
	{
		return iResult;
	}

	iResult = GetPackAsset()->CanExternWarehouse(rowNum);
	if (RE_SUCCESS != iResult)
	{
		return iResult;
	}

	int ExternTime = GetPackAsset()->ExternWarehouseTime();
	int llCash = 0;
	for(int i=0;i<rowNum;++i)
	{
		CF_WarehouseCost::DataEntry * entry = SafeGetCSVFileDataEntryPtr<CF_WarehouseCost>(ExternTime+i);
		if( NULL == entry)
		{
			return RE_SYSTEM_ERROR_BASE;
		}

		 llCash += entry->_lCost;
	}
	
	if(!IS_VALID_CHAR_MONEY(llCash))
	{
		return RE_FAIL;
	}

	if (!IsCashEnough(llCash,CT_UNBIND_YUANBAO))
	{
		return RE_WAREHOUSE_EXPAND_FAIL_CASH_NOT_ENOUGH;
	}

	iResult = GetPackAsset()->ExternWarehouse(rowNum);
	if (RE_SUCCESS != iResult)
	{
		return iResult;
	}
	//仓库扩充--埋点日志
	LC_WarehousePack& pack = GetPackAsset()->GetWarehousePack();
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(pack.m_iSize);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, USERPACK_OPERATE,OPERATE_LOGTYPE3);
	ReduceCash(llCash, CT_UNBIND_YUANBAO, SYS_FUNC_TP_WAREHOUSE, 0, nLogCode, nParams);
	return RE_WAREHOUSE_EXPAND_SUCCESS;
}

void LC_ServerPlayer::BuyVIPTimes(int32_t storyId, int32_t buyType, uint32_t times)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	if(!IsMainController())
	{
		return pkOwnerPlayer->BuyVIPTimes(storyId, buyType, times);
	}

	MG_RLT_GetBuyTimes respMsg;
	respMsg.m_type = MSGTYPE_BUY_TIMES;
	respMsg.m_iGameStoryId = storyId;
	respMsg.m_iBuyType = buyType;
	respMsg.m_iResult = RE_FAIL;

	do 
	{
		respMsg.m_iResult = UT_ServerHelper::CheckBuyTypeVaild(buyType);
		if(RE_SUCCESS != respMsg.m_iResult)
		{
			break;
		}

		GameStory::LC_GameStory_Manager* pkStoryManager = SERVER_GET_GAMESTORY_MANAGER;
		if (NULL == pkStoryManager)
		{
			break;
		}

		respMsg.m_iResult = pkStoryManager->ReqBuyTimes(this, storyId, buyType, times);
		if(RE_SHOPCITY_BUY_SUCESS != respMsg.m_iResult)
		{
			break;
		}

		GetBuyVipInfo(storyId, buyType, respMsg.m_buyTimes, respMsg.m_totalTimes, respMsg.m_costCount, respMsg.m_costType);	
	} while (0);
	SendMsgToClient(MGPT_RLT_GET_BUY_TIMES, &respMsg);
}

void LC_ServerPlayer::GetBuyVipInfo(int32_t storyId, uint32_t buyType, uint32_t& buyTimes, uint32_t& totalTimes, uint32_t& costCount, uint32_t& costType)
{
	GameStory::GameStoryConfig* pkStoryConfig = SERVER_GET_GAMESTORY_MANAGER->GetStoryConfig(storyId);
	if(NULL == pkStoryConfig)
	{
		return;
	}

	GameLogic::GameStoryEntry* pkStoryEntry = GetGameStoryAsset().GetGameStoryEntry(storyId);
	if (NULL == pkStoryEntry)
	{
		return;
	}

	LC_ShopIDMap kShopIDMap;
	switch(buyType)
	{
	case Buy_Raid_Type_VIPProfit:
		{
			buyTimes = pkStoryConfig->GetBuyVIPProfitTimesShopIDMap(pkStoryEntry, kShopIDMap);
			totalTimes = GetVIPAsset().GetProfitValue(pkStoryConfig->m_iBuyTimesVIPID);
		}
		break;
	case Buy_Raid_Type_VIPPrivilegeCard:
		{
			buyTimes = pkStoryConfig->GetBuyVIPPrivilegeCardTimesShopIDMap(pkStoryEntry, kShopIDMap);
			totalTimes = LC_Helper::GetVipPrivilegeTimesByType(pkStoryConfig->m_iBuyTimesVIPPrivilegeID, this);
		}
		break;
	default:
		return;
	}

	if(buyTimes >= totalTimes)
	{
		//已经购买次数达到最大购买次数的时候，不能再购买，所以cost不能是下次购买花费的货币数，因此用0特殊标记
		costCount = 0;
		return;
	}
	
	if(kShopIDMap.size() != 1)
	{
		//配表错误
		return;
	}

	LC_ShopIDMap::const_iterator it = kShopIDMap.begin();
	if(it == kShopIDMap.end())
	{
		return;
	}

	CF_ShopList::DataEntry* pkShopData = SafeGetCSVFileDataEntryPtr<CF_ShopList>(it->first);
	if(NULL == pkShopData)
	{
		return;
	}

	Utility::LC_ComMap::const_iterator kIt = pkShopData->_ActualPriceList.begin();
	if(kIt == pkShopData->_ActualPriceList.end())
	{
		return;
	}

	costType = kIt->first;
	costCount = kIt->second;
}
//-------------------------------------------------------------
ResultType LC_ServerPlayer::_operationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys, uint64_t* pnScore)
{
	ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iFuncID);
	if (!pkSuitData)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	ItemEffectManage * ie_inst = ItemEffectManage::GetSingletonPtr();
	ItemSuitInfoType::ItemSuitEffectTypes effect;
	uint64_t nScore = 0;
	if(pkSuitData->GetEfectTypes(iSuitNum, effect, pkHappenedKeys))
	{
		SetSuitEffectScope(true);
		for(ItemSuitInfoType::ItemSuitEffectTypes::iterator i = effect.begin(); i != effect.end(); ++i)
		{
			nScore += LC_ServerItem::_operationEquipAttrib(this, *i, 0, ie_inst, NULL);
		}
		SetSuitEffectScope(false);
	}
	if (pnScore)
		*pnScore = nScore;
	return RE_SUCCESS;
}
//-------------------------------------------------------------
ResultType LC_ServerPlayer::_unoperationSuitAttribute(int iFuncID,int iSuitNum, mem::set<int32_t>* pkHappenedKeys, uint64_t* pnScore)
{
	ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iFuncID);
	if (!pkSuitData)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	ItemEffectManage * ie_inst = ItemEffectManage::GetSingletonPtr();

	uint64_t nScore = 0;
	ItemSuitInfoType::ItemSuitEffectTypes effect;
	if(pkSuitData->GetEfectTypes(iSuitNum, effect, pkHappenedKeys))
	{
		SetSuitEffectScope(true);
		for(ItemSuitInfoType::ItemSuitEffectTypes::iterator i = effect.begin(); i != effect.end(); ++i)
		{
			nScore += LC_ServerItem::_unoperationEquipAttrib(this, *i,0,ie_inst, NULL);
		}
		SetSuitEffectScope(false);
	}
	if (pnScore)
		*pnScore = nScore;
	return RE_SUCCESS;
}

bool LC_ServerPlayer::CheckItemReqInPackage(LC_PlayerBase* pkPlayer, int32_t nBuddyId, int iPackType, int iEntryID, int lItemID)
{
	LC_BackPackEntry* pkEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(iPackType, iEntryID);
	if (!pkEntry || !pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return false;
	}
	else if (pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID) != lItemID)
	{
		return false;
	}
	return true;
}

void LC_ServerPlayer::EquipAdvance( int _type, int _index )
{
	LC_PackAsset* pkPackAsset = GetPackAsset();
	if( !pkPackAsset)
	{
		return;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if( !pkItemFactory )
	{
		return;
	}

	CItemConfigManager* pItemMgr = CItemConfigManager::GetSingletonPtr();
	if( !pItemMgr )
	{
		return;
	}

	MG_SC_EquipAdvance_Ret sendMsg;
	sendMsg.result = RE_SUCCESS;

	GameLogic::LC_BackPackEntry curEntry;	//当前操作的装备
	LC_BackPackEntry* packEntry = NULL;	//背包中的装备

	if( _type == 1 )
	{
		//背包
		packEntry = pkPackAsset->GetBackPackEntry( PACK_TYPE_BASIC, _index );
		if( !packEntry || !packEntry->GetBackPackValueByType(LPSFET_VALID) )
		{
			sendMsg.result = RE_UNVAILD_ITEM;
			SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
			return;
		}
		curEntry.Clone( packEntry );
	}
	else if( _type == 2 )
	{
		//装备栏
		const LC_BackPackEntry& equipEntry = GetCommandEquipPack().GetEquipData( _index );
		if( !equipEntry.GetBackPackValueByType(LPSFET_VALID) )
		{
			sendMsg.result = RE_ITEM_INVALID;
			SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
			return;
		}
		curEntry.Clone( equipEntry );
	}
	else
	{
		return;
	}

	int itemId = curEntry.GetBackPackValueByType(LPSFET_TYPE_ID);

	//检验数据是否合法
	LC_ItemBase* pkItem = pkItemFactory->RequestItem( itemId );
	if( !pkItem )
	{
		sendMsg.result = RE_Item_Error;
		SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
		return;
	}

	//判断能否进阶
	CF_CompositionFormula::DataEntry* pkData= pItemMgr->GetAdvanceData( itemId );
	if( !pkData )
	{
		sendMsg.result = RE_Equip_NoAdvance;
		SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
		return;
	}

	//判断类型是否合法
	if( pkData->_FormulaType != 2 )
	{
		sendMsg.result = RE_Equip_Advance_TypeError;
		SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
		return;
	}

	//判断进阶的id是否合法
	pkItem = pkItemFactory->RequestItem( pkData->_lCompositionItemID );
	if( !pkItem )
	{
		sendMsg.result = RE_Item_Error;
		SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
		return;
	}

	//判断进阶后的装备是否大于玩家等级
	int levelLimit = pkItem->GetEquipLevelLimit(); //等级限制
	int playerLevel = GetLevel();
	if( levelLimit > playerLevel )
	{
		sendMsg.result = RE_SUBTYPE_ERROR_7 ;
		SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
		return;
	}

	//判断消耗是否足够
	for(Utility::UT_SIMDataList::iterator ItemIt= pkData->_kNeedMaterials.begin(); ItemIt!=pkData->_kNeedMaterials.end(); ++ItemIt)
	{
		ResultType res = TryDeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum());
		if( res != RE_SUCCESS )
		{
			sendMsg.result = res;
			SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
			return;
		}
	}
	//锻造进阶--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(itemId);
	nParams.push_back( pkData->_lCompositionItemID);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ITEMFORGE_OPERATE, OPERATE_LOGTYPE8);
	//删除材料
	for(Utility::UT_SIMDataList::iterator ItemIt=pkData->_kNeedMaterials.begin(); ItemIt!=pkData->_kNeedMaterials.end(); ++ItemIt)
	{
		DeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum(),nLogCode, nParams);
	}

	//设置数据
	curEntry.SetBackPackValueByType(LPSFET_TYPE_ID, pkData->_lCompositionItemID );
	curEntry.SetToProtocol( sendMsg.m_Data );
	sendMsg.m_Type = _type;
	sendMsg.m_Index = _index;
	if( _type == 1 )
	{
		packEntry->Clone( curEntry );
		//更新背包
		SyncBackPackToClient( PACK_TYPE_BASIC );
	}
	else
	{
		GetCommandEquipPack().SetEquipData( _index, curEntry );
		//更新属性
		GetCommanderAttrMgr().RefreshCommanderEquipAttributeMap( this, false, SPART_Commander_4 );
	}
	
	SendMsgToClient( MGPT_SC_EquipAdvance_Ret, &sendMsg );
}