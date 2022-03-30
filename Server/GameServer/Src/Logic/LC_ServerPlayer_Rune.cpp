#include "LC_ServerPlayer.h"
#include "../Buddy/BuddyInstance.h"
#include "SystemError.h"
#include "GlobalSettings.h"
#include "UT_Log.h"

#include "MG_ItemProtocol.h"

#include "CF_RuneLevelUp.h"
#include "CF_RuneStar.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;

void LC_ServerPlayer::RuneUpgrade_New(int32_t buddyId, int32_t slotIndex, int32_t targetLevel)
{
	if(0 == buddyId)
	{
		//�������ǻ����������װ��
		BasicPackRuneUpgrade(slotIndex, targetLevel);
	}
	else
	{
		BuddyRuneUpgrade(buddyId, slotIndex, targetLevel);
	}
}

void LC_ServerPlayer::RuneStar_New(int32_t buddyId, int32_t slotIndex)
{
	if(0 == buddyId)
	{
		//�����������װ������
		BasicPackRuneStar(slotIndex);
	}
	else
	{
		BuddyRuneStar(buddyId, slotIndex);
	}
}

void LC_ServerPlayer::ResetRune_New(int32_t buddyId, int32_t slotIndex)
{
	if(0 == buddyId)
	{
		BasicPackRuneReset(slotIndex);
	}
	else
	{
		BuddyRuneReset(buddyId, slotIndex);
	}
}

void LC_ServerPlayer::EquipAllRuneItem(bool login)
{
	for(mem::map<int32_t, BuddyInstance*>::iterator it = m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			pkBuddy->EquipAllRuneItem(login);
		}
	}
}

void LC_ServerPlayer::EquipRuneToBuddy(int32_t srcBuddyID, int32_t srcSlotIndex, int32_t destBuddyID, int32_t destSlotIndex)
{
	MG_RLT_EquipRune rltMsg;
	rltMsg.m_iSrcBuddyID = srcBuddyID;
	rltMsg.m_iSrcIndex = srcSlotIndex; 
	rltMsg.m_iDestBuddyID = destBuddyID;
	rltMsg.m_iDestIndex = destSlotIndex;
	rltMsg.m_iResult = DoEquipRuneToBuddy(srcBuddyID, srcSlotIndex, destBuddyID, destSlotIndex, rltMsg.m_SrcSlotData, rltMsg.m_DestSlotData);
	SendMsgToClient(MGPT_RLT_EQUIP_RUNE, &rltMsg);
}

void LC_ServerPlayer::UnEquipRuneFromBuddy(int32_t srcBuddyID, int32_t srcSlotIndex)
{
	MG_RLT_UnEquipRune rltMsg;
	rltMsg.m_iBuddyID = srcBuddyID;
	rltMsg.m_iSrcSlotIndex = srcSlotIndex;
	rltMsg.m_iResult = DoUnEquipRuneFromBuddy(srcBuddyID, srcSlotIndex);
	SendMsgToClient(MGPT_RLT_UNEQUIP_RUNE, &rltMsg);
}

void LC_ServerPlayer::OneKeyEquipRuneToBuddy(int32_t buddyID)
{
	MG_RLT_OneKeyEquipRune rltMsg;
	rltMsg.m_iBuddyID = buddyID;
	rltMsg.m_iResult = DoOneKeyEquipRuneToBuddy(buddyID, rltMsg.m_DestSlotData);
	SendMsgToClient(MGPT_RLT_ONEKEY_EQUIP_RUNE, &rltMsg);
}

void LC_ServerPlayer::OneKeyUnEquipRuneFromBuddy(int32_t buddyID)
{
	MG_RLT_OneKeyUnEquipRune respMsg;
	respMsg.m_iBuddyID = buddyID;
	respMsg.m_iResult = DoOneKeyUnEquipRuneFromBuddy(buddyID, respMsg.m_SlotIndexVec);
	SendMsgToClient(MGPT_RLT_ONEKEY_UNEQUIP_RUNE, &respMsg);
}

void LC_ServerPlayer::RuneReward(int32_t lItemID)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if (pkOwnerPlayer == NULL)
	{
		return;
	}

	if(!IsMainController())
	{
		return pkOwnerPlayer->RuneReward(lItemID);
	}

	m_RuneData.RuneReward(this, lItemID);
}

bool	LC_ServerPlayer::GetBuddyRuneEquipState()
{
	if(!IsMainController())
	{
		return false;
	}

	return m_RuneData.GetBuddyRuneEquipState();
}

bool	LC_ServerPlayer::SetBuddyRuneSlotState()
{
	if(!IsMainController())
	{
		return false;
	}

	bool slotState = GetBuddyRuneEquipState();
	if(slotState != true)
	{
		m_RuneData.SetBuddyRuneSlotState();
	}

	return true;
}

bool LC_ServerPlayer::GetRuneSlotDressState(int32_t buddyID, int32_t slotIndex)
{
	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyID);
	if(NULL != pkBuddy)
	{
		return pkBuddy->GetRuneSlotDressState(slotIndex);
	}

	return false;
}

void LC_ServerPlayer::SetRuneSlotDressState(int32_t buddyID, int32_t slotIndex)
{
	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyID);
	if(NULL != pkBuddy)
	{
		pkBuddy->SetRuneSlotDressState(slotIndex);
	}
}

void LC_ServerPlayer::SyncAllBuddyRuneEquipToClient()
{
	for(mem::map<int32_t, BuddyInstance*>::iterator it=m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			pkBuddy->SyncRunePackToClient();
		}
	}
}

void LC_ServerPlayer::BasicPackRuneUpgrade(int32_t slotIndex, int32_t targetLevel)
{
	if(!IsMainController())
	{
		return;
	}

	MG_RLT_RuneUpgrade rltMsg;
	rltMsg.m_iBuddyID = 0;
	rltMsg.m_iSlotIndex = slotIndex;
	rltMsg.m_iResult = DoBasicPackRuneUpgrade(slotIndex, targetLevel, rltMsg.m_SlotData);
	SendMsgToClient(MGPT_RLT_RUNE_UPGRADE, &rltMsg);
}

ResultType LC_ServerPlayer::DoBasicPackRuneUpgrade(int32_t slotIndex, int32_t targetLevel, PS_ItemAssetEntry& slotData)
{
	//�������
	LC_PackAsset* pkControllerPackAsset = GetPackAsset();
	if (NULL == pkControllerPackAsset)
	{
		return RE_FAIL;
	}

	LC_BackPackEntry* pkRuneData = pkControllerPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, slotIndex);
	if (NULL==pkRuneData || !pkRuneData->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_ERROR_PARAM;
	}

	int32_t runeId = pkRuneData->GetBackPackValueByType(LPSFET_TYPE_ID);
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(runeId);
	if (NULL == pkItemData)
	{
		return RE_ERROR_PARAM;
	}

	if(!LC_Helper::CheckIsRune(runeId))
	{
		return RE_ERROR_PARAM;
	}

	int runeOldLevel = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL);//���ߵ�ǰ�ȼ�
	int runeStarLevel = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_STAR);//���ߵ�ǰ�Ǽ�
	int runeMaxStarLevel = pkItemData->_MaxStar;//���߿ɴﵽ����Ǽ�
	int starType = pkItemData->_StarUpType;//������������

	int runeMaxLevel = -1;//���ߵ�ǰ�ɴﵽ���ȼ�  ���
	Utility::UT_SIMDataList consumeItems;
	int consumeItemID1;//�ɱ������itemid
	int consumeItemCount = 0;//�ɱ������item������
	int consumeItemID2;//�����itemID	
	if(false == UT_ServerHelper::GetRuneCSVInfo(starType, runeStarLevel, runeMaxLevel, consumeItems, consumeItemID1, consumeItemCount, consumeItemID2))
	{
		return RE_FAIL;
	}

	//�жϱ��ߵĵȼ��Ƿ񳬹��˿����������ȼ�
	if(runeOldLevel >= runeMaxLevel)
	{
		if(runeStarLevel < runeMaxStarLevel)
		{
			return RE_RUNE_UPGRADE_STAR_NOT_ENOUGH;
		}
		else if(runeStarLevel == runeMaxStarLevel)
		{
			return RE_RUNE_UPGRADE_LEVEL_BE_LIMIT;
		}
	}

	//����Ҫ������Ŀ��ȼ��Ƿ���ȷ
	if(targetLevel<=runeOldLevel || targetLevel>runeMaxLevel)
	{
		return RE_FAIL;
	}

	int runeSkill = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);//���ߵ�ǰ����
	int newRuneSkill = runeSkill;
	Utility::LC_CashMap ConsumeCashMap;
	for(int i=runeOldLevel+1; i<=targetLevel; ++i)
	{
		CF_RuneLevelUp::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(i);
		if(NULL == pkData)
		{
			return RE_FAIL;
		}

		int controllerLevel = GetControllerLevel();
		if(controllerLevel < pkData->_iCommanderLevel)
		{
			return RE_CONTROLLER_LEVEL_NOT_ENOUGH_RUNE;
		}

		//�����Ҫ���ĵĻ���
		Utility::LC_ComMap& consume = pkData->_ConsumeMoney;
		for(Utility::LC_ComMap::iterator it=consume.begin(); it!=consume.end(); ++it)
		{
			Utility::LC_ComMap::iterator findIt = ConsumeCashMap.find(it->first);
			if(findIt == ConsumeCashMap.end())
			{
				ConsumeCashMap.insert(std::make_pair(it->first, it->second));
			}
			else
			{
				findIt->second += it->second;
			}
		}

		if(pkData->_bSkillLevelUp && newRuneSkill!=0)
		{
			newRuneSkill += 1;
		}
	}

	//�����Ҫ���ĵĻ����Ƿ��㹻
	ResultType res = LC_Helper::CheckRuneUpgradeItemIsEnough(this, ConsumeCashMap);
	if(RE_SUCCESS != res)
	{
		return res;
	}
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(runeId);
	nParams.push_back(runeOldLevel);
	nParams.push_back(targetLevel);

	//�۳�����
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BAOJU_OPERATE,OPERATE_LOGTYPE2);
	for(Utility::LC_ComMap::iterator it=ConsumeCashMap.begin(); it!=ConsumeCashMap.end(); ++it)
	{
		CashType cashType = (CashType)(it->first);
		int64_t cashCount = it->second;
		if(!ReduceCash(cashCount, cashType, SYS_FUNC_TP_UPGRADE_RUNE, runeId, nLogCode, nParams))
		{
			return RE_FAIL;
		}
	}

	//���±��ߵȼ��ͱ��߼���
	pkRuneData->SetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL, targetLevel);
	pkRuneData->SetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL, newRuneSkill);

	for(int level=runeOldLevel+1; level<=targetLevel; ++level)
	{
		//�ɾ�ϵͳ���
		AchievementEvent kEvent;
		kEvent.SetConditionInfo(level);
		kEvent.SetNum(1);
		UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_RUNE_LEVEL, kEvent);
	}	

	//���±����ܵȼ�
	RecomputerRuneTotalLevel();

	UpdateRankInfoToRedis(Sub_Reward_Type_RuneItemCombatScore,  false, runeId);
	UpdateRankInfoToRedis(Sub_Reward_Type_AllRuneItemCombatScore);

	pkRuneData->SetToProtocol(slotData);

	SyncBackPackToClient(PACK_TYPE_BASIC);

	return RE_SUCCESS;
}

void LC_ServerPlayer::BuddyRuneUpgrade(int32_t buddyId, int32_t slotIndex, int32_t targetLevel)
{
	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyId);
	if(NULL != pkBuddy)
	{
		pkBuddy->RuneUpgrade(slotIndex, targetLevel);
	}
}

void LC_ServerPlayer::BasicPackRuneStar(int32_t slotIndex)
{
	if(!IsMainController())
	{
		return;
	}

	MG_RLT_RuneStar rltMsg;
	rltMsg.m_iBuddyID = 0;
	rltMsg.m_iSlotIndex = slotIndex;
	rltMsg.m_iResult = DoBasicPackRuneStar(slotIndex, rltMsg.m_SlotData);
	SendMsgToClient(MGPT_RLT_RUNE_STAR, &rltMsg);
}

ResultType LC_ServerPlayer::DoBasicPackRuneStar(int32_t slotIndex, PS_ItemAssetEntry& slotData)
{
	LC_PackAsset* pkOwnerPackAsset= GetPackAsset();
	if(NULL == pkOwnerPackAsset)
	{
		return RE_FAIL;
	}

	LC_BackPackEntry* pkRuneData = pkOwnerPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, slotIndex);
	if (NULL == pkRuneData|| !pkRuneData->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_ERROR_PARAM;
	}

	int32_t runeId = pkRuneData->GetBackPackValueByType(LPSFET_TYPE_ID);
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(runeId);
	if (NULL == pkItemData)
	{
		return RE_ERROR_PARAM;
	}

	if(!LC_Helper::CheckIsRune(runeId))
	{
		return RE_ERROR_PARAM;
	}

	//�Ƿ��Ѿ��ﵽ���ȼ�
	int32_t runeLevel = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL);
	int32_t runeOldStar = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_STAR);
	if(runeOldStar >= pkItemData->_MaxStar)
	{
		return RE_RUNE_STATLEVEL_BE_LIMIT;
	}

	int32_t starType = pkItemData->_StarUpType;
	int runeNewStar = runeOldStar+1;

	int newRuneMaxLevel = 0;
	Utility::UT_SIMDataList newConsumeItems;
	int consumeItemID1 = 0;//�ɱ������itemid
	int consumeItemCount = 0;//�ɱ������item������
	int consumeItemID2 = 0;//�����itemID	
	if(false == UT_ServerHelper::GetRuneCSVInfo(starType, runeNewStar, newRuneMaxLevel, newConsumeItems, consumeItemID1, consumeItemCount, consumeItemID2))
	{
		return RE_FAIL;
	}

	if(0!=consumeItemID1 && 0!=consumeItemID2 && 0!=consumeItemCount)
	{
		int num = GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, consumeItemID1);
		if(num < consumeItemCount)
		{
			UT_SIMDataInfo data1;
			data1.SetItemInfo(consumeItemID1, num);
			newConsumeItems.push_back(data1);

			UT_SIMDataInfo data2;
			data2.SetItemInfo(consumeItemID2, consumeItemCount-num);
			newConsumeItems.push_back(data2);
		}
		else
		{
			UT_SIMDataInfo data;
			data.SetItemInfo(consumeItemID1, consumeItemCount);
			newConsumeItems.push_back(data);
		}
	}

	//�������Ƿ���
	ResultType res = LC_Helper::IsItemsEnough(GetPackAsset(), newConsumeItems);
	if(RE_SUCCESS != res)
	{
		return RE_CONSUME_ITEM_NOT_ENOUGH;
	}

	//ɾ������
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(runeId);
	nParams.push_back(runeOldStar);
	nParams.push_back(runeNewStar);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BAOJU_OPERATE, OPERATE_LOGTYPE3);
	for(Utility::UT_SIMDataList::const_iterator it=newConsumeItems.begin(); it!=newConsumeItems.end(); ++it)
	{
		DeleteItem(it->IID(), it->ParamA(), nLogCode, nParams);
	}

	//���µ�ǰ�Ǽ�
	pkRuneData->SetBackPackValueByType(LCSPET_EQUIP_STAR, runeNewStar);

	//֪ͨ�ɾ�ϵͳ
	AchievementEvent kEvent;
	kEvent.SetConditionInfo(pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_STAR));
	kEvent.SetNum(1);
	UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_RUNE_STARLEVEL, kEvent);

	//�Ǽ�>6��������
	if(runeNewStar > 6)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetChannelType(PROMPT_CHANNEL_SYSTEM_TIP);
		noticeFmort.SetMsgID(39);
		noticeFmort.AddPlayerInfo(GetInstanceUniqueID(), GetOwnerCharName());
		noticeFmort.AddItemName(pkRuneData->GetBackPackValueByType(LPSFET_TYPE_ID));
		noticeFmort.AddNumber(runeNewStar-6);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
	}

	//���±������Ǽ�
	RecomputerRuneTotalStarLevel();

	UpdateRankInfoToRedis(Sub_Reward_Type_RuneItemCombatScore,  false, runeId);
	UpdateRankInfoToRedis(Sub_Reward_Type_AllRuneItemCombatScore);

	SyncBackPackToClient(PACK_TYPE_BASIC);
	return RE_SUCCESS;
}

void LC_ServerPlayer::BuddyRuneStar(int32_t buddyId, int32_t slotIndex)
{
	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyId);
	if(NULL != pkBuddy)
	{
		pkBuddy->BuddyRuneStar(slotIndex);
	}
}

void LC_ServerPlayer::BasicPackRuneReset(int32_t slotIndex)
{
	if(!IsMainController())
	{
		return;
	}

	MG_RLT_ResetRune rltMsg;
	rltMsg.m_iBuddyID = 0;
	rltMsg.m_iSlotIndex = slotIndex;
	rltMsg.m_iResult = DoBasicPackRuneReset(slotIndex, rltMsg.m_SlotData);

	//˳���ܻ� �ͻ���Ҫ����MGPT_RLT_RESET_RUNE ��SyncBackPackToClient(PACK_TYPE_BASIC);
	SendMsgToClient(MGPT_RLT_RESET_RUNE, &rltMsg);
	SyncBackPackToClient(PACK_TYPE_BASIC);
}

ResultType LC_ServerPlayer::DoBasicPackRuneReset(int32_t slotIndex, PS_ItemAssetEntry& slotData)
{
	LC_PackAsset* pkOwnerPackAsset= GetPackAsset();
	LC_BackPackEntry* pkRuneData = pkOwnerPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, slotIndex);
	if (NULL == pkRuneData|| !pkRuneData->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_ERROR_PARAM;
	}

	int32_t runeId = pkRuneData->GetBackPackValueByType(LPSFET_TYPE_ID);
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(runeId);
	if (NULL == pkItemData)
	{
		return RE_ERROR_PARAM;
	}
	
	int initLevel = pkItemData->_lRuneLevel;
	if(initLevel > 1)
	{
		return RE_RUNE_LEVEL_NOT_VALID;
	}

	int runeLevel = pkRuneData->GetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL);
	if(runeLevel <= initLevel)
	{
		return RE_RUNE_LEVEL_TOO_LOW;
	}

	if(!LC_Helper::CheckIsRune(runeId))
	{
		return RE_ERROR_PARAM;
	}

	//����������������
	Utility::LC_ComMap money;
	Utility::UT_SIMDataList consumeItem;
	GetRuneConsume(pkRuneData, consumeItem, money);
	ResultType res = RestituteRuneConsume(this, pkRuneData, consumeItem, money);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	//���õȼ� �Ǽ� ����
	pkRuneData->SetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL, initLevel);
	pkRuneData->SetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL, pkItemData->_AffiliatedSkill);
	pkRuneData->SetToProtocol(slotData);

	return RE_SUCCESS;
}

void LC_ServerPlayer::BuddyRuneReset(int32_t buddyId, int32_t slotIndex)
{
	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyId);
	if(NULL != pkBuddy)
	{
		pkBuddy->BuddyRuneReset(slotIndex);
	}
}

ResultType LC_ServerPlayer::DoEquipRuneToBuddy(int32_t srcBuddyID, int32_t srcSlotIndex, int32_t destBuddyID, int32_t destSlotIndex, PS_ItemAssetEntry& srcData, PS_ItemAssetEntry& destData)
{
	if(!IsMainController())
	{
		return RE_FAIL;
	}

	 const unique_id_impl& InstanceID = GetInstance();
	//����ϵͳ�Ƿ���
	bool tag = GetBuddyRuneEquipState();
	if(!tag)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r1, c_cid:%d, c_citizenship:%d", InstanceID.detail(), InstanceID.catagory());
		return RE_RUNE_BUDDY_SLOT_NOT_BE_ACTIVATED;
	}

	//Ŀ��buddy�Ƿ񼤻�
	BuddyInstance* pkDestBuddy = GetBuddyInstanceByID(destBuddyID);
	if(NULL == pkDestBuddy)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r2, buddyid:%d, c_cid:%d, c_citizenship:%d", destBuddyID, InstanceID.detail(), InstanceID.catagory());
		return RE_BUDDY_NOT_BE_ACTIVED;
	}

	//��λ�Ƿ���Դ���
	tag = pkDestBuddy->GetRuneSlotDressState(destSlotIndex);
	if(!tag)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r3, buddyid:%d, destSlotIndex:%d, c_cid:%d, c_citizenship:%d", destBuddyID, destSlotIndex, InstanceID.detail(), InstanceID.catagory());
		return RE_RUNE_BUDDY_RUNESLOT_DRESSSTATE_FAIL;
	}

	BuddyInstance* pkSrcBuddy = GetBuddyInstanceByID(srcBuddyID);
	if(0!=srcBuddyID && NULL==pkSrcBuddy)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r4, srcBuddyID:%d, destSlotIndex:%d, c_cid:%d, c_citizenship:%d", srcBuddyID, destSlotIndex, InstanceID.detail(), InstanceID.catagory());
		return RE_BUDDY_NOT_BE_ACTIVED;
	}

	//�õ�Դ����
	LC_BackPackEntry* pkSrcRuneItem = NULL;
	if(srcBuddyID == 0)
	{
		pkSrcRuneItem = GetPackAsset()->GetBackPackEntry(PACK_TYPE_BASIC, srcSlotIndex);
	}
	else
	{
		pkSrcRuneItem = GetRuneItemFromBuddy(srcBuddyID, srcSlotIndex);		
	}

	//Դ��Ч
	if(NULL==pkSrcRuneItem || !pkSrcRuneItem->GetBackPackValueByType(LPSFET_VALID))
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r5, srcBuddyID:%d, srcSlotIndex:%d, c_cid:%d, c_citizenship:%d", srcBuddyID, srcSlotIndex, InstanceID.detail(), InstanceID.catagory());
		return RE_UNVAILD_ITEM;
	}

	if(!LC_Helper::CheckIsRune(pkSrcRuneItem->GetBackPackValueByType(LPSFET_TYPE_ID)))
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r6, srcBuddyID:%d, srcSlotIndex:%d, c_cid:%d, c_citizenship:%d, itemID:%d", srcBuddyID, srcSlotIndex, InstanceID.detail(), InstanceID.catagory(), pkSrcRuneItem->GetBackPackValueByType(LPSFET_TYPE_ID));
		return RE_UNVAILD_ITEM;
	}

	LC_BackPackEntry* pkDestRuneItem = GetRuneItemFromBuddy(destBuddyID, destSlotIndex);
	if(NULL == pkDestRuneItem)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "DoEquipRuneToBuddy r7, destBuddyID:%d, destSlotIndex:%d, c_cid:%d, c_citizenship:%d", destBuddyID, destSlotIndex, InstanceID.detail(), InstanceID.catagory());
		return RE_UNVAILD_ITEM;
	}

	LC_BackPackEntry srcRuneItem_Temp;
	srcRuneItem_Temp.Clone(pkSrcRuneItem);	

	ResultType res = RE_FAIL;

	LC_BackPackEntry destRuneItem_Temp;
	destRuneItem_Temp.Clone(pkDestRuneItem);

	//Ŀ���λ��װ��
	if(pkDestRuneItem->GetBackPackValueByType(LPSFET_VALID))
	{
		//Դ��buddy����Դ��Ŀ�꣬���߻���
		if(NULL != pkSrcBuddy)
		{
			res = pkSrcBuddy->SwitchBuddyEquipRune(pkDestBuddy, srcSlotIndex, destSlotIndex);
		}
		else
		{
			res = pkDestBuddy->SwitchBasicRuneEquipRune(pkSrcRuneItem, destSlotIndex);
		}
	}
	else
	{
		//Ŀ���λû��װ��
		//Դ��buddy��Դbuddy�ı��ߺ�Ŀ��buddy�Ŀղ�λ����
		if(NULL != pkSrcBuddy)
		{
			res = pkSrcBuddy->SwitchBuddyEquipRune(pkDestBuddy, srcSlotIndex, destSlotIndex);
		}
		else
		{
			//Դ�ǻ�������
			res = pkDestBuddy->EquipRuneToBuddy(destSlotIndex, pkSrcRuneItem);
		}
	}

	if(RE_SUCCESS==res)
	{
		//���������־
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BAOJU_OPERATE, OPERATE_LOGTYPE1);
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(srcSlotIndex);
		if(pkSrcRuneItem)
			RecordItemOperateLog(this,nLogCode,CONSUME_OPERATE, pkDestRuneItem->GetBackPackValueByType(LPSFET_TYPE_ID), pkSrcRuneItem->GetBackPackValueByType(LPSFET_OVERLAP_COUNT), 1,nParams);
		if(pkDestRuneItem)
			RecordItemOperateLog(this,nLogCode,GET_OPERATE, pkDestRuneItem->GetBackPackValueByType(LPSFET_TYPE_ID), pkDestRuneItem->GetBackPackValueByType(LPSFET_OVERLAP_COUNT), 1,nParams);

		srcRuneItem_Temp.SetToProtocol(destData);
		if(NULL!=pkSrcBuddy)
		{
			destRuneItem_Temp.SetToProtocol(srcData);
		}
		else
		{
			SyncBackPackToClient(PACK_TYPE_BASIC);
		}		
	}
	
	return res;
}

ResultType LC_ServerPlayer::DoUnEquipRuneFromBuddy(int32_t srcBuddyID, int32_t srcSlotIndex)
{
	if(!IsMainController())
	{
		return RE_FAIL;
	}

	BuddyInstance* pkSrcBuddy = GetBuddyInstanceByID(srcBuddyID);
	if(NULL == pkSrcBuddy)
	{
		return RE_BUDDY_NOT_BE_ACTIVED;
	}

	//Դ�Ƿ���װ��
	LC_BackPackEntry* pkSrcRuneItem = GetRuneItemFromBuddy(srcBuddyID, srcSlotIndex);
	if(NULL == pkSrcRuneItem || !pkSrcRuneItem->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_UNVAILD_ITEM;
	}

	//Ŀ��װ������
	return pkSrcBuddy->UnEquipRuneFromBuddy(srcSlotIndex);
}

ResultType LC_ServerPlayer::DoOneKeyEquipRuneToBuddy(int32_t buddyID, std::map<int32_t, PS_ItemAssetEntry>& destSlotData)
{
	destSlotData.clear();

	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyID);
	if(NULL == pkBuddy)
	{
		return RE_FAIL;
	}

	if(!IsMainController())
	{
		return RE_FAIL;
	}

	LC_PackAsset* pkAsset = GetPackAsset();
	if(NULL == pkAsset)
	{
		return RE_FAIL;
	}

	//���һ�����������Ƿ���
	int vipLevel = GetVIPAsset().GetVIPLevel();
	LC_VIPCmnManager* pkMgr = LC_VIPCmnManager::GetSingletonPtr();
	LC_VIPProfitDataEntry* pkDataEntry = pkMgr->GetVIPProfitDataEntry(VIP_PROFIT_ONEKEY_OPERATOR_RUNE);
	if( NULL == pkDataEntry || !pkDataEntry->GetValue(vipLevel, false))
	{	
		return RE_FAIL;
	}

	int32_t dressRuneCount = pkBuddy->GetCanDressRuneSlotCount();

	mem::vector<int32_t> srcSlotIndex;
	mem::vector<int32_t> destSlotIndex;

	//�����б��ߵ�ս����slotIndex
	mem::multimap<int32_t, uint64_t, std::greater<int32_t> > baseSlotRuneCombatScoreMap;
	LC_BasicPack& pkBasicSlot = pkAsset->GetBasicPack();
	for(int p = 0; p<pkBasicSlot.GetCapacity(); ++p)
	{
		LC_BackPackEntry* pkItemEntry = &(pkBasicSlot.m_aData[p]);
		if(pkItemEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			int itemID = pkItemEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
			if(LC_Helper::CheckIsRune(itemID))
			{
				uint64_t combatScore = LC_Helper::GetRuneEquipScore(pkItemEntry);
				baseSlotRuneCombatScoreMap.insert(std::make_pair(combatScore, p));
			}
		}
	}

	//buddy���ڴ����ı��ߵ�ս����slotIndex
	mem::multimap<int32_t, uint64_t> runeSlotRuneCombatScoreMap;
	for(int slotIndex=0; slotIndex<GetGlobalSetting.RunePackSize; ++slotIndex)
	{
		LC_BackPackEntry* pkRuneSlotEntry = GetRuneItemFromBuddy(buddyID, slotIndex);
		if(NULL!=pkRuneSlotEntry && pkRuneSlotEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			uint64_t combatScore = LC_Helper::GetRuneEquipScore(pkRuneSlotEntry);
			runeSlotRuneCombatScoreMap.insert(std::make_pair(combatScore, slotIndex));
		}
		else if(GetRuneSlotDressState(buddyID, slotIndex))
		{
			destSlotIndex.push_back(slotIndex);
		}
	}
	
	//����ǰn�����ڿ�λ
	int n = dressRuneCount-runeSlotRuneCombatScoreMap.size();
	mem::multimap<int32_t, uint64_t, std::greater<int32_t> >::iterator baseIt=baseSlotRuneCombatScoreMap.begin();
	while(n>0 && baseIt!=baseSlotRuneCombatScoreMap.end())
	{
		srcSlotIndex.push_back(baseIt->second);
		++baseIt;
		--n;
	}

	while(baseIt!=baseSlotRuneCombatScoreMap.end() && srcSlotIndex.size()+n<dressRuneCount)
	{
		int slotScore = baseIt->first;
		mem::multimap<int32_t, uint64_t>::iterator runeIt=runeSlotRuneCombatScoreMap.begin(); 
		if(runeIt!=runeSlotRuneCombatScoreMap.end())
		{
			if(slotScore > runeIt->first)
			{
				srcSlotIndex.push_back(baseIt->second);
				destSlotIndex.push_back(runeIt->second);
				runeSlotRuneCombatScoreMap.erase(runeIt);
			}
			else
			{
				break;
			}
		}
		++baseIt;
	}

	if(srcSlotIndex.empty() || destSlotIndex.empty() || srcSlotIndex.size() > destSlotIndex.size())
	{
		return RE_FAIL;
	}

	//��
	for(int j=0; j<srcSlotIndex.size(); ++j)
	{
		int32_t srcIndex = srcSlotIndex[j];
		int32_t destIndex = destSlotIndex[j];
		PS_ItemAssetEntry srcData;
		PS_ItemAssetEntry destData;
		DoEquipRuneToBuddy(0, srcIndex, buddyID, destIndex, srcData, destData);
		destSlotData[destIndex] = destData;
	}

	pkBuddy->GetBuddyAttrMgr().RefreshRuneAttrMap(pkBuddy);
	pkBuddy->GetBuddyAttrMgr().RefreshHeroAllAttributeMap(this, pkBuddy);
	//ˢ�´�Ӣ�� �ṩ�����������
	pkBuddy->GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(this);
	//ˢ�����������һ�����������
	RefreshChainBuddyCoreAttrByOnChain(pkBuddy->GetTemplateID(), false, SPART_Buddy_7);
	pkBuddy->GetBuddyAttrMgr().RefreshNewFinalAttrMap(this, pkBuddy, false, SPART_Buddy_7);

	return RE_SUCCESS;
}

ResultType LC_ServerPlayer::DoOneKeyUnEquipRuneFromBuddy(int32_t buddyID, std::set<int32_t>& slotIndexVec)
{
	slotIndexVec.clear();

	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyID);
	if(NULL == pkBuddy)
	{
		return RE_FAIL;
	}

	if(!IsMainController())
	{
		return RE_FAIL;
	}

	LC_PackAsset* pkAsset = GetPackAsset();
	if(NULL == pkAsset)
	{
		return RE_FAIL;
	}

	// ���һ�����¹����Ƿ���
	int vipLevel = GetVIPAsset().GetVIPLevel();
	LC_VIPCmnManager* pkMgr = LC_VIPCmnManager::GetSingletonPtr();
	LC_VIPProfitDataEntry* pkDataEntry = pkMgr->GetVIPProfitDataEntry(VIP_PROFIT_ONEKEY_OPERATOR_RUNE);
	if( NULL == pkDataEntry || false == pkDataEntry->GetValue(vipLevel, false))
	{
		return RE_FAIL;
	}

	//���ڴ��˼�������
	int32_t count = 0;	
	for(int slotIndex=0; slotIndex<GetGlobalSetting.RunePackSize; ++slotIndex)
	{
		LC_BackPackEntry* pkRuneSlotEntry = GetRuneItemFromBuddy(buddyID, slotIndex);
		if(NULL!=pkRuneSlotEntry && pkRuneSlotEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			++count;
		}
	}

	//����ʣ�����
	int emptyCount = pkAsset->GetPackEmptyEntryCount(PACK_TYPE_BASIC);
	if(count > emptyCount)
	{
		return RE_ITEM_CANT_PUT_PACKAGE;
	}

	ResultType res = RE_FAIL;
	for(int32_t slotIndex=0; slotIndex<GetGlobalSetting.RunePackSize; ++slotIndex)
	{
		if(RE_SUCCESS == DoUnEquipRuneFromBuddy(buddyID, slotIndex))
		{
			res = RE_SUCCESS;
			slotIndexVec.insert(slotIndex);
		}
	}

	if(RE_SUCCESS == res)
	{
		pkBuddy->GetBuddyAttrMgr().RefreshRuneAttrMap(pkBuddy);
		pkBuddy->GetBuddyAttrMgr().RefreshHeroAllAttributeMap(this,pkBuddy);
		//ˢ�´�Ӣ�� �ṩ�����������
		pkBuddy->GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(this);
		//ˢ�����������һ�����������
		RefreshChainBuddyCoreAttrByOnChain(pkBuddy->GetTemplateID(), false);
		pkBuddy->GetBuddyAttrMgr().RefreshNewFinalAttrMap(this,pkBuddy, false);
	}

	return res;
}

const LC_BackPackEntry* LC_ServerPlayer::GetRuneDataFromBuddyRune(int32_t itemID)
{
	const LC_BackPackEntry* pkItem = NULL;
	for(mem::map<int32_t, BuddyInstance*>::iterator it = m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			pkItem = pkBuddy->GetRuneDataFromBuddyRune(itemID);
			if(NULL != pkItem)
			{
				break;
			}
		}
	}

	return pkItem;
}

LC_BackPackEntry* LC_ServerPlayer::GetRuneItemFromBuddy(int32_t buddyID, int32_t slotIndex)
{
	if(!IsMainController())
	{
		return NULL;
	}

	if(0 == buddyID)
	{
		return NULL;
	}

	BuddyInstance* pkBuddy = GetBuddyInstanceByID(buddyID);
	if(NULL != pkBuddy)
	{
		return pkBuddy->GetRuneItemByIndex(slotIndex);
	}

	return NULL;
}

void LC_ServerPlayer::GetAllBuddyRuneItemLevelData(mem::map<int32_t, int32_t>& itemID2LevelMap)
{
	itemID2LevelMap.clear();
	for(mem::map<int32_t, BuddyInstance*>::iterator it = m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			pkBuddy->GetAllBuddyRuneItemLevelData(itemID2LevelMap);
		}
	}
}

void LC_ServerPlayer::GetAllBuddyRuneItemStarLevelData(mem::map<int32_t, int32_t>& itemID2StarLevelMap)
{
	itemID2StarLevelMap.clear();
	for(mem::map<int32_t, BuddyInstance*>::iterator it = m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL != pkBuddy)
		{
			pkBuddy->GetAllBuddyRuneItemStarLevelData(itemID2StarLevelMap);
		}
	}
}

void LC_ServerPlayer::UpdateRuneRewardRecord(int32_t lItemID, bool addReward, bool modifyAddReward)
{
	bool tag = m_RuneData.UpdateRuneRewardRecord(lItemID, addReward, modifyAddReward);
	if(tag)
	{
		RecomputerRuneTotalLevel();
		RecomputerRuneTotalStarLevel();
	}	
}

void LC_ServerPlayer::CheckRuneSlotDressState()
{
	for(mem::map<int32_t, BuddyInstance*>::iterator it= m_ppUsableBuddyInstance.begin(); it!=m_ppUsableBuddyInstance.end(); ++it)
	{
		BuddyInstance* pkBuddy = it->second;
		if(NULL == pkBuddy)
		{
			continue;
		}

		pkBuddy->CheckFlushRuneSlotDressState();
	}
}
