#include "GameServerApp.h"
#include "time.h"
#include "UT_ServerHelper.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include <boost/algorithm/string/replace.hpp>
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_ServerMapLogic_WarPlay.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "LC_ServerMapLogic_AutoSpawnPlay.h"
#include "LC_ServerMapLogic_ArenaPlay.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_PlayManager.h"
#include "LC_ServerMapLogic_LogicArea.h"
#include "LC_ServerMapLogic_GuildLadderPlay.h"
#include "LC_ServerMapLogic_WatchTowerPlace.h"
#include "LC_ServerMapLogic_SkyPlay.h"
#include "CF_ForcesBattle.h"
#include "LC_ServerMapLogic_TranscriptionRuleManger.h"
#include "CF_WorldMapList.h"
#include "LC_ServerMapLogic_SummitTeamPlay.h"
#include "LC_ServerMapLogic_SummitWheelPlay.h"
using namespace GameLogic;
using namespace Skill;
using namespace Protocol;
using namespace CSVFile;
using namespace Utility;

ResultType LC_ServerMapLogic_PlayManager::AssemblePlay(LC_ServerMapLogic_Place* theplace)
{
	//if (theplace->GetMapLogicType() == MT_EIGHTEEN_BRONZE_MAN) //普通副本
	int32_t MapLogicType = theplace->GetMapLogicType();
	switch (MapLogicType)
	{
	case MT_GUILD_MAP:
	case MT_WATCHTOWER:
	case MT_EIGHTEEN_BRONZE_MAN:
	case MT_REANSCRIPTION_RANK:
	case MT_GUILD_MAP2:
	case MT_GUILD_PARTYFUBEN:
	{
		 //普通副本
		return AssembleNormalRaid(theplace);
	}
	break;
	case MT_GUILDWAR:
	{
		return AssembleGuildWar(theplace);
	}
	break;
	case MT_GUILD_LADDER_WAR:
	{
		return AssembleGuildLadderWar(theplace);
	}
	break;
	case MT_ARENA_PLAYFIGHT:
	case MT_ARENA_SOLO:
	{
		return AssembleSoloDule(theplace, GameLogic::eSoloDuleType_Arena);
	}
	break;
	case MT_ARENA_GUILD_OFFLINE_SOLO:
	{
		return AssembleSoloDule(theplace, GameLogic::eSoloDuleType_GuildOffline);
	}
	break;
	case MT_ARENA_PVP:
	{
		return AssemblePVP(theplace);
	}
	break;
	case MT_HOOK_MAP:
	case MT_NORMAL:
	case MT_NEWBEE:
	{
		return AssembleNormal(theplace);
	}
	break;
	case MT_ARENA_SKY:
	{
		return AssembleSkyWar(theplace);
	}
	break;
	case MT_ARENA_SUMMIT_TEAM:
	{
		return AssembleSummitTeamWar(theplace);
	}
	break;
	case MT_ARENA_SUMMIT:
	{
		return AssembleSummitWheelWar(theplace);
	}
	break;
	default:
		break;
	}
	
	return RE_FAIL;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleNormalRaid(LC_ServerMapLogic_Place* theplace)
{
	//副本必须要有规则
	LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
	CF_TranscriptionRule::DataEntry* pkEntry = rulemanager->GetRule(theplace->GetMapResID());
	if (pkEntry == NULL)
	{
		return RE_FAIL;
	}
	bool autostart = (pkEntry->_iAutoStart == 1);
	//获取配置
	LC_ServerMapLogic_BatchPlay* batch = NULL;
	Dungeon_Config* config = (Dungeon_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL != config)
	{
		batch = T_NEW_D LC_ServerMapLogic_BatchPlay;

		bool settle = true;
		if (pkEntry->_kSettle > 0)
		{
			settle = false;
		}
		bool ret = batch->Init(theplace, config, settle);
		if (!ret)
		{
			batch->UnInit();
			T_DELETE_D batch;
			return RE_PLAY_INIT_ERR;
		}
		theplace->AddPlay2And(batch, autostart);
	}

	LC_ServerMapLogic_AutoSpawnPlay* autoplay = NULL;
	DungeonConfigData* data = DungeonConfigDataManager::GetSingletonPtr()->GetConfigData(theplace->GetMapResID());
	if (data != NULL)
	{
		autoplay = T_NEW_D LC_ServerMapLogic_AutoSpawnPlay;
		bool ret = autoplay->Init(theplace, data->m_AutoSpawnMonsterList);
		if (!ret)
		{
			autoplay->UnInit();
			T_DELETE_D autoplay;
			return RE_PLAY_INIT_ERR;
		}
		theplace->AddPlay2And(autoplay, autostart);
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleNormal(LC_ServerMapLogic_Place* theplace)
{
	LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	theplace->SetPersistTime(0);
	theplace->SetOfflineTime(0);

	//获取配置
	//循环刷怪玩法
	LC_ServerMapLogic_AutoSpawnPlay* autoplay = NULL;
	DungeonConfigData* data = DungeonConfigDataManager::GetSingletonPtr()->GetConfigData(theplace->GetMapResID());
	if (data != NULL)
	{
		autoplay = T_NEW_D LC_ServerMapLogic_AutoSpawnPlay;
		bool ret = autoplay->Init(theplace, data->m_AutoSpawnMonsterList);
		if (!ret)
		{
			autoplay->UnInit();
			T_DELETE_D autoplay;
			return RE_PLAY_INIT_ERR;
		}
		theplace->AddPlay2And(autoplay);
	}

	bool bCommonMap = IS_COMMON_MAP(theplace->GetMapLogicType());
	int iResult = RE_SUCCESS;

	//非野外和新手
	//分线地图
	do
	{
		if (bCommonMap == false || pkWorldMgr->HasTributaryMap(theplace->GetMapResID()) == true)
		{
			CF_ForcesBattle* obj = CF_ForcesBattle::GetSingletonPtr();
			CF_ForcesBattle::DataEntry* data = obj->GetEntryPtr(theplace->GetMapResID());
			if (data == NULL || theplace->getTributaryIndex() != data->_iLineID)
			{
				break;
			}
			LC_ServerMapLogic_WarPlay* war = T_NEW_D LC_ServerMapLogic_WarPlay;
			if (war == NULL)
			{
				if (autoplay != NULL)
				{
					autoplay->UnInit();
					T_DELETE_D autoplay;
				}
				iResult = RE_FAIL;
				break;
			}
			War_Config* config = (War_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
			if (NULL == config)
			{
				if (autoplay != NULL)
				{
					autoplay->UnInit();
					T_DELETE_D autoplay;
				}
				war->UnInit();
				T_DELETE_D war;
				GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize replica play, missing configuration file! mapID:%d.", theplace->GetMapResID());
				iResult = RE_CONFIG_ERR;
				break;
			}
			war->Init(theplace, config);
			war->SetWarType(WarType_DomainWar);
			theplace->AddPlay2And(war, false);
		}
	}while(0);

	if (iResult != RE_SUCCESS)
	{
		return iResult;
	}

	//野外地图
	if(bCommonMap == true || theplace->GetMapLogicType()==MT_HOOK_MAP)
	{
		CF_WorldMapList* pMapList = CF_WorldMapList::GetSingletonPtr();
		CF_WorldMapList::DataEntry* pMapCfg = pMapList->GetEntryPtr(theplace->GetMapResID());
		if(pMapCfg == NULL || pMapCfg->_iLogicPlayType != PLT_AREA)
		{
			return RE_SUCCESS;
		}

		bool bFail = false;
		LC_ServerMapLogic_LogicArea* logicPlay = NULL;
		do 
		{
			logicPlay = T_NEW_D LC_ServerMapLogic_LogicArea;
			if (logicPlay == NULL)
			{
				bFail = true;
				break;
			}
			LogicArea_Config* config = (LogicArea_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
			if (NULL == config)
			{
				bFail = true;
				GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize replica play, missing configuration file! mapID:%d.", theplace->GetMapResID());
				break;
			}
			logicPlay->Init(theplace, config);
			theplace->AddPlay2And(logicPlay, false);
		} while (false);
		
		if(bFail == true)
		{
			if(logicPlay != NULL)
			{
				logicPlay->UnInit();
				T_DELETE_D logicPlay;
			}
			if (autoplay != NULL)
			{
				autoplay->UnInit();
				T_DELETE_D autoplay;
			}
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleGuildWar(LC_ServerMapLogic_Place* theplace)
{
	theplace->SetRecordPK(false);
	theplace->SetOfflineTime(0);
	theplace->SetCountDownTime(60);
	theplace->SetAutoFinishPlayEnd(true);
	LC_ServerMapLogic_WarPlay* war = T_NEW_D LC_ServerMapLogic_WarPlay;
	if (war == NULL)
	{
		return RE_FAIL;
	}
	LC_ServerMapLogic_BatchPlay* batch = T_NEW_D LC_ServerMapLogic_BatchPlay;
	if (batch == NULL)
	{
		return RE_FAIL;
	}
	//获取配置
	War_Config* config = (War_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL == config)
	{
		batch->UnInit();
		T_DELETE_D batch;
		war->UnInit();
		T_DELETE_D war;
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "init copymap error, lose setting file :%d.", theplace->GetMapResID());
		return RE_CONFIG_ERR;
	}
	bool ret = war->Init(theplace, config);
	if (!ret)
	{
		batch->UnInit();
		T_DELETE_D batch;
		war->UnInit();
		T_DELETE_D war;
		return RE_PLAY_INIT_ERR;
	}
	ret = batch->Init(theplace, &(config->Monster), false);
	if (!ret)
	{
		batch->UnInit();
		T_DELETE_D batch;
		war->UnInit();
		T_DELETE_D war;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(batch, false);
	theplace->AddPlay2And(war, false);
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleSoloDule(LC_ServerMapLogic_Place* theplace, GameLogic::SOLO_DULE_TYPE eSoloType)
{
	theplace->SetOfflineTime(0);
	theplace->SetAutoFinishAllPlayerLeave(true);
	theplace->SetAutoFinishPlayEnd(true);
	LC_ServerMapLogic_SoloDulePlay* solodule = T_NEW_D LC_ServerMapLogic_SoloDulePlay;
	if (solodule == NULL)
	{
		return RE_FAIL;
	}
	//获取配置
	LC_Dule_Config* config = (LC_Dule_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL == config)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize replica play, missing configuration file! mapID:%d.", theplace->GetMapResID());
		return RE_CONFIG_ERR;
	}
	bool ret = solodule->Init(theplace, config, eSoloType);
	if (!ret)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(solodule, true);
	return RE_SUCCESS;
}
ResultType LC_ServerMapLogic_PlayManager::AssemblePVP(LC_ServerMapLogic_Place* theplace)
{
	theplace->SetOfflineTime(0);
	theplace->SetAutoFinishAllPlayerLeave(true);
	theplace->SetAutoFinishPlayEnd(true);

	LC_ServerMapLogic_ArenaPlay* pvp = T_NEW_D LC_ServerMapLogic_ArenaPlay;
	if (pvp == NULL)
	{
		return RE_FAIL;
	}
	//获取配置
	LC_Dule_Config* config = (LC_Dule_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL == config)
	{
		pvp->UnInit();
		T_DELETE_D pvp;
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize replica play, missing configuration file! mapID:%d.", theplace->GetMapResID());
		return RE_CONFIG_ERR;
	}
	bool ret = pvp->Init(theplace, config);
	if (!ret)
	{
		pvp->UnInit();
		T_DELETE_D pvp;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(pvp, true);
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleGuildLadderWar(LC_ServerMapLogic_Place* theplace)
{
	LC_ServerMapLogic_GuildLadderPlay* pkGuildLadderPlay = T_NEW_D LC_ServerMapLogic_GuildLadderPlay;
	if (pkGuildLadderPlay == NULL)
	{
		return RE_FAIL;
	}
	bool ret = pkGuildLadderPlay->Init(theplace);
	if (!ret)
	{
		pkGuildLadderPlay->UnInit();
		T_DELETE_D pkGuildLadderPlay;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(pkGuildLadderPlay, true);
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleSkyWar(LC_ServerMapLogic_Place* theplace)
{
	theplace->SetOfflineTime(0);
	theplace->SetAutoFinishAllPlayerLeave(true);
	theplace->SetAutoFinishPlayEnd(true);
	LC_ServerMapLogic_SkyPlay* solodule = T_NEW_D LC_ServerMapLogic_SkyPlay;
	if (solodule == NULL)
		return RE_FAIL;
	//获取配置
	LC_Dule_Config* config = (LC_Dule_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL == config)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize LC_ServerMapLogic_SkyPlay  play, missing configuration file! mapID:%d.", theplace->GetMapResID());
		return RE_CONFIG_ERR;
	}
	bool ret = solodule->Init(theplace, config);
	if (!ret)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(solodule, true);
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleSummitTeamWar(LC_ServerMapLogic_Place* theplace)
{
	theplace->SetOfflineTime(0);
	theplace->SetAutoFinishAllPlayerLeave(true);
	theplace->SetAutoFinishPlayEnd(true);
	LC_ServerMapLogic_SummitTeamPlay* solodule = T_NEW_D LC_ServerMapLogic_SummitTeamPlay;
	if (solodule == NULL)
		return RE_FAIL;
	//获取配置
	LC_Dule_Config* config = (LC_Dule_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL == config)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize LC_ServerMapLogic_SummitTeamPlay  play, missing configuration file! mapID:%d.", theplace->GetMapResID());
		return RE_CONFIG_ERR;
	}
	bool ret = solodule->Init(theplace, config);
	if (!ret)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(solodule, true);
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_PlayManager::AssembleSummitWheelWar(LC_ServerMapLogic_Place* theplace)
{
	theplace->SetOfflineTime(0);
	theplace->SetAutoFinishAllPlayerLeave(true);
	theplace->SetAutoFinishPlayEnd(true);
	LC_ServerMapLogic_SummitWheelPlay* solodule = T_NEW_D LC_ServerMapLogic_SummitWheelPlay;
	if (solodule == NULL)
		return RE_FAIL;
	//获取配置
	LC_Dule_Config* config = (LC_Dule_Config*)(DungeonConfigDataManager::GetSingletonPtr()->GetConfigDataEx(theplace->GetMapResID()));
	if (NULL == config)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "Failed to initialize LC_ServerMapLogic_SummitWheelPlay  play, missing configuration file! mapID:%d.", theplace->GetMapResID());
		return RE_CONFIG_ERR;
	}
	bool ret = solodule->Init(theplace, config);
	if (!ret)
	{
		solodule->UnInit();
		T_DELETE_D solodule;
		return RE_PLAY_INIT_ERR;
	}
	theplace->AddPlay2And(solodule, true);
	return RE_SUCCESS;
}