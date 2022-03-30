#include "LC_SubEquipPack.h"
#include "LC_ServerPlayer.h"
#include "LC_ItemFactoryBase.h"
#include "SystemError.h"
#include "char_data.pb.h"
#include "ItemConfigManager.h"

#include "CF_EquipCommanderSlotMoldingSpirit.h"
#include "CF_EquipCommanderSlotMoldingSpiritSuit.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;

CCommanderSubEquipPack::CCommanderSubEquipPack() : CEquipBasePack()
{

}

CCommanderSubEquipPack::~CCommanderSubEquipPack()
{

}

void CCommanderSubEquipPack::Init(int32_t subType, int32_t size)
{
	m_nSubType = subType;
	m_nSubSuitPercentAttrMap.Clear();
	m_nSubSuitValueAttrMap.Clear();
	CEquipBasePack::Init(size);
}

void CCommanderSubEquipPack::LoadData(int32_t subType, const char_data_define::PBPlayerEquip& data)
{
	m_nSubType = subType;
	switch(m_nSubType)
	{
	case LC_SUB_TP_VEHICLE://����
		EquipSlotFromDB(data.mountequip());
		break;
	case LC_SUB_TP_DEVA_EYE://�ȳ�
		EquipSlotFromDB(data.petequip());
		break;
	case LC_SUB_TP_MUHUN://����
		EquipSlotFromDB(data.wingequip());
		break;
	}
}

void CCommanderSubEquipPack::SaveData(char_data_define::PBPlayerEquip* pkData)
{
	if(NULL != pkData)
	{
		switch(m_nSubType)
		{
			case LC_SUB_TP_VEHICLE://����
				EquipSlotToDB(pkData->mutable_mountequip());
				break;
			case LC_SUB_TP_DEVA_EYE://�ȳ�
				EquipSlotToDB(pkData->mutable_petequip());
				break;
			case LC_SUB_TP_MUHUN://����
				EquipSlotToDB(pkData->mutable_wingequip());
				break;
		}		
	}	
}

void CCommanderSubEquipPack::GetItemInfo(std::map<int32_t,PS_SubEquipSlotItemInfo>& data)
{
	data.clear();
	for (int i=0; i<m_SlotNum; ++i)
	{
		PS_SubEquipSlotItemInfo result;
		const SEquipSlot& slotData = GetEquipSlotData(i);
		result.m_EquipItem = slotData.m_Data.GetBackPackValueByType(LPSFET_TYPE_ID);
		result.m_EquipLevel1 = slotData.m_SuiteId;

		data.insert(std::make_pair(i, result));
	}
}

void CCommanderSubEquipPack::UpdateSubAttributeMap(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	LC_SubUpgradeData* pkSubData = pkPlayer->GetSubordinateAsset().GetSubData(m_nSubType);
	if (NULL == pkSubData || !pkSubData->IsEnabled())
	{
		return;
	}

	m_AttributeMap.Clear();

	//���� ����
	LC_ActorAttributeMap TrainAttrAttrMap;
	LC_Helper::GetCimeliaTrainAttr(pkPlayer, m_nSubType, pkSubData->GetStage(),TrainAttrAttrMap);
	TrainAttrAttrMap.PrintAttrMap("TrainAttrAttrMap");
	
	//Ƥ�� ����
	LC_ActorAttributeMap SkinAttrAttrMap;
	LC_Helper::GetCimeliaSkinAttr(pkPlayer, m_nSubType, SkinAttrAttrMap);
	SkinAttrAttrMap.PrintAttrMap("SkinAttrAttrMap");
	
	//װ��ս��
	int nAppendixScore = 0;
	LC_ActorAttributeMap EquipAttrAttrMap;
	GetSubEquipAttr(&EquipAttrAttrMap, nAppendixScore);
	EquipAttrAttrMap.PrintAttrMap("EquipAttrAttrMap");

	//��ҩ �ṩ�Ļ������� +��ҩ �ṩ�Ľ����߼ӳ�
	LC_ActorAttributeMap DrugAttrAttrMap;
	LC_Helper::GetCimeliaDrugAttr(pkPlayer, m_nSubType, pkSubData->GetStage(),DrugAttrAttrMap);
	DrugAttrAttrMap.PrintAttrMap("DrugAttrAttrMap");

	//���� ���� 
	LC_ActorAttributeMap MoldingAttributeMap; 
	GetSubEquipSlotMoldingAttr(pkPlayer, &MoldingAttributeMap);
	MoldingAttributeMap.PrintAttrMap("MoldingAttributeMap");
	
	//������װ + �̶�ֵ 
	LC_ActorAttributeMap SAttrValueMap;//�ӹ̶�����ֵ
	GetSubEquipSlotMoldingSuitValueAttr(&SAttrValueMap);
	SAttrValueMap.PrintAttrMap("SAttrValueMap");

	//������װ�ٷֱ�����
	LC_ActorAttributeMap attrMoldingSuitMap;
	GetSubEquipSlotMoldingSuitPercentAttr(&attrMoldingSuitMap);
	attrMoldingSuitMap.AddAttributeMap(TrainAttrAttrMap,true);	
	attrMoldingSuitMap.RecomputeAllDelatValue();
	attrMoldingSuitMap.PrintAttrMap("attrMoldingSuitMap");

	for(int i = 0; i < ATT_MAX_COUNT; i++)
	{
		m_AttributeMap.AddAttributeMapByType(i,TrainAttrAttrMap);
		m_AttributeMap.AddAttributeMapByType(i,SkinAttrAttrMap);
		m_AttributeMap.AddAttributeMapByType(i,EquipAttrAttrMap);
		m_AttributeMap.AddAttributeMapByType(i,DrugAttrAttrMap);
		m_AttributeMap.AddAttributeMapByType(i,MoldingAttributeMap);
		m_AttributeMap.AddAttributeMapByType(i,SAttrValueMap);
	}
	m_AttributeMap.AddAttributeMap_AddValue(attrMoldingSuitMap);
	m_AttributeMap.PrintAttrMap("OutAttrMap");
}

void CCommanderSubEquipPack::GetSubEquipAttr(LC_ActorAttributeMap* pkMap, int& nAppendixScore)
{
	if(NULL == pkMap)
	{
		return;
	}

	nAppendixScore = 0;
	pkMap->Clear();
	for (int i=0; i<m_SlotNum; ++i)
	{
		const LC_BackPackEntry& equipEntry = GetEquipData(i);
		if(equipEntry.GetBackPackValueByType(LPSFET_VALID))
		{
			int iItemID = equipEntry.GetBackPackValueByType(LPSFET_TYPE_ID);

			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			LC_ItemBase* pkItem = pkItemFactory->RequestItem(iItemID);
			if (NULL == pkItem || !IS_EQUIP_ID(iItemID))
			{
				return ;
			}
			LC_ActorAttributeMap attrMap;
			pkItem->ComputerAttribMapModify(&attrMap, const_cast<GameLogic::LC_BackPackEntry*>(&equipEntry));
			nAppendixScore += pkItem->CalcSubordinateAppendixScore(m_nSubType, 1);
			pkMap->AddAttributeMap(attrMap);
		}
	}
}

void CCommanderSubEquipPack::GetSubEquipSlotMoldingAttr(LC_ServerPlayer* pkPlayer, LC_ActorAttributeMap* pkMap)
{
	if(NULL==pkPlayer || NULL==pkMap)
	{
		return;
	}

	for(int slotIndex=0; slotIndex<m_SlotNum; ++slotIndex)
	{
		int level = GetEquipSuiteId(slotIndex);
		CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = CItemConfigManager::GetSingletonPtr()->GetSoulEquipData(m_nSubType, slotIndex, level);
		if(NULL != pkData)
		{
			LC_Helper::GetCharAttrMap(pkPlayer, pkData->_lAttrFuncID, &m_AttributeMap);
		}
	}
}

void CCommanderSubEquipPack::GetSubEquipSlotMoldingSuitValueAttr(LC_ActorAttributeMap* pkMap)
{
	if(NULL != pkMap)
	{
		pkMap->CopyFrom(&m_nSubSuitValueAttrMap);
	}
}

void CCommanderSubEquipPack::GetSubEquipSlotMoldingSuitPercentAttr(LC_ActorAttributeMap* pkMap)
{
	if(NULL != pkMap)
	{
		pkMap->CopyFrom(&m_nSubSuitPercentAttrMap);
	}
}

void CCommanderSubEquipPack::EquipAllSubItem(LC_ServerPlayer* pkPlayer)
{
	UpdateSubMoldingSpiritSuitInfo(pkPlayer);
	UpdateSubAttributeMap(pkPlayer);
}

void CCommanderSubEquipPack::UpdateSubEquipSlotMoldingSpirit(LC_ServerPlayer* pkPlayer, int32_t slotIndex)
{
	MG_RLT_SubEquipSlotMoldingSpirit rltMsg;
	rltMsg.m_nSlotIndex = slotIndex;
	rltMsg.m_nSubType = m_nSubType;	
	rltMsg.m_nResult = DoUpdateSubEquipSlotMoldingSpirit(pkPlayer, slotIndex, rltMsg.m_nSlotLevel);
	pkPlayer->SendMsgToClient(MGPT_RLT_SUBEQUIPSLOTMOLDINGSPIRIT, &rltMsg);
}

ResultType CCommanderSubEquipPack::DoUpdateSubEquipSlotMoldingSpirit(LC_ServerPlayer* pkPlayer, int32_t slotIndex, int32_t& m_nSlotLevel)
{
	//�������
	if(pkPlayer==NULL || slotIndex<0 || slotIndex>=m_SlotNum)
	{
		return RE_FAIL;
	}

	//��λ�Ƿ���װ��
	const GameLogic::LC_BackPackEntry& itemEntry = GetEquipData(slotIndex);
	if(!itemEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_EQUIPSLOT_MOLDING_SPIRIT_ERROR_1;
	}

	int itemID = itemEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
	CF_ItemList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(itemID);
	if(NULL == pkEntry)
	{
		return RE_FAIL;
	}

	int currentLevel = GetEquipSuiteId(slotIndex);
	CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = CItemConfigManager::GetSingletonPtr()->GetSoulEquipData(m_nSubType, slotIndex, currentLevel);
	if(NULL == pkData)
	{
		return RE_FAIL;
	}

	//�ж��µȼ��Ƿ����
	int newLevel = currentLevel + 1;                                                                                                                                       
	CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkNewData = CItemConfigManager::GetSingletonPtr()->GetSoulEquipData(m_nSubType, slotIndex, newLevel);
	if(NULL == pkNewData)
	{
		return RE_FAIL;
	}

	//װ���Ƚ��Ƿ�ﵽҪ��
	if(pkEntry->_lLevel < pkData->_iCommanderEquipLevel)
	{
		return RE_COMTROLLEREQUIPSLOT_LEVEL_NOT_ENOUGH;
	}

	//ָ�ӹٵȼ��Ƿ��㹻
	if(pkPlayer->GetLevel() < pkData->_iCommanderLevel)
	{
		return RE_COMMANDER_LEVEL_NOT_RNOUGH;
	}

	//��Ҫ���ĵĲ����Ƿ��㹻
	for(Utility::UT_SIMDataList::iterator ItemIt=pkData->_kItem.begin(); ItemIt!=pkData->_kItem.end(); ++ItemIt)
	{
		ResultType res = pkPlayer->TryDeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum());
		if(RE_SUCCESS != res)
		{
			return res;
		}
	}

	bool tag = SetEquipSuiteId(slotIndex, currentLevel+1);
	if(!tag)
	{
		return RE_EQUIPSLOT_MOLDING_SPIRIT_ERROR_2;
	}
	//����--�����־
	int nLogCode = 0;
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(slotIndex);
	nParams.push_back(currentLevel);
	nParams.push_back(currentLevel+1);
	switch(m_nSubType)
	{
	case LC_SUB_TP_VEHICLE:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ITEMFORGE_OPERATE, OPERATE_LOGTYPE4);
		break;
	case LC_SUB_TP_DEVA_EYE:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ITEMFORGE_OPERATE, OPERATE_LOGTYPE6);
		break;
	case LC_SUB_TP_MUHUN:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ITEMFORGE_OPERATE, OPERATE_LOGTYPE5);;
		break;
	}

	//ɾ������
	for(Utility::UT_SIMDataList::iterator ItemIt=pkData->_kItem.begin(); ItemIt!=pkData->_kItem.end(); ++ItemIt)
	{
		ResultType res = pkPlayer->DeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum(),nLogCode, nParams);
		if(RE_SUCCESS != res)
		{
			return res;
		}
	}

	m_nSlotLevel = GetEquipSuiteId(slotIndex);

	UpdateSubMoldingSpiritSuitInfo(pkPlayer);

	int32_t reasonType = SPART_None;
	switch(m_nSubType)
	{
	case LC_SUB_TP_VEHICLE:
		reasonType = SPART_Sub_Vehicle_5;
		break;
	case LC_SUB_TP_DEVA_EYE:
		reasonType = SPART_Sub_Devaeye_5;
		break;
	case LC_SUB_TP_MUHUN:
		reasonType = SPART_Sub_Wing_5;
		break;
	}
	//ˢ�½���������
	pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, m_nSubType);
	return RE_SUCCESS;
}

void CCommanderSubEquipPack::EquipSubItem(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_Rlt_EquipSubItem rltMsg;
	rltMsg.m_nSrcSlotIndex = srcSlotIndex;
	rltMsg.m_nType = m_nSubType;
	rltMsg.m_nResult = DoEquipSubItemBySlotIndex(pkPlayer, srcSlotIndex, rltMsg.m_nDestSlotIndex, rltMsg.m_nDestEquipData);
	pkPlayer->SendMsgToClient(MGPT_RLT_EQUIPSUBITEM, &rltMsg);
}


ResultType CCommanderSubEquipPack::DoEquipSubItemBySlotIndex(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex, int32_t& destSlotIndex, PS_ItemAssetEntry& slotData)
{
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	if(NULL == pkPackAsset)
	{
		return RE_SUBTYPE_ERROR_1;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory )
	{
		return RE_SUBTYPE_ERROR_2;
	}

	//���鱳������
	LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, srcSlotIndex);
	if(NULL==pkEntry || false==pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_SUBTYPE_ERROR_3;
	}

	int itemId = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
	//����װ�������Ƿ�Ϸ�
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemId);
	if(NULL == pkItem)
	{
		return RE_SUBTYPE_ERROR_4;
	}

	int packType = pkItem->GetEquipPackLimit(); //װ��������
	int destIndex = pkItem->GetEquipTypeLimit(); //װ����λ
	int levelLimit = pkItem->GetEquipLevelLimit(); //�ȼ�����
	int playerLevel = pkPlayer->GetLevel();

	//װ�������ͼ��
	bool tag = false;
	if( (LC_SUB_TP_VEHICLE==m_nSubType&&MountPack==packType) ||
		(LC_SUB_TP_DEVA_EYE==m_nSubType&&PetPack==packType) ||
		(LC_SUB_TP_MUHUN==m_nSubType&&WingPack==packType) )
	{
		tag = true;
	}
	if(false == tag)
	{
		return RE_SUBTYPE_ERROR_5;
	}

	//װ����λ�쳣
	if(destIndex >= m_SlotNum)
	{
		return RE_SUBTYPE_ERROR_6;
	}

	//�ȼ���ƥ��
	if(levelLimit > playerLevel)
	{
		return RE_SUBTYPE_ERROR_7;
	}

	//�ж�ԭ����λ����װ��
	pkEntry->SetToProtocol(slotData);	//����Ŀ��װ��������
	const LC_BackPackEntry& oldEntry = GetEquipData(destIndex);
	if(oldEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		//����id��һ�£�����
		LC_BackPackEntry temp;
		temp.Clone(pkEntry);
		pkEntry->Clone(oldEntry);
		SetEquipData(destIndex, temp);
	}
	else
	{
		//װ������
		SetEquipData(destIndex, *pkEntry);

		//ɾ��Դ�ĵ���
		pkPlayer->DeleteItemByIndex(PACK_TYPE_BASIC, srcSlotIndex, 1);
	}

	//���±��� ����ͬ��������ɾ��
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

	int32_t reasonType = SPART_None;
	switch(m_nSubType)
	{
	case LC_SUB_TP_VEHICLE:
		reasonType = SPART_Sub_Vehicle_4;
		break;
	case LC_SUB_TP_DEVA_EYE:
		reasonType = SPART_Sub_Devaeye_4;
		break;
	case LC_SUB_TP_MUHUN:
		reasonType = SPART_Sub_Wing_4;
		break;
	}
	//��������
	pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, m_nSubType);

	//����Ŀ���λindex
	destSlotIndex = destIndex;

	return RE_SUCCESS;
}

ResultType CCommanderSubEquipPack::DoEquipSubItemByItemEntry(LC_ServerPlayer* pkPlayer, LC_BackPackEntry* pkEntry)
{
	//�������
	if(NULL==pkPlayer)
	{
		return RE_FAIL;
	}

	if(NULL==pkEntry || false==pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_SUBTYPE_ERROR_3;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
	{
		return RE_SUBTYPE_ERROR_2;
	}

	//����װ�������Ƿ�Ϸ�
	int itemId = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);	
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemId);
	if(NULL == pkItem)
	{
		return RE_SUBTYPE_ERROR_4;
	}

	int packType = pkItem->GetEquipPackLimit(); //װ��������
	int destIndex = pkItem->GetEquipTypeLimit(); //װ����λ
	int levelLimit = pkItem->GetEquipLevelLimit(); //�ȼ�����
	int playerLevel = pkPlayer->GetLevel();

	//װ�������ͼ��
	bool tag = false;
	if( (LC_SUB_TP_VEHICLE==m_nSubType&&MountPack==packType) ||
		(LC_SUB_TP_DEVA_EYE==m_nSubType&&PetPack==packType) ||
		(LC_SUB_TP_MUHUN==m_nSubType&&WingPack==packType) )
	{
		tag = true;
	}
	if(false == tag)
	{
		return RE_SUBTYPE_ERROR_5;
	}

	//װ����λ�쳣
	if(destIndex >= m_SlotNum)
	{
		return RE_SUBTYPE_ERROR_6;
	}

	//�ȼ���ƥ��
	if(levelLimit > playerLevel)
	{
		return RE_SUBTYPE_ERROR_7;
	}

	//�ж�ԭ����λ����װ��
	const LC_BackPackEntry& oldEntry = GetEquipData(destIndex);
	if(oldEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		//����id��һ�£�����
		LC_BackPackEntry temp;
		temp.Clone(pkEntry);
		pkEntry->Clone(oldEntry);
		SetEquipData(destIndex, temp);
	}
	else
	{
		//װ������
		SetEquipData(destIndex, *pkEntry);
	}

	int32_t reasonType = SPART_None;
	switch(m_nSubType)
	{
	case LC_SUB_TP_VEHICLE:
		reasonType = SPART_Sub_Vehicle_6;
		break;
	case LC_SUB_TP_DEVA_EYE:
		reasonType = SPART_Sub_Devaeye_6;
		break;
	case LC_SUB_TP_MUHUN:
		reasonType = SPART_Sub_Wing_6;
		break;
	}
	//��������
	pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, m_nSubType);

	return RE_SUCCESS;
}

void CCommanderSubEquipPack::UnEquipSubItem(LC_ServerPlayer* pkPlayer, int srcSlotIndex)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_Rlt_UnEquipSubItem rltMsg;
	rltMsg.m_nSrcSlotIndex = srcSlotIndex;
	rltMsg.m_nType = m_nSubType;
	rltMsg.m_nResult = DoUnEquipSubItem(pkPlayer, srcSlotIndex);
	pkPlayer->SendMsgToClient(MGPT_RLT_UNEQUIPSUBITEM, &rltMsg);
}

ResultType CCommanderSubEquipPack::DoUnEquipSubItem(LC_ServerPlayer* pkPlayer, int srcSlotIndex)
{
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	if(NULL == pkPackAsset)
	{
		return RE_SUBTYPE_ERROR_1;
	}

	//����Դ��λ�Ƿ����
	const GameLogic::LC_BackPackEntry& srcEntry = GetEquipData(srcSlotIndex);
	if(false == srcEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_SUBTYPE_ERROR_9;
	}

	//Ŀ�걳���Ƿ��п������
	int destVaildIndex = -1;
	LC_BackPackEntry* pkDestEntry = pkPackAsset->GetBasicPack().GetEmptyEntry(destVaildIndex);
	if(NULL==pkDestEntry || -1==destVaildIndex)
	{
		return RE_SUBTYPE_ERROR_10;
	}

	//��Դ���߸��Ƶ�����������
	pkDestEntry->Clone(srcEntry);

	//���Դ����
	(const_cast<GameLogic::LC_BackPackEntry&>(srcEntry)).Reset();
	
	//���±��� ����ͬ��������ɾ��
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

	int32_t reasonType = SPART_None;
	switch(m_nSubType)
	{
	case LC_SUB_TP_VEHICLE:
		reasonType = SPART_Sub_Vehicle_5;
		break;
	case LC_SUB_TP_DEVA_EYE:
		reasonType = SPART_Sub_Devaeye_5;
		break;
	case LC_SUB_TP_MUHUN:
		reasonType = SPART_Sub_Wing_5;
		break;
	}

	//��������
	pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, m_nSubType);

	return RE_SUCCESS;
}

void CCommanderSubEquipPack::SubEquipAdvance(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_RLT_SubEquipAdvance rltMsg;
	rltMsg.m_nResult = RE_SUCCESS;
	rltMsg.m_nSubType = m_nSubType;
	rltMsg.m_nSlotIndex = srcSlotIndex;
	rltMsg.m_nResult = DoSubEquipAdvance(pkPlayer, srcSlotIndex, rltMsg.m_Data);
	pkPlayer->SendMsgToClient(MGPT_RLT_SUBEQUIPADVANCE, &rltMsg);
}

ResultType CCommanderSubEquipPack::DoSubEquipAdvance(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex, PS_ItemAssetEntry& data)
{
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	if(NULL == pkPackAsset)
	{
		return RE_FAIL;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
	{
		return RE_FAIL;
	}

	CItemConfigManager* pItemMgr = CItemConfigManager::GetSingletonPtr();
	if(NULL == pItemMgr)
	{
		return RE_FAIL;
	}

	const LC_BackPackEntry& equipEntry = GetEquipData(srcSlotIndex);
	if(!equipEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_FAIL;
	}

	GameLogic::LC_BackPackEntry curEntry(equipEntry);//��ǰ������װ��
	int itemId = curEntry.GetBackPackValueByType(LPSFET_TYPE_ID);

	//���������Ƿ�Ϸ�
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemId);
	if(NULL == pkItem)
	{
		return RE_Item_Error;
	}

	//�ж��ܷ����
	CF_CompositionFormula::DataEntry* pkData= pItemMgr->GetAdvanceData(itemId);
	if(NULL == pkData)
	{
		return RE_Equip_NoAdvance;
	}

	//�ж������Ƿ�Ϸ�
	if(2 != pkData->_FormulaType)
	{
		return RE_Equip_Advance_TypeError;
	}

	//�жϽ��׵�id�Ƿ�Ϸ�
	pkItem = pkItemFactory->RequestItem(pkData->_lCompositionItemID);
	if(NULL == pkItem)
	{
		return RE_Item_Error;
	}

	GameLogic::LC_BackPackEntry newEntry;	//�����ɵ�װ��
	newEntry.SetBackPackValueByType(LPSFET_VALID, true);
	newEntry.SetBackPackValueByType(LPSFET_OVERLAP_COUNT,1);
	newEntry.SetBackPackValueByType(LPSFET_TYPE_ID, pkData->_lCompositionItemID);

	//�ж������Ƿ��㹻
	ResultType res = RE_SUCCESS;
	for(Utility::UT_SIMDataList::iterator ItemIt= pkData->_kNeedMaterials.begin(); ItemIt!=pkData->_kNeedMaterials.end(); ++ItemIt)
	{
		res = pkPlayer->TryDeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum());
		if(RE_SUCCESS != res)
		{
			return res;
		}
	}

	//��ԭ����װ��������
	res = DoUnEquipSubItem(pkPlayer, srcSlotIndex);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	//ɾ��Դ����
	pkPlayer->DeleteItem(itemId, 1);

	//ɾ������
	for(Utility::UT_SIMDataList::iterator ItemIt=pkData->_kNeedMaterials.begin(); ItemIt!=pkData->_kNeedMaterials.end(); ++ItemIt)
	{
		pkPlayer->DeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum());
	}

	res = DoEquipSubItemByItemEntry(pkPlayer, &newEntry);
	if(RE_SUCCESS == res)
	{
		//���ɹ��ˣ�����Ŀ�Ĳ�λ������
		newEntry.SetToProtocol(data);
	}
	else
	{
		//�ӽ�����
		//pkPlayer->AddItem(pkData->_lCompositionItemID, 1);
		LC_Helper::AddItem(pkPlayer->GetPackAsset(), pkData->_lCompositionItemID, 1, -1, false);
	}

	//���±��� ����ͬ��������ɾ��
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

	return res;
}

void CCommanderSubEquipPack::UpdateSubMoldingSpiritSuitInfo(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	m_nSubSuitValueAttrMap.Clear();
	m_nSubSuitPercentAttrMap.Clear();

	mem::map<int, int> subSuitInfo;
	for(int slotIndex=0; slotIndex<m_SlotNum; ++slotIndex)
	{
		int suitId = GetEquipSuiteId(slotIndex);
		if(0 == suitId)
		{
			continue;
		}

		CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = CItemConfigManager::GetSingletonPtr()->GetSoulEquipData(m_nSubType, slotIndex, suitId);
		if(pkData)
		{
			subSuitInfo[pkData->_iSuitID] += 1;
		}
	}

	if(subSuitInfo.empty())
	{
		return;
	}

	mem::map<int, int>::reverse_iterator it1 =subSuitInfo.rbegin();
	mem::map<int, int>::reverse_iterator it2 = ++subSuitInfo.rbegin();
	while(it2 != subSuitInfo.rend())
	{
		it2->second += it1->second;
		++it1;
		++it2;
	}

	for(mem::map<int, int>::iterator it=subSuitInfo.begin(); it!=subSuitInfo.end(); ++it)
	{
		AchievementEvent kEvent;
		kEvent.SetConditionInfo(m_nSubType, it->first);
		kEvent.SetNum(it->second);
		pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_SUB_SOUL_SUIT, kEvent);
	}

	LC_Helper::GetSubSuitAttrInfoFromItemEffectFunc(&subSuitInfo, &m_nSubSuitValueAttrMap, &m_nSubSuitPercentAttrMap, m_nSubType);

	m_nSubSuitValueAttrMap.RecomputeAll();
	m_nSubSuitPercentAttrMap.RecomputeAll();
}