#include "ClientPCH.h"

#include "GD_ClientActorModel.h"
#include "LC_ClientItem.h"
#include "CF_ItemAvartarFunc.h"
#include "CF_ItemGen.h"
#include "CF_ItemGenEffect.h"

using namespace CSVFile;
using namespace Data;
using namespace GameLogic;

//-------------------------------------------------------------
LC_ClientItem::LC_ClientItem()
: LC_ItemBase()
{
	m_kItemDescriptionEntry.Reset();
}
//-------------------------------------------------------------
LC_ClientItem::~LC_ClientItem()
{
}
//-------------------------------------------------------------
bool LC_ClientItem::OperationAvatarFunc(long lCharType, 
                                        GD_ClientActorModel* pkActor, 
                                        GameLogic::LC_BackPackEntry* pkItemEntry, 
                                        float fCurrentTime)
{
	if (m_lAvatarFuncID <= 0 || !pkActor)
	{
		return false;
	}

	CF_ItemAvartarFunc::DataEntry* pkItemAvatarFuncCSVData = CF_ItemAvartarFunc::GetSingleton().GetEntryPtr(m_lAvatarFuncID);
	if (!pkItemAvatarFuncCSVData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, 
            "��Ʒ [%i] AVARTAR �����޷�ʹ��", m_lItemTypeID);
		return false;
	}

	StringType strAvatarModFileName; 
//ToCheck
    //switch (lCharType)
    //{
    //case MALE_TYPE:
    //    strAvatarModFileName = pkItemAvatarFuncCSVData->_kMaleModelPath;
    //	break;
    //case FEMALE_TYPE:
    //    strAvatarModFileName = pkItemAvatarFuncCSVData->_kFeMaleModelPath;
    //    break;
    //default:
    //    T_ASSERT(false);
    //    return false;
    //}
	 strAvatarModFileName = pkItemAvatarFuncCSVData->_kMaleModelPath;
	
    bool bRet = false;
	switch (pkItemAvatarFuncCSVData->_lChangeSuitePosition)
	{
	case ISP_HEAD:			// ͷ����װ
	case ISP_BODY:			// ���廻װ
	case ISP_LEG:			// �Ȳ���װ

		{
			LC_ItemDescriptionEntry* ety = GetItemDescriptionEntry();
			bool bSuit = (ety->GetEquipType() == ET_BODY || ety->GetEquipType() == ET_HEAD || ety->GetEquipType() == ET_FOOT);
			if ( m_lEquipLimitFuncID == 0 || !bSuit )
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, 
					"װ��λ�ô�������Ʒ IDΪ %d", (int)m_lItemTypeID);
				return false;
			}
			bRet = pkActor->ChangeClothing((GD_ClothingModel::ClothingType)pkItemAvatarFuncCSVData->_lChangeSuitePosition, strAvatarModFileName);
        }
		break;
	case ISP_LEFT_HAND:     // ���ֻ�����
	case ISP_RIGHT_HAND:    // ���ֻ�����
	case ISP_BOTH_HAND:     // ˫�ֻ�����
// 		if( !IS_WEAPON_ITEM_ID(m_lItemTypeID) && !IS_FASHION_ID(m_lItemTypeID) )
// 		{
// 			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, 
// 				"����λ�ô�װ����Ʒ IDΪ%d", (int)m_lItemTypeID);
// 			return false;
// 		}
		bRet = pkActor->ChangeWeapon(GD_WeaponModel::WST_LRH, strAvatarModFileName);
		break;
	case ISP_EXTRA1:   //�����װ���Ҽ�����ߵȣ�
		bRet = pkActor->ChangeExtraClothing(strAvatarModFileName );
		break;
	case ISP_BOW:		//������
		if(!IS_BOW_ITEM_ID(m_lItemTypeID))
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, 
				"����λ�ô�װ����Ʒ IDΪ%d", (int)m_lItemTypeID);
			return false;
		}
		bRet = pkActor->ChangeBow(GD_WeaponModel::WST_RH, strAvatarModFileName);
		break;
	case ISP_RIDE_WEAPON:	//����ս����
		if (!IS_RIDE_WEAPON_ITEM_ID(m_lItemTypeID))
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, 
				"��ս����λ�ô�װ����Ʒ IDΪ%d", (int)m_lItemTypeID);
			return false;
		}
		bRet = pkActor->ChangeWeapon(GD_WeaponModel::WST_RH, strAvatarModFileName);
		break;
	case ISP_WING: //�л�����
		bRet = pkActor->ChangeWing(GD_WeaponModel::WST_NONE, strAvatarModFileName);
		break;
   default:
        return false;
	}

	return bRet;
}
//-------------------------------------------------------------
bool LC_ClientItem::UnoperationAvatarFunc(long lCharType, 
                                          GD_ClientActorModel* pkActor, 
                                          GameLogic::LC_BackPackEntry* pkItemEntry, 
                                          float fCurrentTime)
{
    if (m_lAvatarFuncID <= 0 || !pkActor)
    {
        return false;
    }

	CF_ItemAvartarFunc::DataEntry* pkData = CF_ItemAvartarFunc::GetSingleton().GetEntryPtr(m_lAvatarFuncID);
	if (!pkData)
	{
		Utility::GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "��Ʒ[%i]AVARTAR�����޷�ʹ��", m_lItemTypeID);
		return false;
	}

	switch (pkData->_lChangeSuitePosition)
	{
	case ISP_HEAD:			// ͷ����װ
	case ISP_BODY:			// ���廻װ
	case ISP_LEG:			// �Ȳ���װ
		pkActor->ChangeClothing((GD_ClothingModel::ClothingType)pkData->_lChangeSuitePosition, "");
		break;
	case ISP_LEFT_HAND:     // ���ֻ�����
	case ISP_RIGHT_HAND:    // ���ֻ�����
	case ISP_BOTH_HAND:     // ˫�ֻ�����
		pkActor->ChangeWeapon(GD_WeaponModel::WST_LRH, "");
		break;
	case ISP_EXTRA1:   //�����װ���Ҽ�����ߵȣ�
		pkActor->ChangeExtraClothing("");
		break;
 	case ISP_BOW:		//������
		pkActor->ChangeBow(GD_WeaponModel::WST_LRH, "");
		break;
	case ISP_RIDE_WEAPON:	//����ս����
		pkActor->ChangeWeapon(GD_WeaponModel::WST_LRH, "");
		break;
	case ISP_WING:			//�����
		pkActor->ChangeWing(GD_WeaponModel::WST_NONE, "");
		break;
   default:
        return false;
	}

	return true;
}

int GetItemGenEffectByIndex( CF_ItemGen::DataEntry * gen_entry , int index )
{
	switch(index)
	{
	case 0:
		return gen_entry->GenEffect1;
	case 1:
		return gen_entry->GenEffect2;
	case 2:
		return gen_entry->GenEffect3;
	case 3:
		return gen_entry->GenEffect4;
	case 4:
		return gen_entry->GenEffect5;
	case 5:
		return gen_entry->GenEffect6;
	case 6:
		return gen_entry->GenEffect7;
	case 7:
		return gen_entry->GenEffect8;
	default:
		break;
	}
	return 0;
}

void LC_ClientItem::CalcSubordinateAppendixMapModify( int nType, int32_t nStage, LC_ActorAttributeMap* kAttrMap)
{
	const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(nType, nStage);
	if (NULL == entry)
	{
		return;
	}
	LC_AttrFuncEntry* pkEntry = LC_ActorAttributeManager::GetSingletonPtr()->GetAttrFuncEntry(entry->_iAttrFuncID);
	if (NULL == pkEntry)
	{
		return;
	}

	ItemEffectManage* inst = ItemEffectManage::GetSingletonPtr();
	if ( NULL == inst )
		return;

	for ( int i = 0; i < MAX_ITEM_ATTR_COUNT; i++ )
	{
		int nEffectType = m_BaseEquipAttrib[i].GetEffectID();
		int nEffectLevel = m_BaseEquipAttrib[i].GetEffectLevel();
		ItemEffect* effect = inst->FindEffect(nEffectType);
		if (effect == NULL)
			continue;
		if (effect->m_iEffectFuncID < e_IEP_ATTR_SUB_TP_BEGIN || effect->m_iEffectFuncID >= e_IEP_ATTR_SUB_TP_END )
			continue;
		ItemEffectFuncBase* pFunc = effect->GetEffectFunc();
		if (pFunc->GetType() != eItemEffectFuncType_Subordinate)
			continue;

		attr_value_type nValue = effect->GetValue(nEffectLevel);
		LC_ActorAttributeAppendMap kAttrPlus;
		kAttrPlus.AddAttributeCommonPercent(nValue);
		kAttrPlus.RecomputeAll();
		LC_Helper::ComputeSubordinateAttributeMapModify(pkEntry, kAttrMap, &kAttrPlus);
	}
}