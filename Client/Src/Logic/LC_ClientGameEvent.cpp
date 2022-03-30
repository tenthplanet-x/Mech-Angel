#include "ClientPCH.h"
#include "LC_Define.h"
#include "LC_ClientGameEvent.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_PackStruct.h"
#include "CF_ItemPropertyFunc.h"
#include "CF_Subordinate.h"
#include "CF_ItemGen.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_ExportRobotBuddyTemplateInfo.h"

LC_ClientGameEvent::LC_ClientGameEvent()
:m_iClientEventType(GameLogic::CGET_NONE)
,m_pUserData(NULL)
{
	GameLogic::LC_GameEventBase::SetEventType(GameLogic::GLET_CLIENT_EVENT);
}

LC_ClientGameEvent::~LC_ClientGameEvent()
{

}

void LC_ClientGameEvent::SetEventType(GameLogic::GameEventType eEventType)
{
	T_ASSERT_CRITICAL_S(false);	//在本对象创建时就确定类型了。
}

int LC_ClientGameEvent::GetClientEventType()
{
	return m_iClientEventType;
}

void LC_ClientGameEvent::SetClientEventType(int iClientEventType)
{
	m_iClientEventType = iClientEventType;
}

void LC_ClientGameEvent::Reset()
{
	GameLogic::LC_GameEventBase::Reset();
	m_pUserData = NULL;
}

void LC_ClientGameEvent::SetUserData(LC_ClientGameEventUserDataBase* pData)
{
	m_pUserData = pData;
}

LC_ClientGameEventUserDataBase* LC_ClientGameEvent::GetUserData()
{
	return m_pUserData;
}

LC_ClientGameEventUserData_RaidSettle::LC_ClientGameEventUserData_RaidSettle(const Protocol::MG_Notify_RaidSettle& kMsg,uint16_t _storyID,uint32_t _mapResID)
{
	endFlag =								kMsg.m_isEnd;
	result =								kMsg.m_result;
	level =									kMsg.m_level;
	exp =									kMsg.m_exp;
	sp =									    kMsg.m_sp;
	money =							    kMsg.m_money;
	itemlen =								kMsg.m_itemlen;
	T_ASSERT_CRITICAL_S(itemlen <= MAX_ITEM_LEN);
	for (int i = 0; i < itemlen; ++i)
		item[i] = kMsg.m_item[i];
	for(int i = 0; i < itemlen; ++i)
		itemcount[i] = kMsg.m_itemCount[i];
	monsters =							 kMsg.m_monsters;
	deads =								 kMsg.m_deads;
	passtime =							 kMsg.m_passtime;
	controllerLevel =					 kMsg.m_controllerLevel;
	buddyCount =						 kMsg.m_buddyCount;
	hasBuddyDead =					 kMsg.m_hasBuddyDead;
	bFirstPass =                     kMsg.m_bFirstPass;
	preEnd =								 kMsg.m_preEnd;
	storyID =								 _storyID;
	mapResID =							 _mapResID;
	for (int i = 0; i < MAX_ACTIVE_BUDDY_NUM; ++i)
		buddyId[i] = kMsg.m_buddyId[i];
	for(int i = 0; i < MAX_ACTIVE_BUDDY_NUM; ++i)
		buddyExp[i] = kMsg.m_buddyExp[i];
	for(int i = 0; i < MAX_ACTIVE_BUDDY_NUM; ++i)
		buddyLevel[i] = kMsg.m_buddyCurrentLevel[i];
	for(int i = 0; i < MAX_ACTIVE_BUDDY_NUM; ++i)
		buddyOldLevel[i] = kMsg.m_buddyOldLevel[i];

}
long LC_ClientGameEventUserData_RaidSettle::GetItem(int iIndex)
{
	return item[iIndex];
}

int LC_ClientGameEventUserData_RaidSettle::GetItemCount(int iIndex)
{
	return itemcount[iIndex];
}

int LC_ClientGameEventUserData_RaidSettle::GetBuddyId(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
	{
		return 0;
	}
	return buddyId[iIndex];
}

int LC_ClientGameEventUserData_RaidSettle::GetBuddyExp(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
	{
		return 0;
	}
	return buddyExp[iIndex];
}

int LC_ClientGameEventUserData_RaidSettle::GetBuddyNewLevel(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
	{
		return 0;
	}
	return buddyLevel[iIndex];
}

int LC_ClientGameEventUserData_RaidSettle::GetBuddyOldLevel(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
	{
		return 0;
	}
	return buddyOldLevel[iIndex];
}

LC_ClientGameEventUserData_String::LC_ClientGameEventUserData_String(const StringType &sValue)
{
	kString = sValue;
}

const StringType& LC_ClientGameEventUserData_String::GetString()
{
	return kString;
}

LC_ClientRankUserInfo::LC_ClientRankUserInfo()
{
	Reset();
}

LC_ClientRankUserInfo::LC_ClientRankUserInfo(const PS_User_Info& kUseInfo)
{
	Reset();

	*this = kUseInfo;
}

LC_ClientRankUserInfo& LC_ClientRankUserInfo::operator= (const PS_User_Info& other)
{
	Reset();
	
	m_lUserID = other.m_CharID;
	m_kName = STDSTR2TPSTR(other.m_sCharName);
	m_iLevel = other.m_nLevel;
	m_GuildName = STDSTR2TPSTR(other.m_sGuildName);
	m_bornCharType = other.m_nBornType;
	m_CommonderScore = other.m_nControllerCombatScore;
	m_CommanderTitleLevel=other.m_nCommanderTitleID;
	m_CommanderHeadFrame = other.m_nCommanderTitleFrameID;

	const std::map<int32_t,int64_t>& kAttrMap = other.m_nControllerAttrMap;
	for(size_t i=0; i<ATT_MAX_COUNT; ++i)
	{
		int64_t* iAttrType = GetAttribute(i);
		if(iAttrType != NULL)
		{
			std::map<int32_t,int64_t>::const_iterator iter = kAttrMap.find(i);
			if(iter != kAttrMap.end())
			{
				*iAttrType = iter->second;
			}
		}
	}

	const std::map<int32_t,int64_t>& kChainAttrMap = other.m_nControllerCoreAttrMap;
	for(size_t i=0; i<ATT_MAX_COUNT; ++i)
	{
		int64_t* iAttrType = GetChainAttribute(i);
		if(iAttrType != NULL)
		{
			std::map<int32_t,int64_t>::const_iterator iter = kChainAttrMap.find(i);
			if(iter != kChainAttrMap.end())
			{
				*iAttrType = iter->second;
			}
			else
			{
				*iAttrType = 0;
			}
		}
	}

	m_mControllerEquipSlotMap.clear();
	std::map<int32_t,PS_ControllerEquipSlotItemInfo>::const_iterator equipItr = other.m_nControllerEquipSlotItemInfo.begin();
	for(; equipItr!=other.m_nControllerEquipSlotItemInfo.end(); ++equipItr)
	{
		m_mControllerEquipSlotMap[equipItr->first] = equipItr->second;
	}

	m_kActiveBuddyVec.clear();
	const std::vector<int32_t>& kActiveBuddys = other.m_nActivateBuddyID;
	for(size_t i=0; i<kActiveBuddys.size(); ++i)
	{
		m_kActiveBuddyVec.push_back(kActiveBuddys[i]);
	}

	m_buddyInfos.clear();
	const std::map<int32_t,PS_Buddy_Info_BuddyCS>& buddyInfoMap = other.m_BuddyInfo;
	const std::map<int32_t, uint64_t>& buddyScores = other.m_nBuddyCombatScore;
	const std::map<int,int>& RankIdxMap = other.m_nBuddyRankIndex;

	int index = 0;
	std::map<int32_t,PS_Buddy_Info_BuddyCS>::const_iterator iter = buddyInfoMap.begin();
	for( ; iter!=buddyInfoMap.end(); ++iter)
	{
		const PS_Buddy_Info_BuddyCS& kData = iter->second;

		BuddyInfo buddyInfo;
		buddyInfo.iCid = kData.m_nBuddyID;
		buddyInfo.iLevel = kData.m_nBuddyLevel;
		buddyInfo.iStarLevel = kData.m_nBuddyStarLevel;
		buddyInfo.iAllEquipReinforceLevel = kData.m_nAllEquipSlotReinforceLevel;
		buddyInfo.iActivateAllSkill = kData.m_bActivateAllSkill;

		std::map<int,int>::const_iterator iter2=RankIdxMap.find(buddyInfo.iCid);
		if (iter2 != RankIdxMap.end())
		{
			buddyInfo.iRank=iter2->second;
		}
		else
		{
			buddyInfo.iRank=-1;
		}
		std::map<int32_t, uint64_t>::const_iterator scoreIter = buddyScores.find(buddyInfo.iCid);
		if(scoreIter != buddyScores.end())
		{
			buddyInfo.iCombatScore = scoreIter->second;
		}

		const std::map<int32_t,PS_RuneItemInfo_BuddyCS>& kRuneStarMap = kData.m_nBuddyRuneInfo;
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator it = kRuneStarMap.begin();
		for(; it!=kRuneStarMap.end(); ++it)
		{
			PS_RuneItemInfo_BuddyCS& kInfo = buddyInfo.iRuneInfo[it->first];
			kInfo.m_nRuneItemID = it->second.m_nRuneItemID;
			kInfo.m_nRuneStarLevel = it->second.m_nRuneStarLevel;
			kInfo.m_nRuneLevel = it->second.m_nRuneLevel;
		}
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itor = kData.m_nEquipSlotInfo.begin();
		for(; itor!=kData.m_nEquipSlotInfo.end(); ++itor)
		{
			PS_EquipItemInfo_BuddyCS& equipInfo = buddyInfo.iEquipInfo[itor->first];
			equipInfo.m_nEquipItemID = itor->second.m_nEquipItemID;
			equipInfo.m_nEquipSlotStarLevel = itor->second.m_nEquipSlotStarLevel;
			equipInfo.m_nEquipSlotLevel = itor->second.m_nEquipSlotLevel;
		}
		
		m_buddyInfos.push_back(buddyInfo);

		index++;
	}
	
	m_kSubInfoMap.clear();
	const std::vector<PS_User_Sub_Info>& kSubInfo = other.m_nSubInfo;
	std::vector<PS_User_Sub_Info>::const_iterator subIter = kSubInfo.begin();
	for( ; subIter!=kSubInfo.end();++subIter)
	{
		mem::set<int>& kIds = m_kSubInfoMap[subIter->m_nSubType];
		kIds.clear();

		for(size_t i=0; i<subIter->m_nSubTransform.size(); ++i)
		{
			kIds.insert(subIter->m_nSubTransform[i]);
		}
	}

	m_iActivatePetID = other.m_nActivatePetID;
	m_kPetInfoMap.clear();
	const std::vector<PS_User_Pet_Info>& kPetInfo = other.m_nPetInfo;
	std::vector<PS_User_Pet_Info>::const_iterator petIter = kPetInfo.begin();
	for( ; petIter!=kPetInfo.end();++petIter)
	{
		m_kPetInfoMap[petIter->m_nPetID] = petIter->m_nPetStarLevel;
	}

	return *this;
}

LC_ClientRankUserInfo& LC_ClientRankUserInfo::operator= (const LC_ClientRankUserInfo& other)
{
	if (this == &other)
		return *this;

	m_lUserID						= other.m_lUserID;
	m_ServerId						= other.m_ServerId;
	m_iLevel						= other.m_iLevel;
	m_kName							= other.m_kName;
	m_Value							= other.m_Value;
	m_GuildID						= other.m_GuildID;
	m_bIsRobot						= other.m_bIsRobot;
	m_CommonderScore				= other.m_CommonderScore;
	m_ActiveScore					= other.m_ActiveScore;
	m_Life							= other.m_Life;
	m_Mp							= other.m_Mp;
	m_Atk							= other.m_Atk;
	m_Cirrate						= other.m_Cirrate;
	m_Cirdam						= other.m_Cirdam;
	m_Def							= other.m_Def;
	m_Dograte						= other.m_Dograte;
	m_Movespeed						= other.m_Movespeed;
	m_CirDef						= other.m_CirDef;
	m_DmgRate						= other.m_DmgRate;
	m_DmgDeRate						= other.m_DmgDeRate;
	m_PkValue						= other.m_PkValue;
	m_CirDamDerate					= other.m_CirDamDerate;
	m_AtkRate						= other.m_AtkRate;
	m_ExpRate						= other.m_ExpRate;
	m_ChainLife						= other.m_ChainLife;
	m_ChainAtk						= other.m_ChainAtk;
	m_ChainAtkRate					= other.m_ChainAtkRate;
	m_ChainCirrate					= other.m_ChainCirrate;
	m_ChainCirdam					= other.m_ChainCirdam;
	m_ChainDef						= other.m_ChainDef;
	m_ChainDograte					= other.m_ChainDograte;
	m_ChainCirDef					= other.m_ChainCirDef;
	m_ChainDmgRate					= other.m_ChainDmgRate;
	m_ChainDmgDeRate				= other.m_ChainDmgDeRate;
	m_ChainCirDamDerate				= other.m_ChainCirDamDerate;
	m_GuildName						= other.m_GuildName;
	m_SubInfos						= other.m_SubInfos;
	m_buddyInfos					= other.m_buddyInfos;
	m_MaxCombatScorePetID			= other.m_MaxCombatScorePetID;
	m_MaxCombatScorePetIDStarLevel	= other.m_MaxCombatScorePetIDStarLevel;
	m_platforminfo					= other.m_platforminfo;
	m_CurrentBuddyID				= other.m_CurrentBuddyID;
	m_CurrentBuddyTransform			= other.m_CurrentBuddyTransform;
	m_iActivatePetID				= other.m_iActivatePetID;
	m_bornCharType					= other.m_bornCharType;
	m_CommanderHeadFrame			= other.m_CommanderHeadFrame;
	m_CommanderTitleLevel			= other.m_CommanderTitleLevel;
	m_kSubInfoMap					= other.m_kSubInfoMap;
	m_kPetInfoMap					= other.m_kPetInfoMap;
	m_kActiveBuddyVec				= other.m_kActiveBuddyVec;
	m_mControllerEquipSlotMap		= other.m_mControllerEquipSlotMap;

	return *this;
}

void LC_ClientRankUserInfo::SetUserInfo(const Protocol::PS_Rank_PlayerBaseData& kUserInfo)
{
	m_bIsRobot				= kUserInfo.m_Robot;
	m_CommanderTitleLevel	= kUserInfo.m_CommanderTitleID;
	m_CommanderHeadFrame	= kUserInfo.m_CommanderTitleFrameID;
	m_bornCharType			= kUserInfo.m_ProfilePictureID;
	m_iLevel				= kUserInfo.m_Level;
	m_lUserID				= kUserInfo.m_CharID;
	m_CommonderScore		= kUserInfo.m_CommanderScore;
	m_kName					= STDSTR2TPSTR(kUserInfo.m_CharName);
	m_GuildName				= STDSTR2TPSTR(kUserInfo.m_nGuildName);

	//机器人的阵容数据 只需填一次
	if (m_bIsRobot && m_SchemeInfo.empty())
	{
		//机器人的UserId 小于 10000000
		SetRobotUserInfo((int32_t)m_lUserID);
	}
}

void LC_ClientRankUserInfo::SetUserInfo(const Protocol::PS_Rank_BuddyData& pkBuddyData)
{
	m_CurrentBuddyID			= pkBuddyData.m_CurrentBuddyID;
	m_CurrentBuddyTransform		= pkBuddyData.m_CurrentBuddyTransformID;
	m_ActiveScore				= pkBuddyData.m_ActiveBuddyBattle;

	m_buddyInfos.clear();
	for (int i = 0; i < pkBuddyData.m_BuddyInfo.size(); ++i)
	{
		PS_Buddy_Info_BuddyCS pkData = pkBuddyData.m_BuddyInfo[i];

		BuddyInfo info;
		info.iCid = pkData.m_nBuddyID;
		info.iLevel =  pkData.m_nBuddyLevel;
		info.iStarLevel = pkData.m_nBuddyStarLevel;
		info.iAllEquipReinforceLevel = pkData.m_nAllEquipSlotReinforceLevel;
		info.iActivateAllSkill = pkData.m_bActivateAllSkill;
		info.iCombatScore = pkData.m_nBuddyCombatScore;
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator it = pkData.m_nBuddyRuneInfo.begin();
		for(; it!=pkData.m_nBuddyRuneInfo.end(); ++it)
		{
			PS_RuneItemInfo_BuddyCS& kRuneInfo = info.iRuneInfo[it->first];
			kRuneInfo.m_nRuneItemID = it->second.m_nRuneItemID;
			kRuneInfo.m_nRuneStarLevel = it->second.m_nRuneStarLevel;
			kRuneInfo.m_nRuneLevel = it->second.m_nRuneLevel;
		}
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itor = pkData.m_nEquipSlotInfo.begin();
		for(; itor!=pkData.m_nEquipSlotInfo.end(); ++itor)
		{
			PS_EquipItemInfo_BuddyCS& kEquipInfo = info.iEquipInfo[itor->first];
			kEquipInfo.m_nEquipItemID = itor->second.m_nEquipItemID;
			kEquipInfo.m_nEquipSlotStarLevel = itor->second.m_nEquipSlotStarLevel;
			kEquipInfo.m_nEquipSlotLevel = itor->second.m_nEquipSlotLevel;
		}

		m_buddyInfos.push_back(info);
	}
}

void LC_ClientRankUserInfo::SetUserInfo(const Protocol::PS_AllSchemeBuddyData& pkSchemeBuddyData)
{
	m_SchemeInfo.clear();
	for (int i = 0; i < pkSchemeBuddyData.m_vSchemeData.size(); i++)
	{
		const PS_SchemeBuddyData& SchemeBuddyData = pkSchemeBuddyData.m_vSchemeData[i];
		BuddyInfoVec infoVec;
		for (int j = 0; j < SchemeBuddyData.m_nBuddyData.size(); j++)
		{
			const PS_Buddy_Info_BuddyCS& pkData = pkSchemeBuddyData.m_vSchemeData[i].m_nBuddyData[j];

			BuddyInfo info;
			info.iCid = pkData.m_nBuddyID;
			info.iLevel =  pkData.m_nBuddyLevel;
			info.iStarLevel = pkData.m_nBuddyStarLevel;
			info.iAllEquipReinforceLevel = pkData.m_nAllEquipSlotReinforceLevel;
			info.iActivateAllSkill = pkData.m_bActivateAllSkill;
			info.iCombatScore = pkData.m_nBuddyCombatScore;
			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator it = pkData.m_nBuddyRuneInfo.begin();
			for(; it!=pkData.m_nBuddyRuneInfo.end(); ++it)
			{
				PS_RuneItemInfo_BuddyCS& kRuneInfo = info.iRuneInfo[it->first];
				kRuneInfo.m_nRuneItemID = it->second.m_nRuneItemID;
				kRuneInfo.m_nRuneStarLevel = it->second.m_nRuneStarLevel;
				kRuneInfo.m_nRuneLevel = it->second.m_nRuneLevel;
			}
			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itor = pkData.m_nEquipSlotInfo.begin();
			for(; itor!=pkData.m_nEquipSlotInfo.end(); ++itor)
			{
				PS_EquipItemInfo_BuddyCS& kEquipInfo = info.iEquipInfo[itor->first];
				kEquipInfo.m_nEquipItemID = itor->second.m_nEquipItemID;
				kEquipInfo.m_nEquipSlotStarLevel = itor->second.m_nEquipSlotStarLevel;
				kEquipInfo.m_nEquipSlotLevel = itor->second.m_nEquipSlotLevel;
			}
			infoVec.push_back(info);
		}
		m_SchemeInfo[SchemeBuddyData.m_nSchemeGroupID] = infoVec;
	}
}

void LC_ClientRankUserInfo::SetUserInfo(const Protocol::PS_Rank_PetData& pkPetData)
{
	m_MaxCombatScorePetID = pkPetData.m_MaxCombatScorePetID;
	m_MaxCombatScorePetIDStarLevel = pkPetData.m_MaxCombatScorePetIDStarLevel;
}

void LC_ClientRankUserInfo::SetUserInfo(const Protocol::PS_Rank_SubData& pkSubData, int iRankType)
{
	SubDetail info;
	info.m_nSubType = pkSubData.m_nSubType;
	info.m_nSubStage = pkSubData.m_nSubStage;
	info.m_nSubTransform = pkSubData.m_nSubTransform;
	info.m_nSubCombatScore = pkSubData.m_nSubCombatScore;
	std::map<int32_t,PS_SubEquipSlotItemInfo>::const_iterator it = pkSubData.m_SubEquipInfo.begin();
	for(; it!=pkSubData.m_SubEquipInfo.end(); ++it)
	{
		PS_SubEquipSlotItemInfo& kEquipInfo = info.m_SubEquipInfo[it->first];
		kEquipInfo.m_EquipItem = it->second.m_EquipItem;
		kEquipInfo.m_EquipLevel1 = it->second.m_EquipLevel1;
	}
	m_SubInfos[iRankType] = info;
}

void LC_ClientRankUserInfo::SetUserInfo(const Protocol::PS_RankTopNPlayerSingleInfo& pkUserData)
{
	PS_Rank_PlayerBaseData kUserInfo = pkUserData.m_PlayerData;
	m_bIsRobot				= kUserInfo.m_Robot;
	m_CommanderTitleLevel	= kUserInfo.m_CommanderTitleID;
	m_CommanderHeadFrame	= kUserInfo.m_CommanderTitleFrameID;
	m_bornCharType			= kUserInfo.m_ProfilePictureID;
	m_iLevel				= kUserInfo.m_Level;
	m_lUserID				= kUserInfo.m_CharID;
	m_CommonderScore		= kUserInfo.m_CommanderScore;
	m_kName					= STDSTR2TPSTR(kUserInfo.m_CharName);

	m_buddyInfos.clear();
	for (int i = 0; i < pkUserData.m_nBuddyData.size(); ++i)
	{
		PS_BuddyRankIndexSingleData pkInfo = pkUserData.m_nBuddyData[i];
		PS_Buddy_Info_BuddyCS pkData = pkInfo.m_BuddyInfo;

		BuddyInfo info;
		info.iCid = pkData.m_nBuddyID;
		info.iLevel =  pkData.m_nBuddyLevel;
		info.iStarLevel = pkData.m_nBuddyStarLevel;
		info.iAllEquipReinforceLevel = pkData.m_nAllEquipSlotReinforceLevel;
		info.iActivateAllSkill = pkData.m_bActivateAllSkill;
		info.iCombatScore = pkData.m_nBuddyCombatScore;
		info.iRank = pkInfo.m_BuddyIndex;
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator it = pkData.m_nBuddyRuneInfo.begin();
		for(; it!=pkData.m_nBuddyRuneInfo.end(); ++it)
		{
			PS_RuneItemInfo_BuddyCS& kRuneInfo = info.iRuneInfo[it->first];
			kRuneInfo.m_nRuneItemID = it->second.m_nRuneItemID;
			kRuneInfo.m_nRuneStarLevel = it->second.m_nRuneStarLevel;
			kRuneInfo.m_nRuneLevel = it->second.m_nRuneLevel;
		}
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itor = pkData.m_nEquipSlotInfo.begin();
		for(; itor!=pkData.m_nEquipSlotInfo.end(); ++itor)
		{
			PS_EquipItemInfo_BuddyCS& kEquipInfo = info.iEquipInfo[itor->first];
			kEquipInfo.m_nEquipItemID = itor->second.m_nEquipItemID;
			kEquipInfo.m_nEquipSlotStarLevel = itor->second.m_nEquipSlotStarLevel;
			kEquipInfo.m_nEquipSlotLevel = itor->second.m_nEquipSlotLevel;
		}

		m_buddyInfos.push_back(info);
	}
}

void LC_ClientRankUserInfo::SetUserInfo(const std::vector<KeyValue_Base>& pkBuddyRankIndex)
{
	for(int i = 0; i < pkBuddyRankIndex.size(); ++i)
	{
		KeyValue_Base info = pkBuddyRankIndex[i];
		m_buddyRankMap[info.key] = info.value;
	}
}

void LC_ClientRankUserInfo::SetRobotUserInfo(int32_t charID)
{
	CF_ExportSoloDuleRobotInfo::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(charID);
	if(NULL == pkDataEntry)
	{
		return;
	}

	mem::map<int32_t, int32_t> buddyIDToTemplateID;
	for(uint32_t i=0; i<pkDataEntry->_buddyBaseInfo.size(); ++i)
	{
		int32_t buddyID = pkDataEntry->_buddyBaseInfo[i].IID();
		int32_t templateID = pkDataEntry->_buddyBaseInfo[i].ParamA();
		buddyIDToTemplateID[buddyID] = templateID;
	}

	for(uint32_t i=0; i<pkDataEntry->_groupInfo.size(); ++i)
	{
		BuddyInfoVec infoVec;
		uint32_t iGroupID = pkDataEntry->_groupInfo[i].IID();

		int arr[3] = {pkDataEntry->_groupInfo[i].ParamA(), pkDataEntry->_groupInfo[i].ParamB(), pkDataEntry->_groupInfo[i].ParamC()};
		for(uint32_t j=0; j<sizeof(arr)/sizeof(arr[0]); ++j)
		{
			int buddyID = arr[j];
			if(buddyID != 0)
			{
				mem::map<int32_t, int32_t>::iterator findIt = buddyIDToTemplateID.find(buddyID);
				if(findIt == buddyIDToTemplateID.end())
				{
					continue;
				}

				CF_ExportRobotBuddyTemplateInfo::DataEntry* pkDate = SafeGetCSVFileDataEntryPtr<CF_ExportRobotBuddyTemplateInfo>(findIt->second);
				if(NULL==pkDate || pkDate->_buddyBaseInfo.empty())
				{
					continue;
				}

				//buddy基础数据
				BuddyInfo info;
				info.iCid = buddyID;
				info.iTransformId = buddyID*100+1;
				info.iLevel = pkDate->_buddyBaseInfo[0].IID();
				info.iStarLevel = pkDate->_buddyBaseInfo[0].ParamA();
				info.iActivateAllSkill = pkDate->_buddyBaseInfo[0].ParamB() > 0;
				info.iCombatScore = LC_Helper::GetRobotCombatScore(pkDate->_buddyBaseInfo[0].ParamC());

				//宝具数据
				const Utility::UT_SIMDataList& runeInfo = pkDate->_buddyRuneInfo;
				for(int i=0; i<runeInfo.size(); ++i)
				{
					int slotIndex = runeInfo[i].IID();
					PS_RuneItemInfo_BuddyCS& data = info.iRuneInfo[slotIndex];
					data.m_nRuneItemID = runeInfo[i].ParamA();
					data.m_nRuneLevel = runeInfo[i].ParamB();
					data.m_nRuneStarLevel = runeInfo[i].ParamC();
				}

				//装备数据
				int totalLevel = 0;
				const Utility::UT_SIMDataList& equipInfo = pkDate->_buddyEquipSlotInfo;
				for(int i=0; i<equipInfo.size(); ++i)
				{
					int slotIndex = equipInfo[i].IID();

					PS_EquipItemInfo_BuddyCS& data = info.iEquipInfo[slotIndex];
					data.m_nEquipItemID = equipInfo[i].ParamA();
					data.m_nEquipSlotLevel = equipInfo[i].ParamB();
					data.m_nEquipSlotStarLevel = equipInfo[i].ParamC();

					totalLevel += data.m_nEquipSlotStarLevel;
				}
				info.iAllEquipReinforceLevel = totalLevel;

				infoVec.push_back(info);
			}
		}
		m_SchemeInfo[iGroupID] = infoVec;
	}
}

void LC_ClientRankUserInfo::Reset()
{
	m_lUserID						= 0;
	m_ServerId						= 0;
	m_iLevel						= 0;
	m_kName							= "";
	m_Value							= 0;
	m_GuildID						= 0;
	m_bIsRobot						= false;
	m_CommonderScore				= 0;
	m_ActiveScore					= 0;

	m_Life							= 0;
	m_Mp							= 0;
	m_Atk							= 0;
	m_Cirrate						= 0;
	m_Cirdam						= 0;
	m_Def							= 0;
	m_Dograte						= 0;
	m_Movespeed						= 0;
	m_CirDef						= 0;
	m_DmgRate						= 0;
	m_DmgDeRate						= 0;
	m_PkValue						= 0;
	m_CirDamDerate					= 0;
	m_AtkRate						= 0;
	m_ExpRate						= 0;

	m_ChainLife						= 0;
	m_ChainAtk						= 0;
	m_ChainAtkRate					= 0;
	m_ChainCirrate					= 0;
	m_ChainCirdam					= 0;
	m_ChainDef						= 0;
	m_ChainDograte					= 0;
	m_ChainCirDef					= 0;
	m_ChainDmgRate					= 0;
	m_ChainDmgDeRate				= 0;
	m_ChainCirDamDerate				= 0;

	m_GuildName						= "";
	m_SubInfos.clear();
	m_buddyInfos.clear();
	m_MaxCombatScorePetID			= 0;
	m_MaxCombatScorePetIDStarLevel	= 0;
	m_platforminfo.clear();
	m_CurrentBuddyID				= 0;
	m_CurrentBuddyTransform			= 0;
	m_iActivatePetID				= 0;
	m_bornCharType					= 0;
	m_CommanderHeadFrame			= 0;
	m_CommanderTitleLevel			= 0;
	m_kSubInfoMap.clear();
	m_kPetInfoMap.clear();
	m_kActiveBuddyVec.clear();
	m_mControllerEquipSlotMap.clear();
}

unique_id_type LC_ClientRankUserInfo::GetUserID()
{
	return m_lUserID;
}
uint32_t LC_ClientRankUserInfo::GetServerID()
{
	return m_ServerId;
}

int LC_ClientRankUserInfo::GetLevel()
{
	return m_iLevel;
}
const StringType& LC_ClientRankUserInfo::GetUserName()
{
	return m_kName;
}

int64_t LC_ClientRankUserInfo::GetCommonderScore()
{
	return m_CommonderScore;
}

int64_t LC_ClientRankUserInfo::GetActiveScore()
{
	return m_ActiveScore;
}

int64_t* LC_ClientRankUserInfo::GetAttribute(int iAttributeType)
{
	GameLogic::AttributeType eType = (GameLogic::AttributeType)iAttributeType;

	switch (eType)
	{
	case GameLogic::ATT_MAX_HP:
		return &m_Life;
	case GameLogic::ATT_MAX_MP:
		return &m_Mp;
	case GameLogic::ATT_MAX_ATK:
		return &m_Atk;
	case GameLogic::ATT_CIR_RATE:
		return &m_Cirrate;
	case GameLogic::ATT_CIR_DEF:
		return &m_CirDef;
	case GameLogic::ATT_CIR_DAM:
		return &m_Cirdam;
	case GameLogic::ATT_DEF:
		return &m_Def;
	case GameLogic::ATT_DOG_RATE:
		return &m_Dograte;
	case GameLogic::ATT_MOVE_SPEED:
		return &m_Movespeed;
	case GameLogic::ATT_ATK_RATE:
		return &m_AtkRate;
	case GameLogic::ATT_EXP_RATE:
		return &m_ExpRate;
	case GameLogic::ATT_DMG_RATE:
		return &m_DmgRate;
	case GameLogic::ATT_DMG_DERATE:
		return &m_DmgDeRate;
	case GameLogic::ATT_CRI_DMG_DERATE:
		return &m_CirDamDerate;
	}

	return NULL;
}

int64_t* LC_ClientRankUserInfo::GetChainAttribute(int iAttributeType)
{
	GameLogic::AttributeType eType = (GameLogic::AttributeType)iAttributeType;

	switch (eType)
	{
	case GameLogic::ATT_MAX_HP:
		return &m_ChainLife;
	case GameLogic::ATT_MAX_ATK:
		return &m_ChainAtk;
	case GameLogic::ATT_CIR_RATE:
		return &m_ChainCirrate;
	case GameLogic::ATT_CIR_DEF:
		return &m_ChainCirDef;
	case GameLogic::ATT_CIR_DAM:
		return &m_ChainCirdam;
	case GameLogic::ATT_DEF:
		return &m_ChainDef;
	case GameLogic::ATT_DOG_RATE:
		return &m_ChainDograte;
	case GameLogic::ATT_ATK_RATE:
		return &m_ChainAtkRate;
	case GameLogic::ATT_DMG_RATE:
		return &m_ChainDmgRate;
	case GameLogic::ATT_DMG_DERATE:
		return &m_ChainDmgDeRate;
	case GameLogic::ATT_CRI_DMG_DERATE:
		return &m_ChainCirDamDerate;
	default:
		return NULL;
	}
}

int64_t LC_ClientRankUserInfo::GetAttributeValue(int iAttributeType)
{
	int64_t* pkAttrib = GetAttribute(iAttributeType);
	if(pkAttrib == NULL)
		return 0;

	return *pkAttrib;
}

int64_t LC_ClientRankUserInfo::GetChainAttributeValue(int iAttributeType)
{
	int64_t* pkAttrib = GetChainAttribute(iAttributeType);
	if(pkAttrib == NULL)
		return 0;

	return *pkAttrib;
}

bool LC_ClientRankUserInfo::GetIsRobot()
{
	return m_bIsRobot;
}

StringType LC_ClientRankUserInfo::GetGuildName()
{
	return m_GuildName;
}

int LC_ClientRankUserInfo::GetCurrentBuddyCID()
{
	return m_CurrentBuddyID;
}

int LC_ClientRankUserInfo::GetCurrentBuddyTransform()
{
	return m_CurrentBuddyTransform;
}

int64_t LC_ClientRankUserInfo::GetValue()
{
	return m_Value;
}

guild_id_type LC_ClientRankUserInfo::GetGuildID()
{
	return m_GuildID;
}

platform_info*	LC_ClientRankUserInfo::GetPlatformInfo(const StringType& sPlatformType)
{
	PlatformMap::iterator it = m_platforminfo.find(sPlatformType);
	if (it != m_platforminfo.end())
		return &it->second;
	return NULL;
}

void LC_ClientRankUserInfo::AddPlatformInfo(const StringType& sPlatType, uint8_t platInfo)
{
	platform_info kInfo;
	kInfo.decode_vip(sPlatType, platInfo); 
	m_platforminfo[sPlatType] = kInfo;
}

int LC_ClientRankUserInfo::GetPlatformVipLevel(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->vip_level;
	return 0;
}

bool LC_ClientRankUserInfo::IsPlatformYearVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->is_year_vip != 0;
	return false;
}

bool LC_ClientRankUserInfo::IsPlatformVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->is_vip != 0;
	return false;
}

bool LC_ClientRankUserInfo::IsPlatformHighVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->is_high_vip != 0;
	return false;
}

StringType LC_ClientRankUserInfo::GetBuddyInfo()
{
	StringStreamType sStream("");
	for(int i = 0; i < m_buddyInfos.size(); ++i)
	{
		BuddyInfo kInfo = m_buddyInfos[i];
		sStream << kInfo.iCid << "#" << kInfo.iLevel << "#" << kInfo.iStarLevel << "#" << kInfo.iTransformId << "#";
		sStream << kInfo.iCombatScore << "#" << kInfo.iAllEquipReinforceLevel << "#" << kInfo.iActivateAllSkill << "#" << kInfo.iRank << "#" ;
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator itor = kInfo.iEquipInfo.begin();
		for(; itor!=kInfo.iEquipInfo.end(); ++itor)
			sStream << itor->first << ":" << itor->second.m_nEquipItemID << ":" << itor->second.m_nEquipSlotStarLevel << ":" << itor->second.m_nEquipSlotLevel << ";";
		sStream << "#";
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator iter = kInfo.iRuneInfo.begin();
		for(; iter!=kInfo.iRuneInfo.end(); ++iter)
			sStream << iter->first << ":" << iter->second.m_nRuneItemID << ":" << iter->second.m_nRuneStarLevel << ":" << iter->second.m_nRuneLevel << ";" ;
		sStream << "#";
		sStream << "|";
	}

	return sStream.str();
}

StringType LC_ClientRankUserInfo::GetRanksInfo(uint32_t SchemeGroupID)
{
	StringStreamType sStream("");

	SchemeInfo::iterator iter	= m_SchemeInfo.find(SchemeGroupID);
	if(iter == m_SchemeInfo.end())
	{
		return sStream.str();
	}

	const  BuddyInfoVec& kInfoVec = iter->second;

	for(int i = 0; i < kInfoVec.size(); ++i)
	{
		BuddyInfo kInfo = kInfoVec[i];
		sStream << kInfo.iCid << "#" << kInfo.iLevel << "#" << kInfo.iStarLevel << "#" << kInfo.iTransformId << "#";
		sStream << kInfo.iCombatScore << "#" << kInfo.iAllEquipReinforceLevel << "#" << kInfo.iActivateAllSkill << "#" << kInfo.iRank << "#" ;
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator itor = kInfo.iEquipInfo.begin();
		for(; itor!=kInfo.iEquipInfo.end(); ++itor)
			sStream << itor->first << ":" << itor->second.m_nEquipItemID << ":" << itor->second.m_nEquipSlotStarLevel << ":" << itor->second.m_nEquipSlotLevel << ";";
		sStream << "#";
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator iter = kInfo.iRuneInfo.begin();
		for(; iter!=kInfo.iRuneInfo.end(); ++iter)
			sStream << iter->first << ":" << iter->second.m_nRuneItemID << ":" << iter->second.m_nRuneStarLevel << ":" << iter->second.m_nRuneLevel << ";" ;
		sStream << "#";
		sStream << "|";
	}

	return sStream.str();
}

StringType LC_ClientRankUserInfo::GetBuddyRankInfo()
{
	StringStreamType sStream("");
	mem::map<int, int>::iterator iter = m_buddyRankMap.begin();
	for(; iter != m_buddyRankMap.end(); ++iter)
	{
		sStream << iter->first << "|" << iter->second << "#" ;
	}
	return sStream.str();
}

int64_t LC_ClientRankUserInfo::GetAllSchemeScore()
{
	int64_t iScore = 0;
	SchemeInfo::const_iterator iter = m_SchemeInfo.begin();
	for( ; iter!=m_SchemeInfo.end(); ++iter)
	{
		const BuddyInfoVec& buddyVec = iter->second;
		BuddyInfoVec::const_iterator buddyItr = buddyVec.begin();
		for( ; buddyItr!=buddyVec.end(); ++buddyItr)
		{
			const BuddyInfo& buddyInfo = *buddyItr;
			iScore += buddyInfo.iCombatScore;
		}
	}

	return iScore;
}

bool LC_ClientRankUserInfo::IsSubTransformActive(int iSubType, int iId)
{
	mem::map<int, mem::set<int>>::iterator iter = m_kSubInfoMap.find(iSubType);
	if(iter == m_kSubInfoMap.end())
		return false;

	const mem::set<int>& kIds = iter->second;
	return kIds.find(iId) != kIds.end();
}

StringType LC_ClientRankUserInfo::GetPetInfo()
{
	StringStreamType sStream;
	mem::map<int, int>::const_iterator iter = m_kPetInfoMap.begin();
	for(; iter!=m_kPetInfoMap.end(); ++iter)
	{
		sStream << iter->first << '#' << iter->second << '|';
	}

	return sStream.str();
}

int32_t LC_ClientRankUserInfo::GetPetID()
{
	return m_MaxCombatScorePetID;
}

int32_t LC_ClientRankUserInfo::GetPetStarLevel()
{
	return m_MaxCombatScorePetIDStarLevel;
}

StringType LC_ClientRankUserInfo::GetSubInfo(int32_t subType)
{
	StringStreamType sStream("");

	SubInfoMap::iterator itor = m_SubInfos.find(subType);
	if (itor != m_SubInfos.end())
	{
		sStream << itor->second.m_nSubType << "#" << itor->second.m_nSubStage << "#" << itor->second.m_nSubTransform << "#" << itor->second.m_nSubCombatScore << "#";
		mem::map<int32_t,PS_SubEquipSlotItemInfo>::iterator iter = itor->second.m_SubEquipInfo.begin();
		for (; iter != itor->second.m_SubEquipInfo.end(); ++iter)
		{
			sStream << iter->first << ":" << iter->second.m_EquipItem << ":" << iter->second.m_EquipLevel1 << ";";
		}
	}

	return sStream.str();
}

int32_t LC_ClientRankUserInfo::GetBornCharType()
{
	return m_bornCharType;
}

int32_t LC_ClientRankUserInfo::GetCommanderHeadFrame()
{
	return m_CommanderHeadFrame;
}

int LC_ClientRankUserInfo::GetActiveBuddyCID(int iIdx)
{
	if(iIdx < 0 || iIdx >= m_kActiveBuddyVec.size())
		return 0;

	return m_kActiveBuddyVec[iIdx];
}

int LC_ClientRankUserInfo::GetCommanderTitleLevel()
{
	if (m_CommanderTitleLevel<1||m_CommanderTitleLevel>26)
	{
		return 1;
	}
	return m_CommanderTitleLevel;
}

StringType LC_ClientRankUserInfo::GetCommanderEquipInfo(int iEquipSlot)
{
	StringStreamType sStream;

	int iMinReinforceLevel = 999;
	int iMinRefineLevel = 999;
	mem::map<int, PS_ControllerEquipSlotItemInfo>::const_iterator iter = m_mControllerEquipSlotMap.begin();
	for( ; iter!=m_mControllerEquipSlotMap.end(); ++iter)
	{
		const PS_ControllerEquipSlotItemInfo& itemInfo = iter->second;
		if(iter->first == iEquipSlot)
		{
			sStream << itemInfo.m_nEquipItem << '#' << itemInfo.m_nEquipSlotReinfroce << '#';
			sStream << itemInfo.m_nEquipSlotRefine << '#' << itemInfo.m_nEquipSlotMoldingSpirit << '#';
		}

		if(itemInfo.m_nEquipSlotReinfroce < iMinReinforceLevel)
			iMinReinforceLevel = itemInfo.m_nEquipSlotReinfroce;

		if(itemInfo.m_nEquipSlotRefine < iMinRefineLevel)
			iMinRefineLevel = itemInfo.m_nEquipSlotRefine;
	}
	
	sStream << iMinReinforceLevel << '#' << iMinRefineLevel;
	return sStream.str();
}

int64_t LC_ClientRankUserInfo::GetBaseAttrMapScore()
{
	LC_ActorAttributeMap attrMap;

	for(size_t i=0; i<ATT_MAX_COUNT; ++i)
	{
		int64_t* iAttrType = GetAttribute(i);
		if(iAttrType != NULL)
		{
			attrMap.SetAttributeBase(i, *iAttrType);
		}
	}

	attrMap.RecomputeAll();
	return LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

int64_t LC_ClientRankUserInfo::GetChainAttrMapScore()
{
	LC_ActorAttributeMap attrMap;

	for(size_t i=0; i<ATT_MAX_COUNT; ++i)
	{
		int64_t* iAttrType = GetChainAttribute(i);
		if(iAttrType != NULL)
		{
			attrMap.SetAttributeBase(i, *iAttrType);
		}
	}
	
	attrMap.RecomputeAll();
	return LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

LC_ClientGameEventUserData_ArenaBattle_BattleInfo::LC_ClientGameEventUserData_ArenaBattle_BattleInfo(const Protocol::MG_RESP_BattleInfo& kMsg)
{
	battlesize = kMsg.m_battlesize;
	for (int i = 0; i < battlesize; i++)
	{
		m_UserBattleInfos[i] = Protocol::PS_Rank_User_Battle_Info(kMsg.m_UserBattleInfos[battlesize - 1 -i]);
	}
	topsize = kMsg.m_topsize;
	for (int i = 0; i < topsize; i++)
	{
		m_TopBattleInfos[i] = Protocol::PS_Rank_User_Battle_Info(kMsg.m_TopBattleInfos[topsize - 1 - i]);
	}
	serverFlag = getRankFlagByID(kMsg.m_RankID);
}

unique_id_type LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetTopChallengeID(int iIndex)
{
	return m_TopBattleInfos[iIndex].m_ChallengeCharID;
}
const char* LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetTopChallengeName(int iIndex)
{
	return m_TopBattleInfos[iIndex].m_ChallengeName.c_str();
}
unique_id_type LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetTopBeChallengeID(int iIndex)
{
	return m_TopBattleInfos[iIndex].m_beChallengeCharID;
}
const char* LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetTopBeChallengeName(int iIndex)
{
	return m_TopBattleInfos[iIndex].m_beChallengeName.c_str();
}
bool LC_ClientGameEventUserData_ArenaBattle_BattleInfo::TopIsSuccess(int iIndex)
{
	return m_TopBattleInfos[iIndex].m_sucess;
}

int LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetTopFinalRankIndex(int iIndex)
{
	return m_TopBattleInfos[iIndex].m_beChallengeIndex;
}

unique_id_type LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetUserChallengeID(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_ChallengeCharID;
}
const char* LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetUserChallengeName(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_ChallengeName.c_str();
}
int LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetUserChallengeRank(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_ChallengeIndex;
}
unique_id_type LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetUserBeChallengeID(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_beChallengeCharID;
}
const char* LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetUserBeChallengeName(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_beChallengeName.c_str();
}
int LC_ClientGameEventUserData_ArenaBattle_BattleInfo::GetUserBeChallengeRank(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_beChallengeIndex;
}
bool LC_ClientGameEventUserData_ArenaBattle_BattleInfo::UserIsSuccess(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_sucess;
}
bool LC_ClientGameEventUserData_ArenaBattle_BattleInfo::UserIsExchange(int iIndex)
{
	return m_UserBattleInfos[iIndex].m_exchange; 
}

LC_ClientGameEventUserData_ArenaSettleInfo::LC_ClientGameEventUserData_ArenaSettleInfo(const Protocol::MG_NOTIFY_Challenge& kMsg)
{
	iType = kMsg.m_nSoloType;
	PreIndex = kMsg.m_PreIndex;
	CurIndex = kMsg.m_CurIndex;
	Successed = kMsg.m_sucess;
	Exp = kMsg.m_Exp;
	Money = kMsg.m_Money;
	SP = kMsg.m_SP;
	Courage = kMsg.m_Courage;
	LiJin = kMsg.m_Lijin;
	//BeChallengerUserName = kMsg.m_beChallengeUser.m_nPlayerBaseData.m_CharName.c_str();
}

LC_ClientGameEventUserData_Common::LC_ClientGameEventUserData_Common(int iParam1, const StringType& content)
{
	Param1 = iParam1;
	Content = content;
}

LC_ClientBattleRecordDataBase::LC_ClientBattleRecordDataBase()
{
	Reset();
}

void LC_ClientBattleRecordDataBase::Reset()
{
	m_nMyUserBaseData.clear();
	m_nOtherUserBaseData.clear();
	m_bLeft = false;
	m_nResult = 0;
	m_CurrentValue = 0;
	m_LastValue = 0;
	m_nAddValue = 0;
	m_nLogTime = 0;
	m_bDetail = false;
	m_nDetail.clear();
}

void LC_ClientBattleRecordDataBase::SetUserBaseData(PS_Rank_PlayerBaseData& data, bool bMine)
{
	if (bMine)
		m_nMyUserBaseData = data;
	else
		m_nOtherUserBaseData = data;
}

void LC_ClientBattleRecordDataBase::SetDeatil(const std::vector<BaseBattlefieldReport>& report, bool bLeft)
{
	m_bLeft = bLeft;
	m_nDetail.clear();
	m_bDetail = true;
	for (int i = 0; i < report.size(); ++ i)
	{
		m_nDetail.push_back(report[i]);
	}
}

uint64_t LC_ClientBattleRecordDataBase::GetUniqueID()
{
	return UniqueID;
}

int32_t LC_ClientBattleRecordDataBase::GetFinalResult()
{
	return m_nResult;
}

int32_t LC_ClientBattleRecordDataBase::GetFinalValue()
{
	return m_CurrentValue;
}

int32_t LC_ClientBattleRecordDataBase::GetLastValue()
{
	return m_LastValue;
}

int32_t LC_ClientBattleRecordDataBase::GetChangeValue()
{
	return m_nAddValue;
}

int32_t LC_ClientBattleRecordDataBase::GetLogTime()
{
	return m_nLogTime;
}	

StringType LC_ClientBattleRecordDataBase::GetUserBaseData(bool bMine)
{
	StringStreamType sStream;
	PS_Rank_PlayerBaseData* BaseData = &m_nOtherUserBaseData;
	if (bMine)
		BaseData = &m_nMyUserBaseData;
	
	sStream << BaseData->m_Robot << "#" << BaseData->m_CommanderTitleID << "#" << BaseData->m_CommanderTitleFrameID << "#"
		<< BaseData->m_ProfilePictureID << "#" << BaseData->m_Level << "#" << BaseData->m_CharID << "#"
		<< BaseData->m_CommanderScore << "#" << BaseData->m_CharName << "#";

	return sStream.str();
}

uint64_t LC_ClientBattleRecordDataBase::GetRepalyIdByIndex(int index)
{
	return m_nDetail[index].m_nRepalyId;
}

bool LC_ClientBattleRecordDataBase::GetRepalyIdValidByIndex(int index)
{
	return m_nDetail[index].b_IsFightValid;
}

int LC_ClientBattleRecordDataBase::GetBuddyCombatResult(bool bMine, int index)
{
	if (bMine == m_bLeft)
		return m_nDetail[index].m_nLeftBuddyData.m_nResult;
	else
		return m_nDetail[index].m_nRightBuddyData.m_nResult;
}

StringType LC_ClientBattleRecordDataBase::GetBuddyBaseDataByIndex(bool bMine, int index)
{
	if (bMine == m_bLeft)
		return GetBuddyBaseDate(m_nDetail[index].m_nLeftBuddyData);
	else
		return GetBuddyBaseDate(m_nDetail[index].m_nRightBuddyData);
}

StringType LC_ClientBattleRecordDataBase::GetBuddyBaseDate(BattleBuddyRecord& data)
{
	StringStreamType sStream;
	for (int i = 0; i < data.m_nBuddyData.size(); ++i)
	{
		BaseBattlefieldBuddyData& BuddyData = data.m_nBuddyData[i];
		sStream << (int)BuddyData.m_nReliveState << "|";
		PS_Buddy& Detail = BuddyData.m_BuddyInfo;
		sStream << Detail.m_nBuddyIndex << "|";
		PS_Buddy_Info_BuddyCS& BuddyInfo = Detail.m_nBuddyData;
		sStream << BuddyInfo.m_nBuddyID << "|" << BuddyInfo.m_nBuddyLevel << "|" << BuddyInfo.m_nBuddyStarLevel << "|"
			<< BuddyInfo.m_nBuddyCombatScore << "|" << BuddyInfo.m_nBuddyTransformID << "|" << BuddyInfo.m_bActivateAllSkill << "|";

		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator itor = BuddyInfo.m_nBuddyRuneInfo.begin();
		for (; itor != BuddyInfo.m_nBuddyRuneInfo.end(); ++itor)
		{
			sStream << itor->first << ":" << itor->second.m_nRuneItemID << ":" << itor->second.m_nRuneStarLevel << ":" << itor->second.m_nRuneLevel << ";"; 
		}
		sStream << "|";

		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator iter = BuddyInfo.m_nEquipSlotInfo.begin();
		for (; iter != BuddyInfo.m_nEquipSlotInfo.end(); ++iter)
		{
			sStream << iter->first << ":" << iter->second.m_nEquipItemID << ":" << iter->second.m_nEquipSlotStarLevel << ":" << iter->second.m_nEquipSlotLevel << ";"; 
		}
		sStream << "|";

		sStream << BuddyInfo.m_nAllEquipSlotReinforceLevel << "|";

		sStream << Detail.m_nDamageData << "|" << Detail.m_nHurtData << "|" << Detail.m_nHealData << "|";

		sStream << "#";
	}

	return sStream.str();
}

int LC_ClientBattleRecordDataBase::GetReportResultByIndex(bool bMine, int index)
{
	BattleBuddyRecord* BaseData = &m_nDetail[index].m_nRightBuddyData;
	if (bMine == m_bLeft)
		BaseData = &m_nDetail[index].m_nLeftBuddyData;

	return BaseData->m_nResult;
}

int LC_ClientBattleRecordDataBase::GetReportGroupIDByIndex(bool bMine, int index)
{
	BattleBuddyRecord* BaseData = &m_nDetail[index].m_nRightBuddyData;
	if (bMine == m_bLeft)
		BaseData = &m_nDetail[index].m_nLeftBuddyData;

	return BaseData->m_nGroupID;
}

bool LC_ClientBattleRecordDataBase::GetIsDetail()
{
	return m_bDetail;
}

int LC_ClientBattleRecordDataBase::GetDetailNumber()
{
	return m_nDetail.size();
}

void LC_ClientGuessDataBase :: Reset()
{
	m_nLeftUserBaseData.clear();
	m_nRightUserBaseData.clear();
	m_bGetWiner = false;
	m_nSummitType = 0;
	m_nAgainstID = 0;
	m_nChoseCharID = 0;
	m_nWinerCharID = 0;
}
void LC_ClientGuessDataBase :: SetGuessBaseData(PS_GuessInfo_SummitInfo& data)
{
	PS_GuessInfo_SummitArena& GuessInfo = data.m_nGuessInfo;
	std::vector<PS_Rank_PlayerBaseData>& PlayerData = data.m_PlayerData;

	if(PlayerData.size() == 0)
	{
		return;
	}
	m_nLeftUserBaseData = PlayerData[0];		//左边玩家基本信息
	m_nRightUserBaseData = PlayerData[1];		//右边玩家基本信息
	m_bGetWiner = GuessInfo.m_bGetWiner;		//是否查看过winer
	m_nSummitType = GuessInfo.m_nSummitType;	//竞猜的类型
	m_nAgainstID = GuessInfo.m_nAgainstID;		//对战id
	m_nChoseCharID = GuessInfo.m_nChoseCharID;	//选中的玩家id
	m_nWinerCharID = GuessInfo.m_nWinerCharID;	//胜利者id

}


StringType LC_ClientGuessDataBase ::GetUserBaseData(bool bleft)
{
	StringStreamType sStream;
	PS_Rank_PlayerBaseData* BaseData = &m_nRightUserBaseData;
	if (bleft)
		BaseData = &m_nLeftUserBaseData;

	sStream << BaseData->m_Robot << "#" << BaseData->m_CommanderTitleID << "#" << BaseData->m_CommanderTitleFrameID << "#"
		<< BaseData->m_ProfilePictureID << "#" << BaseData->m_Level << "#" << BaseData->m_CharID << "#"
		<< BaseData->m_CommanderScore << "#" << BaseData->m_CharName << "#";

	return sStream.str();
}

bool LC_ClientGuessDataBase ::GetLooked()
{
	return m_bGetWiner;
}

uint32_t LC_ClientGuessDataBase ::GetAgainstID()
{
	return m_nAgainstID;
}
uint32_t LC_ClientGuessDataBase ::GetSummitType()
{
	return m_nSummitType;
}

uint64_t LC_ClientGuessDataBase ::GetChoseCharID()
{
	return m_nChoseCharID;
}

uint64_t LC_ClientGuessDataBase ::GetWinerCharID()
{
	return m_nWinerCharID;
}

void LC_ClientGuessDataBase ::SetChoseCharID(uint64_t ChoseCharID)
{
	m_nChoseCharID = ChoseCharID;
}
