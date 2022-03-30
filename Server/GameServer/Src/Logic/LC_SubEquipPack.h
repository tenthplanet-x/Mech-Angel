#ifndef _H_SubEquipPack_H_
#define _H_SubEquipPack_H_
#pragma once

#include "LC_Define.h"
#include "EquipPack.h"
#include "LC_ActorAttributeMap.h"

class CCommanderSubEquipPack : public CEquipBasePack
{
public:
	CCommanderSubEquipPack();
	~CCommanderSubEquipPack();

	//初始化
	void Init(int32_t subType, int32_t size);
	//加载
	void LoadData(int32_t subType, const char_data_define::PBPlayerEquip& data);
	//存储
	void SaveData(char_data_define::PBPlayerEquip* pkData);

	//获得进阶线装备数据
	void GetItemInfo(std::map<int32_t,PS_SubEquipSlotItemInfo>& data);

	//获得进阶线装备铸魂基础属性
	void UpdateSubAttributeMap(LC_ServerPlayer* pkPlayer);
	//进阶线装备属性
	void GetSubEquipAttr(LC_ActorAttributeMap* pkMap, int& nAppendixScore);
	//进阶线铸魂属性
	void GetSubEquipSlotMoldingAttr(LC_ServerPlayer* pkPlayer, LC_ActorAttributeMap* pkMap);
	//获得进阶线铸魂值属性
	void GetSubEquipSlotMoldingSuitValueAttr(LC_ActorAttributeMap* pkMap);
	//获得进阶线铸魂百分比属性
	void GetSubEquipSlotMoldingSuitPercentAttr(LC_ActorAttributeMap* pkMap);

	//登录，重新计算属性
	void EquipAllSubItem(LC_ServerPlayer* pkPlayer);

	//根据槽位格子，铸魂
	void UpdateSubEquipSlotMoldingSpirit(LC_ServerPlayer* pkPlayer, int32_t slotIndex);
	ResultType DoUpdateSubEquipSlotMoldingSpirit(LC_ServerPlayer* pkPlayer, int32_t slotIndex, int32_t& m_nSlotLevel);

	//根据槽位格子，穿戴装备
	void EquipSubItem(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex);
	ResultType DoEquipSubItemBySlotIndex(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex, int32_t& destSlotIndex, PS_ItemAssetEntry& slotData);
	ResultType DoEquipSubItemByItemEntry(LC_ServerPlayer* pkPlayer, LC_BackPackEntry* pkEntry);
	
	//根据槽位格子，脱装备
	void UnEquipSubItem(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex);
	ResultType DoUnEquipSubItem(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex);

	//根据槽位格子，进阶装备
	void SubEquipAdvance(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex);
	ResultType DoSubEquipAdvance(LC_ServerPlayer* pkPlayer, int32_t srcSlotIndex, PS_ItemAssetEntry& data);
protected:
	//更新套装属性信息
	void UpdateSubMoldingSpiritSuitInfo(LC_ServerPlayer* pkPlayer);
private:
	int32_t m_nSubType;
	LC_ActorAttributeMap m_nSubSuitPercentAttrMap;//进阶线套装百分比属性
	LC_ActorAttributeMap m_nSubSuitValueAttrMap;//进阶线套装值属性
};

#endif