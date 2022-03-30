#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_SequenceManager.h"

#include "LC_GameEvent.h"
#include "LC_GameStory_Transcript.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMap.h"

#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerGroup.h"

#include "MG_Game2DB.h"
#include "NW_Helper.h"
#include "LC_ServerNPC.h"

#include "GameServerApp.h"
#include "LC_ServerPlayerGroupManager.h"


#include "UT_Timer.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_GameStory_Manager.h"

// 
// #include "TS_TranManager.h"
// #include "TS_TranType.h"
// 

using namespace GameLogic;
using namespace GameStory;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

/******************************* Class Implement *****************************************/

LC_GameStory_Transcript::LC_GameStory_Transcript(int logicID, GameStoryRecord *pkRecord)
:LC_Server_GameStory_Base(logicID, pkRecord)
{
	
}
LC_GameStory_Transcript::~LC_GameStory_Transcript()
{

}
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
void LC_GameStory_Transcript::Init()
{
	LC_Server_GameStory_Base::Init();
}

//---------------------------------------------------------------------
void LC_GameStory_Transcript::UnInit()
{
	LC_Server_GameStory_Base::UnInit();
}

void LC_GameStory_Transcript::ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured)
{
	LC_Server_GameStory_Base::ActorBeKilled(pkKiller, pkInjured);
}
void LC_GameStory_Transcript::DoTranscriptionEntered(LC_ServerMapLogic_Place* pkPlace, LC_ServerPlayer *pkPlayer)
{
	//进阶副本的挑战次数是先购买，再挑战
	//其他的副本是挑战时候，如果钻石足够，直接扣钻石，进副本
	//所以对进阶副本特殊处理，能调到这个函数，挑战次数肯定足够，所以直接return
	int mapID = pkPlace->GetMapResID();
	CF_GameStory::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_GameStory>(mapID);
	if(NULL == pkEntry)
	{
		return;
	}

	//12：该副本类型是进阶副本
	if(pkEntry->_lTabType == 12)
	{
		return;
	}

	GameStoryConfig *pkConfig = GetConfig();
	if(pkPlace && pkPlayer && pkConfig)
	{
		int32_t lMapResID = pkPlace->GetMapResID();
		MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
		const MapSetting* pkMapSetting = kMapCtrlSetting.GetMapSetting(lMapResID);

		if (pkMapSetting)
		{
			GameStoryEntry* pkStoryEntry = pkPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
			if (NULL == pkStoryEntry)
			{
				return;
			}

			LC_StatType eStatType = LC_ST_TP_NA;
			pkStoryEntry->ResolveCheckType(eStatType);
			GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
			if (NULL == pkStat)
			{
				return;
			}

			LC_ShopIDMap kShopIDMap;
			pkConfig->GetMapEnterShopIDMap(lMapResID, pkStoryEntry, kShopIDMap);
			if (!kShopIDMap.empty())
			{
				LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_STORY_TIMES, lMapResID);
			}
		}
	}
}

void LC_GameStory_Transcript::DoTranscriptionFinished(LC_ServerMapLogic_Place* pkPlace, GSTranscriptResult &trans)
{
	if (NULL == pkPlace)
	{
		return;
	}
	GameStoryConfig *pkConfig = GetConfig();
	if(NULL == pkConfig)
	{
		return;
	}
	LC_MapBase* pkMap = pkPlace->GetMap();
	if (NULL == pkMap)
	{
		return;
	}

	MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	const MapSetting* pkMapSetting = kMapCtrlSetting.GetMapSetting(trans.lMapResID);
	if (NULL == pkMapSetting)
	{
		return;
	}

	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;

	GSRTRankEntry* pkRTRankEntry = m_kRTRankInfo.GetRankEntry(RankID(GS_RANK_TP_RAID_CLEAR_TIME, trans.lMapResID));

	for (GameLogic::LC_IDSet::iterator it=trans.kSucIds.begin(); it!=trans.kSucIds.end(); ++it)
	{
		if (!IS_PLAYER_ID(*it))
		{
			continue;
		}

		LC_ServerPlayer *pkPlayer = (LC_ServerPlayer *)(pkMap->FindPlayer(*it));
		if(pkPlayer)
		{
			PlayerFinish(pkPlayer, pkPlayer->GetMapResID());

			GSRTPlayerCommonStat* pkPlyCmnStat = GetPlayerStats(pkPlayer);
			if (pkPlyCmnStat && !pkPlyCmnStat->IsAccumRewarded())
			{
				GSServerReward gsReward;
				GetCurrentStatReward(pkPlayer->GetInstance(), gsReward);

				gsReward.Reward(pkPlayer, GetStoryID());

				pkPlyCmnStat->SetAccumRewarded(true);

				if (pkRTRankEntry)
				{
					const RaidInfo* pkRaid = pkPlayer->GetRaidAsset()->FindRaid(trans.lMapResID);
					if (pkRaid && pkRaid->m_iClearRecord<trans.lClearTime)
					{
						pkPlyCmnStat->SetRaidClearRecord(pkRaid->m_iRaidId, trans.lClearTime);
						pkRTRankEntry->Update(pkPlyCmnStat);
					}
				}
			}

			GameStoryEntry* pkStoryEntry = pkPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
			if (NULL == pkStoryEntry)
			{
				continue;
			}

			GameStoryRoundStat st = pkConfig->GetTranscriptionRoundState(pkStoryEntry, trans.lMapResID);
			if(st != GS_RT_ST_PROGRESS)
			{
				continue;
			}
		}
	}
	
	for (LC_IDSet::iterator it = trans.kFailIds.begin(); it!=trans.kFailIds.end(); ++it)
	{
		if (!IS_PLAYER_ID(*it))
		{
			continue;
		}
		LC_ServerPlayer *pkPlayer = (LC_ServerPlayer *)(pkMap->FindPlayer(*it));
		if(pkPlayer)
		{
			GSRTPlayerCommonStat* pkPlyCmnStat = GetPlayerStats(pkPlayer);
			if (NULL == pkPlyCmnStat || pkPlyCmnStat->IsAccumRewarded())
			{
				continue;
			}

			
			GSServerReward gsReward;
			GetCurrentStatReward(pkPlayer->GetInstance(), gsReward);

			gsReward.Reward(pkPlayer, GetStoryID());

			pkPlyCmnStat->SetAccumRewarded(true);
		}
	}
}

