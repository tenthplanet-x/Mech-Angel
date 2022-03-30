#include "ClientPCH.h"
#include "LC_ClientBuddyInstance.h"
#include "GlobalSettings.h"

GameLogic::LC_ClientBuddyInstance::LC_ClientBuddyInstance()
{
	m_pkShortcurtBar = T_NEW_D LC_ClientShortcutBar(NULL);
	Reuse();

	m_vRuneEquipList.resize(GetGlobalSetting.RunePackSize);
	m_vEquipList.resize(GetGlobalSetting.ArtifactPackSize);
}

GameLogic::LC_ClientBuddyInstance::~LC_ClientBuddyInstance()
{
	T_SAFE_DELETE( m_pkShortcurtBar );
	Reuse();
}

void GameLogic::LC_ClientBuddyInstance::Reuse()
{
	m_initial = true;
	iCID = -1;
	iLevel = 1;
	iCombateScore = 0;
	iExp = 0;
	iHP = 0;
	iStarLevel = 0;
	iEquipShowLevel = 1;
	m_iRank = -1;
	m_kSkillAsset.setOwner(NULL);
	m_iCurrentTransformID = 0;
	m_vActiveTransformRecord.clear();
	m_mActiveTransformInvaildTime.clear();
	m_bEntrusted=false;
	m_vRuneEquipList.clear();
	m_vEquipList.clear();
}

void GameLogic::LC_ClientBuddyInstance::Init()
{
	Reuse();
	m_kSkillAsset.Clear();
}

void GameLogic::LC_ClientBuddyInstance::ParseProtocol(BuddyData_Base& buddy)
{
	iCID = buddy.nBuddyTemplateId;
	iLevel = buddy.nLevel;
	iExp = buddy.nExp;
	iHP = buddy.nHP;
	iStarLevel = buddy.nStarLevel;
	//iCombateScore = buddy.nCombatscore;
	SetCurrentTransformID(buddy.nCurrentTransformID);
}

void GameLogic::LC_ClientBuddyInstance::InitAttributeMap()
{
	const CF_Buddy::DataEntry* pkData  = CF_Buddy::GetSingleton().GetEntryPtr(iCID);
	if (pkData != NULL)
	{
		m_kAttributePlus.Clear();
		m_kAttributeMap.Clear();
		m_kAttributeMap.SetAttributeBase(ATT_MAX_HP,	pkData->_lMaxHP);
		m_kAttributeMap.SetAttributeBase(ATT_MAX_MP,	0);
		m_kAttributeMap.SetAttributeBase(ATT_MAX_ATK,	pkData->_lMaxPHA);
		m_kAttributeMap.SetAttributeBase(ATT_MAX_AMM,	0);
		m_kAttributeMap.SetAttributeBase(ATT_MAX_STM,	0);
		m_kAttributeMap.SetAttributeBase(ATT_DEF,		pkData->_lPHD);
		m_kAttributeMap.SetAttributeBase(ATT_HP_RECOVER,		0);
		m_kAttributeMap.SetAttributeBase(ATT_MP_RECOVER,		0);
		m_kAttributeMap.SetAttributeBase(ATT_DOG_RATE,	pkData->_lDogRATE);
		m_kAttributeMap.SetAttributeBase(ATT_CIR_RATE,	pkData->_lCirRATE);
		m_kAttributeMap.SetAttributeBase(ATT_CIR_DAM,	0);
		m_kAttributeMap.SetAttributeBase(ATT_CIR_DEF,	pkData->_lCirDefAdd);
		m_kAttributeMap.SetAttributeBase(ATT_ATK_RATE,	pkData->_lATKRateAdd);
		m_kAttributeMap.SetAttributeBase(ATT_MOVE_SPEED, (attr_value_type)CHAR_MOVE_SPEED);
		m_kAttributeMap.SetAttributeBase(ATT_PICK_CRASH_RNG, GetGlobalSetting.iGoldPickRadius);

		m_kAttributeMap.AddAttributePercentFunc(ATT_MAX_HP, 0);
		m_kAttributeMap.AddAttributePercentFunc(ATT_MAX_ATK, 0);
		m_kAttributeMap.AddAttributePercentFunc(ATT_DEF, 0);

		//这里指的是比率的改变
		m_kAttributeMap.SetAttributeBase(ATT_MP_COST_RATE, 0);
		m_kAttributeMap.SetAttributeBase(ATT_HP_COST_RATE, 0);
		m_kAttributeMap.SetAttributeBase(ATT_MP_RECOVER_RATE, 0);
		m_kAttributeMap.SetAttributeBase(ATT_HP_RECOVER_RATE, 0);
		m_kAttributeMap.SetAttributeBase(ATT_SP_RECOVER_RATE, 10000);
		m_kAttributeMap.SetAttributeBase(ATT_STM_RECOVER_RATE, 0);
		m_kAttributeMap.SetAttributeBase(ATT_AMM_RECOVER_RATE, 0);
		//这里指的是百分比*10000（存成整数）
		m_kAttributeMap.SetAttributeBase(ATT_EXP_RATE, 10000);
		m_kAttributeMap.SetAttributeBase(ATT_CASH_RATE, 10000);
		m_kAttributeMap.SetAttributeBase(ATT_MF_RATE, 10000);

		//伤害增益的参数
		m_kAttributeMap.SetAttributeBase(ATT_DMG_RATE, 10000);
		m_kAttributeMap.SetAttributeBase(ATT_DMG_DERATE, 0);
		m_kAttributeMap.SetAttributeBase(ATT_IGNORE_DEF, 0);
		m_kAttributeMap.SetAttributeBase(ATT_DMG_REBOUND, 0);
		m_kAttributeMap.SetAttributeBase(ATT_CRI_DMG_DERATE, 0);

		m_kAttributeMap.RecomputeAll();
		
		InitCombate(pkData->_lMaxHP, 
					pkData->_lPHD, 
					pkData->_lMaxPHA, 
					pkData->_lATKRateAdd, 
					pkData->_lDogRATE, 
					pkData->_lCirRATE, 
					pkData->_lCirDefAdd);

	}
}


void GameLogic::LC_ClientBuddyInstance::InitCombate(long hp, int64_t def, long ack
													, long ackRate, long dog, long cir
													, long cirDef)
{
	double fResult = 0; 
	//生命
	fResult += GetGlobalSetting.AttrHp*hp;
	//防御
	fResult += GetGlobalSetting.AttrDefault*def;
	//攻击
	fResult += GetGlobalSetting.AttrAttack*ack;
	//命中等级
	fResult +=  GetGlobalSetting.AttrAtkRate*ackRate;
	//闪避
	fResult += GetGlobalSetting.AttrDogRate*dog;
	//暴击等级
	fResult += GetGlobalSetting.AttrCirRate*cir;
	//韧性
	fResult += GetGlobalSetting.AttrCirDef*cirDef;


	iCombateScore = fResult;
}

bool GameLogic::LC_ClientBuddyInstance::SetActiveTransformRecord(std::vector<int32_t> m_vRecord)
{
	bool bchange = false;
	for (int i = 0;i < m_vRecord.size();++i)
	{
		if (m_vActiveTransformRecord.insert(m_vRecord[i]).second)
		{
			bchange = true;
			break;
		}
	}

	bchange = bchange || m_vRecord.size() != m_vActiveTransformRecord.size();

	if (bchange)
	{
		m_vActiveTransformRecord.clear();
		for (int i = 0;i < m_vRecord.size();++i)
		{
			m_vActiveTransformRecord.insert(m_vRecord[i]);
		}
	}

	return bchange;
}

bool GameLogic::LC_ClientBuddyInstance::GetIsActiveTransform(int iID)
{
	mem::set<int32_t>::const_iterator it = m_vActiveTransformRecord.find(iID);
	if (it == m_vActiveTransformRecord.end())
	{
		return false;
	}
	return true;
}

bool GameLogic::LC_ClientBuddyInstance::SetActiveTransformInvaildTime(std::vector<BuddyTransformInvaildTime> m_vTime)
{
	bool bchange = false;
	for (int i = 0; i < m_vTime.size(); ++i)
	{
		mem::map<int32_t, int32_t>::iterator it = m_mActiveTransformInvaildTime.find(m_vTime[i].m_nTransformID);
		if (it == m_mActiveTransformInvaildTime.end())
		{
			bchange = true;
			break;
		}
		else if (it->second != m_vTime[i].m_nInvaildTime)
		{
			bchange = true;
			break;
		}
	}
	bchange = bchange || m_vTime.size() != m_mActiveTransformInvaildTime.size();
	m_mActiveTransformInvaildTime.clear();
	for (int i = 0; i < m_vTime.size(); ++i)
	{
		m_mActiveTransformInvaildTime[m_vTime[i].m_nTransformID] = m_vTime[i].m_nInvaildTime;
	}
	return bchange;
}

int32_t GameLogic::LC_ClientBuddyInstance::GetActiveTransformTime(int iID)
{
	mem::map<int32_t, int32_t>::iterator it = m_mActiveTransformInvaildTime.find(iID);
	if (it != m_mActiveTransformInvaildTime.end())
	{
		return it->second;
	}
	return 0;
}

void LC_ClientBuddyInstance::UpdateRuneEquip(int iIndex, const PS_ItemAssetEntry& assetEntry)
{
	LC_BackPackEntry& entry = m_vRuneEquipList[iIndex];
	entry.GetFromProtocol(assetEntry);
}

void LC_ClientBuddyInstance::ClearRuneEquipEntry(int iIndex)
{
	LC_BackPackEntry& entry = m_vRuneEquipList[iIndex];
	entry.Reset();
}


void LC_ClientBuddyInstance::UpdateEquip(int iIndex, const Equip_Base& baseInfo)
{
	EquipInfo& info = m_vEquipList[iIndex];
	info.itemEntry.GetFromProtocol(baseInfo.m_Data);
	info.intensifyLevel = baseInfo.m_IntensifyLevel;
	info.refineLevel = baseInfo.m_RefineLevel;
}

void LC_ClientBuddyInstance::UpdateEquip(int iIndex, const PS_ItemAssetEntry& assetEntry)
{
	EquipInfo& info = m_vEquipList[iIndex];
	info.itemEntry.GetFromProtocol(assetEntry);
}

void LC_ClientBuddyInstance::UpdateEquipLevel(int iIndex, int iLevel)
{
	EquipInfo& info = m_vEquipList[iIndex];
	info.intensifyLevel = iLevel;
}

void LC_ClientBuddyInstance::UpdateEquipStarLevel(int iIndex, int iStarLevel)
{
	EquipInfo& info = m_vEquipList[iIndex];
	info.refineLevel = iStarLevel;
}

void LC_ClientBuddyInstance::ClearEquipEntry(int iIndex)
{
	EquipInfo& info = m_vEquipList[iIndex];
	info.Reset();
}

LC_BackPackEntry* LC_ClientBuddyInstance::GetEquipEntry(int iIndex)
{
	EquipInfo& info = m_vEquipList[iIndex];
	return &info.itemEntry;
}

int LC_ClientBuddyInstance::GetEquipIntensifyLevel(int iIndex)
{
	const EquipInfo& info = m_vEquipList[iIndex];
	return info.intensifyLevel;
}

int LC_ClientBuddyInstance::GetEquipRefineLevel(int iIndex)
{
	const EquipInfo& info = m_vEquipList[iIndex];
	return info.refineLevel;
}