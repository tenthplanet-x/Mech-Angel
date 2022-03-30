#include "LC_ServerMapLogic_MonsterConfig.h"
#include "LC_ServerNPC.h"
#include "LC_ActorSkillTrigger.h"
#include "UT_MathBase.h"
#include "UT_DirConfig.h"
#include "SK_Factory.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_AutoFitNPCManager.h"
#include "CF_SpawnPointList.h"
#include "LS_LuaStateFactory.h"
#include "CF_WorldMapList.h"
#include "CF_SkillExecuteRule.h"
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

int ReadLuaNPCType(int type)
{
	switch (type)
	{
		case 0:
			return 1;
		case 2:
			return 2;
		case 1:
		case 3:
			return 3;
	}
	return 1;
}

void DungeonConfigData::ReadConfigFile(const StringType& configFileName, const StringType& kTableName)
{
	StringType pkConfigFileName = TRANSCRIPTION_CONFIG_DIR + configFileName;
	pkConfigFileName = Utility::getFilePath(pkConfigFileName);
	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	if (!pkLuaState)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DungeonConfigData::ReadConfigFile-pkLuaState error! [%s]", pkConfigFileName.c_str());
		return;
	}

	bool bLoadScript = pkLuaState->DoFile(pkConfigFileName);
	if (!bLoadScript)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "DungeonConfigData::ReadConfigFile-DoFile error! [%s]", pkConfigFileName.c_str());
		T_ASSERT_CRITICAL_S(false);
		return;
	}

	static bool bReload = false;

	LS_LuaObject* pkLuaObject = NULL;
	StringType strFindValue = kTableName + ".StartTime";
	pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
	if (pkLuaObject)
	{
		m_fStartTime = pkLuaObject->ToFloat();
	}

	MonsterData transEntry;
	int iTransferNpcOutNumber		= 0;
	strFindValue = kTableName + ".TransferOutNPC" + ".NpcNumber";

	pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
	if (pkLuaObject)
	{
		iTransferNpcOutNumber = pkLuaObject->ToInt();
	}

	char cBuffer[255];

	//Init TransferOut NPC
	for (int iIndex = 0; iIndex < iTransferNpcOutNumber; iIndex++)
	{
		transEntry.Reset();

		sprintf(cBuffer, "TransferOutNPC_%i", iIndex);
		strFindValue = kTableName + ".TransferOutNPC" + "." + cBuffer + "._kSpawnLocation";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject && pkLuaObject->IsArray())
		{
			transEntry.m_kSpawnLocation.x = pkLuaObject->GetChild(0)->ToInt();
			transEntry.m_kSpawnLocation.y = pkLuaObject->GetChild(1)->ToInt();
			transEntry.m_kSpawnLocation.z = pkLuaObject->GetChild(2)->ToInt();
		}

		strFindValue = kTableName + ".TransferOutNPC" + "." + cBuffer + "._iZRotAngle";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iZRotAngle = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".TransferOutNPC" + "." + cBuffer + "._iSpawnCharType";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iSpawnCharType = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".TransferOutNPC" + "." + cBuffer + "._iWanderRadius";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iWanderRadius = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".TransferOutNPC" + "." + cBuffer + "._lWanderPathID";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_lWanderPathID = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".TransferOutNPC" + "." + cBuffer + "._lMonsterType";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iNPCType = ReadLuaNPCType(pkLuaObject->ToInt());
		}
		m_MonsterMap.push_back(transEntry);
	}

	int iMonsterBatchCount = 0;
	strFindValue = kTableName + ".MonsterData" + ".MonsterCount";

	pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
	if (pkLuaObject)
	{
		iMonsterBatchCount = pkLuaObject->ToInt();
	}

	for (int iIndex = 0; iIndex < iMonsterBatchCount; iIndex++)
	{
		transEntry.Reset();

		sprintf(cBuffer, "MonsterData_%i", iIndex);
		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._kSpawnLocation";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject && pkLuaObject->IsArray())
		{
			transEntry.m_kSpawnLocation.x = pkLuaObject->GetChild(0)->ToInt();
			transEntry.m_kSpawnLocation.y = pkLuaObject->GetChild(1)->ToInt();
			transEntry.m_kSpawnLocation.z = pkLuaObject->GetChild(2)->ToInt();
		}

		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._iZRotAngle";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iZRotAngle = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._iSpawnCharType";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iSpawnCharType = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._iWanderRadius";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iWanderRadius = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._lWanderPathID";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_lWanderPathID = pkLuaObject->ToInt();
		}

		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._lMonsterType";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iNPCType = ReadLuaNPCType(pkLuaObject->ToInt());
		}

		strFindValue = kTableName + ".MonsterData" + "." + cBuffer + "._iStageIndex";
		pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
		if (pkLuaObject)
		{
			transEntry.m_iStage = pkLuaObject->ToInt();
		}
		m_MonsterMap.push_back(transEntry);
	}
	merger_();
}

void DungeonConfigData::ReadConfigCF(int MapLogicType)
{
	CF_SpawnPointList* pkSpawnPointCSV = CF_SpawnPointList::GetSingletonPtr();

	CF_SpawnPointList::DataEntryMapExternIterator kIter = pkSpawnPointCSV->GetIterator();
	CF_CharType* pkCharType = CF_CharType::GetSingletonPtr();
	while (kIter.HasMoreElements())
	{
		const CF_SpawnPointList::DataEntry* pkData = kIter.PeekNextValuePtr();

		if (MapLogicType == pkData->_iMapID && SCT_AUTO == (SpawnControlType)pkData->_iSpawnControlType)
		{
			MonsterData transEntry;
			transEntry.m_kSpawnLocation = pkData->_kSpawnLocation;
			transEntry.m_iZRotAngle = pkData->_iZRotAngle;
			transEntry.m_iSpawnCharType = pkData->_iSpawnCharType;
			transEntry.m_iWanderRadius = pkData->_iWanderRadius;
			transEntry.m_lWanderPathID = pkData->_lWanderPathID;
			transEntry.m_iAutoFitDeltaLevel = pkData->_iAutoFitDeltaLevel;
			transEntry.m_iAutoFitMaxLevel =	pkData->_iAutoFitMaxLevel;
			transEntry.m_iAutoFitMinLevel = pkData->_iAutoFitMinLevel;
			transEntry.m_kEliteProp.AddDatas(pkData->_kEliteProp);
			transEntry.m_kEliteProp.FixTotalPro(10000);
			CF_CharType::DataEntry* pkCharData = pkCharType->GetEntryPtr(transEntry.m_iSpawnCharType);
			if (NULL != pkCharData)
			{
				transEntry.m_iNPCType = pkData->_iSpawnLogicType;
				if (pkCharData->_lFaction == FACTION_EVIL)
				{
					transEntry.m_iNPCType = 1;
				}
				else
				{
					transEntry.m_iNPCType = 2;
				}
				transEntry.m_SpawnPointID = pkData->_iID;
				transEntry.m_SpawnContorlType = SCT_AUTO;
				transEntry.m_SpawnTime = pkData->_iCoolDownTime;
				transEntry.m_SpawnTriggerValue = 0;
				m_MonsterMap.push_back(transEntry);
			}
		}
		kIter.MoveNext();
	}
	merger_();
	process_auto_spawn();
}

void DungeonConfigData::process_auto_spawn()
{
	MonsterDataList::iterator	i = m_MonsterMap.begin();
	AutoSpawnMonsterData data;
	for (; i != m_MonsterMap.end(); ++i)
	{
		DungeonConfigData::MonsterData* pkData = &(*i);
		if (SCT_AUTO == pkData->m_SpawnContorlType)
		{
			data._coolDownTime = pkData->m_SpawnTime;
			data._lastActiveTime = -pkData->m_SpawnTime;
			data._spawnData = pkData;
			m_AutoSpawnMonsterList.insert(std::make_pair(pkData->m_SpawnPointID, data));
		}
	}
}

static bool merger_one_(DungeonConfigData::MonsterGroupType& group, DungeonConfigData::MonsterGroupType& all_monsters)
{
	DungeonConfigData::MonsterGroupType::iterator ait;
	for (ait = all_monsters.begin(); ait != all_monsters.end(); ++ait)
	{
		DungeonConfigData::MonsterData* data1 = *ait;
		for (DungeonConfigData::MonsterGroupType::iterator git = group.begin(); git != group.end(); ++git)
		{
			DungeonConfigData::MonsterData* data2 = *git;
			int distance = Utility::UT_MathBase::LineLengthXYInt(data1->m_kSpawnLocation, data2->m_kSpawnLocation);
			if (distance < 800)
			{
				group.push_back(data1);
				all_monsters.erase(ait);
				return true;
			}
		}
	}
	return false;
}

static void merger_group_(DungeonConfigData::MonsterGroupType& group, DungeonConfigData::MonsterGroupType& all_monsters)
{
	bool found = false;
	do
	{
		found = merger_one_(group, all_monsters);
	}
	while (found);
}

void DungeonConfigData::merger_()
{
	MonsterGroupType all_monsters;
	MonsterDataListIter dmiter = m_MonsterMap.begin();
	CF_CharType* pkCharType = CF_CharType::GetSingletonPtr();
	for (; dmiter != m_MonsterMap.end(); ++dmiter)
	{
		if (1 == dmiter->m_iNPCType)
		{
			all_monsters.push_back(&(*dmiter));
		}
	}
	mem::map<int, int> elitemonsermap;
	for (MonsterGroupType::iterator i = all_monsters.begin(); i != all_monsters.end(); ++i)
	{
		MonsterData* data = *i;
		if (elitemonsermap.find(data->m_iSpawnCharType) == elitemonsermap.end())
		{
			CF_CharType::DataEntry* pkData = pkCharType->GetEntryPtr(data->m_iSpawnCharType);
			if (!pkData)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CF_CharType-m_iSpawnCharType error[%i]", data->m_iSpawnCharType);
				continue;
			}
			int elite_level = pkData->_lElite;
			elitemonsermap.insert(std::make_pair(data->m_iSpawnCharType, elite_level));
		}
	}
	for (mem::map<int, int>::iterator i = elitemonsermap.begin(); i != elitemonsermap.end(); ++i)
	{
		if (i->second <= 1)
		{
			continue;
		}
		EliteMonsterType type;
		type.m_NPCCharType = i->first;
		type.m_Elite = i->second;
		m_EliteMonsterTypeList.push_back(type);
	}
	while (!all_monsters.empty())
	{
		MonsterGroupType group;
		group.push_back(*all_monsters.begin());
		all_monsters.erase(all_monsters.begin());
		merger_group_(group, all_monsters);
		m_MonsterGroupList.push_back(group);
	}
}

int DungeonConfigData::GetEliteNumber()
{
	return UT_MathBase::RandInRangeInt(m_EliteGroupMin, m_EliteGroupMax);
}

DungeonConfigDataManager::DungeonConfigDataManager()
{
}

DungeonConfigDataManager::~DungeonConfigDataManager()
{
	for (DataMap::iterator iter = m_DataMap.begin(); iter != m_DataMap.end(); ++iter)
	{
		T_SAFE_DELETE(iter->second);
	}
	for (Dungeon_Config_DataMap::iterator iter = m_Config_DataMap.begin(); iter != m_Config_DataMap.end(); ++iter)
	{
		T_SAFE_DELETE(iter->second);
	}
}

struct TranscriptionConfig
{
	StringType FileName;
	StringType TableName;
	TranscriptionConfig() {}

	TranscriptionConfig(const StringType& filename, const StringType& tablename): FileName(filename), TableName(tablename) {}
	TranscriptionConfig(const TranscriptionConfig& rv): FileName(rv.FileName), TableName(rv.TableName) {}
	TranscriptionConfig& operator = (const TranscriptionConfig& rv)
	{
		this->TableName = rv.TableName;
		this->FileName = rv.FileName;
		return *this;
	}
};
typedef mem::map<int, TranscriptionConfig> TranscriptionMap;

void DungeonConfigDataManager::Init()
{
	TranscriptionMap m_TranMap;
	m_ParamManger.Init();
	/*m_TranMap[104] = TranscriptionConfig("Transcription_UndergroundPalace.lua","Transcription_Palace");
	m_TranMap[106] = TranscriptionConfig("Transcription_Bailian.lua","Transcription_Bailian");*/

	CF_WorldMapList::DataEntryMapExternIterator iter = CF_WorldMapList::GetSingleton().GetIterator();
	while (iter.HasMoreElements())
	{
		const CF_WorldMapList::DataEntry& rkValue = iter.PeekNextValue();
		DungeonConfigData* data = T_NEW_D DungeonConfigData();
		data->ReadConfigCF(rkValue._iID);
		data->m_EliteGroupMin = 0;
		data->m_EliteGroupMax = 0;
		if (data->m_AutoSpawnMonsterList.empty() && data->m_MonsterMap.empty() && data->m_EliteMonsterTypeList.empty())
		{
			T_DELETE_D data;
		}
		else
		{
			m_DataMap[rkValue._iID] = data;
		}
		Logic_ConfigData_Base* config = LC_ServerMapLogicFactory::ReadConfig(rkValue._iLogicPlayType, rkValue._sLogicPlayFile);
		if (config != NULL)
		{
			m_Config_DataMap[rkValue._iID] = config;
		}
		iter.MoveNext();
	}
}

DungeonConfigData* DungeonConfigDataManager::GetConfigData(int MapLogicID)
{
	DataMap::iterator iter = m_DataMap.find(MapLogicID);
	if (iter == m_DataMap.end())
	{
		return NULL;
	}
	return iter->second;
}

Logic_ConfigData_Base* DungeonConfigDataManager::GetConfigDataEx(int MapID)
{
	Dungeon_Config_DataMap::iterator iter = m_Config_DataMap.find(MapID);
	if (iter == m_Config_DataMap.end())
	{
		return NULL;
	}
	return iter->second;
}

EliteMonsterParamManager::EliteMonsterParamManager()
{
}

EliteMonsterParamManager::~EliteMonsterParamManager()
{
	for (EliteExternSkillMapType::iterator iter = m_Params[1].m_SkillList.begin(); iter != m_Params[1].m_SkillList.end(); ++iter)
	{
		GameLogic::LC_SkillTriggerEntry* entry = iter->second;
		T_SAFE_DELETE(entry);
	}
}

int EliteMonsterParamManager::GetMaxSkillCount()
{
	return 4;
}

void EliteMonsterParamManager::Init()
{
	EliteMonsterParam& rParam0 = m_Params[ELT_NORMAL];
	//rParam0.m_SkillCount	= 0;
	rParam0.m_GroupCount	= 0;
	rParam0.m_MinorCount	= 0;

	EliteMonsterParam& rParam1 = m_Params[ELT_BLUE_MID];
	//rParam1.m_SkillCount	= 4;
	rParam1.m_GroupCount	= 3;
	rParam1.m_MinorCount	= 0;
	rParam1.m_PostionList.push_back(Utility::UT_Vec3Int(0, 200, 0));
	rParam1.m_PostionList.push_back(Utility::UT_Vec3Int(-140, -100, 0));
	rParam1.m_PostionList.push_back(Utility::UT_Vec3Int(140, -100, 0));

	EliteMonsterParam& rParam2 = m_Params[ELT_GOLD_MID];
	//rParam2.m_SkillCount	= 4;
	rParam2.m_GroupCount	= 1;
	rParam2.m_MinorCount	= 3;
	rParam2.m_PostionList.push_back(Utility::UT_Vec3Int(0, 0, 0));
	rParam2.m_PostionList.push_back(Utility::UT_Vec3Int(0, 240, 0));
	rParam2.m_PostionList.push_back(Utility::UT_Vec3Int(-180, -140, 0));
	rParam2.m_PostionList.push_back(Utility::UT_Vec3Int(180, -140, 0));

	for (int ii = ELT_STAR_MID_MIN; ii < ELT_STAR_MID_MAX; ++ii)
	{
		EliteMonsterParam& rParamStar = m_Params[ii];
		rParamStar.m_GroupCount	= 1;
		rParamStar.m_PostionList.push_back(Utility::UT_Vec3Int(0, 0, 0));
	}

	CF_SkillExecuteRule* list = CF_SkillExecuteRule::GetSingletonPtr();
	CF_SkillExecuteRule::DataEntryMapExternIterator iter = list->GetIterator();
	Skill::SK_Factory* pkSkillFactory = Skill::SK_Factory::GetSingletonPtr();
	if (NULL == pkSkillFactory)
	{
		return;
	}

	while (iter.HasMoreElements())
	{
		const CF_SkillExecuteRule::DataEntry& rkValue = iter.PeekNextValue();
		if (1 == rkValue._iRandomSkill)
		{
			GameLogic::LC_SkillTriggerEntry* pkEntry = T_NEW_D GameLogic::LC_SkillTriggerEntry();
			pkEntry->SetType((unsigned short)rkValue._lTriggerType);
			pkEntry->SetSkillTypeID(rkValue._lSkillID);
			pkEntry->SetPriority((unsigned short)rkValue._lPrio);
			pkEntry->SetCountLimit((unsigned short)rkValue._lLimitCount);
			pkEntry->SetLifeTime(rkValue._fLifeTime);
			pkEntry->SetData1(rkValue._fData1);
			pkEntry->SetData2(rkValue._fData2);
			pkEntry->SetData3(rkValue._fData3);
			pkEntry->SetShieldSkill(rkValue._iShieldSkill == 1);
			pkEntry->SetValid(true);
			Skill::SK_SkillExecute* pkSkill = pkSkillFactory->RequestSkillExcute(rkValue._lSkillID);
			if (pkSkill)
			{
				pkEntry->SetCDTime((float)pkSkill->GetProperty_CoolDownTime());
			}
			for (int i = rkValue._iEliteLevel ; i <= ELT_STAR_MID_MIN ; ++i)
			{
				EliteMonsterParam& rParamX = m_Params[i - 1];
				rParamX.m_SkillList[rkValue._iID] = pkEntry;
			}
		}
		iter.MoveNext();
	}
}

GameLogic::LC_SkillTriggerEntry* EliteMonsterParamManager::GetSkillTrigger(int level, int Rule_Id)
{
	EliteMonsterParam* parm = GetEliteMonsterParam(level);
	if (parm == NULL)
	{
		return NULL;
	}
	EliteExternSkillMapType::iterator it = parm->m_SkillList.find(Rule_Id);
	if (it == parm->m_SkillList.end())
	{
		return NULL;
	}
	return it->second;
}

EliteMonsterParam* EliteMonsterParamManager::GetEliteMonsterParam(int Elite_Level)
{
	int32_t iIndex = Elite_Level - 1;
	if (iIndex < 0 || iIndex >= (int)(sizeof(m_Params) / sizeof(EliteMonsterParam)))
	{
		return NULL;
	}
	return &m_Params[iIndex];
}

int EliteMonsterParam::RandomSkill(EliteExternSkillListType& list)
{
	EliteExternSkillMapType::iterator beg = m_SkillList.begin();
	for (; beg != m_SkillList.end(); ++beg)
	{
		list.push_back(beg->second);
	}
	std::random_shuffle(list.begin(), list.end());
	return 0;
}

void EliteMonsterGroupManager::RandomAllocIndex()
{
	std::random_shuffle(m_AllocIndexList.begin(), m_AllocIndexList.end());
}

void Add_Attrib_By_Precent(LC_ActorAttributeMap* attrs, int attr_type , int add_precent)
{
	if (add_precent == 0)
	{
		return;
	}
	attr_value_type value	= attrs->GetAttribute(attr_type);
	value = value * (100 + add_precent) / 100;
	attrs->AddAttributeValueFunc(attr_type, value);
}

void Upgrade_Elite_Monster(LC_ServerNPC* npc , int Elite_Level , bool IsMinor , EliteMonsterParam* param , const EliteExternSkillListType& skill_list, int skillcount)
{
	int32_t iEliteLevel = Elite_Level - 1;
	if (iEliteLevel <= ELT_NORMAL)
	{
		return;
	}
	npc->SetEliteLevel(iEliteLevel);
	npc->SetGroupEliteLevel(iEliteLevel);
	LC_ActorAttributeMap* attrs = npc->GetAttributeMap();

	attr_value_type real_hp = attrs->GetAttributeBase(ATT_MAX_HP) , real_attk = attrs->GetAttributeBase(ATT_MAX_ATK) , real_def = attrs->GetAttributeBase(ATT_DEF);
	int autofit_level = npc->GetAutoFitLevel();
	if (0 == autofit_level)
	{
		CF_CharType::DataEntry* entry = npc->GetCharTypePtr();
		if (NULL == entry)
		{
			return;
		}

		if (ELT_BLUE_MID == iEliteLevel)
		{
			real_hp = entry->_BlueChar.getHp();
			real_attk = entry->_BlueChar.getAtk();
			real_def =  entry->_BlueChar.getDef();
		}
		else if (ELT_GOLD_MID == iEliteLevel)
		{
			if (IsMinor)
			{
				real_hp = entry->_GoldCharBoy.getHp();
				real_attk = entry->_GoldCharBoy.getAtk();
				real_def =  entry->_GoldCharBoy.getDef();
				npc->SetEliteLevel(0);
			}
			else
			{
				real_hp = entry->_GoldChar.getHp();
				real_attk = entry->_GoldChar.getAtk();
				real_def =  entry->_GoldChar.getDef();
			}
		}
		else if (iEliteLevel >= ELT_STAR_MID_MIN && iEliteLevel < ELT_STAR_MID_MAX)
		{
			const UT_CharHPAD* stars[ELT_STAR_MID_MAX - ELT_STAR_MID_MIN] = {&entry->_StarChar1, &entry->_StarChar2, &entry->_StarChar3, &entry->_StarChar4, &entry->_StarChar5};
			real_hp = stars[iEliteLevel - ELT_STAR_MID_MIN]->getHp();
			real_attk = stars[iEliteLevel - ELT_STAR_MID_MIN]->getAtk();
			real_def = stars[iEliteLevel - ELT_STAR_MID_MIN]->getDef();
		}
	}
	else
	{
		AutoFitNPCManager* afinst = AutoFitNPCManager::GetSingletonPtr();
		CF_AutoFitCharType::DataEntry* entry = afinst->FindData(npc->GetCharType(), autofit_level);
		if (entry == NULL)
			return;
		if (ELT_BLUE_MID == iEliteLevel)
		{
			real_hp = entry->_lMaxHP1;
			real_attk = entry->_lMaxPHA1;
			real_def =  entry->_lPHD1;
		}
		else if (ELT_GOLD_MID == iEliteLevel)
		{
			if (IsMinor)
			{
				real_hp = entry->_lMaxHP3;
				real_attk = entry->_lMaxPHA3;
				real_def =  entry->_lPHD3;
				npc->SetEliteLevel(0);
			}
			else
			{
				real_hp = entry->_lMaxHP2;
				real_attk = entry->_lMaxPHA2;
				real_def =  entry->_lPHD2;
			}
		}
		else if (iEliteLevel >= ELT_STAR_MID_MIN && iEliteLevel < ELT_STAR_MID_MAX)
		{
			const UT_CharHPAD* stars[ELT_STAR_MID_MAX - ELT_STAR_MID_MIN] = {&entry->_StarChar1, &entry->_StarChar2, &entry->_StarChar3, &entry->_StarChar4, &entry->_StarChar5};
			real_hp = stars[iEliteLevel - ELT_STAR_MID_MIN]->getHp();
			real_attk = stars[iEliteLevel - ELT_STAR_MID_MIN]->getAtk();
			real_def = stars[iEliteLevel - ELT_STAR_MID_MIN]->getDef();
		}
	}
	attrs->SetAttributeBase(ATT_MAX_HP, real_hp);
	attrs->SetAttributeBase(ATT_MAX_ATK, real_attk);
	attrs->SetAttributeBase(ATT_DEF, real_def);
	attrs->RecomputeAttribute(ATT_MAX_HP);
	attrs->RecomputeAttribute(ATT_MAX_ATK);
	attrs->RecomputeAttribute(ATT_DEF);
	npc->SetHP(real_hp);
	for (int i = 0; i < skillcount && i < (int)skill_list.size(); ++i)
	{
		int skillid = skill_list[i]->GetSkillTypeID();
		if (IsMinor)
		{
			CF_SkillExecuteList::DataEntry* entry = LC_Helper::SafeGetSkillExecuteDataEntryPtr(skillid, true, true);
			if (entry == NULL || 0 == entry->_lCanLeanrnByMinor)
			{
				continue;
			}
		}
		npc->AddSkill((*skill_list[i]));
	}
}

void Fill_Spawn_Monster_Data(const DungeonConfigData::MonsterData& monster_data, GameLogic::LC_TranscriptionListEntry& entry)
{
	entry.m_kSpawnLocation		= monster_data.m_kSpawnLocation;
	entry.m_iWanderRadius		= monster_data.m_iWanderRadius;
	entry.m_lWanderPathID		= monster_data.m_lWanderPathID;
	entry.m_iSpawnCharType		= monster_data.m_iSpawnCharType;
	entry.m_iZRotAngle			= monster_data.m_iZRotAngle;
	entry.m_iAutoFitDeltaLevel	= monster_data.m_iAutoFitDeltaLevel;
	entry.m_iAutoFitMaxLevel	= monster_data.m_iAutoFitMaxLevel;
	entry.m_iAutoFitMinLevel	= monster_data.m_iAutoFitMinLevel;
}

DungeonConfigData::EliteMonsterType Random_Elite_Monster(const DungeonConfigData::EliteMonsterTypeListType& list)
{
	DungeonConfigData::EliteMonsterType type;
	if (!list.empty())
	{
		type = list.at(UT_MathBase::RandInRangeInt(0, (int)list.size() - 1));
		type.m_Elite = UT_MathBase::RandInRangeInt(2, type.m_Elite);
	}
	return type;
}

void GetRandomPositionOfTheReferencePos(const Utility::UT_Vec3Int& pkReferencePos, mem::vector<Utility::UT_Vec3Int>& posVec, EliteMonsterParam* Param)
{
	UT_Vec3Int pos = pkReferencePos;
	int iRadius = Utility::UT_MathBase::RandInRangeInt(-100, 100);
	pos.x += iRadius;
	iRadius = Utility::UT_MathBase::RandInRangeInt(-100, 100);
	pos.y += iRadius;
	for (int iIndex = 0; iIndex < Param->m_GroupCount; iIndex++)
	{
		pos += Param->m_PostionList[iIndex];
		posVec.push_back(pos);
	}
}
