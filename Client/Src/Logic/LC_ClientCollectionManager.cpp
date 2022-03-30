#include "ClientPCH.h"
#include "LC_ClientCollectionManager.h"
#include "UT_LogicInfoMessage.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientTaskMap.h"
#include "LC_ClientLogicManager.h"
#include "UI_UIManager.h"
#include "UT_Timer.h"
#include "CF_GameStoryClientUI.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ActorAttributeMap.h"
#include "LC_ClientCollectionManager.h"

#include "AS_ClientSoundManager.h"

using namespace CSVFile;
using namespace Utility;
using namespace Data;
using namespace GameLogic;
using namespace Path;

LC_ClientCollectionInfo::LC_ClientCollectionInfo()
: m_iCollectionID(0)
, m_iSuitID(0)
, m_iPos(0)
, m_iType(0)
{
	m_iActiveLevel.clear();

}

LC_ClientCollectionInfo::~LC_ClientCollectionInfo()
{
	m_iCollectionID = 0;
	m_iSuitID = 0;
	m_iActiveLevel.clear();
}

LC_ClientCollectionManager::LC_ClientCollectionManager()
{
	m_kCollectionData.clear();
	m_kCollectionSuitData.clear();

	LC_DirtyFlagAble::Reset();
}

LC_ClientCollectionManager::~LC_ClientCollectionManager()
{
	LC_ClientCollectionMapIter iter = m_kCollectionData.begin();
	for (; iter != m_kCollectionData.end(); ++iter)
	{
		T_SAFE_DELETE(iter->second);
		iter->second = NULL;
	}
	m_kCollectionData.clear();

	m_kCollectionSuitData.clear();

	LC_DirtyFlagAble::Reset();
}

void LC_ClientCollectionManager::Reset()
{
	LC_ClientCollectionMapIter iter = m_kCollectionData.begin();
	for (; iter != m_kCollectionData.end(); ++iter)
	{
		mem::vector<ActiveLevelInfo>::iterator iter2 = iter->second->m_iActiveLevel.begin();
		for (; iter2 != iter->second->m_iActiveLevel.end(); ++iter2)
		{
			iter2->m_bAcitve = false;
		}
	}

	m_kCollectionSuitData.clear();
}

bool LC_ClientCollectionInfo::ConstructDataEntry(CF_CollectionActivation::DataEntry* pkData)
{
	if (pkData == NULL)
	{
		return false;
	}
	ActiveLevelInfo levelInfo(pkData->id, false, pkData->PropertyID, pkData->_lGrade, &pkData->_kIcon2, 
		&pkData->_kIcon1, pkData->ActivationID, pkData->ActivationStarLevel, &pkData->Name);

	m_iActiveLevel.push_back(levelInfo);

	m_iCollectionID = pkData->CollectionID;
	m_iSuitID = pkData->SuitID;
	m_iPos = pkData->Pos;
	m_iType = pkData->Type;
	m_sAchievePath = pkData->_sAchievePath;
	return true;
}
//----------------------------------------------------------------------------------------------------------
void	LC_ClientCollectionManager::Init()
{
	m_kCollectionData.clear();

	CF_CollectionActivation::DataEntryMapExternIterator kIter = CF_CollectionActivation::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_CollectionActivation::DataEntry* pkData = kIter.PeekNextValuePtr();
		LC_ClientCollectionMapIter Iter = m_kCollectionData.find(pkData->CollectionID);
		if (Iter == m_kCollectionData.end())
		{
			LC_ClientCollectionInfo* Info = T_NEW_D LC_ClientCollectionInfo;
			Info->ConstructDataEntry(pkData);
			m_kCollectionData[pkData->CollectionID] = Info;
		}
		else
		{
			Iter->second->ConstructDataEntry(pkData);
		}


		kIter.MoveNext();
	}
}
//----------------------------------------------------------------------------------------------------------
void LC_ClientCollectionManager::GetDataFromProtocolStruct(const Protocol::MG_SyncCollectionInfo & rltMsg)
{
	Reset();

	m_kCollectionSuitData.resize(rltMsg.m_nCollectionRewardID.size());
	for (int index = 0; index < rltMsg.m_nCollectionRewardID.size(); ++index)
	{
		m_kCollectionSuitData[index] = rltMsg.m_nCollectionRewardID[index];
	}

	for (int index = 0; index < rltMsg.m_nCollectionActivationID.size(); ++index )
	{
		int32_t iCollectionID = rltMsg.m_nCollectionActivationID[index];
		CF_CollectionActivation::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CollectionActivation>(iCollectionID);
		if (pkData == NULL)
		{
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "CF_CollectionActivation表找不到该数据");
			continue;
		}
		
		LC_ClientCollectionMapIter DataIter = m_kCollectionData.find(pkData->CollectionID);
		if (DataIter ==  m_kCollectionData.end())
		{
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "CF_CollectionActivation该数据未初始化成功");
			continue;
		}
		mem::vector<ActiveLevelInfo>::iterator iter = DataIter->second->m_iActiveLevel.begin();
		for (; iter != DataIter->second->m_iActiveLevel.end(); ++iter)
		{
			if (iter->m_iID == pkData->id )
			{
				iter->m_bAcitve = true;
				break;
			}
		}
		
	}

	UpdateDirtyFlag();
}
//----------------------------------------------------------------------------------------------------------
void LC_ClientCollectionManager::GetDataFromProtocolCollection(const std::vector<int32_t>& kIDs)  // 流水id
{
	for(size_t i=0; i<kIDs.size(); ++i)
	{
		CF_CollectionActivation::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CollectionActivation>(kIDs[i]);
		if (pkData != NULL)
		{
			LC_ClientCollectionMapIter DataIter = m_kCollectionData.find(pkData->CollectionID);
			if (DataIter ==  m_kCollectionData.end())
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CF_CollectionActivation该数据未初始化成功");
			}
			else
			{
				mem::vector<ActiveLevelInfo>::iterator iter = DataIter->second->m_iActiveLevel.begin();
				for (; iter != DataIter->second->m_iActiveLevel.end(); ++iter)
				{
					if (iter->m_iID == pkData->id )
					{
						iter->m_bAcitve = true;
						break;
					}
				}
			}
		}
	}

	UpdateDirtyFlag();
}
//----------------------------------------------------------------------------------------------------------
void LC_ClientCollectionManager::GetDataFromProtocolSuit(int32_t SuitiID)  // 流水id
{
	CF_CollectionReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CollectionReward>(SuitiID);
	if (pkData == NULL)
	{
		return;
	}
	m_kCollectionSuitData.push_back(SuitiID);

	UpdateDirtyFlag();
}
//----------------------------------------------------------------------------------------------------------
bool LC_ClientCollectionManager::IsSuitActived(int32_t suitID)
{
	for (int index = 0; index < m_kCollectionSuitData.size(); ++index)
	{
		if (m_kCollectionSuitData[index] == suitID)
		{
			return true;
		}
	}
	return false;
}
//----------------------------------------------------------------------------------------------------------
StringType LC_ClientCollectionManager::GetActiveCollectionInfo(int32_t collectionID)
{
	LC_ClientCollectionMapIter iter = m_kCollectionData.find(collectionID);
	if (iter == m_kCollectionData.end())
	{
		return "";
	}

	LC_ClientCollectionInfo* pkCollectionInfo = iter->second;
	StringStreamType sStream("");
	sStream << pkCollectionInfo->m_iPos << "#";
	sStream << pkCollectionInfo->m_iSuitID << "#";

	mem::vector<ActiveLevelInfo>::iterator iter2 = pkCollectionInfo->m_iActiveLevel.begin();
	if ( !iter2->m_bAcitve) //如果初始未激活
	{
		sStream << iter2->m_iActiveNeedID << "#";
		sStream << iter2->m_iActiveNeedLevel << "#";
		sStream << *(iter2->m_sActiveIcon) << "#";
		sStream << *(iter2->m_sName) << "#";
		sStream << iter2->m_bAcitve << "#";
		sStream << iter2->m_iGrade << "#";
		sStream << iter2->m_iID << "#";
		sStream << iter2->m_iPropertyID << "#";
		sStream << pkCollectionInfo->m_sAchievePath << "#";
		return sStream.str();
	}

	//倒叙寻找最大
	const size_t actLvSize = pkCollectionInfo->m_iActiveLevel.size();
	int i = actLvSize - 1;
	for( ; i>=0; --i)
	{
		const ActiveLevelInfo& kInfo = pkCollectionInfo->m_iActiveLevel[i];
		if(kInfo.m_bAcitve)
		{
			sStream << kInfo.m_iActiveNeedID << "#";
			sStream << kInfo.m_iActiveNeedLevel  << "#";
			sStream << *(kInfo.m_sActiveIcon) << "#";
			sStream << *(kInfo.m_sName) << "#";
			sStream << kInfo.m_bAcitve << "#";
			sStream << kInfo.m_iGrade << "#";
			break;
		}
	}
	
	//需要先取到下一等级的id
	
	if (i < (actLvSize - 1))
	{
		const ActiveLevelInfo& kInfo = pkCollectionInfo->m_iActiveLevel[++i];
		sStream << kInfo.m_iID << "#";
	}
	else
	{
		sStream << "0#";
	}
	//修改为当前累加的属性
	mem::vector<ActiveLevelInfo>::iterator iter4 = pkCollectionInfo->m_iActiveLevel.begin();
	for (; iter4 != pkCollectionInfo->m_iActiveLevel.end(); ++iter4)
	{
		if (iter4->m_bAcitve)
		{
			sStream << iter4->m_iPropertyID << "_";
		}
	}
	sStream << "#";
	sStream << pkCollectionInfo->m_sAchievePath << "#";

	return sStream.str();
}
//----------------------------------------------------------------------------------------------------------
int LC_ClientCollectionManager::GetCollectionSuitTotalLevel(int32_t suitiID)   //流水id
{
	int totalLevel = 0;

	CF_CollectionReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CollectionReward>(suitiID);
	if (pkData == NULL)
	{
		return totalLevel;
	}
	int condition = pkData->Condition;
	
	Utility::UT_SIMDataList::const_iterator it = pkData->CollectionID.begin();
	for (; it!=pkData->CollectionID.end(); ++it)
	{
		int collectionID = 	it->ID();
		if (IsCollectionFirstActived(collectionID))
		{
			if (condition == COLLECTION_Num)
			{	
				totalLevel += 1;
			}
			else if (condition == COLLECTION_StarLevel)
				totalLevel += GetCollectionMaxLevel(collectionID);
		}
	}

	return totalLevel;
}
//----------------------------------------------------------------------------------------------------------
int LC_ClientCollectionManager::GetCollectionMaxLevel(int32_t collectionID)
{
	LC_ClientCollectionMapIter iter = m_kCollectionData.find(collectionID);
	if (iter == m_kCollectionData.end())
	{
		return -1;
	}

	mem::vector<ActiveLevelInfo>::reverse_iterator iter2 = iter->second->m_iActiveLevel.rbegin();
	for (; iter2 != iter->second->m_iActiveLevel.rend(); ++iter2)
	{
		if (iter2->m_bAcitve)
		{
			return iter2->m_iActiveNeedLevel;
		}
	}
	// 初始未激活以-1星
	return -1;
}

//----------------------------------------------------------------------------------------------------------
StringType LC_ClientCollectionManager::GetCollectionSizeByType(int32_t type)
{
	int totalSize = 0;
	int activeSize = 0;
	LC_ClientCollectionMapIter iter = m_kCollectionData.begin();
	for (; iter != m_kCollectionData.end(); ++iter)
	{
		LC_ClientCollectionInfo* pkCollectionInfo = iter->second;
		if (pkCollectionInfo->m_iType == type)
		{
			totalSize += 1;
			if (pkCollectionInfo->m_iActiveLevel.begin()->m_bAcitve)
			{
				activeSize += 1;
			}
		}
	}
	StringStreamType sStream("");
	sStream << totalSize <<"#";
	sStream << activeSize <<"#";
	
	return sStream.str();
}
//----------------------------------------------------------------------------------------------------------
StringType LC_ClientCollectionManager::GetAllAttributes()
{
	mem::map<int, int> FuncIDMap;
	LC_ClientCollectionMapIter iter = m_kCollectionData.begin();
	for(; iter!=m_kCollectionData.end(); ++iter)
	{
		const LC_ClientCollectionInfo* pkCollectionInfo = iter->second;
		mem::vector<ActiveLevelInfo>::const_iterator iter2 = pkCollectionInfo->m_iActiveLevel.begin();
		for (; iter2 != pkCollectionInfo->m_iActiveLevel.end(); ++iter2)
		{
			if (iter2->m_bAcitve)
			{
				FuncIDMap[iter2->m_iPropertyID]++;
			}
		}
	}

	LC_ActorAttributeMap attrMap;
	LC_Helper::GetAttrMapByFuncIDs(&attrMap, FuncIDMap, NULL);

	StringStreamType sStream;
	sStream << Int2String(LC_Helper::CalcAttributeMapCombatScore(&attrMap, false)) << '@';
	for(size_t i=0; i<ATT_MAX_COUNT; ++i)
	{
		sStream << Int2String(i) << '#' << Int2String(attrMap.GetAttribute(i)) << '|';
	}
	return sStream.str();
}
//----------------------------------------------------------------------------------------------------------
StringType LC_ClientCollectionManager::GetPropertyAttriString(long propertyID)
{
	LC_ActorAttributeMap kMap;
	LC_Helper::AttributeMapByAttrFunc(&kMap, propertyID);
	kMap.RecomputeAll();
	StringStreamType sStream("");	

	for (int index = ATT_NONE; index <ATT_RES_END; ++index)
	{
		if (kMap.GetAttribute(index) > 0)
		{
			sStream << index << "#" << kMap.GetAttribute(index) << "#";
		}
	}
	return sStream.str();

}
//----------------------------------------------------------------------------------------------------------
bool LC_ClientCollectionManager::IsCollectionFirstActived(int32_t collectionID)
{
	LC_ClientCollectionMapIter iter = m_kCollectionData.find(collectionID);
	if (iter == m_kCollectionData.end())
	{
		return false;
	}

	LC_ClientCollectionInfo* pkCollectionInfo = iter->second;

	mem::vector<ActiveLevelInfo>::iterator iter2 = pkCollectionInfo->m_iActiveLevel.begin();
	return iter2->m_bAcitve;
}
//----------------------------------------------------------------------------------------------------------
bool LC_ClientCollectionManager::CheckCanActive(int32_t collectionID)
{
	LC_ClientCollectionMapIter iter = m_kCollectionData.find(collectionID);
	if (iter == m_kCollectionData.end())
	{
		return false;
	}
	int type = iter->second->m_iType;
	mem::vector<ActiveLevelInfo>::iterator iter2 =  iter->second->m_iActiveLevel.begin();
	for (; iter2 != iter->second->m_iActiveLevel.end(); ++iter2)
	{
		if (iter2->m_bAcitve == false)
		{
			int activationID = iter2->m_iActiveNeedID;
			int activationStarNeed = iter2->m_iActiveNeedLevel;
			return CheckUpgrad( type, activationID, activationStarNeed );
		}
	}
	// 全满
	return false;
}
//----------------------------------------------------------------------------------------------------------
bool LC_ClientCollectionManager::CheckCanUpgrad(int32_t suitID)
{

	return false;
}
//----------------------------------------------------------------------------------------------------------
bool	LC_ClientCollectionManager::CheckUpgrad(int type, int activationID, int activationStarNeed)
{

	//T_PROFILE_TIMER("	LC_ClientCollectionManager::CheckUpgrad");
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkGamePlayer)
		return false;

	switch (type)
	{
	case COLLECTION_MONSTER:
		{
			LC_SubUpgradeData* pkSub = pkGamePlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_VEHICLE);
			if(!pkSub)
			{
				return false ;
			}

			LC_SubUpgradeData::TransformMap::iterator it = pkSub->m_kTansfroms.begin();
			for(; it != pkSub->m_kTansfroms.end(); ++it)
			{
				if (it->first == activationID )
				{
					return true;
				}
			}
		}
		break;
	case COLLECTION_EQUIP:
		{
			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			if(NULL == pkItemFactory)
			{
				return false;
			}
			LC_ItemDescriptionEntry* ItemEntry = pkItemFactory->GetItemDescriptionEntry(activationID);
			if(NULL == ItemEntry)
			{
				return false;
			}
			int itemCategory = ItemEntry->GetCategory1();

			LC_PackAsset *pkPackAsset = pkGamePlayer->GetPackAsset();
			if(NULL == pkPackAsset)
			{
				return false;
			}
			int iSize = pkPackAsset->GetPackCapacity(PACK_TYPE_BASIC);
			for (int i = 0; i < iSize; ++i)
			{
				LC_BackPackEntry *pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, i);
				int itemID = pkEntry->GetTypeID();
				if(pkEntry->GetValid() && IS_EQUIP_ID(itemID) && itemID == activationID)
				{
					return true;
				}
			}

			for (int index = PACK_TYPE_UNDEFINE; index < PACK_TYPE_MAX_NUM; index ++)
			{
				if (IS_WAREHOUSE_PACK(index))
				{
					int iSize = pkPackAsset->GetPackCapacity(index);
					for (int i = 0; i < iSize; ++i)
					{
						LC_BackPackEntry *pkEntry = pkPackAsset->GetBackPackEntry(index, i);
						int itemID = pkEntry->GetTypeID();
						if(pkEntry->GetValid() && IS_EQUIP_ID(itemID) && itemID == activationID)
						{
							return true;
						}
					}

				}
			}

			if (itemCategory == ITEM_CATEGORY_EQUIP)
			{
				int iSize = GetGlobalSetting.ArtifactPackSize;
				for (int index = 0; index < pkGamePlayer->GetActiveBuddyListSize(); ++index)
				{
					int cID = pkGamePlayer->GetBuddyActiveCID(index);
					for (int i = 0; i < iSize; ++i)
					{
						LC_BackPackEntry *pkEntry = pkGamePlayer->GetBuddyEquipEntry(cID, i);
						int itemID = pkEntry->GetTypeID();
						if(pkEntry->GetValid() && IS_EQUIP_ID(itemID) && itemID == activationID)
						{
							return true;
						}
					}
				}
				for (int index = 0; index < pkGamePlayer->GetTotalBuddyNum(); ++index)
				{
					if (pkGamePlayer->GetBuddyInactiveUnlock(index))
					{
						int cID = pkGamePlayer->GetBuddyInactiveCID(index);
						for (int i = 0; i < iSize; ++i)
						{
							LC_BackPackEntry *pkEntry = pkGamePlayer->GetBuddyEquipEntry(cID, i);
							int itemID = pkEntry->GetTypeID();
							if(pkEntry->GetValid() && IS_EQUIP_ID(itemID) && itemID == activationID)
							{
								return true;
							}
						}
					}
				}
			}
			else if (itemCategory == ITEM_CATEGORY_SUB_EQUIP)
			{
				for(size_t i=WingPack; i<=EquipPack_Count; ++i)
				{
					const EquipVec& equipList = pkGamePlayer->GetCommander()->GetEquipList(i);
					for(size_t j=0; j<equipList.size(); ++j)
					{
						const LC_BackPackEntry& pkEntry = equipList[j];
						if(pkEntry.GetValid() && pkEntry.GetTypeID() == activationID)
						{
							return true;
						}
					}
				}
			}
	
			return false;
		}
		break;
	case COLLECTION_PET:
		{
			LC_SubUpgradeData* pkSub = pkGamePlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_DEVA_EYE);
			if(!pkSub)
			{
				return false ;
			}

			LC_SubUpgradeData::TransformMap::iterator it = pkSub->m_kTansfroms.begin();
			for(; it != pkSub->m_kTansfroms.end(); ++it)
			{
				if (it->first == activationID )
				{
					return true;
				}
			}
		}
		break;
	case COLLECTION_CHARACTER:
		{
			bool bUnLock = pkGamePlayer->GetBuddyUnlock(activationID);
			if (!bUnLock)
			{
				return false;
			}
			int buddyStar = pkGamePlayer->GetBuddyStarLevel(activationID);
			return buddyStar >= activationStarNeed;
		}
		break;
	case COLLECTION_TOY:
		{
			LC_ClientToyPetManager * pkToyPetManager = LC_ClientToyPetManager::GetSingletonPtr();
			bool active = pkToyPetManager->CheckPetcIDActive(activationID);
			if (active)
			{
				int activeLevel = pkToyPetManager->GetcIDMaxLevel(activationID);
				return activeLevel >= activationStarNeed;
			}
			else
				return false;

		}
		break;
	case COLLECTION_WING:
		{
			LC_SubUpgradeData* pkSub = pkGamePlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_MUHUN);
			if(!pkSub)
			{
				return false ;
			}

			LC_SubUpgradeData::TransformMap::iterator it = pkSub->m_kTansfroms.begin();
			for(; it != pkSub->m_kTansfroms.end(); ++it)
			{
				if (it->first == activationID )
				{
					return true;
				}
			}
		}
		break;
	case COLLECTION_RUNE:
		{
			////穿在角色身上的
			const buddyInstanceMap& kInstMap = pkGamePlayer->GetBuddyInstanceMap();
			for (buddyInstanceMap::const_iterator itr = kInstMap.begin(); itr!=kInstMap.end(); ++itr)
			{
				LC_ClientBuddyInstance* pkInst = itr->second;
				for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
				{
					const LC_BackPackEntry* pItemData = pkInst->GetRuneEquipEntry(i);
					if( pItemData->GetValid()
						&& pItemData->GetTypeID() == activationID
						&& pItemData->GetEquipStar()>=activationStarNeed)
					{	
						return true;
					}
				}
			}

			LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
			//背包里没穿的宝具
			const LC_BasicPack& pRuneInPack = pkPackAsset->GetBasicPack();
			for (int i = 0;i<pRuneInPack.GetSize();i++)
			{
				const LC_BackPackEntry& pItemData = pRuneInPack.m_aData[i];
				if(pItemData.GetTypeID() == activationID && pItemData.GetEquipStar()>=activationStarNeed)
				{	
					return true;
				}
			}
		}
		break;

	}

	return false;

}






































































