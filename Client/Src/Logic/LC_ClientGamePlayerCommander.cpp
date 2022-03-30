#include "ClientPCH.h"
#include "LC_ClientGamePlayerCommander.h"
#include "CL_RichTextParse.h"

#include "CF_EquipCommanderSlotMoldingSpirit.h"
#include "CF_EquipCommanderSlotMoldingSpiritSuit.h"


GameLogic::LC_ClientGamePlayerCommander::LC_ClientGamePlayerCommander()
: m_iEXP(-1)
, m_iLevel(0)
, m_cPlayerControllerName("")
, m_nMainControllerId(0)
, m_nInstanceId(0)
, m_iBornCharType(0)
, m_iCommanderMP(0)
, m_iCommanderMaxMP(0)
, m_iTaskHonorPoint(0)
, m_iMainTaskFinishCount(0)
, m_iLotteryCount(0)
, m_iMaxRuneStarLevel(0)
, m_iMaxRuneLevel(0)
, m_iEquipSuitLevel(0)
, m_iCharCreateTime(0)
, m_iCommanderTitleLevel(0)
, m_iSpecialTraningLevel(0)
, m_nCommanderTitleFrameID(1)
{
	Reuse();
}

GameLogic::LC_ClientGamePlayerCommander::~LC_ClientGamePlayerCommander()
{
	Reuse();
}

void GameLogic::LC_ClientGamePlayerCommander::Reset()
{
	m_iEXP = -1;
	m_iLevel = 0;
	m_cPlayerControllerName = "";
	m_nInstanceId = 0;
	m_iCommanderMP = 0;
	m_iCommanderMaxMP = 0;
	m_iTaskHonorPoint = 0;
	m_iMainTaskFinishCount = 0;	
	m_iLotteryCount = 0;
	m_iMaxRuneStarLevel = 0;
	m_iMaxRuneLevel = 0;
	m_iTotalCombateScore = 0;
	m_iChainCombateScore = 0;
	m_iCharCreateTime = 0;
	m_iCommanderEquipScore = 0;
}

void GameLogic::LC_ClientGamePlayerCommander::Reuse()
{
	Reset();

	m_kAttributeMap.Clear();
	m_kChainAttributeMap.Clear();
	m_kAttributePlus.Clear();
	m_iSpecialTraningLevel = 0;
	m_vEquipList.clear();
	m_mEquipReinforceLevels.clear();
	m_mEquipRefineLevels.clear();
	m_mEquipModingSpiritLevels.clear();
	m_mSubSuitAttrPercent.clear();
	m_mSubSuitAttrValue.clear();
	m_mSubAttrValue.clear();
	LC_DirtyFlagAble::Reset();
}

void GameLogic::LC_ClientGamePlayerCommander::InitEquipList()
{
	m_vEquipList.resize(EquipPack_Count);
	m_vEquipList[CommandPack-1].resize(GetGlobalSetting.CommandPackSize);
	m_vEquipList[WingPack-1].resize(GetGlobalSetting.WingPackSize);
	m_vEquipList[MountPack-1].resize(GetGlobalSetting.MountPackSize);
	m_vEquipList[PetPack-1].resize(GetGlobalSetting.PetPackSize);
}

void GameLogic::LC_ClientGamePlayerCommander::ParseProtocol(const MG_SyncPlayerControllerProperty & playerProperty)
{
	m_iEXP = playerProperty.m_iEXP;
	m_iLevel = playerProperty.m_iLevel;
	//m_iTotalCombateScore = playerProperty.m_iTotalCombatScore;
	m_cPlayerControllerName = STDSTR2TPSTR(playerProperty.m_cPlayerControllerName);

	m_nMainControllerId = playerProperty.m_nMainControllerId;
	m_nInstanceId = playerProperty.m_nInstanceId;
	m_iBornCharType = playerProperty.m_iBornCharType;
	m_iCommanderMP = playerProperty.m_iCommanderMP;
	m_iCommanderMaxMP = playerProperty.m_iCommanderMaxMP;
	m_iCommanderTitleLevel = playerProperty.m_nCommanderTitleID;
	m_iCharCreateTime = playerProperty.m_CharCreateTime;

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "ParseProtocol:SetCommanderTitleID:%d",playerProperty.m_nCommanderTitleID);
	m_nCommanderTitleFrameID = playerProperty.m_nCommanderTitleFrameID;
	UpdateDirtyFlag();
}

void GameLogic::LC_ClientGamePlayerCommander::UpdatePropertys(const MG_SyncPlayerControllerProperty & playerProperty)
{
	m_iTaskHonorPoint = playerProperty.m_iTaskHonorPoint;
	//m_iLotteryCount = playerProperty.m_nLotteryCount;
	m_iMaxRuneStarLevel = playerProperty.m_nMaxRuneStarLevel;
	m_iMaxRuneLevel = playerProperty.m_nMaxRuneLevel;
	m_iBornCharType = playerProperty.m_iBornCharType;
	m_iCommanderTitleLevel = playerProperty.m_nCommanderTitleID;
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "UpdatePropertys:SetCommanderTitleID:%d",playerProperty.m_nCommanderTitleID);
	m_nCommanderTitleFrameID = playerProperty.m_nCommanderTitleFrameID;
	UpdateDirtyFlag();
}

bool GameLogic::LC_ClientGamePlayerCommander::CheckEXP(int64_t val)
{
	return m_iEXP != val;
}

bool GameLogic::LC_ClientGamePlayerCommander::CheckLevel(uint16_t val)
{
	return m_iLevel != val;
}

bool GameLogic::LC_ClientGamePlayerCommander::CheckTotalCombateScore(int64_t val)
{
	return m_iTotalCombateScore != val;
}

bool GameLogic::LC_ClientGamePlayerCommander::isInited()
{
	if (m_iEXP == -1)
		return false;
	return true;
}

void GameLogic::LC_ClientGamePlayerCommander::SetPlayerCommanderName(const StringType& InName)
{
	m_cPlayerControllerName = InName;
	//GUISys::getRichTextParse->FilterForbiddenText(InName, m_cPlayerControllerName, TT_TextFilter::TFT_CHAR_NAME);
}

int GameLogic::LC_ClientGamePlayerCommander::GetEquipReinforceLevel(int iEquipType)
{
	mem::map<int, int>::const_iterator iter = m_mEquipReinforceLevels.find(iEquipType);
	if(iter == m_mEquipReinforceLevels.end())
		return 0;

	return iter->second;
}

int GameLogic::LC_ClientGamePlayerCommander::GetEquipRefineLevel(int iEquipType)
{
	mem::map<int, int>::const_iterator iter = m_mEquipRefineLevels.find(iEquipType);
	if(iter == m_mEquipRefineLevels.end())
		return 0;

	return iter->second;
}

int GameLogic::LC_ClientGamePlayerCommander::GetEquipModingSpiritLevel(int iPackType, int iEquipType)
{
	EquipModingSpiritLvMap::const_iterator itr = m_mEquipModingSpiritLevels.find(iPackType);
	if(itr == m_mEquipModingSpiritLevels.end())
		return 0;

	const mem::map<int, int>& lvMap = itr->second;
	mem::map<int, int>::const_iterator iter = lvMap.find(iEquipType);
	if(iter == lvMap.end())
		return 0;

	return iter->second;
}

void GameLogic::LC_ClientGamePlayerCommander::GetSubMoldingSpiritSuitInfo(mem::map<int, int>& suitInfo, int iSubType)
{
	int iPackType = _getPackTypeBySubType(iSubType);
	mem::map<int, int> suitIDs;
	const mem::map<int, int>& LvMap = m_mEquipModingSpiritLevels[iPackType];

	for(mem::map<int, int>::const_iterator it=LvMap.begin(); it!=LvMap.end(); ++it)
	{
		int slotIndex = it->first;
		int MoldingSpiritLevel = it->second;

		CF_EquipCommanderSlotMoldingSpirit::DataEntryMapExternIterator kIter = CF_EquipCommanderSlotMoldingSpirit::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			const CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = kIter.PeekNextValuePtr();
			if(pkData->iType == iSubType
				&& pkData->_iEquipType == slotIndex
				&& pkData->_iLevel == MoldingSpiritLevel)
			{
				int suitID = pkData->_iSuitID;
				CF_EquipCommanderSlotMoldingSpiritSuit::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderSlotMoldingSpiritSuit>(suitID);
				if(NULL != pkEntry)
				{
					suitIDs[suitID]++;
				}

				break;
			}
		}
	}

	for(mem::map<int, int>::const_iterator it=suitIDs.begin(); it!=suitIDs.end(); ++it)
	{
		mem::map<int, int>::iterator findIt = suitInfo.find(it->first);
		if(findIt == suitInfo.end())
		{
			suitInfo.insert(make_pair(it->first, it->second));
		}
		else
		{
			findIt->second += it->second;
		}
		findIt = suitInfo.find(it->first);

		for(mem::map<int, int>::iterator kIt=suitInfo.begin(); kIt!=findIt; ++kIt)
		{
			kIt->second += 1;
		}
	}
}

void GameLogic::LC_ClientGamePlayerCommander::GenSubEquipSlotMoldingSpiritSuitAttr()
{
	m_mSubSuitAttrValue.clear();
	m_mSubAttrValue.clear();

	LC_ActorAttributeMap attrMap;
	
	int iSubTypes[] = {LC_SUB_TP_VEHICLE, LC_SUB_TP_DEVA_EYE, LC_SUB_TP_MUHUN};
	for(int i=0; i<3; i++)
	{
		int iSubType = iSubTypes[i];

		mem::map<int, int> suitInfo;
		GetSubMoldingSpiritSuitInfo(suitInfo, iSubType);

		LC_ActorAttributeMap attrValueMap;//加值的属性
		mem::map<int, int>& subMap = m_mSubSuitAttrPercent[iSubType];

		LC_Helper::GetSubSuitAttrInfoFromItemEffectFunc(&suitInfo, &attrValueMap, subMap, iSubType);

		attrMap.Clear();

		mem::map<int, int>& LvMap = m_mEquipModingSpiritLevels[iSubType];
		for(int j=0; j<=3; j++)
		{
			mem::map<int, int>::const_iterator iter = LvMap.find(j);
			if(iter != LvMap.end())
			{
				int level = iter->second;
				if(level > 0)
				{
					CF_EquipCommanderSlotMoldingSpirit::DataEntryMapExternIterator kIter = CF_EquipCommanderSlotMoldingSpirit::GetSingletonPtr()->GetIterator();
					for(; kIter.HasMoreElements(); kIter.MoveNext())
					{
						const CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = kIter.PeekNextValuePtr();
						if(pkData->iType == iSubType 
							&& pkData->_iEquipType == iter->first 
							&& pkData->_iLevel == level)
						{
							LC_Helper::AttributeMapByAttrFunc(&attrMap, pkData->_lAttrFuncID);
							break;
						}
					}
				}
			}
		}

		attrMap.RecomputeAll();
		
		mem::map<int, int>& subMapValue = m_mSubSuitAttrValue[iSubType];
		mem::map<int, int>& subAttribMap = m_mSubAttrValue[iSubType];
		for(int i=0; i<ATT_MAX_COUNT; i++)
		{
			int iValue = attrValueMap.GetAttribute(i);
			if(iValue > 0)
				subMapValue[i] += iValue;

			iValue = attrMap.GetAttribute(i);
			if(iValue > 0)
				subAttribMap[i] += iValue;
		}
	}
}

void LC_ClientGamePlayerCommander::UpdateEquipList(const MG_SC_SynCommandEquip_Ret& msg)
{
	std::vector<std::vector<Equip_Base> >::const_iterator iter = msg.m_EquipVec.begin();
	for( size_t i=0; i<msg.m_EquipVec.size();i++)
	{
		int iPackType = i + 1;
		const std::vector<Equip_Base>& list = msg.m_EquipVec[i];
		if(list.size() > 0)
		{
			mem::map<int, int>& LvMap = m_mEquipModingSpiritLevels[iPackType];

			EquipVec& equipList = m_vEquipList[i];
			for(size_t j=0; j<list.size(); j++)
			{
				const Equip_Base& info = list[j];
				equipList[j].GetFromProtocol(info.m_Data);

				if(iPackType == CommandPack)
				{
					m_mEquipReinforceLevels[j] = info.m_IntensifyLevel;
					m_mEquipRefineLevels[j] = info.m_RefineLevel;
				}

				LvMap[j] = info.m_SuiteId;
			}
		}
	}

	GenSubEquipSlotMoldingSpiritSuitAttr();
}

void LC_ClientGamePlayerCommander::UpdateCommanderEquip(const MG_SC_EquipCommand_Ret& msg)
{
	EquipVec& equipList = m_vEquipList[CommandPack-1];
	LC_BackPackEntry& entry = equipList[msg.m_EquipIndex];
	entry.GetFromProtocol(msg.m_Data);
}

void LC_ClientGamePlayerCommander::UpdateCommanderEquip(const MG_SC_IntensifyCommandEquip_Ret& msg)
{
	m_mEquipReinforceLevels[msg.m_EquipIndex] = msg.m_Level;
}

void LC_ClientGamePlayerCommander::UpdateCommanderEquip(const MG_SC_RefineCommandEquip_Ret& msg)
{
	m_mEquipRefineLevels[msg.m_EquipIndex] = msg.m_Level;
}

void LC_ClientGamePlayerCommander::UpdateCommanderEquip(const MG_SC_SoulCommandEquip_Ret& msg)
{
	mem::map<int, int>& LvMap = m_mEquipModingSpiritLevels[CommandPack];
	LvMap[msg.m_EquipIndex] = msg.m_SuiteId;
}

void LC_ClientGamePlayerCommander::UpdateSubEquip(const MG_Rlt_EquipSubItem& msg)
{
	int iPackType = _getPackTypeBySubType(msg.m_nType);
	EquipVec& equipList = m_vEquipList[iPackType-1];
	LC_BackPackEntry& entry = equipList[msg.m_nDestSlotIndex];
	entry.GetFromProtocol(msg.m_nDestEquipData);
}

void LC_ClientGamePlayerCommander::UpdateSubEquip(const MG_Rlt_UnEquipSubItem& msg)
{
	int iPackType = _getPackTypeBySubType(msg.m_nType);
	EquipVec& equipList = m_vEquipList[iPackType-1];
	LC_BackPackEntry& entry = equipList[msg.m_nSrcSlotIndex];
	entry.Reset();
}

void LC_ClientGamePlayerCommander::UpdateSubEquip(const MG_RLT_SubEquipSlotMoldingSpirit& msg)
{
	int iPackType = _getPackTypeBySubType(msg.m_nSubType);
	mem::map<int, int>& LvMap = m_mEquipModingSpiritLevels[iPackType];
	LvMap[msg.m_nSlotIndex] = msg.m_nSlotLevel;
}

LC_BackPackEntry* LC_ClientGamePlayerCommander::GetEquipEntry(int iPackType, int iIndex)
{
	EquipVec& equipList = m_vEquipList[iPackType-1];
	return &equipList[iIndex];
}

bool LC_ClientGamePlayerCommander::IsExistSubEquip(int iItemID)
{
	if(iItemID == 0)
		return false;

	for(size_t i=MountPack; i<=PetPack; ++i)
	{
		EquipVec& equipList = m_vEquipList[i-1];
		EquipVec::const_iterator iter = equipList.begin();
		for( ; iter!=equipList.end(); ++iter)
		{
			const LC_BackPackEntry& pkEntry = *iter;
			if(pkEntry.GetValid() && pkEntry.GetTypeID() == iItemID)
			{
				return true;
			}
		}
	}

	return false;
}

int LC_ClientGamePlayerCommander::_getPackTypeBySubType(int iSubType)
{
	switch(iSubType)
	{
	case LC_SUB_TP_VEHICLE:
		return MountPack;
		break;
	case LC_SUB_TP_DEVA_EYE:
		return PetPack;
		break;
	case LC_SUB_TP_MUHUN:
		return WingPack;
		break;
	default:
		T_ASSERT_CRITICAL_S(false);
		break;
	}

	return 0;
}

int LC_ClientGamePlayerCommander::_getSubTypeByPackType(int iPackType)
{
	switch(iPackType)
	{
	case MountPack:
		return LC_SUB_TP_VEHICLE;
		break;
	case PetPack:
		return LC_SUB_TP_DEVA_EYE;
		break;
	case WingPack:
		return LC_SUB_TP_MUHUN;
		break;
	default:
		T_ASSERT_CRITICAL_S(false);
		break;
	}

	return 0;
}
