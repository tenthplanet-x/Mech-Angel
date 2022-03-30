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
	case LC_SUB_TP_VEHICLE://坐骑
		EquipSlotFromDB(data.mountequip());
		break;
	case LC_SUB_TP_DEVA_EYE://萌宠
		EquipSlotFromDB(data.petequip());
		break;
	case LC_SUB_TP_MUHUN://背饰
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
			case LC_SUB_TP_VEHICLE://坐骑
				EquipSlotToDB(pkData->mutable_mountequip());
				break;
			case LC_SUB_TP_DEVA_EYE://萌宠
				EquipSlotToDB(pkData->mutable_petequip());
				break;
			case LC_SUB_TP_MUHUN://背饰
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

	//进阶 属性
	LC_ActorAttributeMap TrainAttrAttrMap;
	LC_Helper::GetCimeliaTrainAttr(pkPlayer, m_nSubType, pkSubData->GetStage(),TrainAttrAttrMap);
	TrainAttrAttrMap.PrintAttrMap("TrainAttrAttrMap");
	
	//皮肤 属性
	LC_ActorAttributeMap SkinAttrAttrMap;
	LC_Helper::GetCimeliaSkinAttr(pkPlayer, m_nSubType, SkinAttrAttrMap);
	SkinAttrAttrMap.PrintAttrMap("SkinAttrAttrMap");
	
	//装备战力
	int nAppendixScore = 0;
	LC_ActorAttributeMap EquipAttrAttrMap;
	GetSubEquipAttr(&EquipAttrAttrMap, nAppendixScore);
	EquipAttrAttrMap.PrintAttrMap("EquipAttrAttrMap");

	//丹药 提供的基础属性 +丹药 提供的进阶线加成
	LC_ActorAttributeMap DrugAttrAttrMap;
	LC_Helper::GetCimeliaDrugAttr(pkPlayer, m_nSubType, pkSubData->GetStage(),DrugAttrAttrMap);
	DrugAttrAttrMap.PrintAttrMap("DrugAttrAttrMap");

	//铸魂 属性 
	LC_ActorAttributeMap MoldingAttributeMap; 
	GetSubEquipSlotMoldingAttr(pkPlayer, &MoldingAttributeMap);
	MoldingAttributeMap.PrintAttrMap("MoldingAttributeMap");
	
	//铸魂套装 + 固定值 
	LC_ActorAttributeMap SAttrValueMap;//加固定属性值
	GetSubEquipSlotMoldingSuitValueAttr(&SAttrValueMap);
	SAttrValueMap.PrintAttrMap("SAttrValueMap");

	//铸魂套装百分比属性
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
	//参数检测
	if(pkPlayer==NULL || slotIndex<0 || slotIndex>=m_SlotNum)
	{
		return RE_FAIL;
	}

	//槽位是否有装备
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

	//判断新等级是否存在
	int newLevel = currentLevel + 1;                                                                                                                                       
	CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkNewData = CItemConfigManager::GetSingletonPtr()->GetSoulEquipData(m_nSubType, slotIndex, newLevel);
	if(NULL == pkNewData)
	{
		return RE_FAIL;
	}

	//装备等阶是否达到要求
	if(pkEntry->_lLevel < pkData->_iCommanderEquipLevel)
	{
		return RE_COMTROLLEREQUIPSLOT_LEVEL_NOT_ENOUGH;
	}

	//指挥官等级是否足够
	if(pkPlayer->GetLevel() < pkData->_iCommanderLevel)
	{
		return RE_COMMANDER_LEVEL_NOT_RNOUGH;
	}

	//需要消耗的材料是否足够
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
	//铸魂--埋点日志
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

	//删除材料
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
	//刷新进阶线属性
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

	//检验背包道具
	LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, srcSlotIndex);
	if(NULL==pkEntry || false==pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_SUBTYPE_ERROR_3;
	}

	int itemId = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
	//检验装备操作是否合法
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemId);
	if(NULL == pkItem)
	{
		return RE_SUBTYPE_ERROR_4;
	}

	int packType = pkItem->GetEquipPackLimit(); //装备栏类型
	int destIndex = pkItem->GetEquipTypeLimit(); //装备栏位
	int levelLimit = pkItem->GetEquipLevelLimit(); //等级限制
	int playerLevel = pkPlayer->GetLevel();

	//装备栏类型检测
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

	//装备栏位异常
	if(destIndex >= m_SlotNum)
	{
		return RE_SUBTYPE_ERROR_6;
	}

	//等级不匹配
	if(levelLimit > playerLevel)
	{
		return RE_SUBTYPE_ERROR_7;
	}

	//判断原来栏位有无装备
	pkEntry->SetToProtocol(slotData);	//设置目的装备的数据
	const LC_BackPackEntry& oldEntry = GetEquipData(destIndex);
	if(oldEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		//道具id不一致，交换
		LC_BackPackEntry temp;
		temp.Clone(pkEntry);
		pkEntry->Clone(oldEntry);
		SetEquipData(destIndex, temp);
	}
	else
	{
		//装备操作
		SetEquipData(destIndex, *pkEntry);

		//删除源的道具
		pkPlayer->DeleteItemByIndex(PACK_TYPE_BASIC, srcSlotIndex, 1);
	}

	//更新背包 增量同步，后续删掉
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
	//更新属性
	pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, m_nSubType);

	//设置目标槽位index
	destSlotIndex = destIndex;

	return RE_SUCCESS;
}

ResultType CCommanderSubEquipPack::DoEquipSubItemByItemEntry(LC_ServerPlayer* pkPlayer, LC_BackPackEntry* pkEntry)
{
	//参数检测
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

	//检验装备操作是否合法
	int itemId = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);	
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemId);
	if(NULL == pkItem)
	{
		return RE_SUBTYPE_ERROR_4;
	}

	int packType = pkItem->GetEquipPackLimit(); //装备栏类型
	int destIndex = pkItem->GetEquipTypeLimit(); //装备栏位
	int levelLimit = pkItem->GetEquipLevelLimit(); //等级限制
	int playerLevel = pkPlayer->GetLevel();

	//装备栏类型检测
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

	//装备栏位异常
	if(destIndex >= m_SlotNum)
	{
		return RE_SUBTYPE_ERROR_6;
	}

	//等级不匹配
	if(levelLimit > playerLevel)
	{
		return RE_SUBTYPE_ERROR_7;
	}

	//判断原来栏位有无装备
	const LC_BackPackEntry& oldEntry = GetEquipData(destIndex);
	if(oldEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		//道具id不一致，交换
		LC_BackPackEntry temp;
		temp.Clone(pkEntry);
		pkEntry->Clone(oldEntry);
		SetEquipData(destIndex, temp);
	}
	else
	{
		//装备操作
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
	//更新属性
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

	//检验源槽位是否道具
	const GameLogic::LC_BackPackEntry& srcEntry = GetEquipData(srcSlotIndex);
	if(false == srcEntry.GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_SUBTYPE_ERROR_9;
	}

	//目标背包是否有空余格子
	int destVaildIndex = -1;
	LC_BackPackEntry* pkDestEntry = pkPackAsset->GetBasicPack().GetEmptyEntry(destVaildIndex);
	if(NULL==pkDestEntry || -1==destVaildIndex)
	{
		return RE_SUBTYPE_ERROR_10;
	}

	//把源道具复制到基础背包里
	pkDestEntry->Clone(srcEntry);

	//清除源道具
	(const_cast<GameLogic::LC_BackPackEntry&>(srcEntry)).Reset();
	
	//更新背包 增量同步，后续删掉
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

	//更新属性
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

	GameLogic::LC_BackPackEntry curEntry(equipEntry);//当前操作的装备
	int itemId = curEntry.GetBackPackValueByType(LPSFET_TYPE_ID);

	//检验数据是否合法
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(itemId);
	if(NULL == pkItem)
	{
		return RE_Item_Error;
	}

	//判断能否进阶
	CF_CompositionFormula::DataEntry* pkData= pItemMgr->GetAdvanceData(itemId);
	if(NULL == pkData)
	{
		return RE_Equip_NoAdvance;
	}

	//判断类型是否合法
	if(2 != pkData->_FormulaType)
	{
		return RE_Equip_Advance_TypeError;
	}

	//判断进阶的id是否合法
	pkItem = pkItemFactory->RequestItem(pkData->_lCompositionItemID);
	if(NULL == pkItem)
	{
		return RE_Item_Error;
	}

	GameLogic::LC_BackPackEntry newEntry;	//新生成的装备
	newEntry.SetBackPackValueByType(LPSFET_VALID, true);
	newEntry.SetBackPackValueByType(LPSFET_OVERLAP_COUNT,1);
	newEntry.SetBackPackValueByType(LPSFET_TYPE_ID, pkData->_lCompositionItemID);

	//判断消耗是否足够
	ResultType res = RE_SUCCESS;
	for(Utility::UT_SIMDataList::iterator ItemIt= pkData->_kNeedMaterials.begin(); ItemIt!=pkData->_kNeedMaterials.end(); ++ItemIt)
	{
		res = pkPlayer->TryDeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum());
		if(RE_SUCCESS != res)
		{
			return res;
		}
	}

	//将原来的装备脱下来
	res = DoUnEquipSubItem(pkPlayer, srcSlotIndex);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	//删除源道具
	pkPlayer->DeleteItem(itemId, 1);

	//删除材料
	for(Utility::UT_SIMDataList::iterator ItemIt=pkData->_kNeedMaterials.begin(); ItemIt!=pkData->_kNeedMaterials.end(); ++ItemIt)
	{
		pkPlayer->DeleteItem(ItemIt->GetItemId(), ItemIt->GetItemNum());
	}

	res = DoEquipSubItemByItemEntry(pkPlayer, &newEntry);
	if(RE_SUCCESS == res)
	{
		//穿成功了，设置目的槽位的数据
		newEntry.SetToProtocol(data);
	}
	else
	{
		//加进背包
		//pkPlayer->AddItem(pkData->_lCompositionItemID, 1);
		LC_Helper::AddItem(pkPlayer->GetPackAsset(), pkData->_lCompositionItemID, 1, -1, false);
	}

	//更新背包 增量同步，后续删掉
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