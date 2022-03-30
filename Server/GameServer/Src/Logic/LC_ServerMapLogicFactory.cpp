#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_NewBee.h"
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_Server_War_Config.h"
#include "LC_ServerSector.h"
#include "LC_ServerMapLogic_LogicArea.h"
#include "GlobalSettings.h"
using namespace Utility;
using namespace GameLogic;
using namespace CSVFile;


//nMap1Size	476	int	500 500 - 476 = 24
#define		MAX_SIEZ_1_MAP_COUNT		30
//nMap4Size	1	int	1 1 - 1 = 0
#define		MAX_SIEZ_4_MAP_COUNT		1
//nMap8Size	300	int	300  300 - 300 = 0
#define		MAX_SIEZ_8_MAP_COUNT		10
//nMap16Size	281	int	300 300 - 281 = 19
#define		MAX_SIEZ_16_MAP_COUNT		30
//nMap32Size	284	int	300 300 - 284 = 16
#define		MAX_SIEZ_32_MAP_COUNT		30
//nMap48Size	695	int	700 700 - 695 = 5
#define		MAX_SIEZ_48_MAP_COUNT		10
//nMapLogicNewBeeSize	394	int	400  400 - 394 = 6
#define		MAX_LOGIC_NEW_BEE_COUNT		12
//nMapLogicBatchSize	3000	int	3000 3000 - 3000 = 0
#define		MAX_LOGIC_BATCH_COUNT		10
//nMapLogicPlaceSize	3942	int	4000 4000 = 3942 = 58
#define		MAX_LOGIC_PLACE_COUNT		80
#define		MAX_LOGIC_RANK_PLACE_COUNT		80
#define		MAX_LOGIC_WATCHTOWER_COUNT 10
#define		MAX_LOGIC_GUILDMAP_COUNT 80
#define		MAX_LOGIC_STORYGUILDMAP_COUNT 80



//------------------------------------------------------------------------
LC_ServerMapLogicFactory::LC_ServerMapLogicFactory()
{
	m_pkMapLogicPlacePool = NULL;
	m_pkMapLogicNewBeePool = NULL;
	m_pkMapLogicBatchPool = NULL;
	m_pkMapLogicRankPlacePool = NULL;
	m_pkMapLogicWatchTowerPlacePool = NULL;
	m_pkMapLogicGuildMapPlacePool = NULL;
	m_pkMapLogicGuildStoryMapPlacePool = NULL;

	m_pkMap_1_1_Pool = NULL;
	m_pkMap_4_4_Pool = NULL;
	m_pkMap_8_8_Pool = NULL;
	m_pkMap_16_16_Pool = NULL;
	m_pkMap_32_32_Pool = NULL;
	m_pkMap_48_48_Pool = NULL;

	m_pkServerSector_Pool = NULL;
}
//------------------------------------------------------------------------
LC_ServerMapLogicFactory::~LC_ServerMapLogicFactory()
{
	UnInit();
}
//------------------------------------------------------------------------
void LC_ServerMapLogicFactory::Init()
{
	m_pkMapLogicPlacePool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_Place>;
	m_pkMapLogicNewBeePool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_NewBee>;
	m_pkMapLogicBatchPool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_Batch>;
	m_pkMapLogicRankPlacePool =T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_RankPlace>;
	m_pkMapLogicWatchTowerPlacePool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_WatchTowerPlace>;
	m_pkMapLogicGuildMapPlacePool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_GuildMapPlace>;
	m_pkMapLogicGuildStoryMapPlacePool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMapLogic_GuildStoryMapPlace>;

	m_pkMapLogicPlacePool->Init(MAX_LOGIC_PLACE_COUNT, 1);
	m_pkMapLogicNewBeePool->Init(MAX_LOGIC_NEW_BEE_COUNT, 1);
	m_pkMapLogicBatchPool->Init(MAX_LOGIC_BATCH_COUNT, 1);
	m_pkMapLogicRankPlacePool->Init(MAX_LOGIC_RANK_PLACE_COUNT, 1);
	m_pkMapLogicWatchTowerPlacePool->Init(MAX_LOGIC_WATCHTOWER_COUNT, 1);
	m_pkMapLogicGuildMapPlacePool->Init(MAX_LOGIC_GUILDMAP_COUNT, 1);
	m_pkMapLogicGuildStoryMapPlacePool->Init(MAX_LOGIC_STORYGUILDMAP_COUNT, 1);

	m_pkMap_1_1_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMap_Template<0> >;
	m_pkMap_4_4_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMap_Template<1> >;
	m_pkMap_8_8_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMap_Template<2> >;
	m_pkMap_16_16_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMap_Template<3> >;
	m_pkMap_32_32_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMap_Template<4> >;
	m_pkMap_48_48_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerMap_Template<5> >;
	m_pkServerSector_Pool = T_NEW_D Utility::UT_MemoryPool<LC_ServerSector>;
	m_pkMap_1_1_Pool->Init(MAX_SIEZ_1_MAP_COUNT, 1);
	m_pkMap_4_4_Pool->Init(MAX_SIEZ_4_MAP_COUNT, 1);
	m_pkMap_8_8_Pool->Init(MAX_SIEZ_8_MAP_COUNT, 1);
	m_pkMap_16_16_Pool->Init(MAX_SIEZ_16_MAP_COUNT, 1);
	m_pkMap_32_32_Pool->Init(MAX_SIEZ_32_MAP_COUNT, 1);
	m_pkMap_48_48_Pool->Init(MAX_SIEZ_48_MAP_COUNT, 1);
	{
		const Utility::MemPoolConfigEntry & plc = GetGlobalSetting.poolSector;
		m_pkServerSector_Pool->Init(plc.InitialCount, plc.IncCount, "LC_ServerMapLogicFactory.m_pkServerSector_Pool");
		m_pkServerSector_Pool->EnableAutoDec(plc.AutoDec, plc.AutoDecInterval, plc.AutoDecCount, plc.AutoDecMinCount);
	}
}
//------------------------------------------------------------------------
void LC_ServerMapLogicFactory::UnInit()
{
	T_SAFE_DELETE(m_pkMapLogicPlacePool);
	T_SAFE_DELETE(m_pkMapLogicNewBeePool);
	T_SAFE_DELETE(m_pkMapLogicBatchPool);
	T_SAFE_DELETE(m_pkMapLogicRankPlacePool);
	T_SAFE_DELETE(m_pkMapLogicWatchTowerPlacePool);
	T_SAFE_DELETE(m_pkMapLogicGuildMapPlacePool);
	T_SAFE_DELETE(m_pkMapLogicGuildStoryMapPlacePool);

	T_SAFE_DELETE(m_pkMap_1_1_Pool);
	T_SAFE_DELETE(m_pkMap_4_4_Pool);
	T_SAFE_DELETE(m_pkMap_8_8_Pool);
	T_SAFE_DELETE(m_pkMap_16_16_Pool);
	T_SAFE_DELETE(m_pkMap_32_32_Pool);
	T_SAFE_DELETE(m_pkMap_48_48_Pool);
	T_SAFE_DELETE(m_pkServerSector_Pool);
}

void LC_ServerMapLogicFactory::Update(float fCurrentTime)
{
	//< 更新对象池
	m_pkServerSector_Pool->Update(fCurrentTime);
}

//------------------------------------------------------------------------

DungeonConfig::Logic_ConfigData_Base* LC_ServerMapLogicFactory::ReadConfig(int iLogicPlayType, const StringType& configFileName)
{
	DungeonConfig::Logic_ConfigData_Base* config = NULL;
	switch (iLogicPlayType)
	{
		//PVE副本
		case PLT_BRONZE:
			{
				if (configFileName.find(".json") == StringType::npos)
				{
					config = NULL;
					break;
				}
				Dungeon_Config* dungeon_data = T_NEW_D Dungeon_Config;
				bool ret = dungeon_data->Init(configFileName);
				if (!ret)
				{
					GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "load json file error!--------------------------------------------------------");
					T_SAFE_DELETE(dungeon_data);
					config = NULL;
					T_ASSERT_CRITICAL_S(false);
					return NULL;
				}
				else
				{
					config = (DungeonConfig::Logic_ConfigData_Base*)dungeon_data;
				}
				//检查怪物
				CF_CharType* CharManager = CF_CharType::GetSingletonPtr();
				MonsterGroupSetType::iterator beg = dungeon_data->MonsterGroup.begin();
				for (; beg != dungeon_data->MonsterGroup.end(); ++beg)
				{
					mem::vector<MonsterEntry>::iterator monsterbeg = beg->begin();
					for (; monsterbeg != beg->end(); ++monsterbeg)
					{
						if (CharManager->GetEntryPtr(monsterbeg->TypeId) == NULL)
						{
							GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "invaild monsterID, %s, %d", configFileName.c_str(), monsterbeg->TypeId);
							T_ASSERT_CRITICAL_S(false);
						}
					}
				}

				DoorSetType::iterator doorbeg = dungeon_data->Doors.begin();
				for (; doorbeg != dungeon_data->Doors.end(); ++doorbeg)
				{
					if (CharManager->GetEntryPtr(doorbeg->TypeId) == NULL)
					{
						GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "invaild monsterID, %s, %d", configFileName.c_str(), doorbeg->TypeId);
						T_ASSERT_CRITICAL_S(false);
					}
				}

				TargetSetType::iterator targetbeg = dungeon_data->Targets.begin();
				for (; targetbeg != dungeon_data->Targets.end(); ++targetbeg)
				{
					if (CharManager->GetEntryPtr(targetbeg->TypeId) == NULL)
					{
						GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "invaild monsterID, %s, %d", configFileName.c_str(), targetbeg->TypeId);
						T_ASSERT_CRITICAL_S(false);
					}
				}
				//检查波次
				mem::set<int> ids;
				int count = 0;
				int possize = (int)dungeon_data->PosGroup.size();
				int monstersize = (int)dungeon_data->MonsterGroup.size();
				int elitesize = (int)dungeon_data->EliteGroup.size();
				BatchSetType::iterator batchbeg = dungeon_data->Batchs.begin();
				for (; batchbeg != dungeon_data->Batchs.end(); ++batchbeg)
				{
					LongSetType::iterator idbeg = batchbeg->Scope.begin();
					for (; idbeg != batchbeg->Scope.end(); ++idbeg)
					{
						if (ids.count(*idbeg) > 0)
						{
							GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "failName:%s, batchIndex Scope same:%d", configFileName.c_str(), *idbeg);
							T_ASSERT_CRITICAL_S(false);
						}
						ids.insert(*idbeg);
						++count;
					}
					//检查怪
					int i = 0;
					BatchGroupSetType::iterator monsterbeg = batchbeg->Group.begin();
					for (; monsterbeg != batchbeg->Group.end(); ++monsterbeg, ++i)
					{
						LongSetType::iterator posbeg = monsterbeg->PosIndex.begin();
						for (; posbeg != monsterbeg->PosIndex.end(); ++posbeg)
						{
							if (*posbeg >= possize)
							{
								GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "index pos index! failName:%s, batchIndex:%d, index:%d", configFileName.c_str(), i, *posbeg);
								T_ASSERT_CRITICAL_S(false);
							}
						}

						MonsteWeightSetType::iterator weightbeg = monsterbeg->MonsterCombins.begin();
						for (; weightbeg != monsterbeg->MonsterCombins.end(); ++weightbeg)
						{
							if (weightbeg->MonsterIndex >= monstersize)
							{
								GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "invaild master index! failName:%s, batchIndex:%d, index:%d", configFileName.c_str(), i, weightbeg->MonsterIndex);
								T_ASSERT_CRITICAL_S(false);
							}
						}
					}
					//检查精英
					EliteWeightSetType::iterator elitebeg = batchbeg->Elite.begin();
					for (; elitebeg != batchbeg->Elite.end(); ++elitebeg)
					{
						if (elitebeg->EliteIndex >= elitesize)
						{
							GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "invaild ElitMonster index! failName:%s, batchIndex:%d, index:%d", configFileName.c_str(), i, elitebeg->EliteIndex);
							T_ASSERT_CRITICAL_S(false);
						}
					}
				}
				//检查波次是否连续
				for (int i = 0; i < count; ++i)
				{
					if (ids.count(i) <= 0)
					{
						GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "failName:%s, currentNum Scope:%d", configFileName.c_str(), i);
						T_ASSERT_CRITICAL_S(false);
					}
				}
				//检查通知编号
				LongSetType::iterator notifybeg = dungeon_data->BatchsNotify.begin();
				for (; notifybeg != dungeon_data->BatchsNotify.end(); ++notifybeg)
				{
					if (*notifybeg >= count)
					{
						GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "failName:%s, currentNum BatchsNotify num:%d，totalNum:%d", configFileName.c_str(), *notifybeg, count);
						T_ASSERT_CRITICAL_S(false);
					}
				}
			}
			break;
		case PLT_SOLO:
			{
				if (configFileName.find(".json") == StringType::npos)
				{
					config = NULL;
					break;
				}
				LC_Dule_Config* dungeon_data = T_NEW_D LC_Dule_Config;
				bool ret = dungeon_data->Init(configFileName);
				if (!ret)
				{
					GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "load json file error--------------------------------------------------------");
					T_SAFE_DELETE(dungeon_data);
					config = NULL;
					T_ASSERT_CRITICAL_S(false);
				}
				else
				{
					config = (DungeonConfig::Logic_ConfigData_Base*)dungeon_data;
				}
			}
			break;
		case PLT_GUILD:
			{
				if (configFileName.find(".json") == StringType::npos)
				{
					config = NULL;
					break;
				}
				War_Config* guildwar_data = T_NEW_D War_Config;
				bool ret = guildwar_data->Init(configFileName);
				if (!ret)
				{
					GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "load json file error--------------------------------------------------------");
					T_SAFE_DELETE(guildwar_data);
					config = NULL;
					T_ASSERT_CRITICAL_S(false);
				}
				else
				{
					config = (DungeonConfig::Logic_ConfigData_Base*)guildwar_data;
				}
			}
			break;
		case PLT_AREA:
			{
				if (configFileName.find(".json") == StringType::npos)
				{
					config = NULL;
					break;
				}
				LogicArea_Config* logicCfg = T_NEW_D LogicArea_Config;
				bool ret = logicCfg->Init(configFileName);
				if (!ret)
				{
					GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "load json file error!--------------------------------------------------------");
					T_SAFE_DELETE(logicCfg);
					config = NULL;
					T_ASSERT_CRITICAL_S(false);
				}
				else
				{
					config = (DungeonConfig::Logic_ConfigData_Base*)logicCfg;
				}
			}
			break;
		default:
			{
				config =  NULL;
			}
			break;
	}
	return config;
}

//------------------------------------------------------------------------
LC_ServerMapLogic_Place* LC_ServerMapLogicFactory::RequestMapLogic(int iMapLogicType)
{
	LC_ServerMapLogic_Place* pkMapLogic = NULL;
	switch (iMapLogicType)
	{
		case MT_NORMAL:
		case MT_ARENA_PVP:
		case MT_ARENA_SOLO:
		case MT_ARENA_PLAYFIGHT:
		case MT_ARENA_GUILD_OFFLINE_SOLO:
		case MT_EIGHTEEN_BRONZE_MAN:
		case MT_GUILDWAR:
		case MT_ARENA_SKY:
		case MT_ARENA_SUMMIT:
		case MT_ARENA_SUMMIT_TEAM:
		case MT_HOOK_MAP:
		case MT_GUILD_LADDER_WAR:
		case MT_GUILD_PARTYFUBEN:
			{
				pkMapLogic = m_pkMapLogicPlacePool->Alloc();
			}
			break;
		//普通地图
		case MT_NEWBEE:
			{
				pkMapLogic = m_pkMapLogicNewBeePool->Alloc();
			}
			break;
		//排行副本(异种侵袭)
		case MT_REANSCRIPTION_RANK:
			{
				pkMapLogic = m_pkMapLogicRankPlacePool->Alloc();
			}
			break;
		case MT_WATCHTOWER:
			{
				pkMapLogic = m_pkMapLogicWatchTowerPlacePool->Alloc();
			}
			break;
		case MT_GUILD_MAP:
			{
				pkMapLogic = m_pkMapLogicGuildMapPlacePool->Alloc();
			}
			break;
		case MT_GUILD_MAP2:
			{
				pkMapLogic = m_pkMapLogicGuildStoryMapPlacePool->Alloc();
			}
			break;		
		default:
			break;
	}

	if (pkMapLogic)
	{
		pkMapLogic->SetMapLogicType((MapType)iMapLogicType);
	}
	else
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RequestMapLogic map error! mapType:[%d]", iMapLogicType);
	}
	return pkMapLogic;
}
//------------------------------------------------------------------------
void LC_ServerMapLogicFactory::ReleaseMapLogic(LC_ServerMapLogic_Place* pkMapLogic)
{
	switch (pkMapLogic->GetMapLogicType())
	{
		//普通地图
		case MT_NEWBEE:
			{
				m_pkMapLogicNewBeePool->Free((LC_ServerMapLogic_NewBee*)pkMapLogic);
			}
			break;
		//普通地图
		case MT_NORMAL:
		//竞技场地图
		case MT_ARENA_PLAYFIGHT:
		case MT_ARENA_SOLO:
		case MT_ARENA_PVP:
		case MT_EIGHTEEN_BRONZE_MAN:
		case MT_GUILDWAR:
		case MT_ARENA_SKY:
		case MT_ARENA_SUMMIT:
		case MT_ARENA_SUMMIT_TEAM:
		case MT_GUILD_LADDER_WAR:
		case MT_GUILD_PARTYFUBEN:
			{
				m_pkMapLogicPlacePool->Free(pkMapLogic);
			}
			break;
		//排行副本(异种侵袭)
		case MT_REANSCRIPTION_RANK:
			{
				m_pkMapLogicRankPlacePool->Free((LC_ServerMapLogic_RankPlace*)pkMapLogic);
			}
			break;
		case MT_WATCHTOWER:
			{
				m_pkMapLogicWatchTowerPlacePool->Free((LC_ServerMapLogic_WatchTowerPlace*)pkMapLogic);
			}
			break;
		case MT_GUILD_MAP:
			{
				m_pkMapLogicGuildMapPlacePool->Free((LC_ServerMapLogic_GuildMapPlace*)pkMapLogic);
			}
			break;
		case MT_GUILD_MAP2:
			{
				m_pkMapLogicGuildStoryMapPlacePool->Free((LC_ServerMapLogic_GuildStoryMapPlace*)pkMapLogic);
			}
			break;
		default:
			break;
	}
}

LC_ServerMap* LC_ServerMapLogicFactory::RequestMap(LC_MapSizeType mapsize)
{
	LC_ServerMap* themap = NULL;
	switch (mapsize)
	{
		case LMST_1_1:
			{
				themap = m_pkMap_1_1_Pool->Alloc();
			}
			break;
		case LMST_4_4:
			{
				themap = m_pkMap_4_4_Pool->Alloc();
			}
			break;
		case LMST_8_8:
			{
				themap = m_pkMap_8_8_Pool->Alloc();
			}
			break;
		case LMST_16_16:
			{
				themap = m_pkMap_16_16_Pool->Alloc();
			}
			break;
		case LMST_32_32:
			{
				themap = m_pkMap_32_32_Pool->Alloc();
			}
			break;
		case LMST_48_48:
			{
				themap = m_pkMap_48_48_Pool->Alloc();
			}
			break;
		default:
			break;
	}
	return themap;
}

void LC_ServerMapLogicFactory::ReleaseMap(LC_ServerMap* pkServerMap)
{
	LC_MapSizeType mapsize = (LC_MapSizeType)pkServerMap->GetMapSizeType();
	switch (mapsize)
	{
		case LMST_1_1:
			{
				m_pkMap_1_1_Pool->Free((LC_ServerMap_Template<0>*)pkServerMap);
			}
			break;
		case LMST_4_4:
			{
				m_pkMap_4_4_Pool->Free((LC_ServerMap_Template<1>*)pkServerMap);
			}
			break;
		case LMST_8_8:
			{
				m_pkMap_8_8_Pool->Free((LC_ServerMap_Template<2>*)pkServerMap);
			}
			break;
		case LMST_16_16:
			{
				m_pkMap_16_16_Pool->Free((LC_ServerMap_Template<3>*)pkServerMap);
			}
			break;
		case LMST_32_32:
			{
				m_pkMap_32_32_Pool->Free((LC_ServerMap_Template<4>*)pkServerMap);
			}
			break;
		case LMST_48_48:
			{
				m_pkMap_48_48_Pool->Free((LC_ServerMap_Template<5>*)pkServerMap);
			}
			break;
		default:
			break;
	}
}

LC_ServerMapLogic_Batch* LC_ServerMapLogicFactory::RequestBatch()
{
	LC_ServerMapLogic_Batch* batch = m_pkMapLogicBatchPool->Alloc();
	return batch;
}

void LC_ServerMapLogicFactory::ReleaseBatch(LC_ServerMapLogic_Batch* batch)
{
	m_pkMapLogicBatchPool->Free(batch);
}

LC_ServerSector* LC_ServerMapLogicFactory::RequestSector()
{
	LC_ServerSector* s = m_pkServerSector_Pool->Alloc();
	s->Reset();
	return s;
}

void LC_ServerMapLogicFactory::ReleaseSector(LC_ServerSector* s)
{
	s->Reset();
	m_pkServerSector_Pool->Free(s);
}
