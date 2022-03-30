#include "GameLogicScriptPCH.h"
#include "IdDefine.h"

#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ItemFactoryBase.h"
#include "LC_GameEvent.h"
#include "LC_GameStoryBase.h"
#include "LC_GameStory_FactionWar.h"
#include "LC_ServerMap.h"
#include "LC_PackEntryFactory.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerNPC.h"
#include "LC_ServerNPCManager.h"
#include "LC_RankManager.h"
#include "MG_Common.h"
#include "UT_Timer.h"

#include "MG_Game2DB.h"
#include "NW_Helper.h"

#include "GameServerApp.h"
#include "LC_ServerPlayerGroupManager.h"

#include "LC_GameEventManager.h"
#include "UT_ServerHelper.h"
#include "UT_Shape.h"
#include "UT_PathFindHelp.h"
#include "UT_MiscFuncs.h"

#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"

#include "LC_ServerTask.h"
#include "LC_ServerTaskMap.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_CrossRealmRaidManager.h"
#include "LC_ServerGuildManager.h"

#include "LC_StoryLogic_Manager.h"
#include "LC_GameStory_Manager.h"
#include "LC_TaskManager.h"

#include "LC_ServerItemFlopManager.h"
#include "TS_TranManager.h"
#include "oss_define.h"
#include "MG_Game2Alliance.h"
#include "CF_EndlessTower.h"
#include "MG_GameProtocol.h"
#include "CF_BestGuildBossRankRewards.h"
#include "CF_RegularBoss.h"
#include "CF_EscortReward.h"
#include "CF_ACTShopA.h"
#include "LC_ServerGoalAsset.h"
#include "CF_GuildTower.h"
#include "CF_GuildTowerRankReward.h"
#include "UT_OperateLog.h"


using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;
using namespace GameStory;
/******************************* Class Implement *****************************************/

void GSServerReward::Reward(GameLogic::LC_PlayerBase* pkPlayer, int lStroyID, int nLogCode, const vector<int>& nParams)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	do
	{
		attr_value_type iExp = Utility::Round2Int64(dExp);
		MoneyType iCash = Utility::Round2Int64(dCash);
		attr_value_type iSP = Utility::Round2Int64(dSP);
		MoneyType iGuild = Utility::Round2Int64(dGuild);
		if (iExp)
		{
			pkPlayer->AddExpWithGamePlay(iExp, SYS_FUNC_TP_STORY_REWARD, lStroyID);
		}

		if (iCash)
		{
			pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_STORY_REWARD, lStroyID, 0, true, NULL, nLogCode, nParams);
		}

		if (iSP)
		{
			pkPlayer->AddSkillLevelupPoints(iSP, SYS_FUNC_TP_STORY_REWARD, lStroyID);
		}

		if (iGuild)
		{
			pkPlayer->AddGuildMoney(iGuild, SYS_FUNC_TP_STORY_REWARD, lStroyID);
		}

		if (!kItems.empty())
		{
			UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_STORY_REWARD, lStroyID, sMailTitle, sMailDesc, true, false, false, nLogCode, nParams);
		}

		if (!kScores.empty())
		{
			LC_GameStoryAsset& kGSAsset = pkPlayer->GetGameStoryAsset();

			Utility::UT_SIMDataList::iterator it = kScores.begin();
			for (; it != kScores.end(); ++it)
			{
				kGSAsset.SetScore(it->ID(), kGSAsset.GetScore(it->ID()) + it->ParamA());
			}
		}
	}
	while (0);
}

void GSServerReward::Email(const MailPlayerMap& kPlayers,int nLogCode, const vector<int>& nParams)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();

	if (NULL == pkItemFactory || NULL == pkPackEntryFactory)
	{
		return;
	}

	int iExp = Utility::Round2Int(dExp);
	int iCash = Utility::Round2Int(dCash);
	int iSP = Utility::Round2Int(dSP);
	int iGuild = Utility::Round2Int(dGuild);

	LC_BackPackEntryList kEntryList;

	int32_t iItemTypeID = 0;
	int32_t iItemCount = 0;
	if (ResolveExpItem(iExp, iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveSPItem((int32_t)iSP, iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveGuildItem((int32_t)iGuild, iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}

	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList, true);
	}

	MailPlayerMap::const_iterator citPlayer = kPlayers.begin();
	for (; citPlayer != kPlayers.end(); ++citPlayer)
	{
		unique_id_type id = citPlayer->first;
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(id));
		UT_ServerHelper::SendToMail(sMailTitle, sMailDesc, citPlayer->first, citPlayer->second, iCash, kEntryList);
	}
}
void GSServerReward::Email(mem::set<int32_t>& kServerIDs, int nLogCode, const vector<int>& nParams)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();

	if (NULL == pkItemFactory || NULL == pkPackEntryFactory)
	{
		return;
	}

	int iExp = Utility::Round2Int(dExp);
	int iCash = Utility::Round2Int(dCash);
	int iSP = Utility::Round2Int(dSP);
	int iGuild = Utility::Round2Int(dGuild);

	LC_BackPackEntryList kEntryList;

	int32_t iItemTypeID = 0;
	int32_t iItemCount = 0;
	if (ResolveExpItem(iExp, iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveSPItem((int32_t)iSP, iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}
	if (ResolveGuildItem((int32_t)iGuild, iItemTypeID, iItemCount))
	{
		if (pkItemFactory->RequestItem(iItemTypeID))
		{
			pkPackEntryFactory->CreateItemEntry(iItemTypeID, iItemCount, CIET_NONE, kEntryList, -1, 0, 0, true);
		}
	}

	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList, true);
	}

	mem::set<int32_t>::iterator it = kServerIDs.begin();
	for (; it != kServerIDs.end(); ++it)
	{
		UT_ServerHelper::SendToMailAll(sMailTitle, sMailDesc, iCash, kEntryList, *it);
	}
}
//---------------------------------------------------------------
LC_Server_GameStory_Base::LC_Server_GameStory_Base(int32_t logicID, GameStoryRecord* pkRecord)
	: m_iLogicID(logicID), m_pkCoreRecord(pkRecord), m_lBaseEliteIdx(0), m_fLastUpdateCrossTime(0), m_fLastSyncHPTime(0), m_bLoaded(false)
	, m_bRepeated( false )
{
	if (pkRecord)
	{
		pkRecord->m_iStoryLogicId = logicID;
	}

	m_kSnapTimer.SetTimer(GET_INTERVAL(CMN_INTER_ENOUGH));
}
//----------------------------------------------------------------
bool LC_Server_GameStory_Base::PostLoaded(void)
{
	if (m_bLoaded)
	{
		return false;
	}
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game story loaded", GetStoryID());
	m_bLoaded = true;

	_setupRank();
	return m_bLoaded;
}

bool LC_Server_GameStory_Base::GetRepeated()
{
	return m_bRepeated;
}

void LC_Server_GameStory_Base::SetRepeated( bool bInput )
{
	m_bRepeated = bInput;
}

//------------------------------------------------------------------
class FilterActorOP : public LC_ActorOperator
{
public:
	FilterActorOP(GSRTPlayerFilter& filter): kRTFilter(filter) {}

	virtual bool operator()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			kRTFilter.AddActorDesc((LC_PlayerBase*)pkActor);
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	GSRTPlayerFilter& kRTFilter;
};
void LC_Server_GameStory_Base::GetPlayers(GSRTPlayerFilter& kRTFilter)
{
	_setupActorFilter(kRTFilter);
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	//LC_ActorBase* pkActor = kRTFilter.GetCoreActors();

	const LC_ActorPtrList& kActors = kRTFilter.GetCoreActors();
	switch (kRTFilter.eTargetType)
	{
		case GS_TAR_TP_NA:
			{
				FilterActorOP kFilterOP(kRTFilter);
				if (kRTFilter.kLogicMaps.empty())
				{
					pkWorldManager->ForAllPlayer(&kFilterOP);
				}
				else
				{
					mem::vector<GameLogic::LC_MapLogicBase*>::iterator it = kRTFilter.kLogicMaps.begin();
					for (; it != kRTFilter.kLogicMaps.end(); ++it)
					{
						LC_MapBase* pkMap = (*it) ? (*it)->GetMap() : NULL;
						if (pkMap)
						{
							pkMap->ForAllPlayer(&kFilterOP);
						}
					}
				}
			}
			break;
		case GS_TAR_TP_ME:
			{
				LC_ActorPtrList::const_iterator cit = kActors.begin();
				for (; cit != kActors.end(); ++cit)
				{
					LC_ActorBase* pkActor = *cit;
					if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
					{
						kRTFilter.AddActorDesc((LC_PlayerBase*)pkActor);
					}
				}
			}
			break;
		case GS_TAR_TP_AREA:
			{
				FilterActorOP kFilterOP(kRTFilter);
				mem::vector<GameLogic::LC_MapLogicBase*>::iterator it = kRTFilter.kLogicMaps.begin();
				for (; it != kRTFilter.kLogicMaps.end(); ++it)
				{
					LC_MapBase* pkMap = (*it) ? (*it)->GetMap() : NULL;
					if (pkMap)
					{
						pkMap->ForAllPlayer(&kFilterOP);
					}
				}
			}
			break;
		case GS_TAR_TP_GUILD:
			{
				GSLogicUint64IDSet::iterator itGuild = kRTFilter.kGuildIDs.begin();
				for (; itGuild != kRTFilter.kGuildIDs.end(); ++itGuild)
				{
					if (INVALID_GUILD_ID != *itGuild)
					{
						LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
						LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(*itGuild);
						if (pkGuild == NULL)
						{
							return;
						}

						unique_id_vector kAllMemberIDs;
						pkGuild->GetAllMember(kAllMemberIDs);

						unique_id_vector::iterator it = kAllMemberIDs.begin();
						for (; it != kAllMemberIDs.end(); ++it)
						{
							GuildServerMember* pkMem = pkGuild->GetMember(*it);
							if (pkMem)
							{
								kRTFilter.AddActorDesc(*it, pkMem->m_CharName);
							}
						}
					}
				}
			}
			break;
		case GS_TAR_TP_WORLD:
			{
				FilterActorOP kFilterOP(kRTFilter);
				pkWorldManager->ForAllPlayer(&kFilterOP);
			}
			break;
		default:
			break;
	}
}
void LC_Server_GameStory_Base::_setupTrigger(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig)
{
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();
	if (pkConfig)
	{
		MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
		TrigerDataList::iterator it = kMapCtrlSetting.m_kTriggerlist.begin();

		for (; it != kMapCtrlSetting.m_kTriggerlist.end(); ++it)
		{
			LC_ServerMapLogicPlaceVector kPlaces;
			_getRTScheduleMaps(iKey, it->iMapResID, kPlaces, pkConfig);

			if (kPlaces.empty())
			{
				continue;
			}

			UT_Circle circle;
			circle.kCenterPoint = it->vecLocation;
			circle.iRadius = it->iRadius;

			LC_ServerMap_Trigger::TriggerCallBack cb = boost::bind(&LC_Server_GameStory_Base::_onTriggerHit, this, _1, _2, _3);

			LC_ServerMapLogicPlaceVector::iterator itPlace = kPlaces.begin();
			for (; itPlace != kPlaces.end(); ++itPlace)
			{
				LC_ServerMapLogic_Place* pkMaplogic = *itPlace;

				if (pkMaplogic)
				{
					//Register trigger
					LC_ServerMap_Trigger* pkTrigger = pkMaplogic->AddCircleTrigger(circle, cb);
					if (pkTrigger)
					{
						pkTrigger->SetAllFilter();
						pkTrigger->Start();
						m_kRTMapTriggerRecordMap[pkMaplogic->GetMapLogicID()][pkTrigger->GetId()] = GSRTTriggerRecord(iKey, pkMaplogic->GetMapLogicID(), pkTrigger->GetId(), &*it);
					}
				}
			}
		}
	}
}

void LC_Server_GameStory_Base::_cleanupTrigger(GameStoryScheduleKey iKey)
{
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();
	{
		GSRTMapTriggerRecordMap::iterator it = m_kRTMapTriggerRecordMap.begin();
		for (; it != m_kRTMapTriggerRecordMap.end(); ++it)
		{
			LC_ServerMapLogic_Place* pkMaplogic = pkWorldManager->GetMapLogic(it->first);
			if (NULL == pkMaplogic)
			{
				continue;
			}

			GSRTTriggerRecordMap::iterator itRecord = it->second.begin();
			for (; itRecord != it->second.end(); ++itRecord)
			{
				GSRTTriggerRecord& kTriRecord = itRecord->second;

				if (kTriRecord.kIndex.eScheduleKey == iKey)
				{
					pkMaplogic->DelTrigger(itRecord->first);
				}
			}
		}
	}
	m_kRTMapTriggerRecordMap.clear();
}
GameStoryScheduleKey LC_Server_GameStory_Base::_peekRTBatchScheduleKey(void)
{
	return !m_kRTBatchScheduleMap.empty() ? m_kRTBatchScheduleMap.begin()->first : GS_SCHEDULE_KEY_COMMON;
}
GSRTBatchSchedule* LC_Server_GameStory_Base::_getRTBatchSchedule(GameStoryScheduleKey iKey)
{
	GSRTBatchScheduleMap::iterator it = m_kRTBatchScheduleMap.find(iKey);
	return (it != m_kRTBatchScheduleMap.end()) ? &it->second : NULL;
}

void LC_Server_GameStory_Base::_getRTScheduleMaps(GameStoryScheduleKey iKey, int32_t iMapResID, LC_ServerMapLogicPlaceVector& kPlaces, GameStoryConfig* pkConfig)
{
	kPlaces.clear();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	if (IsScheduleMapLogicKey(iKey))
	{
		LC_ServerMapLogic_Place* pkMapLogic = pkWorldManager->GetMapLogic(iKey);
		if (NULL == pkMapLogic)
		{
			return;
		}
		int32_t iKeyMapResId = pkMapLogic->GetMapResID();

		if (NULL == pkMapSetting || !pkMapSetting->CheckSchedule(iKey, iKeyMapResId))
		{
			return;
		}
		kPlaces.push_back(pkMapLogic);
	}
	else
	{
		if (pkMapSetting && pkMapSetting->IsSolelySchedule())
		{
			return;
		}
		const LC_ServerMapLogicPlaceVector* pkPlaces = pkWorldManager->GetMapLogicByRes(iMapResID);
		if (pkPlaces)
		{
			kPlaces = *pkPlaces;
		}
	}
}

void LC_Server_GameStory_Base::_getRTScheduleActors(GameStoryScheduleKey iKey, GameLogic::LC_ActorPtrList& kActors, GameStoryConfig* pkConfig)
{
	kActors.clear();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}
	if (IsScheduleMapLogicKey(iKey))
	{
		LC_ServerMapLogicBase* pkMapLogic = pkWorldManager->GetMapLogic(iKey);
		if (NULL == pkMapLogic)
		{
			return;
		}
		pkMapLogic->GetPlayersInMap(kActors);
	}
}

GameStoryScheduleKey LC_Server_GameStory_Base::_filterProperRTScheduleKey(LC_ServerMapLogicBase* pkMapLogic, GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig || NULL == pkMapLogic)
	{
		return GS_SCHEDULE_KEY_COMMON;
	}
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(pkMapLogic->GetMapResID());
	if (pkMapSetting && pkMapSetting->IsSolelySchedule())
	{
		return (GameStoryScheduleKey)pkMapLogic->GetMapLogicID();
	}
	else
	{
		return GS_SCHEDULE_KEY_COMMON;
	}
}

bool LC_Server_GameStory_Base::_isRTScheduleClear(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		return true;
	}
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return true;
	}
	GSRTID2BatchDetailMap::iterator it = pkRTBSchedule->kDetailMap.begin();
	for (; it != pkRTBSchedule->kDetailMap.end(); ++it)
	{
		GSRTBatchIndex& kRTBIdx = it->second.kBatchIdx;
		BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(kRTBIdx.iBatchIdx);
		if (NULL == pkBatchData)
		{
			continue;
		}
		switch (pkBatchData->iType)
		{
			case GSBSTP_SEQUECE:
				{
					if (pkConfig->m_kBatchSechedule.GetBatchIdx(kRTBIdx.iSeqIdx + 1) >= 0)
					{
						return false;
					}
				}
				break;
			case GSBSTP_TIMER:
			case GSBSTP_CYCLE:
				{
					return false;
				}
				break;
			case GSBSTP_TRIGGER:
			case GSBSTP_PASSIVE:
			case GSBSTP_CONDITION:
			default:
				break;
		}
	}
	return true;
}

void LC_Server_GameStory_Base::_processCommonStatEvent(GameLogic::LC_GameEvent* pkEvent, LC_StoryLogic_Manager* pkStoryLogicManager, GameLogic::LC_PlayerBase* pkPlayer)
{
	if (NULL == pkEvent || NULL == pkStoryLogicManager || NULL == pkPlayer)
	{
		return;
	}
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return;
	}

	if (!pkConfig->ContainMap(pkPlayer->GetMapResID()))
	{
		return;
	}

	GameStoryCommonStatType eCmnStatType = pkStoryLogicManager->GetCommonStatType((GameEventType)pkEvent->GetEventType());
	if (eCmnStatType == GS_CMN_STAT_TP_NA)
	{
		return;
	}
	int iStat = 0;
	switch (pkEvent->GetEventType())
	{
		case GLET_ADD_CASH:
			{
				if (pkEvent->GetEventParam(EP_ADD_CASH_TYPE) != CT_UNBIND_CASH)
				{
					return;
				}
				iStat = (int)pkEvent->GetEventParam(EP_ADD_CASH_COUNT);
			}
			break;
		case GLET_EXP_CHANGED:
			{
				if (pkEvent->GetEventParam(EP_EXP_CHANGED_EXP_VALUE) <= 0)
				{
					return;
				}
				iStat = (int)pkEvent->GetEventParam(EP_EXP_CHANGED_EXP_VALUE);
			}
			break;
		case GLET_SP_ADD:
			{
				iStat = (int)pkEvent->GetEventParam(EP_SP_ADD_COUNT);
			}
			break;
		default:
			return;
			break;
	}
	RecordCommonStat(pkPlayer, eCmnStatType, iStat, pkConfig);
}

bool LC_Server_GameStory_Base::_bCareOfEvent(GameLogic::LC_GameEvent* pkEvent)
{
	if (pkEvent == NULL)
	{
		return false;
	}

	switch (pkEvent->GetEventType())
	{
		case GLET_ADD_CASH:
		case GLET_EXP_CHANGED:
		case GLET_SP_ADD:
			{
				return true;
			}
			break;
		case GS_CMN_STAT_TP_CASH:
		case GS_CMN_STAT_TP_EXP:
		case GS_CMN_STAT_TP_SP:
		case GS_CMN_STAT_TP_CNT:
		case GS_CMN_STAT_TP_BATCH:
		case GS_CMN_STAT_TP_PERSIST_TIME:
		case GS_CMN_STAT_TP_KILL_COUNT:
		case GS_CMN_STAT_TP_KILL_SPECIAL_NPC_COUNT:
			{
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

GameLogic::LC_CommonStatEntry* LC_Server_GameStory_Base::GetCommonStatEntry(GameLogic::LC_PlayerBase* pkPlayer, GameStoryConfig* pkConfig)
{
	if (NULL == pkPlayer)
	{
		return NULL;
	}
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (NULL == pkConfig)
	{
		return NULL;
	}

	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return NULL;
	}
	LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();
	return kCommonStatAsset.GetCommonStatEntry(pkConfig->m_kCmnStatData.iID);
}

void LC_Server_GameStory_Base::RecordCommonStat(GameLogic::LC_PlayerBase* pkPlayer, GameStoryCommonStatType eCmnStatType, int iStat, GameStoryConfig* pkConfig)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();
	LC_CommonStatEntry* pkCmnStatEntry = GetCommonStatEntry(pkPlayer, pkConfig);
	if (NULL == pkCmnStatEntry)
	{
		return;
	}
	switch (eCmnStatType)
	{
		case GS_CMN_STAT_TP_CASH:
			{
				pkCmnStatEntry->AddAccumulateStoryGold(iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_EXP:
			{
				pkCmnStatEntry->AddAccumulateStoryExp(iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_SP:
			{
				pkCmnStatEntry->AddAccumulateStorySP(iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_CNT:
			{
				pkCmnStatEntry->AddAccumulateStoryCount(iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_BATCH:
			{
				pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_CUR_BATCH, iStat);
				pkPlayer->UpdateRankUserValueByRankType(Rank_Type_Ultimate);
				if (iStat > (int32_t)pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_MAX_BATCH))
				{
					pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_MAX_BATCH, iStat);

					AchievementEvent kEvent;
					kEvent.SetConditionInfo(GetStoryID());
					kEvent.SetNum(iStat);
					pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ACTIVITY_BATCH, kEvent);

					//针对无尽之塔特殊处理
					int storyID = GetStoryID();
					if(storyID >= STORYID_ENDLESS_TOWER1 && storyID <= STORYID_ENDLESS_TOWER6)
					{
						//成功，发送消息，客户端弹出成功界面
						LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
						LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
						if (NULL != pkGameEvent)
						{
							pkGameEvent->SetEventType(GLET_ENDLESSTOWER_EVENT);
							pkGameEvent->SetEventParam(ET_STATE, 1);//成功
							pkGameEvent->SetEventParam(ET_PLAYER_ID, pkPlayer->GetID());
							pkGameEvent->SetEventParam(ET_STORY_ID, storyID);	
							pkGameEvent->SetEventParam(ET_STORY_STAT, iStat);								
							pkEventManager->InsertGameEvent(pkGameEvent);

							//胜利后，复活死亡buddy并满血
							pkPlayer->ReliveAllBuddyBeforeEnterMap(true, true);
						}

						CF_EndlessTower::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_EndlessTower>(storyID);
						if(NULL != pkData && iStat>=pkData->_iCount)
						{
							//更新副本通关记录
							pkPlayer->GetRaidAsset()->UpdateRaid(pkPlayer, pkPlayer->GetMapResID(), 0, 0);

							//设置CmnStatEntry的状态
							pkCmnStatEntry->SetFinish();
						}

						//限时活动事件
						LC_PlayerBase::TimeLimitEvent actEvent;
						actEvent.storyID = storyID;
						actEvent.floorID = iStat;
						pkPlayer->TimeLimitInfoEvent(actEvent);
					}
				}
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_PERSIST_TIME:
			{
				if (iStat > (int32_t)pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_PERSIST_TIME))
				{
					pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_PERSIST_TIME, iStat);
					kCommonStatAsset.UpdateDirtyFlag();
				}
			}
			break;
		case GS_CMN_STAT_TP_KILL_COUNT:
			{
				pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_KILL_COUNT, iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_CUSTOM:
			{
				pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_CUSTOM, iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		case GS_CMN_STAT_TP_KILL_SPECIAL_NPC_COUNT:
			{
				pkCmnStatEntry->AddKillSpacialNpcCount(iStat);
				kCommonStatAsset.UpdateDirtyFlag();
			}
			break;
		default:
			break;
	}
}

void LC_Server_GameStory_Base::_resetCommonStat(GameLogic::LC_PlayerBase* pkPlayer, uint32_t timestamp, GameLogic::LC_CmnStatCheckResetHandler& handler, GameStoryConfig* pkConfig)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (NULL == pkConfig)
	{
		return;
	}
	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return;
	}
	LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();
	kCommonStatAsset.ResetStory(GetStoryID(), timestamp, pkConfig->m_kCmnStatData.kToResetCmnStatKeys, handler);
}

void LC_Server_GameStory_Base::_resetAllCommonStat(GameLogic::LC_PlayerBase* pkPlayer, GameStoryConfig* pkConfig)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (NULL == pkConfig)
	{
		return;
	}
	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return;
	}

	GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkPlayer);
	if(NULL == pkPlayerStat)
	{
		return;
	}

	int storyID = GetStoryID();
	bool tag = pkPlayer->GetResetAllCommonStatFlag(storyID);
	if(!tag)
	{
		return;
	}
	
	pkPlayerStat->Reset();

	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	int32_t endTime = pkGSMgr->GetStoryEndTime(storyID);
	pkPlayer->UpdateStoryEndTime(storyID, endTime);

	if(storyID == GameStoryIDType_Monkey)
	{
		pkPlayer->ResetCash(CT_CASH_TYPE_29);
	}

	LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();

	kCommonStatAsset.ResetStory_All(storyID);

	pkPlayer->UpdateResetAllCommonStatFlag(storyID);
}
//---------------------------------------------------------------------
void LC_Server_GameStory_Base::ActivateStory()
{
	float fNow = GET_PROCESS_TIME;
	if (m_pkCoreRecord)
	{
		float fCurrent = GET_PROCESS_TIME;
		this->_onActived();
		this->Init();
	}
}

//---------------------------------------------------------------------
void LC_Server_GameStory_Base::DeactivateStory()
{
	float fNow = GET_PROCESS_TIME;
	if (m_pkCoreRecord)
	{
		this->UnInit();
		this->_onDeactived();
		//SERVER_GET_GAMESTORY_MANAGER->ActiveStory(TIMER_ID_NA, m_pkCoreRecord->m_iStoryID);
		if ( m_pkCoreRecord->m_nLoopGroupId > 0 )
		{
			int32_t nStoryId = SERVER_GET_GAMESTORY_MANAGER->GetNextStoryIdInGroup( m_pkCoreRecord->m_nLoopGroupId, m_pkCoreRecord->m_iStoryID );
			SERVER_GET_GAMESTORY_MANAGER->ScheduleGameStory(nStoryId);
		}
	}
}

void LC_Server_GameStory_Base::Init()
{
	m_kRTMonsterRecord.clear();
	m_kEliteGrpMap.clear();
	m_kPlayerCmnStatMap.clear();

	_checkresetRank();

	GameStoryConfig* pkConfig = GetConfig();
	if (pkConfig)
	{
		_initMapSetting(pkConfig);
		_setupBatchSchedule(pkConfig);
	}
}

void LC_Server_GameStory_Base::UnInit()
{
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
	GameStoryConfig* pkConfig = GetConfig();
	if (!pkTimerMgr || !pkWorldManager || !pkNPCManager || !pkConfig)
	{
		return;
	}
	uint32_t uiTotalExistCount = (uint32_t)m_kRTMonsterRecord.size();
	GSRTMonsterRecordMap::iterator it = m_kRTMonsterRecord.begin();
	for (; it != m_kRTMonsterRecord.end(); ++it)
	{
		pkSkillInterface->RemoveLogicObject(it->first);
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%s]story[%d] unit, cycle npc count [%u]", __FUNCTION__, GetStoryID(), uiTotalExistCount);
	GSRTActorTriggerReocrdMap::iterator itRTActorTri = m_kRTActorTriggerReocrdMap.begin();
	for (; itRTActorTri != m_kRTActorTriggerReocrdMap.end(); ++itRTActorTri)
	{
		_clearRTTriActorRecord(itRTActorTri->first);
	}
	m_kRTActorTriggerReocrdMap.clear();
	_resetBatchSchedule(pkConfig);
	m_kRTMonsterRecord.clear();
	m_kEliteGrpMap.clear();
	m_kPlayerCmnStatMap.clear();

	//to reward rank
	_doRankReward(pkConfig);
	if (pkConfig->IsEndToClearPlayer())
	{
		GSResIDList::iterator itResID = pkConfig->m_kMapCtrlSetting.m_kMapResIDList.begin();
		for (; itResID != pkConfig->m_kMapCtrlSetting.m_kMapResIDList.end(); ++itResID)
		{
			GameLogic::LC_IDList kMapLCIDs;
			pkWorldManager->GetLogicMapIDListFromResID(kMapLCIDs, *itResID);
			if (kMapLCIDs.empty())
			{
				continue;
			}
			GameLogic::LC_IDList::iterator itLCID = kMapLCIDs.begin();
			for (; itLCID != kMapLCIDs.end(); ++itLCID)
			{
				LC_ServerMapLogic_Place* pkMaplogic = pkWorldManager->GetMapLogic(*itLCID);

				if (pkMaplogic && pkMaplogic->GetStoryID())
				{
					//to clear
					LC_ActorPtrList kActors;
					pkMaplogic->GetPlayersInMap(kActors);

					LC_ActorPtrList::iterator itActor = kActors.begin();
					for (; itActor != kActors.end(); ++itActor)
					{
						//pkSkillInterface->ReturnToCity(*itActor);
						pkMaplogic->LetPlayerLeave((LC_ServerPlayer*)*itActor);
					}

					pkMaplogic->SetPlaceState(PLACESTATE_END);
				}
			}
		}
	}
	_unitMapSetting(pkConfig);
}
//---------------------------------------------------------------------
void LC_Server_GameStory_Base::Update(float fCurrentTime, float fDeltaTime)
{
	//Record Total Time,including Client Loading Time
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	GSRTTipScheduleList::iterator it = m_kRTTipSchedules.begin();
	while (it != m_kRTTipSchedules.end())
	{
		if (!it->IsValid())
		{
			it->Reset();
			m_kRTTipSchedules.erase(it++);
			continue;
		}
		GSRTTipEntry* pkRTTipEntry = it->Schedule(timestamp);
		if (pkRTTipEntry)
		{
			if (!pkRTTipEntry->NeedActive())
			{
				NotifyTip(*pkRTTipEntry);
			}
			else if (IsActive())
			{
				NotifyTip(*pkRTTipEntry);
			}
		}
		++it;
	}
	if (fCurrentTime - m_fLastUpdateCrossTime >= 20)		//改为20秒
	{
		_updateCrossMapping();
		m_fLastUpdateCrossTime = fCurrentTime;
	}
	if (pkConfig->m_kRankData.IsStoryRankValid())
	{
		GSRTBatchScheduleMap::iterator itSch = m_kRTBatchScheduleMap.begin();
		for (; itSch != m_kRTBatchScheduleMap.end(); ++itSch)
		{
			NotifyRankInfo(itSch->first, NULL, false, pkConfig);
		}
	}

	if (m_kSnapTimer.IsExpired())
	{
		_updateRankInfo();
	}

	if (fCurrentTime - m_fLastSyncHPTime >= 0.5)
	{
		SyncKeyMonsterHP();
		m_fLastSyncHPTime = fCurrentTime;
	}

	CheckSyncFactionScore(fCurrentTime);
}
void LC_Server_GameStory_Base::CheckReset(uint32_t timestamp)
{
	GSRTBatchScheduleMap::iterator it = m_kRTBatchScheduleMap.begin();
	for (; it != m_kRTBatchScheduleMap.end(); ++it)
	{
		it->second.CheckReset(timestamp);
	}
}

void	LC_Server_GameStory_Base::RemovePlayerStateFromCmnStatMap(const unique_id_impl& charID)
{
	m_kPlayerCmnStatMap.erase(charID);
}

void	LC_Server_GameStory_Base::AddKillSpecialNPCCount(GameLogic::LC_ActorBase* pkKillerPlayer, int num)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkKillerPlayer;
	AttackerStatData& kAttackerStatData = pkConfig->m_kAttackerStatData;
	GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkPlayer);
	if (pkPlayerStat && kAttackerStatData.iKillNPCRewardLimit <= 0 || (int)pkPlayerStat->GetKillSpecialNPCCount() < kAttackerStatData.iKillNPCRewardLimit)
	{
		pkPlayerStat->AddKillSpecialNPCCount(1);
		RecordCommonStat(pkPlayer, GS_CMN_STAT_TP_KILL_SPECIAL_NPC_COUNT, 1, pkConfig);
	}
}
//----------------------------------------------------------------------------------------------------
void LC_Server_GameStory_Base::ProgressChanged(LC_ServerPlayer* pkPlayer, int pro)
{
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent)
	{
		pkGameEvent->SetEventType(GLET_GAME_STORY_PROGRESS);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
		pkGameEvent->SetEventParam(EP_STORY_PROCESS_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_STORY_PROCESS_STORY_ID, GetStoryID());
		pkGameEvent->SetEventParam(EP_STORY_PROCESS_PROCESS, pro);
		pkGameEvent->SetEventParam(EP_STORY_PROCESS_MAPID, pkPlayer->GetMapResID());
		//to task
		pkEventManager->InsertGameEvent(pkGameEvent);
	}

}
uint32_t LC_Server_GameStory_Base::GetStartTime()
{
	if(m_pkCoreRecord)
	{
		return (m_pkCoreRecord->m_iEndTime+1)/1000-(m_pkCoreRecord->m_iTimeLen*60);
	}
	return 0;
}

int32_t LC_Server_GameStory_Base::GetEndTime()
{
	if(m_pkCoreRecord)
	{
		return (m_pkCoreRecord->m_iEndTime)/1000;
	}
	return 0;
}
//----------------------------------------------------------------------------------------------------
void LC_Server_GameStory_Base::ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig || NULL == pkInjured || !pkConfig->ContainMap(pkInjured->GetMapResID()))
	{
		return;
	}
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(pkInjured->GetMapResID());
	if (NULL == pkMapSetting)
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(pkInjured->GetMapLogicID());
	if (NULL == pkPlace)
	{
		return;
	}
	GameStoryScheduleKey iKey = _filterProperRTScheduleKey(pkPlace, pkConfig);
	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule || !pkRTBatchSchedule->IsInited())
	{
		return;
	}
	//击杀玩家奖励
	AttackerStatData& kAttackerStatData = pkConfig->m_kAttackerStatData;
	InjuredStatData& kInjuredStatData = pkConfig->m_kInjuredStatData;
	if (pkKiller && IS_PLAYER_ID(pkKiller->GetID()))
	{
		LC_ServerPlayer* pkKillerPlayer = (LC_ServerPlayer*)pkKiller;
		GSRTPlayerCommonStat* pkKillerPlayerStat = GetPlayerStats(pkKillerPlayer);
		if (IS_PLAYER_ID(pkInjured->GetID()))
		{
			if (pkKillerPlayerStat)
			{
				pkKillerPlayerStat->AddKillPlayerCount(1);
				{
					AchievementEvent kEvent;
					kEvent.SetConditionInfo(GetStoryID());
					kEvent.SetNum(1);
					pkKillerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ACTIVITY_PK, kEvent);
				}
				//update
				GameStoryEntry* pkStoryEntry = pkKillerPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
				if (pkStoryEntry)
				{
					pkStoryEntry->UpdateDirtyFlag();
					pkKillerPlayer->GetGameStoryAsset().UpdateDirtyFlag();
				}
			}
			//击杀玩家奖励
			if (kAttackerStatData.IsKillPlayerReward() && (kAttackerStatData.iKillPlayerRewardLimit <= 0 || (int32_t)pkKillerPlayerStat->GetKillPlayerCount() <= kAttackerStatData.iKillPlayerRewardLimit))
			{
				const RewardData& kReward = kAttackerStatData.kKillPlayerReward;
				GameStory::GSRTPlayerFilter kRTActorFilter;
				kRTActorFilter.eTargetType = kReward.eTargetType;
				kRTActorFilter.AddCoreActor(pkKiller);
				GetPlayers(kRTActorFilter);
				if (kRTActorFilter.PlayerSize())
				{
					GSServerReward gsReward;
					kReward.ResolveReward(RewardParam(1.0, pkInjured->GetLevel()), gsReward, pkKiller, pkInjured);

					GSRTPlayerDescMap::const_iterator citActor = kRTActorFilter.GetPlayers().begin();
					for (; citActor != kRTActorFilter.GetPlayers().end(); ++citActor)
					{
						LC_PlayerBase* pkTar = (LC_PlayerBase*)citActor->second.pkPlayer;

						gsReward.Reward(pkTar, GetStoryID());
						if (pkTar)
						{
							GameStoryEntry* pkStoryEntry = pkTar->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
							if (pkStoryEntry)
							{
								pkStoryEntry->UpdateDirtyFlag();
								pkTar->GetGameStoryAsset().UpdateDirtyFlag();
							}
						}
					}
				}
			}
			GSRTRankEntry* pkRTRankEntry = m_kRTRankInfo.GetRankEntry(RankID(GS_RANK_TP_KILL_PLAYER));
			if (pkRTRankEntry)
			{
				pkRTRankEntry->Update(pkKillerPlayerStat);
			}
		}
		else if (IS_NPC_ID(pkInjured->GetID()))
		{
			const NPCReward* pkNPCReward = pkConfig->m_kNPCRewardInfo.GetNPCReward(pkInjured->GetCharType());
			if (NULL != pkNPCReward && !pkNPCReward->kFactionScoreRewards.empty())
			{
				mem::map<int32_t, int32_t>::const_iterator it = pkNPCReward->kFactionScoreRewards.begin();
				for (; it != pkNPCReward->kFactionScoreRewards.end(); ++it)
				{
					if (it->first < 0 || it->first > GS_FACTION_SCORE_MAX)
					{
						continue;
					}
					AddFactionScore(iKey, it->first, it->second, pkKiller, pkInjured);
				}
			}
			if (NULL != pkNPCReward && pkNPCReward->iActorScore > 0)
			{
				AddActorScore(iKey, pkNPCReward->iActorScore, pkKiller, pkInjured);
			}

			GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkKillerPlayer);
			if (pkPlayerStat)
			{
				int stroyID = GetStoryID();
				int npcID = pkInjured->GetCharType();
				int times = pkPlayerStat->GetKillSpecialNPCCount();
				if(stroyID == GameStoryIDType_Chest)
				{
					int vipTimes = 0;
					LC_VIPCmnManager* pkMgr = LC_VIPCmnManager::GetSingletonPtr();
					if(pkMgr != NULL)
					{
						LC_VIPProfitDataEntry* pkDataEntry = pkMgr->GetVIPProfitDataEntry(VIP_PROFIT_VIP_ADD_YIJIXUNBAO_KILLNPC_NUM);
						if(NULL != pkDataEntry)							
						{
							vipTimes = pkDataEntry->GetValue(pkKillerPlayer->GetVIPLevel(), false);
						}
					}

					int totalTimes = GetGlobalSetting.YiJiXunBaoKillSpecialNpcMaxCount + vipTimes;
					if(times < totalTimes)
					{
						for(int i=0; i<GetGlobalSetting.YiJiXunBaoNPCID.size(); ++i)
						{
							if(npcID == GetGlobalSetting.YiJiXunBaoNPCID[i])
							{
								pkPlayerStat->AddKillSpecialNPCCount(1);
								RecordCommonStat(pkKillerPlayer, GS_CMN_STAT_TP_KILL_SPECIAL_NPC_COUNT, 1, pkConfig);
								break;
							}
						}
					}					
				}

				if(kAttackerStatData.iMapType==StoryMapType::PET_ISLANDS)
				{
					int currentStoryID = GetStoryID();
					int shareStoryIDSize = kAttackerStatData.iShareKillNPCRewardLimitStoryID.size();
					if(shareStoryIDSize > 0)
					{
						for(int i=0; i<shareStoryIDSize; ++i)
						{
							int storyID = kAttackerStatData.iShareKillNPCRewardLimitStoryID[i];
							if(storyID == currentStoryID)
							{
								if( kAttackerStatData.iKillNPCRewardLimit <= 0 || (int)pkPlayerStat->GetKillSpecialNPCCount() < kAttackerStatData.iKillNPCRewardLimit)
								{
									pkPlayerStat->AddKillSpecialNPCCount(1);
									RecordCommonStat(pkKillerPlayer, GS_CMN_STAT_TP_KILL_SPECIAL_NPC_COUNT, 1, pkConfig);
									continue;
								}
							}

							LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
							LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
							if (NULL != pkStory)
							{
								pkStory->AddKillSpecialNPCCount(pkKiller, 1);
							}
						}
					}
				}

				pkPlayerStat->AddKillNPCCount(1);

				AchievementEvent kEvent;
				kEvent.SetConditionInfo(GetStoryID(), pkInjured->GetCharType());
				kEvent.SetNum(1);
				pkKillerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ACTIVITY_KILL, kEvent);

				RecordCommonStat(pkKillerPlayer, GS_CMN_STAT_TP_KILL_COUNT, pkPlayerStat->GetKillNPCCount(), pkConfig);
				
				if (pkNPCReward && (kAttackerStatData.iKillNPCRewardLimit <= 0 || (int)pkPlayerStat->GetKillRewardCount() <= kAttackerStatData.iKillNPCRewardLimit))
				{
					pkPlayerStat->AddKillRewardCount(1);
					const RewardData& kReward = pkNPCReward->kReward;
					GameStory::GSRTPlayerFilter kRTActorFilter;
					kRTActorFilter.eTargetType = kReward.eTargetType;
					kRTActorFilter.AddCoreActor(pkKiller);
					GetPlayers(kRTActorFilter);
					if (kRTActorFilter.PlayerSize())
					{
						vector<int> nParams;
						nParams.clear();
						if(pkInjured != NULL) 
						{
							nParams.push_back(pkInjured->GetMapResID());
						}
						int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, OPERATE_LOGTYPE1);
						GSServerReward gsReward;
						GSServerReward::MailPlayerMap kMailPlayers;
						kReward.ResolveReward(RewardParam(1.0, pkInjured->GetLevel()), gsReward, pkKiller, pkInjured);
						GSRTPlayerDescMap::const_iterator citActor = kRTActorFilter.GetPlayers().begin();
						for (; citActor != kRTActorFilter.GetPlayers().end(); ++citActor)
						{
							const GSRTPlayerDesc& rkPlayerDesc = citActor->second;
							if (IS_PLAYER_ID(rkPlayerDesc.uiID))
							{
								GSRTPlayerCommonStat* pkCmnStat = GetPlayerStats(rkPlayerDesc.uiCharID, true, rkPlayerDesc.sName);
								if (NULL == pkCmnStat)
								{
									continue;
								}
								LC_PlayerBase* pkTar = (LC_PlayerBase*)rkPlayerDesc.pkPlayer;
								if (rkPlayerDesc.uiID != pkKiller->GetID())
								{
									pkCmnStat->AddKillNPCCount(1);
									RecordCommonStat(pkTar, GS_CMN_STAT_TP_KILL_COUNT, pkCmnStat->GetKillNPCCount(), pkConfig);
								}
								if (kReward.IsToAccum())
								{
									pkCmnStat->AddAccumReward(gsReward);
								}
								else
								{
									if (!kReward.IsToMail())
									{
										gsReward.Reward(pkTar, GetStoryID(), nLogCode, nParams);
									}
								}
								if (kReward.IsToStat())
								{
									pkCmnStat->AddStatReward(gsReward);
								}
								if (pkTar)
								{
									GameStoryEntry* pkStoryEntry = pkTar->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
									if (pkStoryEntry)
									{
										pkStoryEntry->UpdateDirtyFlag();
										pkTar->GetGameStoryAsset().UpdateDirtyFlag();
									}
								}
							}
							if (rkPlayerDesc.uiCharID && kReward.IsToMail())
							{
								kMailPlayers[citActor->first] = rkPlayerDesc.sName;
							}
						}
						if (!kMailPlayers.empty())
						{
							gsReward.Email(kMailPlayers,  nLogCode, nParams);
						}
					}
				}	
			}
		}
		kAttackerStatData.OnKill(pkKillerPlayer, pkInjured, pkKillerPlayerStat);
	}
	if (pkInjured)
	{
		if (IS_PLAYER_ID(pkInjured->GetID()))
		{
			LC_ServerPlayer* pkInjuredPlayer = (LC_ServerPlayer*)pkInjured;
			GSRTPlayerCommonStat* pkInjuredPlayerStat = GetPlayerStats(pkInjuredPlayer);
			if (pkInjuredPlayerStat)
			{
				pkInjuredPlayerStat->IncDeadTimes();
				//update
				GameStoryEntry* pkStoryEntry = pkInjuredPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
				if (pkStoryEntry)
				{
					pkStoryEntry->UpdateDirtyFlag();
					pkInjuredPlayer->GetGameStoryAsset().UpdateDirtyFlag();
				}
				//被击杀补偿  后续可以考虑抽象代码
				if (kInjuredStatData.IsDeadCompensate() && (kInjuredStatData.iDeadCompensateLimit <= 0 || (int32_t)pkInjuredPlayerStat->GetDeadTimes() <= kInjuredStatData.iDeadCompensateLimit))
				{
					const RewardData& kReward = kInjuredStatData.kDeadCompensateReward;
					GameStory::GSRTPlayerFilter kRTActorFilter;
					kRTActorFilter.eTargetType = kReward.eTargetType;
					kRTActorFilter.AddCoreActor(pkInjured);
					GetPlayers(kRTActorFilter);
					if (kRTActorFilter.PlayerSize())
					{
						GSServerReward gsReward;
						kReward.ResolveReward(RewardParam(1.0, pkInjured->GetLevel()), gsReward);
						GSRTPlayerDescMap::const_iterator citActor = kRTActorFilter.GetPlayers().begin();
						for (; citActor != kRTActorFilter.GetPlayers().end(); ++citActor)
						{
							const GSRTPlayerDesc& rkRTPlayerDesc = citActor->second;
							if (!IS_PLAYER_ID(rkRTPlayerDesc.uiID))
							{
								continue;
							}

							LC_PlayerBase* pkTar = rkRTPlayerDesc.pkPlayer;
							if (pkTar)
							{
								gsReward.Reward(pkTar, GetStoryID());
								GameStoryEntry* pkStoryEntry = pkTar->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
								if (pkStoryEntry)
								{
									pkStoryEntry->UpdateDirtyFlag();
									pkTar->GetGameStoryAsset().UpdateDirtyFlag();
								}
							}
						}
					}
				}
			}

			if (pkMapSetting->IsSolelySchedule())
			{	
				GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
				if (pkRTBSchedule)
				{
					GameLogic::LC_ActorPtrList kPlayerList;
					pkPlace->GetAllRealPlayer(kPlayerList);
					bool bAllDead = true;		
					GameLogic::LC_ActorPtrList::iterator itActor =kPlayerList.begin();
					for (; itActor != kPlayerList.end(); ++itActor)
					{
						GameLogic::LC_ActorBase* pkActor = *itActor;
						if (!pkActor->CheckOwnerDead() && !pkActor->GetDying())
						{
							bAllDead = false;
							break;
						}
					}
					if (bAllDead)
					{
						const GameStory::BatchConfig& kBConfig = pkConfig->m_kBatchSechedule.kConfig;
						if (kBConfig.IsAllPlayerDeadToEnd())
						{
							pkRTBSchedule->SetFail();
							_clearBatchSchedule(iKey, pkConfig);

							//玩家死亡
							
							int storyID = GetStoryID();
							if(storyID >= STORYID_ENDLESS_TOWER1 && storyID <= STORYID_ENDLESS_TOWER6)
							{
								LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
								GameLogic::LC_ActorPtrList::iterator itActor =kPlayerList.begin();
								for (; itActor != kPlayerList.end(); ++itActor)
								{
									GameLogic::LC_ActorBase* pkActor = *itActor;
									LC_CommonStatEntry* pkCmnStatEntry = GetCommonStatEntry((LC_PlayerBase*)pkActor, pkConfig);
									if (NULL == pkCmnStatEntry)
									{
										continue;
									}

									int stat = pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_CUR_BATCH);
									LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
									if (NULL != pkGameEvent)
									{
										pkGameEvent->SetEventType(GLET_ENDLESSTOWER_EVENT);
										pkGameEvent->SetEventParam(ET_STATE, 0);//失败
										pkGameEvent->SetEventParam(ET_PLAYER_ID, pkActor->GetID());
										pkGameEvent->SetEventParam(ET_STORY_ID, storyID);	
										pkGameEvent->SetEventParam(ET_STORY_STAT, stat+1);	
										pkEventManager->InsertGameEvent(pkGameEvent);
									}
								}
							}
						}
					}
				}
			}
			kInjuredStatData.OnDead(pkKiller, pkInjuredPlayer, pkInjuredPlayerStat);
		}
		else if (IS_NPC_ID(pkInjured->GetID()))
		{
			const NPCReward* pkNPCReward = pkConfig->m_kNPCRewardInfo.GetNPCReward(pkInjured->GetCharType());
			//目前只有镖车掉落  而且只有被击杀才掉落
			if (pkNPCReward && pkNPCReward->IsToFlop() && pkKiller)
			{
				LC_ServerItemFlopManager* pkFlopMgr = LC_ServerItemFlopManager::GetSingletonPtr();
				pkFlopMgr->GenerateExtraRewaredOnNPCGlobal((LC_ServerNPC*)pkInjured);
			}
			AttackTargetStat(FactionWarTarget_Npc, pkInjured->GetCharType(), 1);
			//广播 杀怪 tips
			if (m_kRTMonsterRecord.count(pkInjured->GetID()))
			{
				object_id_type pkFirstHitPlayerID = ((LC_ServerNPC*)pkInjured)->GetHitPlayerID();
				LC_ServerPlayer* pkFirstHitPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(pkFirstHitPlayerID);
				if(NULL != pkFirstHitPlayer)
				{
					NotifyKillNPCTip(iKey, pkFirstHitPlayer, pkInjured, pkConfig);
				}
				else
				{
					NotifyKillNPCTip(iKey, pkKiller, pkInjured, pkConfig);
				}
				
				BatchConfig& kBatchConfig = pkConfig->m_kBatchSechedule.kConfig;
				if (kBatchConfig.IsKeyMonsterDeadToEnd())
				{
					//do key info
					GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
					if (pkRTBSchedule)
					{
						if (pkRTBSchedule->ToKeyStat(pkInjured->GetCharType(), 1))
						{
							pkRTBSchedule->SetSuccess();
							_clearBatchSchedule(iKey, pkConfig);
						}
					}
				}
				if(kBatchConfig.SaveMonsterDeadState())
				{
					const GSRTMonsterIndexCluster* pkGSRTMonsterData = GetGSRTMonsterData(pkInjured->GetID());
					if(NULL != pkGSRTMonsterData)
					{
						int32_t endTime = m_pkCoreRecord->m_iEndTime/1000;
						int32_t batchIndex = pkGSRTMonsterData->iBatchIdx;

						std::ostringstream message;
						message << endTime << ":" <<batchIndex;

						mem::vector<uint64_t> nameKey;
						nameKey.push_back(GetGameServerID());

						mem::vector<uint64_t> key;
						key.push_back(pkInjured->GetCharType());

						//char szNameSpace[100] = ":";
						//strcat(szNameSpace, pkConfig->FileName());
						char szNameSpace[MAX_CHAR_NAME_LENGTH + 1] = { '\0' };
						sprintf_s( szNameSpace, MAX_CHAR_NAME_LENGTH + 1, ":%s", pkConfig->FileName() );
						UT_ServerHelper::RedisWriteString(szNameSpace, nameKey, key, message.str());
					}
				}
			}
			//伤害排行榜奖励 -- 怪物死亡
			
			//抵御异种，根据玩家对怪物造成的伤害，给玩家发奖
			_doBestGuildDamageStatRelatedByPlayerDamage(iKey, pkInjured, pkConfig);

			_UpdateBestGuildBossStat(pkInjured);

			//浩劫秘境boss死亡伤害奖励
			_doRegularBossDamageReward(pkInjured);

			//劫镖奖励
			_doEscortRobbedReward(pkInjured, pkKiller);

			_doDamageStatRelated(iKey, pkInjured, pkConfig);
			_clearSpawnedMonster(pkInjured->GetID(), pkKiller);

			if(pkNPCReward && pkNPCReward->pfnDieFunc != NULL)
			{
				(*pkNPCReward->pfnDieFunc)(pkKiller, pkInjured);
			}

			if(IS_PLAYER_CLASS_ID(pkKiller->GetID()))
			{
				LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer*)pkKiller)->GetOwnerPlayer();
				if(NULL != pkOwnerPlayer)
				{
					CheckTriConditions(iKey, pkInjured->GetCharType(), 0, pkOwnerPlayer->GetID());
				}
			}				
		}
		else {}
	}
	int32_t iFactionScoreToEnd = pkConfig->m_kBatchSechedule.kConfig.GetFactionScoreToEnd();
	if (pkKiller && iFactionScoreToEnd > 0)
	{
		if (GetFactionScore(iKey, pkKiller->GetFactionID()) >= iFactionScoreToEnd)
		{
			_clearBatchSchedule(iKey, pkConfig);
		}
	}
}

void LC_Server_GameStory_Base::ActorBeDamaged(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, attr_value_type dmg)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(iMapLogicID);
	if (NULL == pkPlace)
	{
		return;
	}
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	int32_t iMapResID = pkPlace->GetMapResID();
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	if (NULL == pkMapSetting)
	{
		return;
	}
	GameStoryScheduleKey iKey = _filterProperRTScheduleKey(pkPlace, pkConfig);
	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule || !pkRTBatchSchedule->IsInited())
	{
		return;
	}
	object_id_type iInjuredID = pkInjured->GetID();
	if (IS_NPC_ID(iInjuredID))
	{
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkInjured;
		if (NULL == pkAttacker || !IS_PLAYER_ID(pkAttacker->GetID()))
		{
			return;
		}
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkAttacker;
		GSRTMonsterRecordMap::iterator itRTMonIdx = m_kRTMonsterRecord.find(iInjuredID);
		if (itRTMonIdx != m_kRTMonsterRecord.end())
		{
			GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkPlayer);
			if (NULL == pkPlayerStat)
			{
				return;
			}
			AttackerStatData& kAtkStatData = pkConfig->m_kAttackerStatData;
			if (kAtkStatData.IsEachDamageReward())
			{
				GSServerReward gsReward;
				kAtkStatData.kDamageReward.ResolveReward(RewardParam(LC_Helper::GetAttributeRate(pkPlayer, ATT_EXP_RATE)), gsReward, dmg);
				gsReward.Reward(pkPlayer, GetStoryID());
				if (/*pkPlayerStat &&*/ kAtkStatData.kDamageReward.IsToStat())
				{
					pkPlayerStat->AddStatReward(gsReward);
				}
				////
				//if (pkPlayerStat)
				{
					pkPlayerStat->AddDamageCount(1);
				}
				oss_role_counter_attack(pkPlayer, pkNPC, (int)dmg, (int)gsReward.dExp);
				GameStoryEntry* pkStoryEntry = pkPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
				if (pkStoryEntry)
				{
					pkStoryEntry->UpdateDirtyFlag();
					pkPlayer->GetGameStoryAsset().UpdateDirtyFlag();
				}
			}
			GSRTMonsterIndexCluster& kRTIdx = itRTMonIdx->second;
			const MonsterData* pkMonsterData = pkConfig->m_kMapCtrlSetting.GetMonsterData(kRTIdx.iMonsterIdx);
			if (pkMonsterData != NULL)
			{
				int64_t nFullHp = int64_t(pkInjured->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
				uint32_t nCurHpPec = uint32_t(int64_t(pkInjured->GetHP()) * 10000 / nFullHp);
				if (pkMonsterData->IsLowHPProtected() && nCurHpPec <= (uint32_t)pkMonsterData->iProtectHPPro)
				{
					pkPlace->SetEnterLocked(true);
				}
				if (pkMonsterData->kHPNotice.m_NoticeMap.empty() == false)
				{
					uint32_t nNoticeID = 0;
					uint32_t nOldHpPec = uint32_t(int64_t(pkInjured->GetOldHP()) * 100 / nFullHp);
					if (pkMonsterData->kHPNotice.NeedNotice(nOldHpPec, nCurHpPec / 100, nNoticeID) == true && nNoticeID > 0)
					{
						UT_ServerHelper::_GameNoticeFmort noticeFmort;
						noticeFmort.SetMsgID(nNoticeID);
						noticeFmort.AddString(pkInjured->GetOwnerCharName());
						noticeFmort.AddPlayerInfo(pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName());
						noticeFmort.AddNumber(nCurHpPec / 100);
						int32_t nServerID = UT_ServerHelper::GetServerId();
						noticeFmort.AddServerID(nServerID);
						noticeFmort.AddFlyKicks(nServerID, iMapLogicID, iMapResID, pkInjured->GetCurrentLocation());
						noticeFmort.AddPathFound(nServerID, iMapLogicID, iMapResID, pkInjured->GetCurrentLocation());
						UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkPlayer->GetCitizenship(), pkPlayer, pkPlace);
					}
				}
			}
		}
	}
	else if (IS_PLAYER_ID(iInjuredID))
	{
		//LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		//handle some player damaged
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkInjured;
		InjuredStatData& kIjrStatData = pkConfig->m_kInjuredStatData;
		if (kIjrStatData.IsInjuredStat())
		{
			GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkPlayer);
			if (pkPlayerStat)
			{
				pkPlayerStat->AddInjueredCount(1);

				if (kIjrStatData.iInjuredLimit > 0 && kIjrStatData.iInjuredLimit <= (int32_t)pkPlayerStat->GetInjueredCount())
				{
					pkRTBatchSchedule->SetFail();
					_clearBatchSchedule(iKey, pkConfig);
				}
				GameStoryEntry* pkStoryEntry = pkPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
				if (pkStoryEntry)
				{
					pkStoryEntry->UpdateDirtyFlag();
					pkPlayer->GetGameStoryAsset().UpdateDirtyFlag();
				}
			}
		}
	}
}

void LC_Server_GameStory_Base::PlayerEnter(LC_ServerPlayer *pkOwnerPlayer, int32_t mapID, uint32_t times)
{
	if(times < 1)
	{
		return;
	}

	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	GameStoryConfig* pkConfig = GetConfig();
	if(NULL == pkConfig)
	{
		return;
	}

	int32_t storyID = GetStoryID();
	GameStoryEntry *pkStoryEntry = pkOwnerPlayer->GetGameStoryAsset().GetGameStoryEntry(storyID);
	if(NULL == pkStoryEntry)
	{
		return;
	}

	pkStoryEntry->RecordEnterStory(mapID, times);

	{
		//记录神域地宫进入次数，资源找回功能
		//地宫一层...九曾的进入次数，都要记到神域地宫的次数里
		int32_t newStoryID = storyID; 
		int times = -1;
		if(UT_ServerHelper::IsShengYuDiGongStoryID(storyID))
		{
			newStoryID = GameStoryIDType_Loulan;
			LC_StatType eStatType = LC_ST_TP_NA;
			GameStoryEntry *pkStoryEntryPlus = pkOwnerPlayer->GetGameStoryAsset().GetGameStoryEntry(newStoryID);
			if(NULL != pkStoryEntryPlus)
			{
				pkStoryEntryPlus->ResolveCheckType(eStatType);
				GameStoryCommonStatEntry* pkStat = pkStoryEntryPlus->GetCommonStatEntry(eStatType);
				if (NULL != pkStat)
				{
					times = pkStat->m_kTimes.EnteredTimes();
				}
				pkStoryEntryPlus->RecordEnterStory(pkOwnerPlayer->GetMapResID());
			}
		}
		else
		{
			times = pkOwnerPlayer->GetGameStoryEnterTimes(storyID);
		}

		if(0 == times)
		{
			//每天参与了几次活动
			pkOwnerPlayer->UpdateGameStoryJoinTimes(newStoryID);
		}
	}

	if (pkConfig->m_kCmnStatData.IsValid())
	{
		LC_CommonStatAsset& kCommonStatAsset = pkOwnerPlayer->GetCommonStatAsset();
		LC_CommonStatEntry* pkCmnStatEntry = kCommonStatAsset.GetCommonStatEntry(pkConfig->m_kCmnStatData.iID);
		if (pkCmnStatEntry)
		{
			pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_JOIN, pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_JOIN)+1);
		}
	}
}

void LC_Server_GameStory_Base::PlayerFinish(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times)
{
	if(times < 1)
	{
		return;
	}

	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	GameStoryConfig* pkConfig = GetConfig();
	if(NULL == pkConfig)
	{
		return;
	}

	GameStoryEntry *pkStoryEntry = pkOwnerPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
	if(NULL == pkStoryEntry)
	{
		return;
	}

	if(pkConfig->m_iFirstFinishNotRecordRaidFinished == 1)
	{
		//第一次通关副本，不记录次数
		const RaidInfo* raidInfo = pkOwnerPlayer->GetRaidAsset()->FindRaid(mapID);
		if(NULL != raidInfo)
		{
			pkStoryEntry->RecordFinishStory(mapID, times);
		}
	}
	else
	{
		pkStoryEntry->RecordFinishStory(mapID, times);
	}

	if (pkConfig->m_kCmnStatData.IsValid())
	{
		LC_CommonStatAsset& kCommonStatAsset = pkOwnerPlayer->GetCommonStatAsset();
		LC_CommonStatEntry* pkCmnStatEntry = kCommonStatAsset.GetCommonStatEntry(pkConfig->m_kCmnStatData.iID);
		if (pkCmnStatEntry)
		{
			pkCmnStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_FINISH, pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_FINISH)+1);
		}
	}
}

void LC_Server_GameStory_Base::OnAcceptTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (pkConfig && pkConfig->m_kTaskSetting.GetTaskData(pkTask->GetTaskID()))
	{
		if (pkConfig->m_kTaskSetting.IsAcceptComsumeJoinTimes())
		{
			PlayerEnter((LC_ServerPlayer*)pkPlayer, pkPlayer->GetMapResID());
		}
	}
}

void LC_Server_GameStory_Base::OnFinishTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (pkConfig && pkConfig->m_kTaskSetting.GetTaskData(pkTask->GetTaskID()))
	{
		LC_CommonStatEntry* pkCommonStatEntry = GetCommonStatEntry(pkPlayer, pkConfig);
		if (pkCommonStatEntry)
		{
			pkCommonStatEntry->AddAccumulateStoryCount(1);
		}

		if (pkConfig->m_kTaskSetting.IsFinisheComsumeFinishTimes())
		{
			PlayerFinish((LC_ServerPlayer*)pkPlayer, pkPlayer->GetMapResID());
		}
		
		//record the task id
		RecordCommonStat(pkPlayer, GS_CMN_STAT_TP_CUSTOM, pkTask->GetTaskID(), pkConfig);
	}

	if (pkConfig && pkConfig->m_kTaskSetting.IsFinishAssignTask())
	{
		const TaskData* pkTaskData = NULL;
		ResultType res = CanTaskAppoint(pkPlayer, GetGlobalSetting.AssignEscortTaskID, pkTaskData);
		if (RE_SUCCESS != res)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "LC_Server_GameStory_Base finishAssignTask CanTaskAppoint playerid:%d, res:%d", pkPlayer->GetInstance(), res);
			return;
		}

		res = TaskAppoint(pkPlayer, GetGlobalSetting.AssignEscortTaskID);
		if(RE_SUCCESS != res)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "LC_Server_GameStory_Base finishAssignTask TaskAppoint playerid:%d, res:%d", pkPlayer->GetInstance(), res);
		}
	}
}

void LC_Server_GameStory_Base::OnCancelTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig)
{

}

void LC_Server_GameStory_Base::PreMapDestoryed(LC_ServerMapLogicBase* pkMapLogic)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	int32_t iMapResID = pkMapLogic->GetMapResID();
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	if (pkMapSetting)
	{
		if (pkMapSetting->IsSolelySchedule())
		{
			_clearBatchSchedule((GameStoryScheduleKey)pkMapLogic->GetMapLogicID(), pkConfig);
		}
	}
}

void LC_Server_GameStory_Base::PostMapCreated(LC_ServerMapLogicBase* pkMapLogic, int iAutoFitLevel)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	if (!IsActive())
	{
		return;
	}
	int32_t iMapResID = pkMapLogic->GetMapResID();
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	if (pkMapSetting)
	{
		if (pkMapSetting->IsSolelySchedule())
		{
			_addBatchSchedule((GameStoryScheduleKey)pkMapLogic->GetMapLogicID(), iAutoFitLevel, GetConfig());
		}
	}
}

void LC_Server_GameStory_Base::PrePlayerLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkMapLogic)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	LC_ServerMapLogic_Place* pkPlace = (LC_ServerMapLogic_Place*)pkMapLogic;
	//handle stat settle
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(pkPlace->GetMapResID());
	if (pkMapSetting)
	{
		if (pkMapSetting->IsSolelySchedule())
		{
			GameStoryScheduleKey iKey = _filterProperRTScheduleKey(pkPlace, pkConfig);
			GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
			if (pkRTBSchedule)
			{
				GameLogic::LC_ActorPtrList kPlayerList;
				pkPlace->GetAllRealPlayer(kPlayerList);
				bool bAllDead = true;
				GameLogic::LC_ActorPtrList::iterator itActor = kPlayerList.begin();
				for (; itActor != kPlayerList.end(); ++itActor)
				{
					GameLogic::LC_ActorBase* pkActor = *itActor;
					if (pkActor == pkPlayer)
					{
						continue;
					}
					if (!pkActor->GetDead() && !pkActor->GetDying())
					{
						bAllDead = false;
						break;
					}
				}
				if (bAllDead)
				{
					const GameStory::BatchConfig& kBConfig = pkConfig->m_kBatchSechedule.kConfig;
					if (kBConfig.IsAllPlayerDeadToEnd())
					{
						pkRTBSchedule->SetFail();
						_clearBatchSchedule(iKey, pkConfig);
					}
				}
			}
		}
		if (pkConfig->m_kTaskSetting.IsLeaveMapTaskFailed())
		{
			TaskIDSet kTaskIDs;
			pkConfig->m_kTaskSetting.GetTaskProcessing(pkPlayer, kTaskIDs);
			TaskIDSet::iterator it = kTaskIDs.begin();
			for (; it != kTaskIDs.end(); ++it)
			{
				pkPlayer->GetTaskMap()->SetTaskFail(*it, true);
			}
		}
		if (NULL != pkMapSetting->pfnChooseFaction)
		{
			int32_t iFactionID = (*pkMapSetting->pfnChooseFaction)(pkPlayer);
			//不管进场的时候有没有加成功，退出的时候还是清理一下
			pkPlace->DelActorInFaction(pkPlayer->GetID(), (FactionType)iFactionID);
		}
	}
}

void LC_Server_GameStory_Base::PostPlayerEnterMap(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig || NULL == pkPlace)
	{
		return;
	}
	if (!IsActive() && pkPlace->GetMapLogicType()!=MT_GUILD_MAP)
	{
		return;
	}
	//clear accum ?
	int32_t iMapResID = pkPlace->GetMapResID();
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkPlayer);
	GameStoryScheduleKey iKey = GS_SCHEDULE_KEY_COMMON;
	if (pkMapSetting)
	{
		bool bOffline = pkPlayerStat && pkPlayerStat->IsEnterOffline();
		LC_GameStoryAsset& kGameStoryAsset = pkPlayer->GetGameStoryAsset();
		if (pkMapSetting->IsSolelySchedule())
		{
			if (!bOffline && pkPlayerStat)
			{
				GameStoryEntry* pkStoryEntry = kGameStoryAsset.GetGameStoryEntry(GetStoryID());

				if (pkStoryEntry)
				{
					pkStoryEntry->UpdateDirtyFlag();
					kGameStoryAsset.UpdateDirtyFlag();
				}
			}
			iKey = (GameStoryScheduleKey)pkPlace->GetMapLogicID();
			const unique_id_impl& instance = pkPlayer->GetInstance();
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "PostPlayerEnterMap Story:%d, Key:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d, offline:%d",
				GetStoryID(), iKey, instance.detail(), instance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), bOffline);
		}
		if (pkMapSetting->IsEnterClearCommonStat())
		{
			if (!bOffline && pkPlayerStat)
			{
				Utility::UT_SimOperator<bool, uint32_t, uint32_t> rTmp(true);
				_resetCommonStat(pkPlayer, GET_CURRENT_TIMESTAMP_IN_SECONDS(), rTmp, pkConfig);

				int storyID = GetStoryID();
				if(storyID==GameStoryIDType_Monkey || storyID==GameStoryIDType_Chest)
				{
					_resetAllCommonStat(pkPlayer);
				}

				AttackerStatData& kAttackerStatData = pkConfig->m_kAttackerStatData;
				if(kAttackerStatData.iMapType==StoryMapType::PET_ISLANDS)
				{
					int currentStoryID = GetStoryID();
					int shareStoryIDSize = kAttackerStatData.iShareKillNPCRewardLimitStoryID.size();
					if(shareStoryIDSize > 0)
					{
						for(int i=0; i<shareStoryIDSize; ++i)
						{
							int storyID = kAttackerStatData.iShareKillNPCRewardLimitStoryID[i];
							if(storyID == currentStoryID)
							{
								_resetAllCommonStat(pkPlayer);
								continue;
							}

							LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
							LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
							if (NULL != pkStory)
							{
								pkStory->_resetAllCommonStat(pkPlayer);
							}
						}
					}
				}

				uint32_t flag = pkMapSetting->GetComStatFlag();
				if(0 == flag)
				{
					pkPlayerStat->Reset();
				}
				else
				{
					if(!pkMapSetting->IsEnterSaveCommonStat_Damage())
					{
						pkPlayerStat->SetDamageCount(0);
					}
					if(!pkMapSetting->IsEnterSaveCommonStat_KillNpc())
					{
						pkPlayerStat->SetKillNPCCount(0);
					}				
					if(!pkMapSetting->IsEnterSaveCommonStat_Injured())
					{
						pkPlayerStat->SetInjueredCount(0);
					}
					if(!pkMapSetting->IsEnterSaveCommonStat_Dead())
					{
						pkPlayerStat->SetDeadTimes(0);
					}
					if(!pkMapSetting->IsEnterSaveCommonStat_KillPlayer())
					{
						pkPlayerStat->SetKillPlayerCount(0);
					}
					if(!pkMapSetting->IsEnterSaveCommonStat_KillReward())
					{
						pkPlayerStat->SetKillRewardCount(0);
					}
					if(!pkMapSetting->IsEnterSaveCommonStat_Custom())
					{
						pkPlayerStat->SetCustomCount(0);
					}
					if(!pkMapSetting->IsEnterSaveKillNpcSpecialCount())
					{
						pkPlayerStat->SetKillSpecialNPCCount(0);
					}
				}
			}
		}
		GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
		if (pkRTBSchedule && !pkRTBSchedule->IsInited() && pkConfig->m_kBatchSechedule.kConfig.IsHoldForPlayer())
		{
			int iSeq = 0;
			if (pkConfig->m_kCmnStatData.IsValid() && pkConfig->m_kCmnStatData.GetCmnStatEntry(GS_CMN_STAT_TP_BATCH))
			{
				LC_CommonStatEntry* pkEntry = pkPlayer->GetCommonStatAsset().GetCommonStatEntry(pkConfig->m_kCmnStatData.iID);
				if (pkEntry)
				{
					iSeq = (int)pkEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_CUR_BATCH);
				}
			}
			if (pkConfig->m_kBatchSechedule.kConfig.iPrepareTime > 0)
			{
				UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
				pkRTBSchedule->iInitTimerID = pkTimerMgr->DelayCall((int64_t)pkConfig->m_kBatchSechedule.kConfig.iPrepareTime * ONE_SECOND_MILI,
											  _initBatchCB,
											  (void(*)(Utility::timer_id_type, LC_Server_GameStory_Base*, GameStoryScheduleKey, int))NULL,
											  this,
											  iKey,
											  iSeq);
			}
			else
			{
				_initBatchSchedule(iKey, pkConfig, false, iSeq, Utility::TIMER_ID_NA, (LC_ServerPlayer*)pkPlayer);
			}
		}
		else
		{
			if (pkMapSetting->IsSolelySchedule())
			{
				NotifyPlayerGameStoryBatch(iKey, (LC_ServerPlayer*)pkPlayer, Utility::TIMER_ID_NA,  pkConfig);
			}
		}
		if (!bOffline && pkMapSetting->IsEnterComsumeJoinTimes())
		{
			PlayerEnter((LC_ServerPlayer*)pkPlayer, pkPlayer->GetMapResID());
		}
		NotifyRankInfo(iKey, (LC_ServerPlayer*)pkPlayer, true, pkConfig);

		if(NULL != pkMapSetting->pfnChooseFaction)
		{
			int32_t iFactionID = (*pkMapSetting->pfnChooseFaction)(pkPlayer);
			((LC_ServerMapLogic_Place*)pkPlace)->AddActor2Faction(pkPlayer, (FactionType)iFactionID);
			SetFactionScoreDirty(iKey);
			OnPlayerInWar(pkPlayer);
		}
		if (NULL != pkMapSetting->pfnActorBuff && pkPlayer->IsMainController())
		{
			(*pkMapSetting->pfnActorBuff)(pkPlayer, pkPlayerStat);
		}
		if (!pkMapSetting->kFactionBuffs.empty() && pkPlayer->IsMainController())
		{
			const FactionBuffDataList& kFactionBuffs = pkMapSetting->kFactionBuffs;
			FactionBuffDataList::const_iterator it = kFactionBuffs.begin();
			for (; it != kFactionBuffs.end(); ++it)
			{
				it->AttachTo(pkPlayer);
			}
		}
	}
	if (pkPlayerStat)
	{
		pkPlayerStat->SetEnterOffline(false);
	}
}

void LC_Server_GameStory_Base::PlayerLogicAreaChanged(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace, int32_t iPre)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig || NULL == pkPlace)
	{
		return;
	}
	int32_t iMapResID = pkPlace->GetMapResID();
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	if (pkMapSetting && pkPlayer)
	{
		if(false == pkPlayer->IsMainController())
		{
			return;
		}

		BuffDataList::const_iterator itBuff = pkMapSetting->kBuffs.begin();
		for (; itBuff != pkMapSetting->kBuffs.end(); ++itBuff)
		{
			if (itBuff->IsKeyID(iPre) && itBuff->IsKeyID(pkPlayer->GetLogicAreaId()))
			{
				continue;
			}
			if (iPre > 0)
			{
				itBuff->DettachFrom(pkPlayer, iPre);
			}
			if (pkPlayer->GetLogicAreaId() > 0)
			{
				itBuff->AttachTo(pkPlayer, pkPlayer->GetLogicAreaId());
			}
		}
	}
}

void LC_Server_GameStory_Base::NotifyEnterOfflinePlayer(LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	//clear accum ?
	int32_t iMapResID = pkPlace->GetMapResID();
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	if (pkMapSetting)
	{
		GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(pkPlayer);
		if (pkPlayerStat)
		{
			pkPlayerStat->SetEnterOffline(true);
		}
	}
}

void LC_Server_GameStory_Base::ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent, LC_StoryLogic_Manager* pkStoryLogicManager, GameLogic::LC_PlayerBase* pkPlayer)
{
	_processCommonStatEvent(pkEvent, pkStoryLogicManager, pkPlayer);
}
//----------------------------------------------------------------------------------------------------
void LC_Server_GameStory_Base::NotifyUserStoryStart(LC_ServerPlayer* pkLoginPlayer)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (pkConfig)
	{
		GSRTTipSchedule kRTTipSchedule;
		kRTTipSchedule.uiBenchmark = 1;
		kRTTipSchedule.AddTip(pkConfig->m_kBeginTip, pkLoginPlayer);
		GSRTTipEntry* pkRTTipEntry = kRTTipSchedule.Schedule(1);
		if (pkRTTipEntry)
		{
			pkRTTipEntry->kActorFilter.eTargetType = GS_TAR_TP_ME;

			NotifyTip(*pkRTTipEntry);
		}
		kRTTipSchedule.Reset();
	}
}
//----------------------------------------------------------------------------------------------------
void LC_Server_GameStory_Base::NotifyTip(GSRTTipEntry& kRTTipEntry, GameLogic::LC_ActorBase* pkKiller)
{
	GSRTPlayerFilter& kRTFilter = kRTTipEntry.kActorFilter;
	_setupActorFilter(kRTFilter);
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	const GameLogic::LC_ActorPtrList& kActors = kRTFilter.GetCoreActors();

	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(kRTTipEntry.RandomContent(), noticeFmort);
	if(!res)
	{
		return;
	}
	noticeFmort.SetChannelType(kRTTipEntry.iChannel);

	MG_GameNotice noticeMsg;
	UT_ServerHelper::FillGameNoticeMsg(noticeFmort, noticeMsg);

	switch (kRTFilter.eTargetType)
	{
		case GS_TAR_TP_NA:
			{
				if (kRTFilter.kLogicMaps.empty())
				{
					pkWorldManager->BroadCast(MGPT_GAME_NOTICE_MSG, &noticeMsg);
				}
				else
				{
					mem::vector<GameLogic::LC_MapLogicBase*>::iterator it = kRTFilter.kLogicMaps.begin();
					for (; it != kRTFilter.kLogicMaps.end(); ++it)
					{
						((LC_ServerMapLogicBase*)(*it))->BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
					}
				}
			}
			break;
		case GS_TAR_TP_ME:
			{
				LC_ActorPtrList::const_iterator cit = kActors.begin();
				for (; cit != kActors.end(); ++cit)
				{
					LC_ActorBase* pkActor = *cit;
					if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
					{
						((LC_ServerPlayer*)pkActor)->SendMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
					}
				}
			}
			break;
		case GS_TAR_TP_AREA:
			{
				mem::vector<GameLogic::LC_MapLogicBase*>::iterator it = kRTFilter.kLogicMaps.begin();
				for (; it != kRTFilter.kLogicMaps.end(); ++it)
				{
					((LC_ServerMapLogicBase*)(*it))->BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
				}
			}
			break;
		case GS_TAR_TP_GUILD:
		case GS_TAR_TP_GUILDCROSS:
			{
				GSLogicUint64IDSet::iterator itGuild = kRTFilter.kGuildIDs.begin();
				for (; itGuild != kRTFilter.kGuildIDs.end(); ++itGuild)
				{
					uint64_t guildID = *itGuild;
					bool bOnce = false;
					if (pkKiller != NULL && INVALID_GUILD_ID == guildID)
					{
						guildID = ((LC_PlayerBase*)pkKiller)->GetGuildID();
						bOnce = true;
					}
					if (INVALID_GUILD_ID != guildID)
					{
						LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
						LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(guildID);
						if (pkGuild != NULL)
						{
							if (kRTFilter.eTargetType == GS_TAR_TP_GUILDCROSS)
							{
								noticeMsg.m_nGuildGroupID  = guildID;
								SendMsgToSociety(MGPT_GAME_NOTICE_MSG, &noticeMsg);
							}
							else
							{
								pkGuild->BroadCastMsg(MGPT_GAME_NOTICE_MSG, &noticeMsg, INVALID_UNIQUE_ID, true);
							}
						}
						if (bOnce == true)
						{
							break;
						}
					}
				}
			}
			break;
		case GS_TAR_TP_WORLD:
			{
				//全服公告，主节点NotifyTip
				if(GetGameServerEntryID() == GetGameServerID())
				{
					pkWorldManager->BroadCast(MGPT_GAME_NOTICE_MSG, &noticeMsg, true);
				}
			}
			break;
		case GS_TAR_TP_GLOBAL_SERVERNOTIFY:
			{
				if(true == GetGameServerApp()->IsNodeType_Global())
				{
					pkWorldManager->BroadCast(MGPT_GAME_NOTICE_MSG, &noticeMsg, true);
				}
			}
			break;
		case GS_TAP_TP_SINGLE_WORLD:
			{
				pkWorldManager->BroadCast(MGPT_GAME_NOTICE_MSG, &noticeMsg, false);
			}
			break;
		case GS_TAR_TP_SERVERGROUP:
		case GS_TAR_TP_SERVERGROUPNID:
			{
				pkWorldManager->BroadCast(MGPT_GAME_NOTICE_MSG, &noticeMsg, true, UT_ServerHelper::GetServerId(), MG_SERVER_ROUTER_FLAG_GROUP);
			}
			break;
		default:
			break;
	}
}

int32_t LC_Server_GameStory_Base::NotifyPlayerGameStoryBatch(GameStoryScheduleKey iKey, LC_ServerPlayer* pkServerPlayer, Utility::timer_id_type iTimerID, GameStoryConfig* pkConfig)
{
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();

	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (NULL == pkConfig)
	{
		return 0;
	}
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return 1;
	}

	uint32_t nextTriTimestamp = 0;

	MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	MG_GAMESTORY_BATCH_INFO_SYNC syncMsg;
	GAMESTORY_BATCH_INFO& kStoryBatch = syncMsg.m_kGameStoryBatch;
	kStoryBatch.m_uiStoryID = GetStoryID();
	kStoryBatch.m_uiFlag = pkRTBSchedule->GetFlag();
	kStoryBatch.m_uiInitTime = pkRTBSchedule->uiInitTime;
	kStoryBatch.m_uiClearTime = pkRTBSchedule->uiCleartime;
	GSRTMonsterSetting& kMonSetting = pkRTBSchedule->kMonsterSetting;
	kStoryBatch.m_kMonExtraSkills.insert(kMonSetting.kExtraSkillIDs.begin(), kMonSetting.kExtraSkillIDs.end());
	GSRTID2BatchDetailMap::iterator itBD = pkRTBSchedule->kDetailMap.begin();
	mem::vector <uint32_t> vecNextTimer;
	for (; itBD != pkRTBSchedule->kDetailMap.end(); ++itBD)
	{
		GSRTBatchDetail& kRTBDetail = itBD->second;
		PS_GAMESTORY_BATCH_DETAIL psDetail;
		int64_t iTriTime  = 0;
		int64_t iEndTime = 0;
		if (kRTBDetail.iTriTimerID > 0)
		{
			if (kRTBDetail.iTriTimerID != iTimerID)
			{
				iTriTime = pkTimerMgr->GetTriTimestamp(kRTBDetail.iTriTimerID);
			}
			else
			{
				iTriTime = pkTimerMgr->GetNextTriTimestamp(kRTBDetail.iTriTimerID);
			}
		}
		if (kRTBDetail.iEndTimerID > 0)
		{
			if (kRTBDetail.iEndTimerID != iTimerID)
			{
				iEndTime = pkTimerMgr->GetTriTimestamp(kRTBDetail.iEndTimerID);
			}
			else
			{
				iEndTime = pkTimerMgr->GetNextTriTimestamp(kRTBDetail.iEndTimerID);
			}
		}
		psDetail.m_uiSeqIdx = (uint16_t)kRTBDetail.kBatchIdx.iSeqIdx;
		psDetail.m_uiBatchIdx = (uint16_t)kRTBDetail.kBatchIdx.iBatchIdx;
		psDetail.m_iTriTimestamp = int32_t(iTriTime / 1000);
		psDetail.m_iEndTimestamp = int32_t(iEndTime / 1000);
		kStoryBatch.m_kDetails.push_back(psDetail);

		vecNextTimer.push_back(psDetail.m_iTriTimestamp);
	}
	AreaIndices::iterator it = kMapCtrlSetting.m_kKeyAreaIndics.begin();
	bool tag = kMapCtrlSetting.IsRefreshNextTime();
	int maxBatchIdx = 0;
	if(tag)
	{
		for (; it != kMapCtrlSetting.m_kKeyAreaIndics.end(); ++it)
		{
			const AreaData* pkAreaData = kMapCtrlSetting.GetAreaData(*it);
			if (NULL == pkAreaData)
			{
				continue;
			}

			GSRTCommonStats& kRTCmnStat = pkRTBSchedule->kAreaCmnStat[*it];
			PS_GAMESTORY_AREA_INFO psArea;
			psArea.m_uiAreaIndex = *it;
			if (!kRTCmnStat.kKeyMonsterMap.empty())
			{
				PS_GAMESTORY_KEY_INFO psKey;
				GSRTKeyMonsterMap::iterator itKey = kRTCmnStat.kKeyMonsterMap.begin();
				for (; itKey != kRTCmnStat.kKeyMonsterMap.end(); ++itKey)
				{
					GSRTKeyMonster& kRTKeyMonster = itKey->second;
					if(maxBatchIdx < kRTKeyMonster.kBatchIndex.iBatchIdx)
					{
						maxBatchIdx = kRTKeyMonster.kBatchIndex.iBatchIdx;
					}
				}
			}
		}
	}
	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	int nLoopIdx = 0;
	for (it=kMapCtrlSetting.m_kKeyAreaIndics.begin(); it!=kMapCtrlSetting.m_kKeyAreaIndics.end(); ++it, ++nLoopIdx)
	{
		int i = *it;
		const AreaData* pkAreaData = kMapCtrlSetting.GetAreaData(*it);
		if (NULL == pkAreaData)
		{
			continue;
		}
		GSRTCommonStats& kRTCmnStat = pkRTBSchedule->kAreaCmnStat[*it];
		PS_GAMESTORY_AREA_INFO psArea;
		psArea.m_uiAreaIndex = *it;
		if(tag && kRTCmnStat.kKeyMonsterMap.empty())
		{
			PS_GAMESTORY_KEY_INFO psKey;
			psKey.m_nState =E_MASTER_GROUP_STATE_DIE;
			psKey.m_iResID =  kMapCtrlSetting.m_kMonsterSetting.kMonsterDatas[i].lCharTypeID;
			
			//对最后一波特殊处理
			if(nextTriTimestamp-currentTime>30*60)
			{
				nextTriTimestamp = 0;
			}
			//psKey.m_uiTimestamp = nextTriTimestamp;
			psKey.m_uiTimestamp = vecNextTimer[nLoopIdx];
			if(pkServerPlayer != NULL)
			{
				psKey.m_nLineNum = pkServerPlayer->GetGlobalLineNum();
			}
			else
			{
				LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
				LC_ServerMapLogic_Place* pLogic = pkWorldManager->GetMapLogic(iKey);
				if ( pLogic )
				{
					psKey.m_nLineNum = pLogic->GetMap()->GetGlobalLineNum();
				}
			}
			psArea.m_kKeyInfos.push_back(psKey);
		}

		if (!kRTCmnStat.kKeyMonsterMap.empty())
		{
			PS_GAMESTORY_KEY_INFO psKey;
			GSRTKeyMonsterMap::iterator itKey = kRTCmnStat.kKeyMonsterMap.begin();
			for (; itKey != kRTCmnStat.kKeyMonsterMap.end(); ++itKey)
			{
				GSRTKeyMonster& kRTKeyMonster = itKey->second;
				if (kRTKeyMonster.uiTimestamp <= psKey.m_uiTimestamp && kRTKeyMonster.IsDead())
				{
					continue;
				}
				psKey.m_iResID = kRTKeyMonster.iResID;
				psKey.m_iLCID = itKey->first;
				psKey.m_uiCharID = kRTKeyMonster.uiCharID;
				TPSTR2STDSTR(kRTKeyMonster.sCharName, psKey.m_szCharName);
				psKey.m_nState = kRTKeyMonster.IsDead() ? E_MASTER_GROUP_STATE_DIE : E_MASTER_GROUP_STATE_SURVIVE ;
				if(tag)
				{
					//对最后一波特殊处理
					if(nextTriTimestamp-currentTime>30*60)
					{
						nextTriTimestamp = 0;
					}
					psKey.m_uiTimestamp = (psKey.m_nState == E_MASTER_GROUP_STATE_DIE) ? nextTriTimestamp : kRTKeyMonster.uiTimestamp;					
				}
				else
				{
					psKey.m_uiTimestamp = kRTKeyMonster.uiTimestamp;
				}
				psKey.m_nLineNum = kRTKeyMonster.lineNum;
				psArea.m_kKeyInfos.push_back(psKey);
			}
		}
		kStoryBatch.m_kAreaInfos.push_back(psArea);
	}

	int32_t res = 0;
	if (pkServerPlayer)
	{
		pkServerPlayer->SendMsgToClient(MGPT_GAMESTORY_BATCH_INFO_SYNC, &syncMsg);
		res = 2;
	}
	else if (IsScheduleMapLogicKey(iKey))
	{
		LC_ActorPtrList kActors;
		_getRTScheduleActors(iKey, kActors, pkConfig);

		LC_ActorPtrList::iterator it = kActors.begin();
		for (; it != kActors.end(); ++it)
		{
			((LC_ServerPlayer*)*it)->SendMsgToClient(MGPT_GAMESTORY_BATCH_INFO_SYNC, &syncMsg);
		}
		ReportMosterStateInfo(iKey, iTimerID);
		res = 3;
	}
	else
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		pkWorldManager->BroadCast(MGPT_GAMESTORY_BATCH_INFO_SYNC, &syncMsg, true, GetGameServerID());
		ReportMosterStateInfo(iKey, iTimerID);
		res = 4;
	}

	return res;
}

void LC_Server_GameStory_Base::NotifyRankInfo(GameStoryScheduleKey iKey, LC_ServerPlayer* pkServerPlayer, bool force, GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule || NULL == pkConfig)
	{
		return;
	}
	MG_GAMESTORY_RANK_INFO_SYNC kSyncMsg;
	int32_t sz = 0;
	GSRTRankEntryMap& kRankMap = m_kRTRankInfo.kEntryMap;
	GSRTRankEntryMap::iterator it = kRankMap.begin();
	for (; it != kRankMap.end(); ++it)
	{
		GSRTRankEntry& kRTRankEntry = it->second;
		if (kRTRankEntry.pkDataEntry && (force || kRTRankEntry.GetSyncDirty()))
		{
			kSyncMsg.kRanks.resize(sz + 1);
			GAMESTORY_RANK_INFO& psRankInfo = kSyncMsg.kRanks[sz++];
			psRankInfo.m_iRankType = kRTRankEntry.pkDataEntry->RankType();
			GSRTPlayerCmnStatIndices::iterator itStat = kRTRankEntry.kIndices.begin();
			for (int i = 0; itStat != kRTRankEntry.kIndices.end() && i < GS_RANK_TP_MAX_SIZE; ++itStat, ++i)
			{
				GSRTPlayerCommonStat* pkStat = *itStat;
				if (NULL == pkStat)
				{
					continue;
				}
				PS_RankActorDesc kDesc;
				kDesc.m_uiRank = (uint32_t)i;
				kDesc.m_uiCharID = pkStat->uiCharID;
				TPSTR2STDSTR(pkStat->sName, kDesc.m_szCharName);
				kDesc.m_iStat = (int64_t)kRTRankEntry.pkDataEntry->GetKeyValue(pkStat);
				psRankInfo.m_kActors.push_back(kDesc);
			}
			if (!force)
			{
				kRTRankEntry.SetSyncDirty(false);
			}
		}
	}

	if (!kSyncMsg.kRanks.empty())
	{
		if (pkServerPlayer)
		{
			pkServerPlayer->SendMsgToClient(MGPT_GAMESTORY_RANK_INFO_SYNC, &kSyncMsg);
		}
		else if (IsScheduleMapLogicKey(iKey))
		{
			LC_ActorPtrList kActors;
			_getRTScheduleActors(iKey, kActors, pkConfig);
			LC_ActorPtrList::iterator it = kActors.begin();
			for (; it != kActors.end(); ++it)
			{
				((LC_ServerPlayer*)*it)->SendMsgToClient(MGPT_GAMESTORY_RANK_INFO_SYNC, &kSyncMsg);
			}
		}
		else
		{
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			pkWorldManager->BroadCast(MGPT_GAMESTORY_RANK_INFO_SYNC, &kSyncMsg, true);
		}
	}
}

void LC_Server_GameStory_Base::NotifyActorEncountedStat(LC_ServerPlayer* pkServerPlayer, object_id_type uiID)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(uiID);
	if (NULL == pkActor)
	{
		return;
	}

	LC_EncountedStatMgr& kStatMgr = pkActor->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	unique_id_type uiCharID = pkServerPlayer->GetInstance();
	object_id_type uiPlayerID = pkServerPlayer->GetID();

	MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_NOTIFY notifyMsg;
	notifyMsg.m_iStroyID = GetStoryID();
	notifyMsg.m_uiID = uiID;
	notifyMsg.m_uiMapResID = pkActor->GetMapResID();
	notifyMsg.m_iLocX = pkActor->GetCurrentLocation().x;
	notifyMsg.m_iLocY = pkActor->GetCurrentLocation().y;
	notifyMsg.m_iMaxHP = pkActor->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
	notifyMsg.m_iCurHP = pkActor->GetHP();

	{
		bool bSelfIN = false;
		const uint64_object_id_vector* pkList = pkStat->GetSortedIDs(LC_ECT_CAT_ACTOR);
		if (pkList && !pkList->empty())
		{
			PS_ActorEcountedStat psStat;
			psStat.m_uiType = LC_ECT_CAT_ACTOR;
			uint64_object_id_vector::const_iterator cit = pkList->begin();
			uint16_t uiSkipCnt = 0;
			for (int i = 0; cit != pkList->end() && i < MAX_ENCOUNTED_COUNT; ++cit)
			{
				if (!IS_PLAYER_ID((int32_t)*cit))
				{
					++ uiSkipCnt;
					continue;
				}
				LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(LC_ECT_CAT_ACTOR);
				if (NULL == pkStatCon)
				{
					continue;
				}
				PS_RankActorDesc psDesc;
				psDesc.m_uiRank = i;
				psDesc.m_uiCharID = pkStatCon->GetStatCharID(*cit);
				if (*cit == uiPlayerID)
				{
					bSelfIN = true;
				}
				TPSTR2STDSTR(kStatMgr.GetActorName((object_id_type)*cit), psDesc.m_szCharName);
				psDesc.m_iStat = (int64_t)pkStatCon->GetStatValue(*cit);
				psStat.m_kStats.push_back(psDesc);
				++i;
			}
			if (!bSelfIN)
			{
				PS_RankActorDesc psDesc;
				cit = std::find(cit, pkList->end(), uiPlayerID);
				if (cit == pkList->end())
				{
					psDesc.m_uiRank = (uint16_t)pkList->size();
				}
				else
				{
					psDesc.m_uiRank = (uint16_t)(cit - pkList->begin()) - uiSkipCnt;
				}
				psDesc.m_uiCharID = uiCharID;
				TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName(), psDesc.m_szCharName);
				psDesc.m_iStat = (int64_t)pkStat->GetEncountedParam(LC_ECT_CAT_ACTOR, uiPlayerID);
				psStat.m_kStats.push_back(psDesc);
			}
			notifyMsg.m_kStats.push_back(psStat);
		}
	}
	{
		const uint64_object_id_vector* pkList = pkStat->GetSortedIDs(LC_ECT_CAT_SERVER);
		if (pkList && !pkList->empty())
		{
			PS_ActorEcountedStat psStat;
			psStat.m_uiType = LC_ECT_CAT_SERVER;
			uint64_object_id_vector::const_iterator cit = pkList->begin();
			//uint16_t uiSkipCnt = 0;
			for (int i = 0; cit != pkList->end() && i < MAX_ENCOUNTED_COUNT; ++cit)
			{
				LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(LC_ECT_CAT_SERVER);
				if (NULL == pkStatCon)
				{
					continue;
				}
				PS_RankActorDesc psDesc;
				psDesc.m_uiRank = i;
				psDesc.m_uiCharID = *cit;
				psDesc.m_iStat = (int64_t)pkStatCon->GetStatValue(*cit);
				psStat.m_kStats.push_back(psDesc);
				++i;
			}
			notifyMsg.m_kStats.push_back(psStat);
		}
	}
	pkServerPlayer->SendMsgToClient(MGPT_GAMESTORY_KEY_ECOUNTED_STAT_NOTIFY, &notifyMsg);
}

void	LC_Server_GameStory_Base::NotifyActorEncountedStatByDamageType(LC_ServerPlayer* pkServerPlayer, object_id_type uiID, int damageType)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(uiID);
	if (NULL == pkActor)
	{
		return;
	}

	LC_EncountedStatMgr& kStatMgr = pkActor->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	unique_id_type uiCharID = pkServerPlayer->GetInstance();
	object_id_type uiPlayerID = pkServerPlayer->GetID();
	uint64_t guildID = pkServerPlayer->GetGuildID();

	MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_NOTIFY_EX notifyMsg;
	notifyMsg.m_iStroyID = GetStoryID();
	notifyMsg.m_uiID = uiID;
	notifyMsg.m_uiMapResID = pkActor->GetMapResID();
	notifyMsg.m_iLocX = pkActor->GetCurrentLocation().x;
	notifyMsg.m_iLocY = pkActor->GetCurrentLocation().y;
	notifyMsg.m_iMaxHP = pkActor->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
	notifyMsg.m_iCurHP = pkActor->GetHP();
	notifyMsg.m_nDamageType = damageType;

	bool bSelfIN = false;
	int uiSkipCnt = 0;
	if(damageType==LC_ECT_CAT_ACTOR || damageType==LC_ECT_CAT_GUILD)
	{		
		const uint64_object_id_vector* pkList = pkStat->GetSortedIDs(damageType, true);
		if (pkList && !pkList->empty())
		{
			uint64_object_id_vector::const_iterator cit = pkList->begin();
			for (int i = 0; cit != pkList->end() && i < MAX_ENCOUNTED_COUNT; ++cit)
			{
				if (damageType==LC_ECT_CAT_ACTOR && !IS_PLAYER_ID(*cit))
				{
					++uiSkipCnt;
					continue;
				}

				LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(damageType);
				if (NULL != pkStatCon)
				{
					PS_RankActorDesc psDesc;
					psDesc.m_uiRank = i;
					psDesc.m_uiCharID = pkStatCon->GetStatCharID(*cit);
					psDesc.m_iStat = (int64_t)pkStatCon->GetStatValue(*cit);

					if(damageType==LC_ECT_CAT_ACTOR)
					{
						TPSTR2STDSTR(kStatMgr.GetActorName((object_id_type)*cit), psDesc.m_szCharName);
					}
					else if(damageType ==LC_ECT_CAT_GUILD)
					{
						TPSTR2STDSTR(kStatMgr.GetGuildName(*cit), psDesc.m_szCharName);
					}
					notifyMsg.m_kStats.push_back(psDesc);
					++i;

					if(damageType==LC_ECT_CAT_ACTOR && *cit==uiPlayerID)
					{
						bSelfIN = true;
						notifyMsg.m_kSelfStats.m_uiRank = psDesc.m_uiRank;
						notifyMsg.m_kSelfStats.m_uiCharID = psDesc.m_uiCharID;
						notifyMsg.m_kSelfStats.m_iStat = psDesc.m_iStat;
						notifyMsg.m_kSelfStats.m_szCharName = psDesc.m_szCharName;
					}
					else if(damageType==LC_ECT_CAT_GUILD && *cit==guildID)
					{
						bSelfIN = true;
						notifyMsg.m_kSelfStats.m_uiRank = psDesc.m_uiRank;
						notifyMsg.m_kSelfStats.m_uiCharID = psDesc.m_uiCharID;
						notifyMsg.m_kSelfStats.m_iStat = psDesc.m_iStat;
						notifyMsg.m_kSelfStats.m_szCharName = psDesc.m_szCharName;
					}
				}
			}

			if (!bSelfIN)
			{
				if(damageType == LC_ECT_CAT_ACTOR)
				{
					cit = std::find(cit, pkList->end(), uiPlayerID);
					if (cit == pkList->end())
					{
						notifyMsg.m_kSelfStats.m_uiRank = SPECIES_RANK_NONE;//未上榜
					}
					else
					{
						notifyMsg.m_kSelfStats.m_uiRank = (uint16_t)(cit - pkList->begin()) - uiSkipCnt;
					}
					notifyMsg.m_kSelfStats.m_uiCharID = uiCharID;
					notifyMsg.m_kSelfStats.m_iStat = (int64_t)pkStat->GetEncountedParam(damageType, uiPlayerID);
					TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName(), notifyMsg.m_kSelfStats.m_szCharName);
				}
				else if(damageType == LC_ECT_CAT_GUILD)
				{
					cit = std::find(cit, pkList->end(), guildID);
					if (cit == pkList->end())
					{
						notifyMsg.m_kSelfStats.m_uiRank = SPECIES_RANK_NONE;//未上榜
						notifyMsg.m_kSelfStats.m_uiCharID = guildID;
						notifyMsg.m_kSelfStats.m_iStat = (int64_t)pkStat->GetEncountedParam(damageType, guildID);
						notifyMsg.m_kSelfStats.m_szCharName = TPSTR2STDSTR(pkServerPlayer->GetGuildName());
					}
					else
					{
						notifyMsg.m_kSelfStats.m_uiRank = (uint16_t)(cit - pkList->begin()) - uiSkipCnt;
						notifyMsg.m_kSelfStats.m_uiCharID = guildID;
						notifyMsg.m_kSelfStats.m_iStat = (int64_t)pkStat->GetEncountedParam(damageType, guildID);
						TPSTR2STDSTR(kStatMgr.GetGuildName(*cit), notifyMsg.m_kSelfStats.m_szCharName);
					}
				}
			}
		}
		else
		{
			if(damageType==LC_ECT_CAT_ACTOR)
			{
				notifyMsg.m_kSelfStats.m_uiRank = SPECIES_RANK_NONE;//未上榜
				notifyMsg.m_kSelfStats.m_uiCharID = uiCharID;
				notifyMsg.m_kSelfStats.m_iStat = 0;
				notifyMsg.m_kSelfStats.m_szCharName = TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName());
			}
			else if(damageType==LC_ECT_CAT_GUILD)
			{
				notifyMsg.m_kSelfStats.m_uiRank = SPECIES_RANK_NONE;//未上榜
				notifyMsg.m_kSelfStats.m_uiCharID = guildID;
				notifyMsg.m_kSelfStats.m_iStat = 0;
				notifyMsg.m_kSelfStats.m_szCharName = TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName());
			}
		}
	}
	else if(damageType==LC_ECT_CAT_ACTOR_AND_GUILD && guildID!=0)
	{
		notifyMsg.m_kSelfStats.m_uiRank = SPECIES_RANK_NONE;//未上榜
		notifyMsg.m_kSelfStats.m_uiCharID = uiCharID;
		notifyMsg.m_kSelfStats.m_iStat = 0;
		notifyMsg.m_kSelfStats.m_szCharName = TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName());

		//按照伤害数据排的玩家id
		const uint64_object_id_vector* pkList = pkStat->GetSortedIDs(LC_ECT_CAT_ACTOR, true);
		LC_EncountedStatContainer* pkStatPlayerCon = pkStat->GetEncountedStat(LC_ECT_CAT_ACTOR);
		if(NULL != pkList && NULL != pkStatPlayerCon)
		{
			uint64_object_id_vector::const_iterator cit = pkList->begin();
			for (int i = 0; cit != pkList->end() && i < 3*MAX_ENCOUNTED_COUNT; ++cit)
			{
				if (!IS_PLAYER_ID(*cit))
				{
					++uiSkipCnt;
					continue;
				}

				LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
				LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildID);
				if(NULL != pkGuildInfo)
				{
					unique_id_type playerID = pkStatPlayerCon->GetStatCharID(*cit);
					if(pkGuildInfo->GetMember(playerID))
					{
						PS_RankActorDesc psDesc;
						psDesc.m_uiRank = i;
						psDesc.m_uiCharID = pkStatPlayerCon->GetStatCharID(*cit);
						psDesc.m_iStat = (int64_t)pkStatPlayerCon->GetStatValue(*cit);
						TPSTR2STDSTR(kStatMgr.GetActorName((object_id_type)*cit), psDesc.m_szCharName);
						notifyMsg.m_kStats.push_back(psDesc);
						 ++i;

						if(*cit==uiPlayerID)
						{
							bSelfIN = true;
							notifyMsg.m_kSelfStats.m_uiRank = psDesc.m_uiRank;
							notifyMsg.m_kSelfStats.m_uiCharID = psDesc.m_uiCharID;
							notifyMsg.m_kSelfStats.m_iStat = psDesc.m_iStat;
							notifyMsg.m_kSelfStats.m_szCharName = psDesc.m_szCharName;
						}
					}
				}
			}

			if(!bSelfIN)
			{
				cit = std::find(cit, pkList->end(), uiPlayerID);
				if (cit == pkList->end())
				{
					notifyMsg.m_kSelfStats.m_uiRank = SPECIES_RANK_NONE;//未上榜
				}
				else
				{
					notifyMsg.m_kSelfStats.m_uiRank = (uint16_t)(cit - pkList->begin()) - uiSkipCnt;
				}
				notifyMsg.m_kSelfStats.m_uiCharID = uiCharID;
				notifyMsg.m_kSelfStats.m_iStat = (int64_t)pkStat->GetEncountedParam(damageType, uiPlayerID);
				TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName(), notifyMsg.m_kSelfStats.m_szCharName);
			}
		}
	}

	pkServerPlayer->SendMsgToClient(MGPT_GAMESTORY_KEY_ECOUNTED_STAT_NOTIFY_EX, &notifyMsg);
}

uint64_t LC_Server_GameStory_Base::GetActorDamageData(LC_ServerPlayer* pkOwnerPlayer, object_id_type uiID, int type)
{
	if(NULL == pkOwnerPlayer)
	{
		return 0;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(uiID);
	if (NULL == pkActor)
	{
		return 0;
	}

	LC_EncountedStatMgr& kStatMgr = pkActor->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return 0;
	}

	LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(type);
	if (NULL == pkStatCon)
	{
		return 0;
	}

	uint64_t value = 0;
	switch(type)
	{
	case LC_ECT_CAT_ACTOR:
		value = pkStatCon->GetStatValue(pkOwnerPlayer->GetID());
		break;
	case LC_ECT_CAT_GUILD:
		value = pkStatCon->GetStatValue(pkOwnerPlayer->GetGuildID());
		break;
	default:
		break;
	}

	return value;
}

//----------------------------------------------------------------------------------------------------
GSRTPlayerCommonStat* LC_Server_GameStory_Base::GetPlayerStats(const unique_id_impl& charID, bool bForceExist, StringType sName)
{
	GSRTPlayerCmnStatMap::iterator it = m_kPlayerCmnStatMap.find(charID);
	GSRTPlayerCommonStat* pkPlayerStat = NULL;
	if (it != m_kPlayerCmnStatMap.end())
	{
		pkPlayerStat = &it->second;
	}
	else
	{
		if (bForceExist)
		{
			pkPlayerStat = &m_kPlayerCmnStatMap[charID];
			pkPlayerStat->uiCharID = charID;
		}
	}
	if (pkPlayerStat && !sName.empty())
	{
		pkPlayerStat->sName = sName;
	}
	return pkPlayerStat;
}
//----------------------------------------------------------------------------------------------------
GSRTPlayerCommonStat* LC_Server_GameStory_Base::GetPlayerStats(GameLogic::LC_PlayerBase* pkPlayer)
{
	const unique_id_impl& charID = pkPlayer->GetInstance();
	GSRTPlayerCmnStatMap::iterator it = m_kPlayerCmnStatMap.find(charID);
	GSRTPlayerCommonStat* pkPlayerStat = NULL;
	if (it != m_kPlayerCmnStatMap.end())
	{
		pkPlayerStat = &it->second;
	}
	else
	{
		pkPlayerStat = &m_kPlayerCmnStatMap[charID];
		pkPlayerStat->uiID = pkPlayer->GetID();
		pkPlayerStat->uiCharID = charID;
	}

	{
		pkPlayerStat->sName = pkPlayer->GetOwnerCharName();
		pkPlayerStat->iLevel = pkPlayer->GetLevel();
	}
	return pkPlayerStat;
}
void LC_Server_GameStory_Base::GetAllPlayerIDs(mem::vector<object_id_type>& playerVec)
{	
	for(GSRTPlayerCmnStatMap::iterator it=m_kPlayerCmnStatMap.begin(); it!=m_kPlayerCmnStatMap.end(); ++it)
	{
		object_id_type playerID = it->second.uiID;
		playerVec.push_back(playerID);
	}	
}
void LC_Server_GameStory_Base::GetStoryPlayer(GameStoryScheduleKey iKey, mem::vector<object_id_type>& playerVec, GameStoryConfig* pkConfig)
{
	GameLogic::LC_ActorPtrList kActors;
	_getRTScheduleActors(iKey, kActors, pkConfig);

	for(LC_ActorPtrList::iterator it=kActors.begin(); it!=kActors.end(); ++it)
	{
		object_id_type id = (*it)->GetOwnerObjectID();
		playerVec.push_back(id);
	}
}
//---------------------------------------------------------------------------------------------------
void LC_Server_GameStory_Base::GetCurrentStatReward(const unique_id_impl& charID, GSCommonReward& gsReward)
{
	GSRTPlayerCommonStat* pkPlyCmnStat = GetPlayerStats(charID);
	if (pkPlyCmnStat)
	{
		gsReward.dExp += pkPlyCmnStat->dAccumExp;
		gsReward.dCash += pkPlyCmnStat->dAccumCash;
		gsReward.dSP += pkPlyCmnStat->dAccumSP;
		gsReward.iStatCnt += pkPlyCmnStat->GetKillNPCCount();
		gsReward.AddItems(pkPlyCmnStat->kAccumItems);
	}

	GameStoryConfig* pkConfig = GetConfig();
	if (pkConfig)
	{
		const ThresholdReward* pkSTReward = pkConfig->m_kNPCStatRewardInfo.GetCurrentStatReward(gsReward.iStatCnt);
		if (pkSTReward)
		{
			pkSTReward->kReward.ResolveReward(RewardParam(), gsReward);
		}
		if (pkConfig->m_sRewardMailTitle.empty())
		{
			gsReward.sMailTitle = GetCoreRecord()->m_sStoryName;
		}
		else
		{
			gsReward.sMailTitle = pkConfig->m_sRewardMailTitle;
		}
		if (pkConfig->m_sRewardMailDesc.empty())
		{
			gsReward.sMailDesc = GetCoreRecord()->m_sStoryName;
		}
		else
		{
			gsReward.sMailDesc = pkConfig->m_sRewardMailDesc;
		}
	}
}
void LC_Server_GameStory_Base::_initBatchCB(Utility::timer_id_type iTimerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iSeq)
{
	if (NULL == pkStory)
	{
		return;
	}
	GameStoryConfig* pkConfig = pkStory->GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	pkStory->_initBatchSchedule(iKey, pkConfig, false, iSeq, iTimerID);
}

void LC_Server_GameStory_Base::_triggerBatchCB(Utility::timer_id_type iTimerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, int iAutoFitLevel)
{
	if (NULL == pkStory)
	{
		return;
	}
	_triggerBatchCB_CF(iTimerID, pkStory, iKey, iDetailIndex, iAutoFitLevel, pkStory->GetConfig());
}

void LC_Server_GameStory_Base::_triggerBatchCB_CF(Utility::timer_id_type iTimerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, int iAutoFitLevel, GameStoryConfig* pkConfig, bool limitNotice, object_id_type iParam3, bool triggerSave, int64_t vaildTime)
{
	if (NULL == pkStory || NULL == pkConfig)
	{
		return;
	}

	int storyID = pkStory->GetStoryID();
	GSRTBatchSchedule* pkRTBSchedule = pkStory->_getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}
	if (!pkRTBSchedule->IsNormal())
	{
		return;
	}
	GSRTBatchDetail* pkBDetail = pkRTBSchedule->GetBatchDetail(iDetailIndex);
	if (NULL == pkBDetail)
	{
		return;
	}
	
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GSRTBatchIndex& kRTBIdx = pkBDetail->kBatchIdx;
	int32_t iBatchIdx = kRTBIdx.iBatchIdx;
	BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(iBatchIdx);
	if (pkBatchData)
	{
		int isClear = pkBatchData->iIsClear;
		switch (pkBatchData->iType)
		{
			case GSBSTP_SEQUECE:
			case GSBSTP_CYCLE:
				{
					if (isClear!=1 && !IsTimerIDNA(pkBDetail->iTriTimerID))
					//if (!IsTimerIDNA(pkBDetail->iTriTimerID))
					{
						pkTimerMgr->CancelTimer(pkBDetail->iTriTimerID);
						pkBDetail->iTriTimerID = TIMER_ID_NA;
					}
				}
				break;
			case GSBSTP_TRIGGER:
			case GSBSTP_TIMER:
				break;
			case GSBSTP_PASSIVE:
			case GSBSTP_CONDITION:
				{
					if (pkBatchData->iBeTriTimes>0 && pkBatchData->iBeTriTimes<=kRTBIdx.iTimes)
					{
						return;
					}
				}
				break;
			default:
				break;
		}
		++kRTBIdx.iTimes;
		pkStory->_spawnBatchMonster(iKey, iDetailIndex, pkConfig, NULL, NULL, iAutoFitLevel, pkConfig, TIMER_ID_NA, iParam3);

		uint64_t endTime = 0;
		int batchTime = 0;
		uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if(pkConfig->IsSaveState() && true==GetGameServerApp()->IsNodeType_Global())
		{
			if(pkBatchData->iBatchTime > 1)
			{
				batchTime = vaildTime==0 ? pkBatchData->iBatchTime : vaildTime;
				endTime = currentTime + batchTime;
			}
			
			mem::vector<uint64_t> input;
			input.push_back(iBatchIdx);

			//将刷怪进度存到redis
			char szNameSpace[MAX_CHAR_NAME_LENGTH + 1] = { '\0' };
			sprintf_s( szNameSpace, MAX_CHAR_NAME_LENGTH + 1, ":%s", pkConfig->FileName() );
			UT_ServerHelper::RedisSetData_Int(szNameSpace, input, endTime);

			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "save refresh npc state to redis! serverid:%d, fildName:%s, batchIndex:%d, endTime:%d\n", GetGameServerID(), pkConfig->FileName(), iBatchIdx, endTime);
		}

		int endTimerID = 0;
		if (pkBatchData->iBatchTime > 0)
		{
			if(triggerSave || pkBatchData->iBatchTime>1)
			{
				batchTime = vaildTime==0 ? pkBatchData->iBatchTime : vaildTime;

				pkBDetail->iEndTimerID = pkTimerMgr->DelayCall(batchTime * ONE_SECOND_MILI,
					LC_Server_GameStory_Base::_endBatchEndCB,
					(void (*)(Utility::timer_id_type, LC_Server_GameStory_Base*, GameStoryScheduleKey, int, LC_ActorBase*))(NULL),
					pkStory,
					iKey,
					iDetailIndex,
					(LC_ActorBase*)NULL);
				endTimerID = pkBDetail->iEndTimerID;
			}
		}
		else
		{
			if(triggerSave)
			{
				_endBatchEndCB_CF(iTimerID, pkStory, iKey, iDetailIndex, NULL, pkConfig);
			}			
		}
		
		//do lua actions
		if(triggerSave)
		{
			BatchData::ActionList::iterator it = pkBatchData->kActions.begin();
			for (; it != pkBatchData->kActions.end(); ++it)
			{
				if (*it)
				{
					(**it)(pkStory->GetStoryID(), iKey, iDetailIndex, endTimerID);
				}
			}
		}
		if (!limitNotice && !pkBatchData->IsTrivial())
		{
			pkStory->NotifyPlayerGameStoryBatch(iKey, NULL, iTimerID, pkConfig);
		}
	}
}

void LC_Server_GameStory_Base::_endBatchEndCB(Utility::timer_id_type timerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, GameLogic::LC_ActorBase* pkTriActor)
{
	if (NULL == pkStory)
	{
		return;
	}
	_endBatchEndCB_CF(timerID, pkStory, iKey, iDetailIndex, pkTriActor, pkStory->GetConfig());
}

void LC_Server_GameStory_Base::_endBatchEndCB_CF(Utility::timer_id_type timerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, LC_ActorBase* pkTriActor, GameStoryConfig* pkConfig)
{
	if (NULL == pkStory || NULL == pkConfig)
	{
		return;
	}

	int storyID = pkStory->GetStoryID();
	GSRTBatchSchedule* pkRTBSchedule = pkStory->_getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}
	GSRTBatchDetail* pkBDetail = pkRTBSchedule->GetBatchDetail(iDetailIndex);
	if (NULL == pkBDetail)
	{
		return;
	}
	GSRTBatchIndex& kRTBIdx = pkBDetail->kBatchIdx;
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (!IsTimerIDNA(pkBDetail->iEndTimerID))
	{
		pkTimerMgr->CancelTimer(pkBDetail->iEndTimerID);
		pkBDetail->iEndTimerID = TIMER_ID_NA;
	}
	int32_t iBatchIdx = pkBDetail->kBatchIdx.iBatchIdx;
	BatchConfig& kBConfig = pkConfig->m_kBatchSechedule.kConfig;
	BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(iBatchIdx);

	if (pkBatchData)
	{
		if(pkConfig->IsSaveState() && !pkBatchData->bNpcDeadRemoveBatchIndex && true==GetGameServerApp()->IsNodeType_Global())
		{
			//将刷怪进度从redis删掉
			mem::vector<uint64_t> input;
			input.push_back(iBatchIdx);

			char szNameSpace[MAX_CHAR_NAME_LENGTH + 1] = { '\0' };
			sprintf_s( szNameSpace, MAX_CHAR_NAME_LENGTH + 1, ":%s", pkConfig->FileName() );
			UT_ServerHelper::RedisRemoveData_Int(szNameSpace, input);
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "delete refresh npc state from redis! serverid:%d, fildName:%s, batchIndex:%d\n", GetGameServerID(), pkConfig->FileName(), iBatchIdx);
		}

		//handle batch end op
		if (pkRTBSchedule->IsNormal())
		{
			if (kBConfig.IsBatchEndToCheckClear())
			{
				if (!pkRTBSchedule->IsCleared())
				{
					// to end the schedule
					pkRTBSchedule->SetFail();
					pkStory->_clearBatchSchedule(iKey, pkConfig);
					
					int storyID = pkStory->GetStoryID();
					if(storyID >= STORYID_ENDLESS_TOWER1 && storyID <= STORYID_ENDLESS_TOWER6)
					{
						mem::vector<object_id_type> playerVec;
						pkStory->GetStoryPlayer(iKey, playerVec, pkConfig);
						for(int i=0; i<playerVec.size(); ++i)
						{
							LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
							GameLogic::LC_PlayerBase* pPlayer = (GameLogic::LC_PlayerBase*)pkWorldManager->FindObject(playerVec[i]);
							if ( pPlayer == NULL )
							{
								continue;
							}

							LC_CommonStatAsset& kCommonStatAsset = pPlayer->GetCommonStatAsset();
							LC_CommonStatEntry* pkCmnStatEntry = kCommonStatAsset.GetCommonStatEntry(pkConfig->m_kCmnStatData.iID);
							if(NULL == pkCmnStatEntry)
							{
								continue;
							}

							int stat = pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_CUR_BATCH);

							LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
							LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
							if (NULL != pkGameEvent)
							{
								pkGameEvent->SetEventType(GLET_ENDLESSTOWER_EVENT);
								pkGameEvent->SetEventParam(ET_STATE, 0);//失败
								pkGameEvent->SetEventParam(ET_PLAYER_ID, playerVec[i]);
								pkGameEvent->SetEventParam(ET_STORY_ID, storyID);	
								pkGameEvent->SetEventParam(ET_STORY_STAT, stat+1);	
								pkEventManager->InsertGameEvent(pkGameEvent);
							}
						}
					}
					return;
				}
			}
			
			if (kBConfig.IsBatchEndToReward())
			{
				int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, 1);
				vector<int> nParams;
				nParams.clear();
				if(pkTriActor != NULL)
					nParams.push_back(pkTriActor->GetMapResID());

				LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
				if (kBConfig.kBatchReward.IsValid())
				{
					GameStory::GSRTPlayerFilter kRTActorFilter;
					kRTActorFilter.eTargetType = kBConfig.kBatchReward.eTargetType;
					kRTActorFilter.eScheduleKey = iKey;
					kRTActorFilter.AddCoreActor(pkTriActor);

					pkStory->GetPlayers(kRTActorFilter);

					if (/*pkWorldManager &&*/ kRTActorFilter.PlayerSize())
					{
						GSRTPlayerDescMap::const_iterator cit = kRTActorFilter.GetPlayers().begin();
						for (; cit != kRTActorFilter.GetPlayers().end(); ++cit)
						{
							const GSRTPlayerDesc& rkRTPlayerDesc = cit->second;
							if (!IS_PLAYER_ID(rkRTPlayerDesc.uiID))
							{
								continue;
							}
							LC_PlayerBase* pkPlayer = rkRTPlayerDesc.pkPlayer;
							if (pkPlayer)
							{
								GSServerReward gsReward;
								kBConfig.kBatchReward.ResolveReward(RewardParam(), gsReward, pkPlayer, kRTBIdx.iSeqIdx, kRTBIdx.iSeqIdx);
								gsReward.Reward(pkPlayer, pkStory->GetStoryID(),nLogCode,nParams);
							}
						}
					}
				}
				if (pkBatchData->kReward.IsValid())
				{
					GameStory::GSRTPlayerFilter kRTActorFilter;
					kRTActorFilter.eTargetType = pkBatchData->kReward.eTargetType;
					kRTActorFilter.eScheduleKey = iKey;
					kRTActorFilter.AddCoreActor(pkTriActor);

					pkStory->GetPlayers(kRTActorFilter);

					if (/*pkWorldManager &&*/ kRTActorFilter.PlayerSize())
					{
						GSRTPlayerDescMap::const_iterator cit = kRTActorFilter.GetPlayers().begin();
						for (; cit != kRTActorFilter.GetPlayers().end(); ++cit)
						{
							const GSRTPlayerDesc& rkRTPlayerDesc = cit->second;
							if (!IS_PLAYER_ID(rkRTPlayerDesc.uiID))
							{
								continue;
							}
							LC_PlayerBase* pkPlayer = rkRTPlayerDesc.pkPlayer;
							if (pkPlayer)
							{
								GSServerReward gsReward;
								int32_t iMaxBatch = kRTBIdx.iSeqIdx;
								if (pkConfig->m_kCmnStatData.IsValid())
								{
									LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();
									LC_CommonStatEntry* pkCmnStatEntry = kCommonStatAsset.GetCommonStatEntry(pkConfig->m_kCmnStatData.iID);
									if (pkCmnStatEntry)
									{
										iMaxBatch = pkCmnStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_STORY_MAX_BATCH);
									}
								}
								pkBatchData->kReward.ResolveReward(RewardParam(), gsReward, pkPlayer, kRTBIdx.iSeqIdx, iMaxBatch);
								gsReward.Reward(pkPlayer, pkStory->GetStoryID(),nLogCode,nParams);
							}
						}
					}
				}
			}

			int clearNpc = pkBatchData->iIsClearBatchNPC;
			if(clearNpc==1)
			{
				SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
				GSRTMonsterRecordMap kMonsterRecord = pkRTBSchedule->kMonsterRecord;
				GSRTMonsterRecordMap::iterator itRTMon = kMonsterRecord.begin();
				for (; itRTMon != kMonsterRecord.end(); ++itRTMon)
				{
					if(itRTMon->second.iBatchIdx == iBatchIdx)
					{
						pkSkillInterface->RemoveLogicObject(itRTMon->first);
						pkStory->_clearSpawnedMonster(itRTMon->first, NULL);
					}
				}
			}

			if (!pkBatchData->kTriBatchs.empty())
			{
				GameLogic::LC_IDSet::iterator itTri = pkBatchData->kTriBatchs.begin();
				for (; itTri!=pkBatchData->kTriBatchs.end(); ++itTri)
				{
					int32_t iBatchID = *itTri;
					int32_t iStoryID = pkStory->GetStoryID();
					GSRTBatchDetail* pkTriDetail = pkRTBSchedule->GetBatchDetailByBatchIdx(iBatchID);
					if (pkTriDetail)
					{
						_triggerBatchCB_CF(timerID, pkStory, iKey, pkTriDetail->iIndex, -1, pkConfig);
					}
				}
			}

			switch (pkBatchData->iType)
			{
				case GSBSTP_SEQUECE:
				case GSBSTP_CYCLE:
					{
						++kRTBIdx.iSeqIdx;
						if (pkConfig->m_kCmnStatData.IsValid() && pkConfig->m_kCmnStatData.GetCmnStatEntry(GS_CMN_STAT_TP_BATCH))
						{
							LC_ActorPtrList kActors;
							pkStory->_getRTScheduleActors(iKey, kActors, pkConfig);

							LC_ActorPtrList::iterator it = kActors.begin();
							for (; it != kActors.end(); ++it)
							{
								GameLogic::LC_PlayerBase* pkPlayer = (GameLogic::LC_PlayerBase*)*it;
								if (pkPlayer->IsMainController())
								{
									pkStory->RecordCommonStat(pkPlayer, GS_CMN_STAT_TP_BATCH, kRTBIdx.iSeqIdx, pkConfig);
								}
							}
						}
						if (GSBSTP_SEQUECE == pkBatchData->iType)
						{
							kRTBIdx.iBatchIdx = pkConfig->m_kBatchSechedule.GetBatchIdx(kRTBIdx.iSeqIdx);
						}
						
						if (kRTBIdx.iBatchIdx >= 0)
						{
							LC_ServerPlayer* pPlayer = NULL;
							if (pkTriActor && pkTriActor->GetType() == LOT_PLAYER)
								pPlayer = (LC_ServerPlayer*)pkTriActor;
							pkStory->_nextBatchSchedule(iKey, iDetailIndex, timerID, pkConfig, false, pPlayer);
						}
						else
						{
							pkStory->NotifyPlayerGameStoryBatch(iKey, NULL, Utility::TIMER_ID_NA, pkConfig);
						}
					}
					break;
				case GSBSTP_TIMER:
					{
						uint32_t timestamp = 0;
						if (timerID == pkBDetail->iTriTimerID)
						{
							timestamp = (uint32_t)(pkTimerMgr->GetNextTriTimestamp(pkBDetail->iTriTimerID) / 1000);
						}
						else
						{
							timestamp = (uint32_t)(pkTimerMgr->GetTriTimestamp(pkBDetail->iTriTimerID) / 1000);
						}
						pkStory->_addTips(iKey, timestamp, pkBatchData->kBatchTip, pkTriActor, 0);
					}
					break;
				case GSBSTP_TRIGGER:
				case GSBSTP_PASSIVE:
				case GSBSTP_CONDITION:
					{
						pkStory->_addTips(iKey, GET_CURRENT_TIMESTAMP_IN_SECONDS(), pkBatchData->kBatchTip, pkTriActor, 0);
					}
					break;
				default:
					break;
			}
		}
	}
}

void LC_Server_GameStory_Base::_syncRankInfoCB(int32_t iStoryLogicID, const RankID& kRankID, int32_t err, const std::string& val)
{
	if (err)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to get story ranksnap [%d]", iStoryLogicID);
		return;
	}

	LC_Server_GameStory_Base* pkStory = SERVER_GET_STORYLOGIC_MANAGER->GetStoryLogic(iStoryLogicID);
	if (NULL == pkStory)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to get story by logic [%d]", iStoryLogicID);
		return;
	}

	GSRTRankSnap kSnap;
	StringType error_string;
	if (!boost::ajson::load_from_buff(kSnap, val.c_str(), error_string))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "bad story[%d] ranksnap id[%d-%d] content[%s]", pkStory->GetStoryID(), kRankID.Type(), kRankID.Detail(), val.c_str());
		return;
	}
	pkStory->_syncRankInfo(kRankID, kSnap);
}

void LC_Server_GameStory_Base::_onCrossTransCreated(int64_t raidid, uint32_t serverid, int32_t resId)
{
	if (!IsActive())
	{
		return;
	}

	GSRTCrossTranscription& kCross = m_kRTCrossTranscriptionMap[resId];
	kCross.iMapResID = resId;
	kCross.iRaidID = raidid;
	kCross.uiServerID = serverid;
}

void LC_Server_GameStory_Base::_onCrossTransFailed(int64_t raidid, uint32_t serverid, int32_t resId)
{
	if (!IsActive())
	{
		return;
	}

	GSRTCrossTranscription& kCross = m_kRTCrossTranscriptionMap[resId];
	kCross.iMapResID = resId;
	if (kCross.uiServerID == serverid || kCross.iRaidID == 0)
	{
		kCross.iRaidID = -1;
		kCross.uiServerID = 0;
	}
}

void LC_Server_GameStory_Base::_onCrossTransDestroyed(int64_t raidid, uint32_t serverid, int32_t resId)
{
	if (!IsActive())
	{
		return;
	}

	GSRTCrossTranscription& kCross = m_kRTCrossTranscriptionMap[resId];
	kCross.iMapResID = resId;
	kCross.iRaidID = -1;
	kCross.uiServerID = 0;
}

void LC_Server_GameStory_Base::_onActived(void)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GameStory[%d] Actived!", GetStoryID());
}

void LC_Server_GameStory_Base::_onDeactived(void)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GameStory[%d] Deactived!", GetStoryID());
}

void LC_Server_GameStory_Base::_spawnBatchMonster(GameStoryScheduleKey iKey, int iDetailIndex, GameStoryConfig* pkImplConfig, LC_ActorIDPtrMap* pkActorMap, GSRTTriggerSrcInfo* pkTriSrc, int iAutoFitLevel, GameStoryConfig* pkConfig, int timeID, object_id_type iParam)
{
	if (NULL == pkConfig || NULL == pkImplConfig)
	{
		return;
	}
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}
	GSRTBatchDetail* pkDetail = pkRTBSchedule->GetBatchDetail(iDetailIndex);
	if (NULL == pkDetail)
	{
		return;
	}
	BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(pkDetail->kBatchIdx.iBatchIdx);
	if(NULL == pkBatchData)
	{
		return;
	}
	
	bool cancleTimer = (pkBatchData->iType==GSBSTP_TIMER) ?  false : true;
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();
	GSRTMonsterIndexCluster monIndicesData;
	monIndicesData.eScheduleKey = iKey;
	monIndicesData.iDetailIndex = iDetailIndex;
	monIndicesData.sConfig = pkImplConfig->FileName();
	monIndicesData.iBatchIdx = pkDetail->kBatchIdx.iBatchIdx;
	uint32_t timestamp = (uint32_t)(GET_CURRENT_TIMESTAMP_IN_MILISECONDS() / 1000);
	GSRTBatchIndex& kRTBatchIdx = pkDetail->kBatchIdx;
	if (pkTriSrc)
	{
		monIndicesData.kTriSrc = *pkTriSrc;
	}
	if (iAutoFitLevel < 0)
	{
		iAutoFitLevel = pkRTBSchedule->GetSelfAdaptingLevel();
	}
	MapCtrlSetting& kCurMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	MapCtrlSetting& kImplMapCtrlSetting = pkImplConfig->m_kMapCtrlSetting;
	GSRTMonsterSetting& kRTMonSetting = pkRTBSchedule->kMonsterSetting;
	BatchDetailLst::iterator itBatchD = pkBatchData->kDetails.begin();
	int isClear = pkBatchData->iIsClear;
	for (; itBatchD != pkBatchData->kDetails.end(); ++itBatchD)
	{
		int count = itBatchD->GetCount();
		MonsterIndices monsterIndices = itBatchD->kMonsterIndeices;
		AreaIndices areaIndices = itBatchD->kAreaIndices; //
		bool match = itBatchD->IsMatch();
		if (monsterIndices.size() && areaIndices.size())
		{
			if(!match)
			{
				std::random_shuffle(monsterIndices.begin(), monsterIndices.end());
				std::random_shuffle(areaIndices.begin(), areaIndices.end());
			}
			MonsterIndices::iterator itMIdx = monsterIndices.begin();
			AreaIndices::iterator itAIdx = areaIndices.begin();
			int ii = 0;
			while (ii < count && monsterIndices.size() > 0)
			{
				do
				{
					LC_ServerMapLogicPlaceVector kPlaces;

					const MonsterConfigData* pkMapMonConfig = NULL;
					const MonsterConfigData* pkAreaMonConfig = NULL;
					if (itMIdx == monsterIndices.end())
					{
						itMIdx = monsterIndices.begin();
					}
					const MonsterData* pkMonsterData = kImplMapCtrlSetting.GetMonsterData(*itMIdx);
					if (NULL == pkMonsterData)
					{
						itMIdx = monsterIndices.erase(itMIdx);
						break;
					}
					const MapSetting* pkMapSetting = NULL;
					AreaIndices::iterator itCmnAIdx = areaIndices.end();
					AreaIndices::iterator itCurCmnAIdx = itAIdx;
					for (uint32_t jj = 0; jj < areaIndices.size(); ++jj, ++itCurCmnAIdx)
					{
						if (itCurCmnAIdx == areaIndices.end())
						{
							itCurCmnAIdx = areaIndices.begin();
						}
						const AreaData* pkAreaEntry = kImplMapCtrlSetting.GetAreaData(*itCurCmnAIdx);
						if (NULL == pkAreaEntry)
						{
							continue;
						}
						pkAreaMonConfig = &pkAreaEntry->kMonsterConfig;
						GSRTCommonStats& kAreaCmnStat = pkRTBSchedule->kAreaCmnStat[*itCurCmnAIdx];
						GSRTCommonStats& kMapCmnStat = pkRTBSchedule->kMapCmnStat[pkAreaEntry->lMapResID];
						if (pkMonsterData->IsElite())
						{
							if (pkAreaMonConfig->iEliteLmt > 0 && pkAreaMonConfig->iEliteLmt <= kAreaCmnStat.iEliteCnt)
							{
								continue;
							}
						}
						if (pkAreaMonConfig->iTotalLmt > 0 && pkAreaMonConfig->iTotalLmt <= kAreaCmnStat.iTotalCnt && isClear!=1)
						{
							continue;
						}
						_getRTScheduleMaps(iKey, pkAreaEntry->lMapResID, kPlaces, pkConfig);
						if (kPlaces.empty())
						{
							continue;
						}
						pkMapSetting = kCurMapCtrlSetting.GetMapSetting(pkAreaEntry->lMapResID);
						if (pkMapSetting)
						{
							pkMapMonConfig = &pkMapSetting->kMonsterConfig;
							if (pkMonsterData->IsElite())
							{
								if (pkMapMonConfig->iEliteLmt > 0 && pkMapMonConfig->iEliteLmt <= kMapCmnStat.iEliteCnt)
								{
									continue;
								}
							}
							if (pkMapMonConfig->iTotalLmt > 0 && pkMapMonConfig->iTotalLmt <= kMapCmnStat.iTotalCnt)
							{
								continue;
							}
						}
						itCmnAIdx = itCurCmnAIdx;
						break;
					}
					if (itCmnAIdx == areaIndices.end())
					{
						itMIdx = monsterIndices.erase(itMIdx);
						break;
					}
					else
					{
						itAIdx = itCmnAIdx;
					}

					monIndicesData.iAreaIdx = *itAIdx++;
					monIndicesData.iMonsterIdx = *itMIdx++;;
					const AreaData* pkAreaEntry = kImplMapCtrlSetting.GetAreaData(monIndicesData.iAreaIdx);
					if (NULL == pkAreaEntry)
					{
						continue;
					}
					bool bInitDir = pkAreaEntry->vecDirection.Length() >= 1e-06f;
					GSRTCommonStats& kAreaCmnStat = pkRTBSchedule->kAreaCmnStat[monIndicesData.iAreaIdx];
					GSRTCommonStats& kMapCmnStat = pkRTBSchedule->kMapCmnStat[pkAreaEntry->lMapResID];
					GSRTMonsterRecordMap& kMonsterRecord = pkRTBSchedule->kMonsterRecord;
					const SkillDataList* pkExtraSkills = NULL;
					if (pkMonsterData->IsExtraSkill())
					{
						pkExtraSkills = kRTMonSetting.GetExtraSkills(pkMonsterData->lCharTypeID);
					}
					LC_ServerMapLogicPlaceVector::iterator itPlace = kPlaces.begin();
					for (; itPlace != kPlaces.end(); ++itPlace)
					{
						bool needRefresh = true;
						LC_ServerMapLogic_Place* pkMaplogic = (*itPlace);
						if (NULL == pkMaplogic || pkWorldManager->IsTributaryMap(pkMaplogic))
						{
							continue;
						}
						pkMaplogic->ComputeAutoFitLevel();
						LC_TranscriptionListEntry transEntry;
						transEntry.m_iMapID				= pkMaplogic->GetMapLogicID();
						transEntry.m_kSpawnLocation		= pkAreaEntry->vecLocation;
						transEntry.m_iWanderRadius		= pkMonsterData->iWanderRadius;
						transEntry.m_lWanderPathID		= pkMonsterData->iWanderPath;
						transEntry.m_iSpawnCharType		= pkMonsterData->lCharTypeID;
						transEntry.m_iZRotAngle			= 0;
						transEntry.m_iAutoFitDeltaLevel = pkAreaEntry->kMonsterConfig.iRelativeLevel;
						if ( pkMapSetting )
						{
							transEntry.m_iAutoFitMaxLevel	= pkMapSetting->kSelfAdaptingLevel.iMax;
							transEntry.m_iAutoFitMinLevel	= pkMapSetting->kSelfAdaptingLevel.iMin;
						}
						else
						{
							transEntry.m_iAutoFitMaxLevel	= 0;
							transEntry.m_iAutoFitMinLevel	= 0;
						}
						if (NULL != pkAreaEntry)
						{
							const RangeData& kLevelRange = pkAreaEntry->kMonsterConfig.kSelfAdaptingLevel;
							transEntry.m_iAutoFitMaxLevel = (kLevelRange.iMax > 0) ? kLevelRange.iMax : transEntry.m_iAutoFitMaxLevel;
							transEntry.m_iAutoFitMinLevel = (kLevelRange.iMin > 0) ? kLevelRange.iMin : transEntry.m_iAutoFitMinLevel;
						}

						BatchConfig& kBatchConfig = pkConfig->m_kBatchSechedule.kConfig;
						//从redis中获取刷怪数据，如果同一拨的怪已经被杀死，则不需要刷新
						if(NULL != m_pkCoreRecord && kBatchConfig.SaveMonsterDeadState())
						{
							mem::vector<uint64_t> nameKey;
							nameKey.push_back(GetGameServerID());

							mem::vector<uint64_t> key;
							key.push_back(transEntry.m_iSpawnCharType);

							char szNameSpace[MAX_CHAR_NAME_LENGTH + 1] = { '\0' };
							sprintf_s( szNameSpace, MAX_CHAR_NAME_LENGTH + 1, ":%s", pkConfig->FileName() );

							std::string result;
							bool tag = UT_ServerHelper::RedisGetStringData(szNameSpace, nameKey, key, result);
							if(tag)
							{
								mem::vector<int64_t> vec;
								LC_Helper::SpliteString2VecInt(result, vec, ":");
								if(vec.size() == 2 && vec[0]==m_pkCoreRecord->m_iEndTime/1000 && vec[1]==pkDetail->kBatchIdx.iBatchIdx)
								{
									continue;
								}								
							}
						}

						LC_ServerMap* pkMap = (LC_ServerMap*)pkMaplogic->GetMap();

						if(NULL!=pkMap && isClear == 1)
						{
							int lineNum = pkMap->GetLocalLineNum();
							int npcID = transEntry.m_iSpawnCharType;
							uint32_t preTime = pkBatchData->iPrepareTime;
							if(pkAreaMonConfig)
							{
								needRefresh = kAreaCmnStat.NeedRefresh(npcID, lineNum, preTime, timestamp);
							}

							if(!needRefresh)
							{
								continue;
							}				
						}

						for (int j = 0; j < pkMonsterData->iCount; ++j)
						{
							if (pkAreaEntry->iRadius > 0)
							{
								UT_PathFindHelp::GenRandomPosInCircle2(pkMap->GetMapResID(), pkAreaEntry->vecLocation, pkAreaEntry->iRadius, transEntry.m_kSpawnLocation);
							}

							int32_t iEliteLevel = pkMonsterData->ResolveEliteLevel();
							//+ 1, disgusting!
							bool bElite = (iEliteLevel >= ELT_BLUE_MID + 1);
							LC_ServerPlayer* pkPlayer = NULL;
							if(IS_PLAYER_ID(iParam))
							{
								LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
								pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(iParam);
							}

							if (bElite)
							{
								DungeonConfig::EliteSkillRuleSetType rTmp;
								GameLogic::LC_ActorPtrList pkNPCs = pkMaplogic->SpawnEliteNPC(transEntry, 0, rTmp, -1, (FactionType)pkMonsterData->iFactionID, iEliteLevel);
								if (pkNPCs.empty())
								{
									GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Fail to SpawnEliteNPC! Map ResID[%d] LogicID[%d] CharType[%d]", pkMap->GetMapResID(), pkMap->GetMapLogicID(), transEntry.m_iSpawnCharType);
									continue;
								}
								int eliteIdx = _autoEliteIdx();
								LC_ActorPtrList::iterator IterNPC;
								TRAV(pkNPCs, IterNPC)
								{
									LC_ServerNPC* pkNPC = (LC_ServerNPC*)*IterNPC;
									if (bInitDir)
									{
										pkNPC->SetForwardDir(pkAreaEntry->vecDirection);
										pkNPC->SetSpawnZRotAngle((int)pkNPC->GetForwardDirAngle());
									}

									if(true==GetGameServerApp()->IsNodeType_Global())
									{
										UT_ServerHelper::UpdateBossDamageInfo(pkNPC);
									}								

									if(NULL != pkPlayer)
									{
										pkNPC->SetGuildID(pkPlayer->GetGuildID());
										GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "setNpcGuildID1 serverID:%d, mapID:%d, charType:%d, guildID:%lld\n", GetGameServerID(), pkPlayer->GetMapResID(), pkNPC->GetCharType(), pkPlayer->GetGuildID());
									}

									monIndicesData.iEliteIdx = eliteIdx;
									m_kRTMonsterRecord[pkNPC->GetID()] = monIndicesData;
									m_kEliteGrpMap[eliteIdx].insert(pkNPC->GetID());

									if (pkMonsterData->iSurviveTime > 0)
									{
										pkNPC->SetSurvivalTime(pkMonsterData->iSurviveTime);
										pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);
									}
									if (pkExtraSkills && !pkExtraSkills->empty())
									{
										SkillDataList::const_iterator citSkill = pkExtraSkills->begin();
										for (; citSkill != pkExtraSkills->end(); ++citSkill)
										{
											pkNPC->AddSkill(citSkill->iRuleID, citSkill->iID);
										}
									}
									if (pkMonsterData->IsGuildRelated())
									{
										int64_t iThreshold = (pkNPC->GetAttributeMap()->GetAttribute(ATT_MAX_HP) * 2 / 1000);
										pkNPC->RegEncountedStatGuild(LC_ECT_TP_Damage, true, iThreshold);
									}
									if (pkMonsterData->IsServerRelated())
									{
										pkNPC->RegEncountedStatServer(LC_ECT_TP_Damage, true);
									}
									if (pkMonsterData->IsBroadcastHP())
									{
										AddKeyMonsterLogicID(iKey, pkNPC->GetID());
									}
									if (pkActorMap)
									{
										(*pkActorMap)[(*IterNPC)->GetID()] = pkNPC;
									}
									if (pkAreaMonConfig)
									{
										kAreaCmnStat.OnActorSpawned(pkNPC, *pkAreaMonConfig, kRTBatchIdx, timestamp, timeID, cancleTimer);
									}
									if (pkMapMonConfig)
									{
										kMapCmnStat.OnActorSpawned(pkNPC, *pkMapMonConfig, kRTBatchIdx, timestamp, timeID, cancleTimer);
									}
									kMonsterRecord[pkNPC->GetID()] = monIndicesData;
								}
							}
							else
							{
								LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMaplogic->SpawnNPC(transEntry, -1, (FactionType)pkMonsterData->iFactionID);
								if (!pkNPC)
								{
									GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Fail to SpawnNPC! Map ResID[%d] LogicID[%d] CharType[%d]", pkMap->GetMapResID(), pkMap->GetMapLogicID(), transEntry.m_iSpawnCharType);
									continue;
								}

								if (bInitDir)
								{
									pkNPC->SetForwardDir(pkAreaEntry->vecDirection);
									pkNPC->SetSpawnZRotAngle((int)pkNPC->GetForwardDirAngle());
								}

								if(true==GetGameServerApp()->IsNodeType_Global())
								{
									UT_ServerHelper::UpdateBossDamageInfo(pkNPC);
								}
								
								if(NULL != pkPlayer)
								{
									pkNPC->SetGuildID(pkPlayer->GetGuildID());
									GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "setNpcGuildID2 serverID:%d, playerid:%d, mapID:%d, charType:%d, guildID:%lld\n", GetGameServerID(), pkPlayer->GetID(), pkPlayer->GetMapResID(), pkNPC->GetCharType(), pkPlayer->GetGuildID());
								}

								monIndicesData.iEliteIdx = 0;
								m_kRTMonsterRecord[pkNPC->GetID()] = monIndicesData;
								if (pkMonsterData->iSurviveTime > 0)
								{
									pkNPC->SetSurvivalTime(pkMonsterData->iSurviveTime);
									pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);
								}
								if (pkExtraSkills && !pkExtraSkills->empty())
								{
									SkillDataList::const_iterator citSkill = pkExtraSkills->begin();
									for (; citSkill != pkExtraSkills->end(); ++citSkill)
									{
										pkNPC->AddSkill(citSkill->iRuleID, citSkill->iID);
									}
								}
								if (pkMonsterData->IsGuildRelated())
								{
									int64_t iThreshold = (pkNPC->GetAttributeMap()->GetAttribute(ATT_MAX_HP) * 2 / 1000);
									pkNPC->RegEncountedStatGuild(LC_ECT_TP_Damage, true, iThreshold);
								}
								if (pkMonsterData->IsServerRelated())
								{
									pkNPC->RegEncountedStatServer(LC_ECT_TP_Damage, true);
								}
								if (pkMonsterData->IsBroadcastHP())
								{
									AddKeyMonsterLogicID(iKey, pkNPC->GetID());
								}
								if (pkActorMap)
								{
									(*pkActorMap)[pkNPC->GetID()] = pkNPC;
								}
								if (pkAreaMonConfig)
								{
									kAreaCmnStat.OnActorSpawned(pkNPC, *pkAreaMonConfig, kRTBatchIdx, timestamp, timeID, cancleTimer);
								}
								if (pkMapMonConfig)
								{
									kMapCmnStat.OnActorSpawned(pkNPC, *pkMapMonConfig, kRTBatchIdx, timestamp, timeID, cancleTimer);
								}
								kMonsterRecord[pkNPC->GetID()] = monIndicesData;
							}

							kAreaCmnStat.AddNPCCnt(1, bElite);
							kMapCmnStat.AddNPCCnt(1, bElite);
							GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "gsid:%d schedulekey:%d batchidx:%d map_stat:%d isElite:%d", GetStoryID(), iKey, monIndicesData.iBatchIdx, kMapCmnStat.iTotalCnt, pkMonsterData->IsElite() ? 1 : 0);
						}
					}
					++ii;
				}
				while (0);
			}
		}
	}

	UT_ServerHelper::BroadcastNpcInfoToPlayer(GetStoryID());
}

GSRTTriggerRecord* LC_Server_GameStory_Base::GetRTTriggerRecord(const GSRTTriggerIndex& kTriIdx)
{
	GSRTMapTriggerRecordMap::iterator itMap = m_kRTMapTriggerRecordMap.find(kTriIdx.iMapLogicID);
	if (itMap != m_kRTMapTriggerRecordMap.end())
	{
		GSRTTriggerRecordMap::iterator itRecord = itMap->second.find(kTriIdx.iTriggerID);
		if (itRecord != itMap->second.end())
		{
			return &itRecord->second;
		}
	}
	return NULL;
}

GSRTActorTriggerReocrd* LC_Server_GameStory_Base::_getRTActorTriggerRecord(object_id_type iID, GameStoryConfig* pkConfig)
{
	GSRTActorTriggerReocrdMap::iterator it = m_kRTActorTriggerReocrdMap.find(iID);
	if (it != m_kRTActorTriggerReocrdMap.end())
	{
		return &it->second;
	}
	else
	{
		LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
		if (NULL == pkWorldManager)
		{
			return NULL;
		}
		LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(iID);
		if (pkConfig == NULL)
		{
			pkConfig = GetConfig();
		}
		return _initRTActorTriggerRecord(pkActor, pkConfig);
	}
}

GSRTActorTriggerReocrd* LC_Server_GameStory_Base::_initRTActorTriggerRecord(GameLogic::LC_ActorBase* pkActor, GameStoryConfig* pkConfig)
{
	if (NULL == pkActor || NULL == pkConfig)
	{
		return NULL;
	}
	object_id_type iID = pkActor->GetID();
	int32_t iCharTypeID = pkActor->GetCharType();
	ActorTriggerData* pkActorTriData = pkConfig->m_kMapCtrlSetting.GetActorTriggerData(iCharTypeID);
	if (NULL == pkActorTriData)
	{
		return NULL;
	}
	GSRTActorTriggerReocrd& kRTActorTriRecord = m_kRTActorTriggerReocrdMap[iID];
	kRTActorTriRecord.iActorID = iID;
	kRTActorTriRecord.pkTriggerData = pkActorTriData;
	int iMapLogicID = pkActor->GetMapLogicID();
	GSRTMapTriggerRecordMap::iterator itRTMapTriMap = m_kRTMapTriggerRecordMap.find(iMapLogicID);
	if (itRTMapTriMap != m_kRTMapTriggerRecordMap.end())
	{
		std::random_shuffle(pkActorTriData->kAvailedTriggerPtrs.begin(), pkActorTriData->kAvailedTriggerPtrs.end());
		GSRTTriggerRecordMap& kRTMapTriMap = itRTMapTriMap->second;
		int iMaxTriggerCnt = pkActorTriData->iMaxTriggerCnt > 0 ? pkActorTriData->iMaxTriggerCnt : (int)pkActorTriData->kAvailedTriggerPtrs.size();
		for (int i = (int)kRTActorTriRecord.kTriggerReged.size(); i < (int)iMaxTriggerCnt; ++i)
		{
			TrigerData* pkTriData = pkActorTriData->kAvailedTriggerPtrs[i];
			GSRTTriggerRecordMap::iterator itRTTriRecord = kRTMapTriMap.begin();
			for (; itRTTriRecord != kRTMapTriMap.end(); ++itRTTriRecord)
			{
				if (itRTTriRecord->second.pkTriggerData == pkTriData)
				{
					kRTActorTriRecord.kTriggerReged[itRTTriRecord->second.kIndex] = 0;
					break;
				}
			}
		}
	}
	return &kRTActorTriRecord;
}

void LC_Server_GameStory_Base::_clearSpawnedMonster(object_id_type iID, GameLogic::LC_ActorBase* pkKiller)
{
	LC_GameStory_Manager* pkStoryManager = SERVER_GET_GAMESTORY_MANAGER;
	uint32_t timestamp = (uint32_t)(GET_CURRENT_TIMESTAMP_IN_MILISECONDS() / 1000);
	GSRTMonsterRecordMap::iterator it = m_kRTMonsterRecord.find(iID);
	if (it != m_kRTMonsterRecord.end())
	{
		GSRTMonsterIndexCluster& kRTMonIndex = it->second;
		GSRTTriggerRecord* pkTriRecord = GetRTTriggerRecord(kRTMonIndex.kTriSrc.kTriIdx);
		if (pkTriRecord)
		{
			object_id_type iTriActorID = pkTriRecord->kObj2TriActorMap[iID];
			GSRTActorTriggerReocrd* pkRTActortriData = _getRTActorTriggerRecord(iTriActorID);
			if (pkRTActortriData)
			{
				pkRTActortriData->kGenActors.erase(iID);
			}
			pkTriRecord->kObj2TriActorMap.erase(iID);
		}
		GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(kRTMonIndex.eScheduleKey);
		if (NULL == pkRTBatchSchedule)
		{
			return;
		}
		GameStoryConfig* pkConfig = pkStoryManager->GetStoryConfig(kRTMonIndex.sConfig);
		const BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(kRTMonIndex.iBatchIdx);
		const AreaData* pkAreaData = pkConfig->m_kMapCtrlSetting.GetAreaData(kRTMonIndex.iAreaIdx);
		if (NULL == pkAreaData)
		{
			GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_ERROR, "gsid:%d schedulekey:%d batchidx:%d _clearSpawnedMonster:%d badareaidx:%d", GetStoryID(), kRTMonIndex.eScheduleKey, kRTMonIndex.iBatchIdx, iID, kRTMonIndex.iAreaIdx);
			return;
		}
		GSRTCommonStats& kAreaCmnStat = pkRTBatchSchedule->kAreaCmnStat[kRTMonIndex.iAreaIdx];
		GSRTCommonStats& kMapCmnStat = pkRTBatchSchedule->kMapCmnStat[pkAreaData->lMapResID];
		GSRTMonsterRecordMap& kMonsterRecord = pkRTBatchSchedule->kMonsterRecord;
		int eliteIdx = kRTMonIndex.iEliteIdx;
		if (eliteIdx != 0 && m_kEliteGrpMap.find(eliteIdx) != m_kEliteGrpMap.end())
		{
			if (m_kEliteGrpMap[eliteIdx].find(iID) != m_kEliteGrpMap[eliteIdx].end())
			{
				m_kEliteGrpMap[eliteIdx].erase(iID);
				if (m_kEliteGrpMap[eliteIdx].empty())
				{
					m_kEliteGrpMap.erase(eliteIdx);

					kAreaCmnStat.AddNPCCnt(-1, true);
					kMapCmnStat.AddNPCCnt(-1, true);
				}
			}
		}
		else
		{
			kAreaCmnStat.AddNPCCnt(-1, false);
			kMapCmnStat.AddNPCCnt(-1, false);
		}
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "gsid:%d schedulekey:%d batchidx:%d after map_stat:%d isElite:%d", GetStoryID(), kRTMonIndex.eScheduleKey, kRTMonIndex.iBatchIdx, kMapCmnStat.iTotalCnt, eliteIdx);

		if(pkConfig->IsSaveState() && pkBatchData->bNpcDeadRemoveBatchIndex && true==GetGameServerApp()->IsNodeType_Global())
		{
			//将刷怪进度从redis删掉
			mem::vector<uint64_t> input;
			input.push_back(kRTMonIndex.iBatchIdx);

			char szNameSpace[MAX_CHAR_NAME_LENGTH + 1] = { '\0' };
			sprintf_s( szNameSpace, MAX_CHAR_NAME_LENGTH + 1, ":%s", pkConfig->FileName() );
			UT_ServerHelper::RedisRemoveData_Int(szNameSpace, input);
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "delete refresh npc state from redis! serverid:%d, fildName:%s, batchIndex:%d\n", GetGameServerID(), pkConfig->FileName(), kRTMonIndex.iBatchIdx);
		}

		kAreaCmnStat.OnActorKilled(iID, pkKiller, timestamp);
		kMapCmnStat.OnActorKilled(iID, pkKiller, timestamp);
		//kMonsterRecord.erase(iID);
		//m_kRTMonsterRecord.erase(it);
		if (pkConfig->IsAllClearToEnd() && pkRTBatchSchedule->IsCleared())
		{
			//do clear things
			{
				if (m_kRTMonsterRecord.empty())
				{
					bool bAllClear = true;
					GSRTBatchScheduleMap::iterator itRTBatch = m_kRTBatchScheduleMap.begin();
					for (; itRTBatch != m_kRTBatchScheduleMap.end(); ++itRTBatch)
					{
						if (!_isRTScheduleClear(itRTBatch->first, pkConfig))
						{
							bAllClear = false;
							break;
						}
					}
					if (bAllClear)
					{
						LC_GameStory_Manager* pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
						pkGSMgr->AyncDeactiveStory(GetStoryID());
						return;
					}
				}
			}
		}

		BatchConfig& kBConfig = pkConfig->m_kBatchSechedule.kConfig;
		if (kBConfig.IsClearToNextBatch() && pkRTBatchSchedule->IsBatchCleared(pkBatchData))
		{
			_endBatchEndCB_CF(TIMER_ID_NA, this, kRTMonIndex.eScheduleKey, kRTMonIndex.iDetailIndex, pkKiller, pkConfig);
		}

		GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(kRTMonIndex.eScheduleKey);
		if (NULL == pkRTBSchedule)
		{
			return;
		}
		GSRTBatchDetail* pkBDetail = pkRTBSchedule->GetBatchDetail(kRTMonIndex.iDetailIndex);
		if (NULL == pkBDetail)
		{
			return;
		}

		int32_t triTimerID = pkBDetail->iTriTimerID;
		kAreaCmnStat.UpdateTriTimeID(iID, triTimerID);
		kMapCmnStat.UpdateTriTimeID(iID, triTimerID);

		if (kAreaCmnStat.IsKeyID(iID))
		{
			NotifyPlayerGameStoryBatch(kRTMonIndex.eScheduleKey, NULL, Utility::TIMER_ID_NA, pkConfig);
		}

		kMonsterRecord.erase(iID);
		m_kRTMonsterRecord.erase(it);
	}
}

void LC_Server_GameStory_Base::_UpdateBestGuildBossStat(GameLogic::LC_ActorBase* pkActor)
{
	if(NULL == pkActor)
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkLogic = (LC_ServerMapLogic_Place*)(pkWorldManager->GetMapLogic(pkActor->GetMapLogicID()));
	if(NULL==pkLogic || pkLogic->GetMapLogicType()!=MT_WATCHTOWER)
	{
		return;
	}

	int logicID = pkActor->GetID();	
	pkLogic->SetDamageNpcLogicID(logicID);
}

void LC_Server_GameStory_Base::_doRegularBossDamageReward(GameLogic::LC_ActorBase* pkActor)
{
	if(NULL == pkActor)
	{
		return;
	}

	int npcTypeID = pkActor->GetCharType();
	int mapResID = pkActor->GetMapResID();
	CF_RegularBoss::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RegularBoss>(npcTypeID);
	if(NULL==pkData || pkData->_lMapResID!=mapResID)
	{
		return;
	}
	
	LC_EncountedStatMgr& kStatMgr = pkActor->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(LC_ECT_CAT_ACTOR);
	if (NULL == pkStatCon)
	{
		return;
	}

	const uint64_object_id_vector& rkList = pkStatCon->GetSortedIDs();
	if (rkList.empty())
	{
		return;
	}

	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();
	if(NULL == pkPlayManager)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if(NULL == pkGuildManager)
	{
		return;
	}	

	object_id_type hitPlayer = ((LC_ServerNPC*)pkActor)->GetHitPlayerID();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkPlayManager->FindPlayer(hitPlayer));
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_SyncRegularBossDrop msg;
	msg.m_nHitPlayerName = TPSTR2STDSTR(pkPlayer->GetOwnerCharName());
	msg.m_nCharTypeID = npcTypeID;
	msg.m_nDropType = RegularBossDrop_1;
	pkPlayer->SendMsgToClient(MGPT_SYNCREGULARBOSSDROP, &msg);

	uint64_t guildID = pkPlayer->GetGuildID();
	LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildID);
	if(NULL == pkGuildInfo)
	{
		return;
	}

	//更新击杀记录
	uint64_t inputKey = LC_Helper::EncodeScore(npcTypeID, mapResID);
	mem::vector<uint64_t> key;
	key.push_back(inputKey);

	RegularBossKillerInfo info;
	info.charName = pkPlayer->GetOwnerCharName().c_str();
	info.guildName = pkGuildInfo->GetGuildName().c_str();

	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	std::string data = ss.str().c_str();

	mem::vector<uint64_t> nameKey;
	nameKey.push_back(GetGameServerID());
	UT_ServerHelper::RedisWriteString(UT_REDIS_KEY_REGULARBOSS_KILLER_INFO, nameKey, key, data);
	UT_ServerHelper::UpdateRegularBossKillerInfo(inputKey, info);

	for(int i=0; i<rkList.size(); ++i)
	{
		object_id_type playerID = (int32_t)rkList[i];
		if (!IS_PLAYER_ID(playerID) || playerID==hitPlayer)
		{
			continue;
		}

		bool isGuildPlayer = false;
		if(pkGuildInfo->IsGuildPlayer(pkStatCon->GetStatCharID(rkList[i])))
		{
			isGuildPlayer = true;
		}

		if(isGuildPlayer)
		{
			StringType mailTitle = "4;";
			StringType mailDesc = "5;";
			int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN6_OPERATE, OPERATE_LOGTYPE2);
			UT_ServerHelper::SendToMail(mailTitle, mailDesc, pkStatCon->GetStatCharID(rkList[i]), kStatMgr.GetActorName((object_id_type)rkList[i]), 0, pkData->_kReward2, "", nLogCode);
			msg.m_nDropType = RegularBossDrop_3;
		}
		else
		{
			StringType mailTitle = "6;";
			StringType mailDesc = "7;";
			int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN6_OPERATE, OPERATE_LOGTYPE3);
			UT_ServerHelper::SendToMail(mailTitle, mailDesc, pkStatCon->GetStatCharID(rkList[i]), kStatMgr.GetActorName((object_id_type)rkList[i]), 0, pkData->_kReward3, "", nLogCode);
			msg.m_nDropType = RegularBossDrop_2;
		}

		LC_ServerPlayer* pkOtherPlayer = (LC_ServerPlayer*)(pkPlayManager->FindPlayer(playerID));
		if(NULL != pkOtherPlayer)
		{
			pkOtherPlayer->SendMsgToClient(MGPT_SYNCREGULARBOSSDROP, &msg);
		}
	}
}

void LC_Server_GameStory_Base::_doRankReward(GameStoryConfig* pkConfig)
{

}

void LC_Server_GameStory_Base::_doRankReward(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		return;
	}
	GSRTBatchSchedule* pkRTBatchSchedule =  _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule || !pkRTBatchSchedule->IsInited())
	{
		return;
	}
	GSRTRankEntryMap::iterator it = m_kRTRankInfo.kEntryMap.begin();
	for (; it != m_kRTRankInfo.kEntryMap.end(); ++it)
	{
		GSRTRankEntry& kRTRankEntry = it->second;
		const RankEntry* pkDataEntry = kRTRankEntry.pkDataEntry;
		if (NULL == pkDataEntry || !IsValidStoryRankType(pkDataEntry->RankType()))
		{
			continue;
		}
		if (kRTRankEntry.kIndices.empty())
		{
			continue;
		}

		const RankRewardEntries* pkRewardEntries = pkConfig->m_kRankRewardData.GetRankReward(pkDataEntry->iRankRewardIndex);
		if (pkRewardEntries)
		{
			LC_RankManager* pkRankManager = SERVER_GET_RANK_MANAGER();
			if (pkRankManager)
			{
				GSRTPlayerCmnStatIndices::iterator itRank = kRTRankEntry.kIndices.begin();
				RankRewardEntries::const_iterator citRR = pkRewardEntries->begin();

				for (; citRR != pkRewardEntries->end() && itRank != kRTRankEntry.kIndices.end(); ++citRR)
				{
					for (int ii = 0; ii < citRR->iSection && itRank != kRTRankEntry.kIndices.end(); ++ii, ++itRank)
					{
						GSServerReward::MailPlayerMap kPlayers;
						GSServerReward gsReward;
						citRR->kReward.ResolveReward(RewardParam(1.0, (*itRank)->iLevel), gsReward);
						kPlayers[(*itRank)->uiCharID] = (*itRank)->sName.c_str();
						gsReward.Email(kPlayers);
					}
				}
			}
		}
	}
}
void LC_Server_GameStory_Base::_setupRank(void)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}

	m_kRTRankInfo.Reset();
	RankEntries::iterator it = pkConfig->m_kRankData.kStoryEntries.begin();
	for (; it != pkConfig->m_kRankData.kStoryEntries.end(); ++it)
	{
		m_kRTRankInfo.AddRank(&*it);
	}
}
void LC_Server_GameStory_Base::_checkresetRank(void)
{
	GSRTRankEntryMap::iterator it = m_kRTRankInfo.kEntryMap.begin();
	for (; it != m_kRTRankInfo.kEntryMap.end(); ++it)
	{
		GSRTRankEntry& kRTRankEntry = it->second;
		const RankEntry* pkDataEntry = kRTRankEntry.pkDataEntry;
		if (NULL == pkDataEntry || pkDataEntry->IsAccum())
		{
			continue;
		}
		kRTRankEntry.Clear();
	}
}
bool LC_Server_GameStory_Base::_getRankKey(const RankEntry* pkRankEntry, std::string& key)
{
	if (NULL == pkRankEntry)
	{
		return false;
	}

	if (pkRankEntry->IsCross())
	{
		key = GetGameServerApp()->GetRedisGroupPrefix()
			  + UT_REDIS_KEY_SHOT_ACTIVITY_GROUP_PREFIX + boost::lexical_cast<std::string>(GetGameServerApp()->GetServerManager()->GetCurGuildGroupId())
			  + UT_REDIS_KEY_STORY_ACTIVITY_RANK + ":"
			  + ":" + boost::lexical_cast<std::string>(GetStoryID());
	}
	else
	{
		key = GetGameServerApp()->GetRedisServerPredix()
			  + UT_REDIS_KEY_STORY_ACTIVITY_RANK + ":"
			  + ":" + boost::lexical_cast<std::string>(GetStoryID());
	}
	return true;
}
void LC_Server_GameStory_Base::_updateRankInfo(void)
{
	Utility::Redis_Channel& kRedisChannel = GetGameServerApp()->GetRedisChannel();
	GSRTRankEntryMap::iterator it = m_kRTRankInfo.kEntryMap.begin();
	for (; it != m_kRTRankInfo.kEntryMap.end(); ++it)
	{
		GSRTRankEntry& kRTRankEntry = it->second;
		const RankEntry* pkDataEntry = kRTRankEntry.pkDataEntry;
		if (NULL == pkDataEntry || !pkDataEntry->IsToSave())
		{
			continue;
		}
		//do save
		if (kRTRankEntry.GetSaveDirty())
		{
			std::string sKey;
			if (_getRankKey(pkDataEntry, sKey))
			{
				kRTRankEntry.AddSaveDirtyRevision();
				Utility::json_op<GSRTRankSnap> _json_op;
				_json_op.json.type = pkDataEntry->RankType();
				_json_op.json.detail = pkDataEntry->RankDetail();
				_json_op.json.version = kRTRankEntry.GetSaveDirtyRevision();
				GSRTPlayerCmnStatIndices::iterator itRank = kRTRankEntry.kIndices.begin();
				for (int i = 1; itRank != kRTRankEntry.kIndices.end(); ++itRank, ++i)
				{
					GSRTRankSnapEntry data;
					data.d = (*itRank)->uiCharID;
					data.v = pkDataEntry->GetKeyValue(*itRank);
					data.i = i;
					data.name = (*itRank)->sName.c_str();
					_json_op.json.info.push_back(data);
				}

				ASYNC_REDIS_REQUEST_NCB(kRedisChannel, &redispp::Connection::hset, sKey, boost::lexical_cast<std::string>(pkDataEntry->GetRankID()), _json_op);
			}
		}
		else if (pkDataEntry->IsCross() || kRTRankEntry.GetSaveDirtyRevision() <= 0)
		{
			_getRankInfo(kRedisChannel, pkDataEntry);
		}
	}
}
void LC_Server_GameStory_Base::_getRankInfo(Utility::Redis_Channel& kRedisChannel, const RankEntry* pkDataEntry)
{
	std::string sKey;
	if (_getRankKey(pkDataEntry, sKey))
	{
		std::string sField = boost::lexical_cast<std::string>(pkDataEntry->GetRankID());
		int32_t ret = 0;
		boost::function<void(int32_t, const std::string&)> cb = boost::bind(&LC_Server_GameStory_Base::_syncRankInfoCB, GetLogicID(), pkDataEntry->GetRankID(), _1, _2);
		ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hget, cb, sKey, sField);
	}
}
void LC_Server_GameStory_Base::_syncRankInfo(const RankID& kRankID, GSRTRankSnap& kRTRankSnap)
{
	GSRTRankEntry* pkRTRankEntry = m_kRTRankInfo.GetRankEntry(kRankID);
	if (NULL == pkRTRankEntry)
	{
		return;
	}

	const RankEntry* pkDataEntry = pkRTRankEntry->pkDataEntry;
	if (NULL == pkDataEntry)
	{
		return;
	}

	if (pkRTRankEntry->GetSaveDirtyRevision() >= kRTRankSnap.version)
	{
		return;
	}

	pkRTRankEntry->Reset();
	pkRTRankEntry->SetSaveDirtyRevision(kRTRankSnap.version);

	GSRTRankEntryDataVec::iterator it = kRTRankSnap.info.begin();
	for (; it != kRTRankSnap.info.end(); ++it)
	{
		GSRTRankSnapEntry& kSnapEntry = *it;
		GSRTPlayerCommonStat* pkPlayerStat = GetPlayerStats(kSnapEntry.d, true, kSnapEntry.name.c_str());
		pkDataEntry->SetKeyValue(pkPlayerStat, kSnapEntry.v);
		pkRTRankEntry->Update(pkPlayerStat);
	}
}
static StringType _resolveDamageStatTip(const TipContent* pkTipContent, object_id_type topID)
{
	StringType sContent;
	if (NULL == pkTipContent)
	{
		return sContent;
	}

	size_t nPos = sContent.find(STC_SERVER_ID_PLACEHOLDER);
	if (nPos != StringType::npos)
	{
		StringType svr_id = boost::lexical_cast<StringType>(topID);
		sContent.replace(nPos, strlen(STC_SERVER_ID_PLACEHOLDER), svr_id);
	}
	return sContent;
}

void LC_Server_GameStory_Base::_doBestGuildDamageStatRelatedByPlayerDamage(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkActor, GameStoryConfig* pkConfig)
{
	if (NULL == pkActor || NULL == pkConfig)
	{
		return;
	}

	int npcID = pkActor->GetCharType();
	int npcMapResID = pkActor->GetMapResID();
	int sendRewardMapId = pkConfig->GetSendRewardMapID(npcID);
	if(npcMapResID != sendRewardMapId)
	{
		return;
	}

	LC_EncountedStatMgr& kStatMgr = pkActor->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	object_id_type id = ((LC_ServerNPC*)pkActor)->GetHitPlayerID();
	CF_CharType::DataEntry* pkCharDataEntry = pkActor->GetCharTypePtr();
	if (NULL==pkCharDataEntry || pkCharDataEntry->_iType==0)
	{
		return;
	}

	LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(LC_ECT_CAT_ACTOR);
	if (NULL == pkStatCon)
	{
		return;
	}

	const uint64_object_id_vector& rkList = pkStatCon->GetSortedIDs();
	if (rkList.empty())
	{
		return;
	}

	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();
	if(NULL == pkPlayManager)
	{
		return;
	}

	GoalEvent kGoalEvent;
	kGoalEvent.SetGoalEvent(npcID, GetStoryID());
	for(int i=0; i<rkList.size(); ++i)
	{
		object_id_type playerID = (int32_t)rkList[i];
		if (!IS_PLAYER_ID(playerID))
		{
			continue;
		}

		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkPlayManager->FindPlayer(playerID));
		if(NULL != pkPlayer)
		{
			pkPlayer->DoGoalEvent(LC_GOAL_TYPE_DAMAGE_BESTGUILD_NPC, &kGoalEvent);
		}

		//抵御异种：有归属权玩家不需要获得参与奖
		if((int32_t)rkList[i] == id)
		{
			continue;
		}
		int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE2);
		CF_BestGuildBossRankRewards::DataEntryMapExternIterator kIter = CF_BestGuildBossRankRewards::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_BestGuildBossRankRewards::DataEntry* pkData = kIter.PeekNextValuePtr();
			if(pkData->_iType==pkCharDataEntry->_iType && pkData->_iRankStartIndex<=i && i<pkData->_iRankEndIndex)
			{
				stringstream id;
				id << pkCharDataEntry->_iID << ";";

				stringstream ss;
				ss<< (i+1) << ";";

				StringType mailTitle = pkData->_iMailTitle + STDSTR2TPSTR(id.str());
				StringType mailDesc = pkData->_iMailDesc + STDSTR2TPSTR(id.str()) + STDSTR2TPSTR(ss.str());

				UT_ServerHelper::SendToMail(mailTitle, mailDesc, pkStatCon->GetStatCharID(rkList[i]), kStatMgr.GetActorName((object_id_type)rkList[i]), 0, pkData->_kReward, "",nLogCode, i+1);
				break;
			}
		}		
	}
}

void LC_Server_GameStory_Base::_doDamageStatRelated(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkActor, GameStoryConfig* pkConfig)
{
	if (NULL == pkActor || NULL == pkConfig)
	{
		return;
	}
	LC_EncountedStatMgr& kStatMgr = pkActor->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}
	//object_id_type iActorID = pkActor->GetID();
	CF_CharType::DataEntry* pkCharDataEntry = pkActor->GetCharTypePtr();
	if (NULL == pkCharDataEntry)
	{
		return;
	}

	//damage stat rank reward
	do
	{
		const NPCReward* pkNPCReward = pkConfig->m_kNPCRewardInfo.GetNPCReward(pkActor->GetCharType());
		if (NULL == pkNPCReward)
		{
			break;
		}

		NPCDamageRankRewardList::const_iterator cit = pkNPCReward->kDamageRankRewards.begin();
		for (; cit != pkNPCReward->kDamageRankRewards.end(); ++cit)
		{
			const NPCDamageRankReward& rkRankReward = *cit;
			const RankRewardEntries* pkRewardEntries = pkConfig->m_kRankRewardData.GetRankReward(rkRankReward.iRankRewardIndex);
			if (NULL == pkRewardEntries)
			{
				continue;
			}

			LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(rkRankReward.iStatCatagory);
			if (NULL == pkStatCon)
			{
				continue;
			}
			const uint64_object_id_vector& rkList = pkStatCon->GetSortedIDs();
			if (rkList.empty())
			{
				continue;
			}

			{
				uint64_t top_id = rkList.front();
				GSRTTipSchedule kRTTipSchedule;
				kRTTipSchedule.uiBenchmark = 1;
				kRTTipSchedule.eScheduleKey = iKey;
				Utility::NCallBackWrapHandler2<StringType, const TipContent*, object_id_type> kHandler((void*)_resolveDamageStatTip, NULL, iKey);
				kRTTipSchedule.AddTip(pkConfig->m_kAttackerStatData.kKillTip, NULL, top_id, &kHandler);
				GSRTTipEntry* pkRTTipEntry = kRTTipSchedule.Schedule(1);
				if (pkRTTipEntry)
				{
					NotifyTip(*pkRTTipEntry);
				}
				kRTTipSchedule.Reset();
			}

			int iActorLevel = pkActor->GetLevel();
			uint64_object_id_vector::const_iterator citID = rkList.begin();
			RankRewardEntries::const_iterator citRR = pkRewardEntries->begin();
			for (; citRR != pkRewardEntries->end() && citID != rkList.end(); ++citRR)
			{
				const RankRewardEntry& rkRankRewardEntry = *citRR;
				for (int ii = 0; ii < rkRankRewardEntry.iSection && citID != rkList.end(); ++citID)
				{
					switch (rkRankReward.iStatCatagory)
					{
						case LC_ECT_CAT_ACTOR:
							{
								if (!IS_PLAYER_ID((int32_t)*citID))
								{
									continue;
								}
								GSServerReward::MailPlayerMap kPlayers;
								GSServerReward gsReward;
								rkRankRewardEntry.kReward.ResolveReward(RewardParam(1.0, iActorLevel), gsReward);
								kPlayers[pkStatCon->GetStatCharID(*citID)] = kStatMgr.GetActorName((object_id_type)*citID);
								gsReward.Email(kPlayers);
							}
							break;
						case LC_ECT_CAT_GROUP:
							break;
						case LC_ECT_CAT_GUILD:
							break;
						case LC_ECT_CAT_SERVER:
							{
								if (rkRankRewardEntry.kReward.IsValid())
								{
									mem::set<int32_t> kSvrIDs;
									kSvrIDs.insert((int32_t)*citID);
									GSServerReward gsReward;
									rkRankRewardEntry.kReward.ResolveReward(RewardParam(), gsReward);
									gsReward.Email(kSvrIDs);
								}
								
								if (rkRankRewardEntry.pfnOnSettle)
								{
									(*rkRankRewardEntry.pfnOnSettle)((int32_t)*citID);
								}
							}
							break;
						default:
							break;
					}
					++ii;
				}
			}
		}
	}
	while (0);
}

void LC_Server_GameStory_Base::_doEscortRobbedReward(GameLogic::LC_ActorBase* pkActor, GameLogic::LC_ActorBase* pkKiller)
{
	int storyID = GetStoryID();
	if(storyID == GameStoryIDType_BJEscort)
	{
		LC_ServerPlayer* pkRobbedPlayer = NULL;
		LC_ServerNPC* pkNpc = NULL;
		if(pkKiller->GetType() == LOT_PLAYER)
		{
			pkRobbedPlayer = (LC_ServerPlayer*)pkKiller;
		}
		if(pkActor->GetType() == LOT_NPC)
		{
			pkNpc = (LC_ServerNPC*)pkActor;
		}

		if(NULL==pkRobbedPlayer || NULL==pkNpc)
		{
			return;
		}

		int npcCharType = pkNpc->GetCharType();
		object_id_type npcSummonID = pkActor->GetSummonOwnerID();
		if(0 != npcSummonID)
		{
			CF_EscortReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_EscortReward>(npcCharType);
			if(NULL != pkData)
			{
				if(!IS_PLAYER_ID(npcSummonID))
				{
					return;
				}

				pkRobbedPlayer->AddEscortRecord(ESCORT_ROBBING_SUCCESS, npcCharType, pkNpc->GetSummonOwnerName());
				GoalEvent kGoalEvent;
				kGoalEvent.SetGoalEvent(pkNpc->GetCharType(), storyID);
				pkRobbedPlayer->DoGoalEvent(LC_GOAL_TYPE_ESCORT_ROBBED, &kGoalEvent);

				LC_ServerPlayer* pkSummonPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(npcSummonID));
				if(NULL == pkSummonPlayer)
				{
					//押镖者不在线，发邮件
					Utility::UT_SIMDataList itemList;
					for(Utility::UT_SIMDataList::iterator it=pkData->_sRewardItem2.begin(); it!=pkData->_sRewardItem2.end(); ++it)
					{
						itemList.push_back(*it);
					}

					UT_SIMDataInfo data;
					int SummonPlayerLevel = pkNpc->GetSummonOwnerLevel();
					CF_EXPList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_EXPList>(SummonPlayerLevel);
					if(NULL != pkEntry)
					{
						data.SetItemInfo(SPEC_ITEM_TID_EXP, pkEntry->_lBaseXP*pkData->_iExp*GetGlobalSetting.RobbingPercent, 0);
						itemList.push_back(data);

						data.clear();
						data.SetItemInfo(SPEC_ITEM_TID_BUDDY_EXP, pkEntry->_lBaseBuddyXP*pkData->_iBuddyExp*GetGlobalSetting.RobbingPercent, 0);
						itemList.push_back(data);

						data.clear();
						data.SetItemInfo(SPEC_ITEM_TID_CASH, pkEntry->_lBaseCash*pkData->_iCash*GetGlobalSetting.RobbingPercent, 0);
						itemList.push_back(data);
					}

					UT_ServerHelper::SendMailToPlayer(npcSummonID, "11;", "12;", itemList);
				}
				else
				{
					pkSummonPlayer->AddEscortRecord(ESCORT_ROBBED_FAIL, npcCharType, pkRobbedPlayer->GetOwnerCharName());
				}
			}
		}
	}
}

void LC_Server_GameStory_Base::_initMapSetting(GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		return;
	}
	MapSettingMap::iterator it = pkConfig->m_kMapCtrlSetting.m_kMapSettingMap.begin();
	MapSettingMap::iterator itEnd = pkConfig->m_kMapCtrlSetting.m_kMapSettingMap.end();
	for (; it != itEnd; ++it)
	{
		MapSetting& kMapSetting = *(it->second);
		if (NULL != kMapSetting.pfnCheckCondition)
		{
			if (false == (*kMapSetting.pfnCheckCondition)())
			{
				continue;
			}
		}
		// GameStory active the maplogic.
		if(kMapSetting.bInitMap)
		{
			RestartMapLogicByRes(kMapSetting.iMapResID );
		}
		
		if (kMapSetting.IsCross())
		{
			_createCrossMapping(it->first);
		}
	}
}

bool LC_Server_GameStory_Base::RestartMapLogicByRes( int32_t nMapResId )
{
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	pkServerWorldManager->RestartMapLogicByRes(nMapResId);
	return true;
}

ResultType LC_Server_GameStory_Base::BuyVIPProfitTimes(LC_PlayerBase* pkOwnerPlayer, uint32_t times)
{
	ResultType res = RE_FAIL;
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return res;
	}

	GameLogic::GameStoryEntry* pkStoryEntry = pkOwnerPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
	if(NULL == pkStoryEntry)
	{
		return res;
	}

	LC_StatType eStatType = LC_ST_TP_NA;
	pkStoryEntry->ResolveCheckType(eStatType);
	GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
	if (NULL == pkStat)
	{
		return res;
	}

	int32_t iValue = pkOwnerPlayer->GetVIPAsset().GetProfitValue(pkConfig->m_iBuyTimesVIPID);
	if (iValue<1 || pkStat->m_iBuyTimes+times>iValue)
	{
		return RE_BUY_TIMES_LIMITED;
	}

	LC_ShopIDMap kShopIDMap;
	pkConfig->GetBuyVIPProfitTimesShopIDMap(pkStoryEntry, kShopIDMap, times);
	res = LC_Helper::IsShopAvailable(pkOwnerPlayer, kShopIDMap);
	if (RE_SUCCESS != res)
	{
		return res;
	}

	vector<int> nParams;
	nParams.clear();
	nParams.push_back(GetStoryID());
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, VIPTIMES_OPERATE, OPERATE_LOGTYPE1);
	res = LC_Helper::PayForShop(pkOwnerPlayer, kShopIDMap, SYS_FUNC_TP_STORY_TIMES, GetStoryID(), NULL,nLogCode,nParams);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	pkStoryEntry->RecordBuy(times);
	return RE_SHOPCITY_BUY_SUCESS;
}

ResultType LC_Server_GameStory_Base::BuyVIPPrivilegeCardTimes(LC_PlayerBase* pkOwnerPlayer, uint32_t times)
{
	ResultType res = RE_FAIL;
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return res;
	}

	GameLogic::GameStoryEntry* pkStoryEntry = pkOwnerPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
	if(NULL == pkStoryEntry)
	{
		return res;
	}

	LC_StatType eStatType = LC_ST_TP_NA;
	pkStoryEntry->ResolveCheckType(eStatType);
	GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
	if (NULL == pkStat)
	{
		return res;
	}

	uint32_t iValue = LC_Helper::GetVipPrivilegeTimesByType(pkConfig->m_iBuyTimesVIPPrivilegeID, pkOwnerPlayer);
	if (0==iValue || pkStat->m_iVIPPrivilegeTimes+times>iValue)
	{
		return RE_BUY_TIMES_LIMITED;
	}

	LC_ShopIDMap kShopIDMap;
	pkConfig->GetBuyVIPPrivilegeCardTimesShopIDMap(pkStoryEntry, kShopIDMap, times);
	res = LC_Helper::IsShopAvailable(pkOwnerPlayer, kShopIDMap);
	if (RE_SUCCESS != res)
	{
		return res;
	}

	vector<int> nParams;
	nParams.clear();
	nParams.push_back(GetStoryID());
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, VIPPRIVILEGECARDTIMES_OPERATE, OPERATE_LOGTYPE1);
	res = LC_Helper::PayForShop(pkOwnerPlayer, kShopIDMap, SYS_FUNC_TP_STORY_TIMES, GetStoryID(), NULL, nLogCode,nParams);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	pkStoryEntry->RecordBuyVIPPrivilegeCardTimes(times);
	return RE_SHOPCITY_BUY_SUCESS;
}

void LC_Server_GameStory_Base::_updateCrossMapping(void)
{
	GSRTCrossTranscriptionMap::iterator it = m_kRTCrossTranscriptionMap.begin();
	for (; it != m_kRTCrossTranscriptionMap.end(); ++it)
	{
		if (it->second.iRaidID <= 0)
		{
			_createCrossMapping(it->first);
		}
	}
}

ResultType LC_Server_GameStory_Base::ReqEnterCrossMap(GameLogic::LC_PlayerBase* pkPlayer, int32_t iMapResID)
{
	LC_CrossRealmRaidManager* pkCSRMgr = LC_CrossRealmRaidManager::GetSingletonPtr();
	GSRTCrossTranscriptionMap::iterator it = m_kRTCrossTranscriptionMap.begin();
	for (; it != m_kRTCrossTranscriptionMap.end(); ++it)
	{
		if (it->first == iMapResID && it->second.iRaidID > 0)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GameServer GameStory ReqEnterCrossMap StoryID:%u StoryID:%d RaidID:%d", pkPlayer->GetID(), GetStoryID(), it->second.iRaidID);
			return pkCSRMgr->EnterCrossRealmRaid((LC_ServerPlayer*)pkPlayer, it->second.iMapResID);
		}
	}
	return RE_FAIL;
}

class TaskWaverOPCBArg : public TS_PostCommit
{
public:
	uint32_t	m_uiRslt;
	uint32_t	m_uiCharID;
	int32_t		m_iStoryLCID;

	TaskWaverOPCBArg(uint32_t uiCharID, int32_t iStoryLCID): m_uiRslt(RE_FAIL), m_uiCharID(uiCharID), m_iStoryLCID(iStoryLCID) {}
	virtual void operator()(void)
	{
		LC_Server_GameStory_Base* pkStory = SERVER_GET_STORYLOGIC_MANAGER->GetStoryLogic(m_iStoryLCID);
		if (NULL == pkStory)
		{
			return;
		}
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(m_uiCharID);
		if (NULL == pkPlayer)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer GameStory TaskWaver player is null !CharID:%u StroyID:%d", m_uiCharID, pkStory->GetStoryID());
			return;
		}

		ResultType res = pkStory->TaskWaver(pkPlayer, m_uiRslt);
		if (RE_SUCCESS != res)
		{
			const unique_id_impl& instance = pkPlayer->GetInstance();
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer GameStory TaskWaver Failed! c_cid:%d, c_citizenship:%d, c_uid:%d, StroyID:%d, rslt:%u", 
				 instance.detail(), instance.catagory(), pkPlayer->GetUserID(), pkStory->GetStoryID(), m_uiRslt);
			return;
		}

		uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		pkPlayer->UpdateAssignEscortTaskTime(currentTime);
		return;
	}
};

ResultType LC_Server_GameStory_Base::ReqTaskWaver(GameLogic::LC_PlayerBase* pkPlayer, bool addItem)
{
	ResultType res = RE_SUCCESS;
	UT_SIMDataList RefreshConsumeInfo;

	UT_SIMDataList itemList;
	ParseFmtString2DataIFs(GetGlobalSetting.RefreshConsumeItem, itemList);
	if(addItem)
	{
		bool tag = true;
		for(Utility::UT_SIMDataList::iterator it=itemList.begin(); it!=itemList.end(); ++it)
		{
			if(RE_SUCCESS != LC_Helper::CanDeleteItem(pkPlayer->GetPackAsset(), it->ID(), it->ItemCount()))
			{
				tag = false;
				break;
			}
		}

		if(!tag)
		{
			ParseFmtString2DataIFs(GetGlobalSetting.RefreshConsumeACTShopItem, RefreshConsumeInfo);
			for(Utility::UT_SIMDataList::iterator consumeIt=RefreshConsumeInfo.begin(); consumeIt!=RefreshConsumeInfo.end(); ++consumeIt)
			{
				CF_ACTShopA::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTShopA>(consumeIt->IID());
				if(NULL != pkData)
				{
					res = LC_Helper::CheckCashMap(pkPlayer, pkData->_kCurrentPrices, consumeIt->ParamA());
					if(RE_SUCCESS != res)
					{
						return res;
					}
				}
			}

			//加道具
			for(Utility::UT_SIMDataList::iterator consumeIt=RefreshConsumeInfo.begin(); consumeIt!=RefreshConsumeInfo.end(); ++consumeIt)
			{
				CF_ACTShopA::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTShopA>(consumeIt->IID());
				if(NULL != pkData)
				{
					for(Utility::UT_SIMDataList::iterator it=pkData->_kItems.begin(); it!=pkData->_kItems.end(); ++it)
					{
						//res = ((LC_ServerPlayer*)(pkPlayer))->AddItem(it->IID(), it->ParamA()*consumeIt->ParamA());
						res = LC_Helper::AddItem(((LC_ServerPlayer*)pkPlayer)->GetPackAsset(), it->IID(), it->ParamA()*consumeIt->ParamA(), SYS_FUNC_TP_ESCORT_ITEM, false);
						if(RE_SUCCESS != res)
						{
							return res;
						}
					}
				}
			}

			for(Utility::UT_SIMDataList::iterator consumeIt=RefreshConsumeInfo.begin(); consumeIt!=RefreshConsumeInfo.end(); ++consumeIt)
			{
				CF_ACTShopA::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTShopA>(consumeIt->IID());
				if(NULL != pkData)
				{
					res = LC_Helper::PayCashMap(pkPlayer, pkData->_kCurrentPrices, SYS_FUNC_TP_ESCORT_ITEM, consumeIt->ID(), consumeIt->ParamA());
					if(RE_SUCCESS != res)
					{
						return res;
					}
				}
			}
		}
	}

	if(RE_SUCCESS == res)
	{
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		//LC_ShopIDMap kShopIDMap;
		res = CanTaskWaver(pkPlayer/*, kShopIDMap*/);
		if (RE_SUCCESS != res)
		{
			return res;
		}
		/*
		if (!kShopIDMap.empty())
		{
			LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_STORY_TASK, GetStoryID());
		}
		*/
		{
			TaskWaverOPCBArg opArg = TaskWaverOPCBArg(pkPlayer->GetID(), GetLogicID());
			opArg();
			if (RE_SUCCESS != opArg.m_uiRslt)
			{
				res = opArg.m_uiRslt;
				return res;
			}

			if(RE_SUCCESS == res)
			{
				//扣道具
				for(Utility::UT_SIMDataList::iterator it=itemList.begin(); it!=itemList.end(); ++it)
				{
					if(RE_SUCCESS != pkPlayer->DeleteItem(it->ID(), it->ItemCount()))
					{
						return RE_ESCORT_REFRESH_ITEM_NOT_ENOUGH;
					}
				}
			}
		}
	}
	return res;
}

ResultType LC_Server_GameStory_Base::ReqShopping(GameLogic::LC_PlayerBase* pkPlayer, shop_id_type iShopID)
{
	ResultType rslt = RE_SUCCESS;
	do
	{
		GameStoryConfig* pkConfig = GetConfig();
		if (NULL == pkConfig)
		{
			rslt = RE_FAIL;
			break;
		}
		LC_ShopIDMap kShopIDMap;
		kShopIDMap[iShopID] = 1;
		rslt = pkConfig->m_kShopSetting.IsShopAvailable(kShopIDMap);
		if (RE_SUCCESS != rslt)
		{
			break;
		}
		rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
		if (RE_SUCCESS != rslt)
		{
			break;
		}
		if (!kShopIDMap.empty())
		{
			rslt = LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_STORY_SHOP, GetStoryID());
		}
	}
	while (0);
	return rslt;
}

ResultType LC_Server_GameStory_Base::ReqTransfer(GameLogic::LC_PlayerBase* pkPlayer, int32_t iAreaIdx)
{
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	ResultType rslt = RE_SUCCESS;
	do
	{
		GameStoryConfig* pkConfig = GetConfig();
		if (NULL == pkConfig)
		{
			rslt = RE_FAIL;
			break;
		}
		const AreaData* pkArea = pkConfig->m_kMapCtrlSetting.GetAreaData(iAreaIdx);
		if (NULL == pkArea || !pkArea->IsTransport())
		{
			rslt = RE_FAIL;
			break;
		}
		int32_t iMapResID = pkPlayer->GetMapResID();
		if (!pkConfig->ContainMap(iMapResID))
		{
			rslt = RE_FAIL;
			break;
		}
		int32_t iMapLogicID = 0;
		if (iMapResID == pkArea->lMapResID)
		{
			iMapLogicID = pkPlayer->GetMapLogicID();
		}

		//跨服状态暂作屏蔽
		const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
		if ((NULL == pkMapSetting || !pkMapSetting->IsCross()) && UT_ServerHelper::IsInCross((LC_ServerPlayer*)pkPlayer))
		{
			rslt = RE_ON_CROSS_SERVER;
			break;
		}

		rslt = pkServerWorldManager->RequestChangePlayerLocation((LC_ServerPlayer*)pkPlayer,
				LMIOT_TRANSFER,
				iMapLogicID,
				iMapResID,
				pkArea->vecLocation);
		//清仇恨的buff 31000002 5秒
		pkPlayer->ActiveSkillState(31000002, 5, pkPlayer->GetID());
	}
	while (0);
	return rslt;
}

ResultType LC_Server_GameStory_Base::ReqTaskAppoint(GameLogic::LC_PlayerBase* pkPlayer, int32_t iTaskID)
{
	ResultType rslt = RE_SUCCESS;
	do
	{
		const TaskData* pkTaskData = NULL;
		rslt = CanTaskAppoint(pkPlayer, iTaskID, pkTaskData);
		if (RE_SUCCESS != rslt)
		{
			break;
		}

		/*
		if (!pkTaskData->kAppointShopIDMap.empty())
		{
			LC_Helper::PayForShop(pkPlayer, pkTaskData->kAppointShopIDMap, SYS_FUNC_TP_STORY_TASK, GetStoryID());
		}
		*/

		{
			//do task appoint
			rslt = TaskAppoint(pkPlayer, iTaskID, true);
			if(RE_SUCCESS == rslt)
			{
				UT_SIMDataList cashList;
				ParseFmtString2DataIFs(GetGlobalSetting.AppointConsumeCash, cashList);
				for(Utility::UT_SIMDataList::iterator it=cashList.begin(); it!=cashList.end(); ++it)
				{
					pkPlayer->ReduceCash(it->ParamA(), (CashType)(it->ID()), SYS_FUNC_TP_ESCORT, 0);
				}
			}
		}
	}
	while (0);
	return rslt;
}

ResultType LC_Server_GameStory_Base::ReqBuyMapStepTimes(GameLogic::LC_PlayerBase* pkPlayer)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return RE_FAIL;
	}

	ResultType rslt = RE_SUCCESS;
	do
	{
		GameLogic::GameStoryEntry* pkStoryEntry = pkPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID());
		if (NULL == pkStoryEntry)
		{
			rslt = RE_FAIL;
			break;
		}

		LC_StatType eStatType = LC_ST_TP_NA;
		pkStoryEntry->ResolveCheckType(eStatType);
		GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
		if (NULL == pkStat)
		{
			rslt = RE_FAIL;
			break;
		}

		LC_ShopIDMap kShopIDMap;
		int buyTimes = pkConfig->GetMapStepCostShopID(pkStoryEntry, kShopIDMap);

		int shopId = 0;
		for(LC_ShopIDMap::iterator it = kShopIDMap.begin(); it!=kShopIDMap.end(); ++it)
		{
			shopId = it->first;
		}

		if(!checkTimesLimit(shopId, buyTimes))
		{
			rslt = RE_ITEM_BUY_TOO_MANY;
			break;
		}
		
		rslt = LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_STORY_TIMES, GetStoryID());
		if(RE_SUCCESS == rslt)
		{
			pkStoryEntry->RecordBuy(1);
		}
	}
	while (0);
	return rslt;
}

bool LC_Server_GameStory_Base::checkTimesLimit(int32_t shopID, int32_t times)
{
	CF_ShopList::DataEntry* pkShopData = SafeGetCSVFileDataEntryPtr<CF_ShopList>(shopID);
	if(NULL == pkShopData) return false;

	for (Utility::LC_ComMap::const_iterator it2 = pkShopData->_sCountLimits.begin(); it2 != pkShopData->_sCountLimits.end(); ++it2)
	{
		int totalTimes= it2->second;
		if(times >= totalTimes)
			return false;
	}

	return true;
}

void LC_Server_GameStory_Base::_checkTask(GameLogic::LC_PlayerBase* pkPlayer, GameStoryConfig* pkConfig)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	if (NULL == pkConfig)
	{
		pkConfig = GetConfig();
	}
	if (NULL == pkConfig)
	{
		return;
	}

	TaskSetting& kTaskSetting = pkConfig->m_kTaskSetting;
	if (!kTaskSetting.IsTaskAutoStart())
	{
		return;
	}

	CheckClearSameTaskType(pkPlayer);
	//LC_ShopIDMap kShopIDs;
	//if (RE_SUCCESS == CanTaskWaver(pkPlayer, kShopIDs) && kShopIDs.empty())
	if (RE_SUCCESS == CanTaskWaver(pkPlayer))
	{
		TaskWaverOPCBArg opArg = TaskWaverOPCBArg(pkPlayer->GetID(), GetLogicID());
		opArg();
	}
}
ResultType LC_Server_GameStory_Base::CanTaskWaver(GameLogic::LC_PlayerBase* pkPlayer/*, LC_ShopIDMap& kShopIDMap*/)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return RE_FAIL;
	}
	TaskSetting& kTaskSetting = pkConfig->m_kTaskSetting;
	//if (!pkConfig->m_kTaskSetting.IsTaskWaverEnabled() || kTaskSetting.kTaskMap.empty())
	if(kTaskSetting.kTaskMap.empty())
	{
		return RE_FAIL;
	}
	LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
	uint32_t uiAssignType = pkTaskMgr->GetAssignType(kTaskSetting.uiTaskType);
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	LC_AssignTaskData* pkAssignData = pkAssignInfo->AddAssignType(uiAssignType, timestamp);
	if (NULL == pkAssignData)
	{
		return RE_FAIL;
	}
	/*
	LC_CommonStatEntry* pkCmnStatEntry = GetCommonStatEntry(pkPlayer, pkConfig);
	if (NULL == pkCmnStatEntry)
	{
		return RE_FAIL;
	}
	*/
	const TaskIDSet& kTaskIDs = pkAssignData->GetAssignTaskIDs();
	TaskIDVec kCurTaskIDVec;
	TaskIDSet::const_iterator citTaskID = kTaskIDs.begin();
	for (; citTaskID != kTaskIDs.end(); ++citTaskID)
	{
		if (kTaskSetting.GetTaskData(*citTaskID))
		{
			kCurTaskIDVec.push_back(*citTaskID);
		}
	}

	ResultType rslt = RE_SUCCESS;
	if (kCurTaskIDVec.empty())
	{
		int32_t iCount = 0;
		rslt = pkAssignData->IsAssignAvailable(pkTaskMgr->GetTaskAssignEntry(uiAssignType), iCount);
		if (RE_SUCCESS != rslt)
		{
			return rslt;
		}
	}

	//护送次数是否够，不足，不能刷新
	ResultType res = pkPlayer->CheckAcceptEscortTask();
	if(RE_SUCCESS != res)
	{
		return res;
	}

	
	//刷新道具是否够	
	UT_SIMDataList itemList;
	ParseFmtString2DataIFs(GetGlobalSetting.RefreshConsumeItem, itemList);
	for(Utility::UT_SIMDataList::iterator it=itemList.begin(); it!=itemList.end(); ++it)
	{
		if(!pkPlayer->IsItemEnough(it->ItemCount(), it->ID()))
		{
			return RE_REFRESH_ESCORT_TASK_CONSUMEITEM_NOT_ENOUGH;
		}
	}

	return rslt;
	/*
	uint32_t uiCount = kTaskSetting.GetWaverCost(kCurTaskIDVec, kShopIDMap, pkCmnStatEntry->GetAccumulateCntDay());
	if (!kShopIDMap.empty())
	{
		if (pkPlayer->GetVIPAsset().GetProfitValue(kTaskSetting.iPayWaverEnabledVIPID) <= 0)
		{
			rslt = RE_GAMESTORY_VIP_NEEDED;
			return rslt;
		}
		rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
		if (RE_SUCCESS != rslt)
		{
			return rslt;
		}
	}
	else if (uiCount && (pkCmnStatEntry->GetAccumulateCntDay() + uiCount > kTaskSetting.uiWaverFreeTimes))
	{
		rslt = RE_FAIL;
	}
	return rslt;
	*/
}

ResultType LC_Server_GameStory_Base::TaskWaver(GameLogic::LC_PlayerBase* pkPlayer, uint32_t& rlt/*, uint32_t& uiCount*/)
{
	rlt = RE_FAIL;
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return RE_FAIL;
	}
	//if (!pkConfig->m_kTaskSetting.IsTaskWaverEnabled() || pkConfig->m_kTaskSetting.kTaskMap.empty())
	if(pkConfig->m_kTaskSetting.kTaskMap.empty())
	{
		return RE_FAIL;
	}
	LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
	uint32_t uiAssignType = pkTaskMgr->GetAssignType(pkConfig->m_kTaskSetting.uiTaskType);
	LC_AssignTaskData* pkAssignData = pkAssignInfo->GetAssignTaskData(uiAssignType);
	if (NULL == pkAssignData)
	{
		return RE_FAIL;
	}
	/*
	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return RE_FAIL;
	}
	LC_CommonStatEntry* pkCmnStatEntry = GetCommonStatEntry(pkPlayer, pkConfig);
	if (NULL == pkCmnStatEntry)
	{
		return RE_FAIL;
	}
	*/
	int oldTaskID = 0;
	int newTaskID = 0;
	TaskSetting& kTaskSetting = pkConfig->m_kTaskSetting;
	const TaskIDSet& kTaskIDs = pkAssignData->GetAssignTaskIDs();
	TaskIDVec kCurTaskIDs;
	TaskIDSet::const_iterator citTaskID = kTaskIDs.begin();
	for (; citTaskID != kTaskIDs.end(); ++citTaskID)
	{
		if (kTaskSetting.GetTaskData(*citTaskID))
		{
			kCurTaskIDs.push_back(*citTaskID);
			oldTaskID = *citTaskID;
		}
	}

	//已经是最好的任务了，不刷新
	if(oldTaskID == GetGlobalSetting.BestEscortTaskID)
	{
		return RE_FAIL;
	}

	TaskIDVec kWaverTaskIDs;
	int uiCount = kTaskSetting.TaskWaver(pkPlayer, kCurTaskIDs, kWaverTaskIDs);
	if(uiCount < 1)
	{
		return RE_FAIL;
	}
	else
	{
		newTaskID = kWaverTaskIDs[0];
		rlt = RE_SUCCESS;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "使用道具刷新护送物资车任务 playeID:%d, oldTaskID:%d, newTaskID:%d\n", pkPlayer->GetID(), oldTaskID, newTaskID);
	if(newTaskID <= oldTaskID)
	{
		return RE_SUCCESS;
	}

	pkAssignData->ReplaceTask(kCurTaskIDs, kWaverTaskIDs);
	if (pkConfig->m_kTaskSetting.IsClearTaskFinishRecord())
	{
		for (TaskIDVec::iterator waveit = kWaverTaskIDs.begin(); waveit != kWaverTaskIDs.end(); ++waveit)
		{
			pkTaskMap->RemoveTaskFinishRecord(*waveit);
		}
	}
	pkTaskMap->UpdateDirtyFlag(DIRTY_FLAG_ASSIGN_TASK);
	return RE_SUCCESS;
}

ResultType LC_Server_GameStory_Base::CanTaskAppoint(GameLogic::LC_PlayerBase* pkPlayer, int32_t iTaskID, const TaskData*& pkTaskData)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return RE_FAIL;
	}
	TaskSetting& kTaskSetting = pkConfig->m_kTaskSetting;
	if (!kTaskSetting.IsIsTaskAppointEnabled() || kTaskSetting.kTaskMap.empty())
	{
		return RE_FAIL;
	}
	LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
	uint32_t uiAssignType = pkTaskMgr->GetAssignType(kTaskSetting.uiTaskType);
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	LC_AssignTaskData* pkAssignData = pkAssignInfo->AddAssignType(uiAssignType, timestamp);
	if (NULL == pkAssignData)
	{
		return RE_FAIL;
	}

	/*
	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return RE_FAIL;
	}
	*/

	//护送次数是否足够
	ResultType res = pkPlayer->CheckAcceptEscortTask();
	if(RE_SUCCESS != res)
	{
		return res;
	}

	//刷新消耗的钻石是否足够
	if(iTaskID == GetGlobalSetting.BestEscortTaskID)
	{
		UT_SIMDataList cashList;
		ParseFmtString2DataIFs(GetGlobalSetting.AppointConsumeCash, cashList);
		for(Utility::UT_SIMDataList::iterator it=cashList.begin(); it!=cashList.end(); ++it)
		{
			if(!pkPlayer->IsCashEnough(it->ParamA(), it->ID()))
			{
				return RE_REFRESH_ESCORT_TASK_APPOINT_NOT_ENOUGH;
			}
		}
	}

	/*
	LC_CommonStatEntry* pkCmnStatEntry = GetCommonStatEntry(pkPlayer, pkConfig);
	if (NULL == pkCmnStatEntry)
	{
		return RE_FAIL;
	}
	*/
	//const TaskIDSet& kTaskIDs = pkAssignData->GetAssignTaskIDs();
	pkTaskData = kTaskSetting.GetTaskData(iTaskID);
	if (NULL == pkTaskData)
	{
		return RE_FAIL;
	}
	int32_t iCount = 0;
	ResultType rslt = pkAssignData->IsAssignAvailable(pkTaskMgr->GetTaskAssignEntry(uiAssignType), iCount);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}

	/*
	if (!pkTaskData->kAppointShopIDMap.empty())
	{
		rslt = LC_Helper::IsShopAvailable(pkPlayer, pkTaskData->kAppointShopIDMap);
		if (RE_SUCCESS != rslt)
		{
			return rslt;
		}
	}
	*/

	return rslt;
}

ResultType LC_Server_GameStory_Base::CheckClearSameTaskType(GameLogic::LC_PlayerBase* pkPlayer)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return RE_FAIL;
	}
	TaskSetting& kTaskSetting = pkConfig->m_kTaskSetting;
	if (!kTaskSetting.IsClearSameTaskType())
	{
		return RE_FAIL;
	}
	GameLogic::TaskIDSet kTaskIDSet;
	kTaskSetting.GetTaskIDSet(kTaskIDSet);
	pkPlayer->CancelTypeTask(kTaskSetting.uiTaskType, kTaskIDSet);
	return RE_SUCCESS;
}

void LC_Server_GameStory_Base::AssignEscortTask(GameLogic::LC_PlayerBase* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	uint32_t lastAssignEscortTaskTime = pkPlayer->GetAssignEscortTaskTime();
	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	bool tag = LC_Helper::CheckIsSameDay(lastAssignEscortTaskTime, currentTime);
	if(tag)
	{
		return;
	}

	//主动派发，派发的是固定任务
	ResultType res = TaskAppoint(pkPlayer, GetGlobalSetting.AssignEscortTaskID);
	if(RE_SUCCESS == res)
	{
		pkPlayer->UpdateAssignEscortTaskTime(currentTime);
	}
}

void LC_Server_GameStory_Base::ClearAreaKeyMonster(GameStoryScheduleKey iKey, int32_t iAreaIndex)
{
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}

	GSRTAreaCmnStatMap::iterator it = pkRTBSchedule->kAreaCmnStat.find(iAreaIndex);
	if (it == pkRTBSchedule->kAreaCmnStat.end())
	{
		return;
	}
	
	static LC_ServerNPCManager* pkNPCMgr = SERVER_GET_NPC_MANAGER();
	GSRTCommonStats& kRTCmnStat = it->second;	
	GSRTKeyMonsterMap::iterator itKey = kRTCmnStat.kKeyMonsterMap.begin();
	for (; itKey != kRTCmnStat.kKeyMonsterMap.end(); ++itKey)
	{
		GSRTKeyMonster& kRTKeyMonster = itKey->second;
		if (!kRTKeyMonster.IsDead())
		{
			LC_NPCBase* pkNPC = pkNPCMgr->FindNPC(itKey->first);
			if (pkNPC)
			{
				pkNPC->SetHP(0);
			}
		}
	}
}

ResultType LC_Server_GameStory_Base::TaskAppoint(GameLogic::LC_PlayerBase* pkPlayer, int32_t iTaskID, bool checkTaskID)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return RE_FAIL;
	}
	if (!pkConfig->m_kTaskSetting.IsIsTaskAppointEnabled() || pkConfig->m_kTaskSetting.kTaskMap.empty())
	{
		return RE_FAIL;
	}
	LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
	uint32_t uiAssignType = pkTaskMgr->GetAssignType(pkConfig->m_kTaskSetting.uiTaskType);
	LC_AssignTaskData* pkAssignData = pkAssignInfo->GetAssignTaskData(uiAssignType);
	if (NULL == pkAssignData)
	{
		uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		pkAssignData = pkAssignInfo->AddAssignType(uiAssignType, timestamp);
		if(NULL == pkAssignData)
		{
			return RE_FAIL;
		}
	}

	/*
	if (!pkConfig->m_kCmnStatData.IsValid())
	{
		return RE_FAIL;
	}
	
	LC_CommonStatEntry* pkCmnStatEntry = GetCommonStatEntry(pkPlayer, pkConfig);
	if (NULL == pkCmnStatEntry)
	{
		return RE_FAIL;
	}
	*/
	TaskSetting& kTaskSetting = pkConfig->m_kTaskSetting;
	const TaskData* pkTaskData = kTaskSetting.GetTaskData(iTaskID);
	if (NULL == pkTaskData)
	{
		return RE_FAIL;
	}

	int oldTaskID = 0;
	const TaskIDSet& kTaskIDs = pkAssignData->GetAssignTaskIDs();
	TaskIDVec kCurTaskIDs;
	TaskIDSet::const_iterator citTaskID = kTaskIDs.begin();
	for (; citTaskID != kTaskIDs.end(); ++citTaskID)
	{
		if (kTaskSetting.GetTaskData(*citTaskID))
		{
			kCurTaskIDs.push_back(*citTaskID);
			oldTaskID = *citTaskID;
		}
	}

	if(checkTaskID && oldTaskID==GetGlobalSetting.BestEscortTaskID)
	{
		return RE_FAIL;
	}

	TaskIDVec kAppointTaskIDs;
	kAppointTaskIDs.push_back(iTaskID);

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "刷新特定护送物资车任务 playeID:%d, iTaskID:%d\n", pkPlayer->GetID(), iTaskID);

	pkAssignData->ReplaceTask(kCurTaskIDs, kAppointTaskIDs);
	pkTaskMap->UpdateDirtyFlag(DIRTY_FLAG_ASSIGN_TASK);
	return RE_SUCCESS;
}

void LC_Server_GameStory_Base::_createCrossMapping(int32_t iMapResID)
{
	//LC_CrossRealmRaidManager* pkCSRMgr = LC_CrossRealmRaidManager::GetSingletonPtr();
	RaidCallBack cbCreate = boost::bind(&LC_Server_GameStory_Base::_onCrossTransCreated, this, _1, _2, _3);
	RaidCallBack cbFail = boost::bind(&LC_Server_GameStory_Base::_onCrossTransFailed, this, _1, _2, _3);
	RaidCallBack cbDestroy = boost::bind(&LC_Server_GameStory_Base::_onCrossTransDestroyed, this, _1, _2, _3);
	m_kRTCrossTranscriptionMap[iMapResID].iMapResID = iMapResID;
	LC_CrossRealmRaidManager::GetSingletonPtr()->CreateCrossRealmRaid(iMapResID, cbCreate, cbFail, cbDestroy);
}

void LC_Server_GameStory_Base::_destroyCrossMapping(int32_t iMapResID)
{
	LC_CrossRealmRaidManager* pkCSRMgr = LC_CrossRealmRaidManager::GetSingletonPtr();
	pkCSRMgr->DestoryCrossRealmRaid(iMapResID);
}

void LC_Server_GameStory_Base::_unitMapSetting(GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		return;
	}
	GSRTCrossTranscriptionMap::iterator it = m_kRTCrossTranscriptionMap.begin();
	for (; it != m_kRTCrossTranscriptionMap.end(); ++it)
	{
		_destroyCrossMapping(it->second.iMapResID);
	}
	m_kRTCrossTranscriptionMap.clear();
}

void LC_Server_GameStory_Base::_setupBatchSchedule(GameStoryConfig* pkConfig)
{
	if (NULL == pkConfig)
	{
		return;
	}

	if((true==pkConfig->IsSaveState() || true==pkConfig->IsGlobalNodeUpdateBossInfo()) && false==GetGameServerApp()->IsNodeType_Global())
	{
		return;
	}
	
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();

	_checkresetRank();

	bool hasCmn = false;
	MapSettingMap::iterator it = pkConfig->m_kMapCtrlSetting.m_kMapSettingMap.begin();
	MapSettingMap::iterator itEnd = pkConfig->m_kMapCtrlSetting.m_kMapSettingMap.end();
	for (; it != itEnd; ++it)
	{
		int32_t iMapResID = it->first;
		MapSetting * pkSetting = it->second;
		if (pkSetting->IsSolelySchedule())
		{
			GameLogic::LC_IDList kMapLCIDs;
			pkWorldManager->GetLogicMapIDListFromResID(kMapLCIDs, iMapResID);
			if (kMapLCIDs.empty())
			{
				continue;
			}
			GameLogic::LC_IDList::iterator itLCID = kMapLCIDs.begin();
			for (; itLCID != kMapLCIDs.end(); ++itLCID)
			{
				int32_t iMapLogicID = *itLCID;
				LC_ServerMapLogic_Place* pkMaplogic = pkWorldManager->GetMapLogic(iMapLogicID);

				GameStoryScheduleKey iKey = (GameStoryScheduleKey)iMapLogicID;

				GSRTBatchScheduleMap::iterator itBatch = m_kRTBatchScheduleMap.find(iKey);

				if (itBatch == m_kRTBatchScheduleMap.end())
				{
					if (pkMaplogic)
					{
						_addBatchSchedule(iKey, pkMaplogic->GetMonsterLevel(), pkConfig);
					}
				}
				else
				{
					if (NULL == pkMaplogic)
					{
						_clearBatchSchedule(iKey, pkConfig);
						m_kRTBatchScheduleMap.erase(itBatch);
					}
				}
			}
		}
		else
		{
			hasCmn = true;
		}
	}

	bool tag = false;
	//有存储的，恢复存储的调度
	if(pkConfig->m_bSaveState)
	{
		tag = _activeBatchSchedule(GS_SCHEDULE_KEY_COMMON, -1, pkConfig);
	}
	
	if (hasCmn && !tag)
	{
		_addBatchSchedule(GS_SCHEDULE_KEY_COMMON, -1, pkConfig, true);
	}
}

void LC_Server_GameStory_Base::_resetBatchSchedule(GameStoryConfig* pkConfig)
{
	GSRTBatchScheduleMap::iterator it = m_kRTBatchScheduleMap.begin();
	for (; it != m_kRTBatchScheduleMap.end(); ++it)
	{
		_clearBatchSchedule(it->first, pkConfig);
		it->second.Reset();
	}
	m_kRTBatchScheduleMap.clear();
}
/////////////////////////////////////////////////////////////////
struct AddBuffOP : public GameLogic::LC_ActorOperator
{
	AddBuffOP(const BuffData& buf): kBuff(buf) {}
	virtual bool operator()(GameLogic::LC_ActorBase* pkPlayer)
	{
		if(false == ((LC_ServerPlayer*)pkPlayer)->IsMainController())
		{
			return false;
		}

		kBuff.AttachTo(pkPlayer, pkPlayer->GetCurrentLogicAreaID());
		return true;
	}
private:
	const BuffData& kBuff;
};
/////////////////////////////////////////////////////////////////
bool  LC_Server_GameStory_Base::_tryTriggerNextBatch(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig, int batchIndex, uint32_t endTime, int iSeq, mem::vector<int>& batchVec)
{
	if(NULL == pkConfig)
	{
		return false;
	}

	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule)
	{
		return false;
	}

	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	BatchData* pkBatch = kBatchSchedule.GetBatchData(batchIndex);
	if(NULL == pkBatch)
	{
		return false;
	}

	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	for(GameLogic::LC_IDSet::iterator it=pkBatch->kTriBatchs.begin(); it!=pkBatch->kTriBatchs.end(); ++it)
	{
		int nextBatchIndex = *it;
		BatchData* pkNextBatch = kBatchSchedule.GetBatchData(nextBatchIndex);
		if(NULL == pkNextBatch)
		{
			continue;
		}

		//这波刷怪也有有效时间，检测这波刷怪的有效时间是否过期
		if(pkNextBatch->iBatchTime > 1)
		{
			uint32_t nextEndTime = endTime+pkNextBatch->iBatchTime;
			if(currentTime > nextEndTime)
			{
				return _tryTriggerNextBatch(iKey, pkConfig, nextBatchIndex, nextEndTime, iSeq, batchVec);
			}
		}
		
		batchVec.push_back(nextBatchIndex);
		int64_t vaildTime = pkNextBatch->iBatchTime-(currentTime-endTime);
		GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
		if (pkRTDetail)
		{
			pkRTDetail->kBatchIdx.iBatchIdx = nextBatchIndex;
			pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
			_triggerBatchCB_CF(TIMER_ID_NA, this, iKey, pkRTDetail->iIndex, -1, pkConfig, false, 0, false, vaildTime);
			_addTips(iKey, GET_CURRENT_TIMESTAMP_IN_SECONDS(), pkBatch->kBatchTip, NULL, 0);
		}
	}

	return true;
}

void	LC_Server_GameStory_Base::_initBatchScheduleByBatchIndex(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig, mem::map<uint64_t, uint64_t>& batchIndexRecord, int iSeq)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Story[%d] Batch Schedule Key[%d] Init, Seq[%d] ", GetStoryID(), iKey, iSeq);
	if (NULL == pkConfig)
	{
		return;
	}

	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule || pkRTBatchSchedule->IsInited() || pkRTBatchSchedule->IsFailed())
	{
		return;
	}
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (IsTimerIDValid(pkRTBatchSchedule->iInitTimerID))
	{
		pkTimerMgr->CancelTimer(pkRTBatchSchedule->iInitTimerID);
		pkRTBatchSchedule->iInitTimerID = TIMER_ID_NA;
	}
	//set up monster setting
	MapCtrlSetting& kMapCtrSetting = pkConfig->m_kMapCtrlSetting;
	{
		GSRTMonsterSetting& kRTMonSetting = pkRTBatchSchedule->kMonsterSetting;
		kRTMonSetting.SetUpExtraMonsterSkills(kMapCtrSetting.m_kMonsterSetting, kMapCtrSetting.m_kSkillSetting);
	}
	//set up key infos
	BatchConfig& kBatchConfig = kBatchSchedule.kConfig;
	{
		KeyStatList::iterator it = kBatchConfig.kKeyStats.begin();
		for (; it != kBatchConfig.kKeyStats.end(); ++it)
		{
			GSRTKeyStat kRTKeyStat(*it);
			pkRTBatchSchedule->kKeyStats.push_back(kRTKeyStat);
		}
	}
	uint32_t curtimestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	pkRTBatchSchedule->SetInit(curtimestamp);
	_setupTrigger(iKey, pkConfig);

	mem::vector<int> vaildBatchIndex;
	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	for(mem::map<uint64_t, uint64_t>::iterator it=batchIndexRecord.begin(); it!=batchIndexRecord.end(); ++it)
	{
		int batchIndex = (int)it->first;
		BatchData* pkBatch = kBatchSchedule.GetBatchData(batchIndex);
		if(NULL == pkBatch)
		{
			continue;
		}

		int64_t vaildTime = 0;
		uint32_t endTime = (uint32_t)it->second;		
		if(endTime != 0)
		{
			if(currentTime < endTime)
			{
				//本次调度时间未结束
				vaildTime = endTime-currentTime;				
			}
			else
			{
				//本次调度时间已结束
				//结束后，是否有下来的调度  有，触发
				_tryTriggerNextBatch(iKey, pkConfig, batchIndex, endTime, iSeq, vaildBatchIndex);
				continue;
			}
		}

		vaildBatchIndex.push_back(batchIndex);
		GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
		if (pkRTDetail)
		{
			pkRTDetail->kBatchIdx.iBatchIdx = batchIndex;
			pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
			_triggerBatchCB_CF(TIMER_ID_NA, this, iKey, pkRTDetail->iIndex, -1, pkConfig, false, 0, false, vaildTime);
			_addTips(iKey, GET_CURRENT_TIMESTAMP_IN_SECONDS(), pkBatch->kBatchTip, NULL, 0);
		}
	}

	//触发其他的组
	if (kBatchSchedule.TriggerSize())
	{
		//
	}
	if (kBatchSchedule.TimerSize())
	{
		//do init timer
		BatchIndices::iterator itIdx = kBatchSchedule.kTimerIndices.begin();
		for (; itIdx != kBatchSchedule.kTimerIndices.end(); ++itIdx)
		{
			int currentIndex = *itIdx;
			for(int i=0; i<vaildBatchIndex.size(); ++i)
			{
				if(vaildBatchIndex[i] == currentIndex)
				{
					continue;
				}
			}

			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch && !pkBatch->sCronStr.empty())
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->iTriTimerID = pkTimerMgr->SetCron(pkBatch->sCronStr, (int64_t)pkBatch->iBatchTime * 1000, LC_Server_GameStory_Base::_triggerBatchCB, this, iKey, pkRTDetail->iIndex, -1);
					uint32_t tritimestamp = (uint32_t)(pkTimerMgr->GetTriTimestamp(pkRTDetail->iTriTimerID) / 1000);
					_addTips(iKey, tritimestamp, pkBatch->kBatchTip, NULL, 0);
				}
			}
		}
	}
	if (kBatchSchedule.CycleSize())
	{
		BatchIndices::iterator itIdx = kBatchSchedule.kCycleIndices.begin();
		for (; itIdx != kBatchSchedule.kCycleIndices.end(); ++itIdx)
		{
			int currentIndex = *itIdx;
			for(int i=0; i<vaildBatchIndex.size(); ++i)
			{
				if(vaildBatchIndex[i] == currentIndex)
				{
					continue;
				}
			}

			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch)
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
					_nextBatchSchedule(iKey, pkRTDetail->iIndex, TIMER_ID_NA, pkConfig);
				}
			}
		}
	}
	if (kBatchSchedule.PassiveSize())
	{
		BatchIndices::iterator itIdx = kBatchSchedule.kPassiveIndices.begin();
		for (; itIdx != kBatchSchedule.kPassiveIndices.end(); ++itIdx)
		{
			int currentIndex = *itIdx;
			for(int i=0; i<vaildBatchIndex.size(); ++i)
			{
				if(vaildBatchIndex[i] == currentIndex)
				{
					continue;
				}
			}
			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch)
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
				}
			}
		}
	}
	if (kBatchSchedule.ConditionSize())
	{
		BatchIndices::iterator itIdx = kBatchSchedule.kConditionIndices.begin();
		for (; itIdx != kBatchSchedule.kConditionIndices.end(); ++itIdx)
		{
			int currentIndex = *itIdx;
			for(int i=0; i<vaildBatchIndex.size(); ++i)
			{
				if(vaildBatchIndex[i] == currentIndex)
				{
					continue;
				}
			}

			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch)
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
				}
			}
		}
	}
	//do init map things
	{
		MapSettingMap::iterator itMap = kMapCtrSetting.m_kMapSettingMap.begin();
		MapSettingMap::iterator itEnd = kMapCtrSetting.m_kMapSettingMap.end();
		for (; itMap != itEnd; ++itMap)
		{
			MapSetting * pkSetting = itMap->second;
			BuffDataList& kBuffs = pkSetting->kBuffs;
			if (kBuffs.empty())
			{
				continue;
			}
			LC_ServerMapLogicPlaceVector kPlaces;
			_getRTScheduleMaps(iKey, itMap->first, kPlaces, pkConfig);
			LC_ServerMapLogicPlaceVector::iterator itPlace = kPlaces.begin();
			for (; itPlace != kPlaces.end(); ++itPlace)
			{
				LC_ServerMapLogic_Place* pkMaplogic = (*itPlace);
				if (pkMaplogic && pkMaplogic->GetMap())
				{
					for (BuffDataList::iterator itBuff = kBuffs.begin(); itBuff != kBuffs.end(); ++itBuff)
					{
						AddBuffOP kAddBuffOP = AddBuffOP(*itBuff);
						pkMaplogic->GetMap()->ForAllPlayer(&kAddBuffOP);
					}
				}
			}
		}
	}
	//init faction score
	InitFactionScore(iKey);
}

void LC_Server_GameStory_Base::_initBatchSchedule(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig, bool limitNotice, int iSeq, Utility::timer_id_type timerID, LC_ServerPlayer* pServerPlayer)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Story[%d] Batch Schedule Key[%d] Init, Seq[%d] ", GetStoryID(), iKey, iSeq);
	if (NULL == pkConfig)
	{
		return;
	}
	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule || pkRTBatchSchedule->IsInited() || pkRTBatchSchedule->IsFailed())
	{
		if (!GetRepeated())
			return;
	}
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (IsTimerIDValid(pkRTBatchSchedule->iInitTimerID))
	{
		pkTimerMgr->CancelTimer(pkRTBatchSchedule->iInitTimerID);
		pkRTBatchSchedule->iInitTimerID = TIMER_ID_NA;
	}
	//set up monster setting
	MapCtrlSetting& kMapCtrSetting = pkConfig->m_kMapCtrlSetting;
	{
		GSRTMonsterSetting& kRTMonSetting = pkRTBatchSchedule->kMonsterSetting;
		kRTMonSetting.SetUpExtraMonsterSkills(kMapCtrSetting.m_kMonsterSetting, kMapCtrSetting.m_kSkillSetting);
	}
	//set up key infos
	BatchConfig& kBatchConfig = kBatchSchedule.kConfig;
	{
		KeyStatList::iterator it = kBatchConfig.kKeyStats.begin();
		for (; it != kBatchConfig.kKeyStats.end(); ++it)
		{
			GSRTKeyStat kRTKeyStat(*it);
			pkRTBatchSchedule->kKeyStats.push_back(kRTKeyStat);
		}
	}
	uint32_t curtimestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	pkRTBatchSchedule->SetInit(curtimestamp);
	_setupTrigger(iKey, pkConfig);
	if (kBatchSchedule.SeqSize())
	{
		//do init seq
		GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
		if (pkRTDetail)
		{
			GSRTBatchIndex& kRTBIndex = pkRTDetail->kBatchIdx;
			kRTBIndex.iSeqIdx = iSeq;
			int iCurBatchIdx = kBatchSchedule.GetBatchIdx(kRTBIndex.iSeqIdx);
			if (iCurBatchIdx < 0)
			{
				return;
			}
			BatchData* pkBatchEntry = kBatchSchedule.GetBatchData(iCurBatchIdx);
			if (NULL == pkBatchEntry)
			{
				return;
			}
			kRTBIndex.iBatchIdx = iCurBatchIdx;
			_nextBatchSchedule(iKey, pkRTDetail->iIndex, TIMER_ID_NA, pkConfig, false, pServerPlayer);
		}
	}
	if (kBatchSchedule.TriggerSize())
	{
		//
	}
	if (kBatchSchedule.TimerSize())
	{
		//do init timer
		BatchIndices::iterator itIdx = kBatchSchedule.kTimerIndices.begin();
		for (; itIdx != kBatchSchedule.kTimerIndices.end(); ++itIdx)
		{
			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch && !pkBatch->sCronStr.empty())
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->iTriTimerID = pkTimerMgr->SetCron(pkBatch->sCronStr, (int64_t)pkBatch->iBatchTime * 1000, LC_Server_GameStory_Base::_triggerBatchCB, this, iKey, pkRTDetail->iIndex, -1);
					uint32_t tritimestamp = (uint32_t)(pkTimerMgr->GetTriTimestamp(pkRTDetail->iTriTimerID) / 1000);
					_addTips(iKey, tritimestamp, pkBatch->kBatchTip, NULL, 0);
				}
			}
		}
	}
	if (kBatchSchedule.CycleSize())
	{
		BatchIndices::iterator itIdx = kBatchSchedule.kCycleIndices.begin();
		for (; itIdx != kBatchSchedule.kCycleIndices.end(); ++itIdx)
		{
			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch)
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
					_nextBatchSchedule(iKey, pkRTDetail->iIndex, TIMER_ID_NA, pkConfig, limitNotice);
				}
			}			
		}

		if(limitNotice)
		{
			NotifyPlayerGameStoryBatch(iKey, NULL, TIMER_ID_NA, pkConfig);
		}
	}
	if (kBatchSchedule.PassiveSize())
	{
		BatchIndices::iterator itIdx = kBatchSchedule.kPassiveIndices.begin();
		for (; itIdx != kBatchSchedule.kPassiveIndices.end(); ++itIdx)
		{
			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch)
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
				}
			}
		}
	}
	if (kBatchSchedule.ConditionSize())
	{
		BatchIndices::iterator itIdx = kBatchSchedule.kConditionIndices.begin();
		for (; itIdx != kBatchSchedule.kConditionIndices.end(); ++itIdx)
		{
			BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
			if (pkBatch)
			{
				GSRTBatchDetail* pkRTDetail = pkRTBatchSchedule->AutoDetail();
				if (pkRTDetail)
				{
					pkRTDetail->kBatchIdx.iBatchIdx = *itIdx;
					pkRTDetail->kBatchIdx.iSeqIdx = iSeq;
				}
			}
		}
	}
	//do init map things
	{
		MapSettingMap::iterator itMap = kMapCtrSetting.m_kMapSettingMap.begin();
		MapSettingMap::iterator itEnd = kMapCtrSetting.m_kMapSettingMap.end();
		for (; itMap != itEnd; ++itMap)
		{
			MapSetting * pkSetting = itMap->second;
			BuffDataList& kBuffs = pkSetting->kBuffs;
			if (kBuffs.empty())
			{
				continue;
			}
			LC_ServerMapLogicPlaceVector kPlaces;
			_getRTScheduleMaps(iKey, itMap->first, kPlaces, pkConfig);
			LC_ServerMapLogicPlaceVector::iterator itPlace = kPlaces.begin();
			for (; itPlace != kPlaces.end(); ++itPlace)
			{
				LC_ServerMapLogic_Place* pkMaplogic = (*itPlace);
				if (pkMaplogic && pkMaplogic->GetMap())
				{
					for (BuffDataList::iterator itBuff = kBuffs.begin(); itBuff != kBuffs.end(); ++itBuff)
					{
						AddBuffOP kAddBuffOP = AddBuffOP(*itBuff);
						pkMaplogic->GetMap()->ForAllPlayer(&kAddBuffOP);
					}
				}
			}
		}
	}
	//init faction score
	InitFactionScore(iKey);
}
/////////////////////////////////////////////////////////////////
struct RemBuffOP : public GameLogic::LC_ActorOperator
{
	RemBuffOP(const BuffData& buf): kBuff(buf) {}
	virtual bool operator()(GameLogic::LC_ActorBase* pkPlayer)
	{
		if(false == ((LC_ServerPlayer*)pkPlayer)->IsMainController())
		{
			return false;
		}

		kBuff.DettachFrom(pkPlayer, pkPlayer->GetCurrentLogicAreaID());
		return true;
	}
private:
	const BuffData& kBuff;
};
struct RemFactionBuffOP : public GameLogic::LC_ActorOperator
{
	RemFactionBuffOP(const FactionBuffData& buf): kBuff(buf) {}
	virtual bool operator()(GameLogic::LC_ActorBase* pkPlayer)
	{
		if(false == ((LC_ServerPlayer*)pkPlayer)->IsMainController())
		{
			return false;
		}

		kBuff.DettachFrom(pkPlayer);
		return true;
	}
private:
	const FactionBuffData& kBuff;
};
/////////////////////////////////////////////////////////////////
void LC_Server_GameStory_Base::_clearBatchSchedule(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Story[%d] Batch Schedule Key[%d] Clear ", GetStoryID(), iKey);
	//UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkConfig || NULL == pkRTBatchSchedule)
	{
		return;
	}
	//pkRTBatchSchedule->ClearTimer();
	_clearTips(iKey);
	_cleanupTrigger(iKey);
	if (!pkRTBatchSchedule->IsClear())
	{
		pkRTBatchSchedule->SetClear(GET_CURRENT_TIMESTAMP_IN_SECONDS());
		uint32_t persisttime = pkRTBatchSchedule->PerssistTime();
		{
			//undo map init things
			MapCtrlSetting& kMapCtrSetting = pkConfig->m_kMapCtrlSetting;
			MapSettingMap::iterator itMap = kMapCtrSetting.m_kMapSettingMap.begin();
			MapSettingMap::iterator itEnd = kMapCtrSetting.m_kMapSettingMap.end();
			for (; itMap != itEnd; ++itMap)
			{
				MapSetting * pkSetting = itMap->second;
				BuffDataList& kBuffs = pkSetting->kBuffs;
				FactionBuffDataList& kFactionBuffs = pkSetting->kFactionBuffs;

				LC_ServerMapLogicPlaceVector kPlaces;
				_getRTScheduleMaps(iKey, itMap->first, kPlaces, pkConfig);
				LC_ServerMapLogicPlaceVector::iterator itPlace = kPlaces.begin();
				for (; itPlace != kPlaces.end(); ++itPlace)
				{
					LC_ServerMapLogic_Place* pkMaplogic = *itPlace;
					if (pkMaplogic && pkMaplogic->GetMap())
					{
						for (BuffDataList::iterator itBuff = kBuffs.begin(); itBuff != kBuffs.end(); ++itBuff)
						{
							RemBuffOP kRemBuffOP = RemBuffOP(*itBuff);
							pkMaplogic->GetMap()->ForAllPlayer(&kRemBuffOP);
						}
						for (FactionBuffDataList::iterator itBuff = kFactionBuffs.begin(); itBuff != kFactionBuffs.end(); ++itBuff)
						{
							RemFactionBuffOP kRemBuffOP = RemFactionBuffOP(*itBuff);
							pkMaplogic->GetMap()->ForAllPlayer(&kRemBuffOP);
						}
					}
				}
			}
		}
		if (pkConfig->m_kCmnStatData.IsValid() && pkConfig->m_kCmnStatData.GetCmnStatEntry(GS_CMN_STAT_TP_PERSIST_TIME))
		{
			LC_ActorPtrList kActors;
			_getRTScheduleActors(iKey, kActors, pkConfig);
			LC_ActorPtrList::iterator it = kActors.begin();
			for (; it != kActors.end(); ++it)
			{
				if (*it)
				{
					RecordCommonStat((GameLogic::LC_PlayerBase*)*it, GS_CMN_STAT_TP_PERSIST_TIME, persisttime, pkConfig);
				}
			}
		}
		if (pkConfig->m_kBatchSechedule.kConfig.IsClearToPersssitTimeReward())
		{
			LC_ActorPtrList kActors;
			_getRTScheduleActors(iKey, kActors, pkConfig);
			if (!kActors.empty())
			{
				const ThresholdReward* pkReward = pkConfig->m_kPersistTimeRewardInfo.GetCurrentStatReward(persisttime);
				if (pkReward)
				{
					LC_ActorPtrList::iterator it = kActors.begin();
					for (; it != kActors.end(); ++it)
					{
						if (*it)
						{
							LC_PlayerBase* pkPlayer = (LC_PlayerBase*)*it;
							GSServerReward gsReward;
							pkReward->kReward.ResolveReward(RewardParam(), gsReward, pkPlayer, persisttime, GetPlayerStats(pkPlayer), pkRTBatchSchedule->IsSuccess());
							gsReward.Reward(pkPlayer, GetStoryID());
						}
					}
				}
			}
		}
		if (pkConfig->m_kBatchSechedule.kConfig.IsClearToFactionReward())
		{
			FactionType iWinFaction = GetWinFactionID();
			if (iWinFaction == FACTION_NONE)
			{
				//攻方未获胜，则防守方胜利
				iWinFaction = FACTION_DEFENDER;
			}
			FactionType iFailFaction = (iWinFaction == FACTION_DEFENDER) ? FACTION_ATTACKER : FACTION_DEFENDER; 
			FactionWar& kFactionWar = pkConfig->m_kFactionWar;
			if (NULL != kFactionWar.pfnOnSettle)
			{
				int32_t iStatus = (iWinFaction == FACTION_DEFENDER) ? FactionWar_DefWin : FactionWar_DefLose;
				(*kFactionWar.pfnOnSettle)(iStatus);
			}
			RewardData* pkWinReward = kFactionWar.kWinRewards.GetReward(iWinFaction);
			RewardData* pkFailReward = kFactionWar.kFailRewards.GetReward(iFailFaction);
			//由于要给所有参与过的玩家发奖励，所以必定走邮件奖励
			GSServerReward::MailPlayerMap *pkWinMailPlayers = NULL, *pkFailMailPlayers = NULL;
			if (iWinFaction == FACTION_ATTACKER)
			{
				pkWinMailPlayers = GetAttackers();
				pkFailMailPlayers = GetDefenders();
			}
			else
			{
				pkWinMailPlayers = GetDefenders();
				pkFailMailPlayers = GetAttackers();
			}
			GSServerReward gsWinReward, gsFailReward;
			if (NULL != pkWinReward && NULL != pkWinMailPlayers)
			{
				pkWinReward->ResolveReward(RewardParam(), gsWinReward);
				if (gsWinReward.sMailTitle.empty())
				{
					gsWinReward.sMailTitle = "15;";
				}
				if (gsWinReward.sMailDesc.empty())
				{
					gsWinReward.sMailDesc = "16;";
				}
				gsWinReward.Email(*pkWinMailPlayers);
			}
			if (NULL != pkFailReward && NULL != pkFailMailPlayers)
			{
				pkFailReward->ResolveReward(RewardParam(), gsFailReward);
				if (gsFailReward.sMailTitle.empty())
				{
					gsFailReward.sMailTitle = "15;";
				}
				if (gsFailReward.sMailDesc.empty())
				{
					gsFailReward.sMailDesc = "16;";
				}
				gsFailReward.Email(*pkFailMailPlayers);
			}
			LC_ActorPtrList kActors;
			_getRTScheduleActors(iKey, kActors, pkConfig);
			if (!kActors.empty())
			{
				MG_GAMESTORY_FACTION_WAR_SETTLE kWinMsg, kFailMsg;
				kWinMsg.m_bIsWin = true;
				kWinMsg.m_iExp = (int64_t)gsWinReward.dExp;
				kWinMsg.m_iMoney = (int64_t)gsWinReward.dSP;
				kWinMsg.m_iMoney = (int64_t)gsWinReward.dSP;
				Utility::UT_SIMDataList::iterator itItem = gsWinReward.kItems.begin();
				for (; itItem != gsWinReward.kItems.end(); ++itItem)
				{
					Utility::UT_SIMDataInfo& kDataInfo = *itItem;
					kWinMsg.m_kItemMap[kDataInfo.ID()] = kDataInfo.ItemCount();
				}
				kFailMsg.m_bIsWin = false;
				kFailMsg.m_iExp = (int64_t)gsFailReward.dExp;
				kFailMsg.m_iMoney = (int64_t)gsFailReward.dSP;
				kFailMsg.m_iMoney = (int64_t)gsFailReward.dSP;
				itItem = gsFailReward.kItems.begin();
				for (; itItem != gsFailReward.kItems.end(); ++itItem)
				{
					Utility::UT_SIMDataInfo& kDataInfo = *itItem;
					kFailMsg.m_kItemMap[kDataInfo.ID()] = kDataInfo.ItemCount();
				}
				LC_ActorPtrList::iterator it = kActors.begin();
				for (; it != kActors.end(); ++it)
				{
					if (NULL == (*it))
					{
						continue;
					}
					LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)*it;
					if (NULL == pkPlayer)
					{
						continue;
					}
					if (pkPlayer->GetFactionID() == iWinFaction)
					{
						pkPlayer->SendMsgToClient(MGPT_GAMESTORY_NOTIFY_FACTION_SETTLE, &kWinMsg);
					}
					else if (pkPlayer->GetFactionID() == iFailFaction)
					{
						pkPlayer->SendMsgToClient(MGPT_GAMESTORY_NOTIFY_FACTION_SETTLE, &kFailMsg);
					}
				}
			}
		}
		_doRankReward(iKey, pkConfig);
		if (pkConfig->m_kBatchSechedule.kConfig.IsClearToClearMonster())
		{
			//do clear monster
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			GSRTMonsterRecordMap kMonsterRecord = pkRTBatchSchedule->kMonsterRecord;
			GSRTMonsterRecordMap::iterator itRTMon = kMonsterRecord.begin();
			for (; itRTMon != kMonsterRecord.end(); ++itRTMon)
			{
				//not to clear record entry
				pkSkillInterface->RemoveLogicObject(itRTMon->first);
				//do to not?
				_clearSpawnedMonster(itRTMon->first, NULL);
			}
			pkRTBatchSchedule->kMonsterRecord.clear();
		}
		if (pkConfig->m_kBatchSechedule.kConfig.IsClearToClearPlayer())
		{
			GSResIDList::iterator itResID = pkConfig->m_kMapCtrlSetting.m_kMapResIDList.begin();
			for (; itResID != pkConfig->m_kMapCtrlSetting.m_kMapResIDList.end(); ++itResID)
			{
				LC_ServerMapLogicPlaceVector kPlaces;
				_getRTScheduleMaps(iKey, *itResID, kPlaces, pkConfig);
				LC_ServerMapLogicPlaceVector::iterator itPlace = kPlaces.begin();
				for (; itPlace != kPlaces.end(); ++itPlace)
				{
					LC_ServerMapLogic_Place* pkMaplogic = *itPlace;
					if (pkMaplogic && pkMaplogic->GetStoryID())
					{
						//to clear
						LC_ActorPtrList kActors;
						pkMaplogic->GetPlayersInMap(kActors);
						LC_ActorPtrList::iterator itActor = kActors.begin();
						for (; itActor != kActors.end(); ++itActor)
						{
							//pkSkillInterface->ReturnToCity(*itActor);
							pkMaplogic->LetPlayerLeave((LC_ServerPlayer*)*itActor);
						}
					}
				}
			}
		}
		NotifyPlayerGameStoryBatch(iKey, NULL, Utility::TIMER_ID_NA, pkConfig);
	}

	pkRTBatchSchedule->ClearTimer();
}

void LC_Server_GameStory_Base::_nextBatchSchedule(GameStoryScheduleKey iKey, int iDetailIdx, Utility::timer_id_type timerID, GameStoryConfig* pkConfig, bool limitNotice, LC_ServerPlayer* pServerPlayer)
{
	if (NULL == pkConfig)
	{
		return;
	}
	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	GSRTBatchSchedule* pkRTBatchSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBatchSchedule)
	{
		return;
	}
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GSRTBatchDetail* pkBDetail = pkRTBatchSchedule->GetBatchDetail(iDetailIdx);
	if (NULL == pkBDetail)
	{
		return;
	}
	GSRTBatchIndex& kRTBIdx = pkBDetail->kBatchIdx;
	int32_t iBatchIdx = kRTBIdx.iBatchIdx;
	BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(iBatchIdx);
	if (pkBatchData)
	{
		switch (pkBatchData->iType)
		{
			case GSBSTP_SEQUECE:
			case GSBSTP_CYCLE:
				{
					int32_t iBreakTime = kBatchSchedule.GetBatchBreak(kRTBIdx);
					//iBreakTime = 300;
					if (iBreakTime > 0)
					{
						pkBDetail->iTriTimerID = pkTimerMgr->DelayCall((int64_t)iBreakTime * ONE_SECOND_MILI,
												 _triggerBatchCB,
												 (void(*)(Utility::timer_id_type, LC_Server_GameStory_Base*, GameStoryScheduleKey, int, int))NULL,
												 this,
												 iKey,
												 iDetailIdx,
												 -1);

						uint32_t timestamp = (uint32_t)(pkTimerMgr->GetTriTimestamp(pkBDetail->iTriTimerID) / ONE_SECOND_MILI);
						_addTips(iKey, timestamp, pkBatchData->kBatchTip, NULL, 0);
						if(!limitNotice)
						{
							NotifyPlayerGameStoryBatch(iKey, NULL, Utility::TIMER_ID_NA, pkConfig);
						}						

						if (pServerPlayer)
						{
							MG_SC_AddTimer addTimer;
							addTimer.m_nTimerId = pkBDetail->iTriTimerID;
							pServerPlayer->SetScheduleKey(iKey);
							pServerPlayer->SendMsgToClient(MGPT_SC_ADD_TIMER, &addTimer);
						}
					}
					else
					{
						_triggerBatchCB_CF(timerID, this, iKey, iDetailIdx, -1, pkConfig, limitNotice);
						_addTips(iKey, GET_CURRENT_TIMESTAMP_IN_SECONDS(), pkBatchData->kBatchTip, NULL, 0);
					}
				}
				break;
			case GSBSTP_TRIGGER:
			case GSBSTP_TIMER:
			case GSBSTP_PASSIVE:
			case GSBSTP_CONDITION:
			default:
				break;
		}
	}
}

bool LC_Server_GameStory_Base::_activeBatchSchedule(GameStoryScheduleKey iKey, int iSelfAdaptingLevel, GameStoryConfig* pkConfig)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_activeBatchSchedule Story[%d] Batch Schedule Key[%d] Add, Adapt[%d] ", GetStoryID(), iKey, iSelfAdaptingLevel);
	if (NULL == pkConfig)
	{
		return false;
	}
	if (m_kRTBatchScheduleMap.find(iKey) != m_kRTBatchScheduleMap.end())
	{
		return false;
	}

	char str[MAX_CHAR_NAME_LENGTH + 1] = { '\0' };
	sprintf_s( str, MAX_CHAR_NAME_LENGTH + 1, ":%s", pkConfig->FileName() );

	//存储的调度是否有效
	bool flag = UT_ServerHelper::GetWatchTowerInfoValidFlag();
	if(true != flag)
	{
		UT_ServerHelper::RedisRank_Clear(str);
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "delete invaild schedule! serverid:%d, namespace:%s", GetGameServerID(), str);
		return false;
	}

	//从redis读出来存储的调度顺序
	mem::map<uint64_t, uint64_t> batchIndexRecord;
	UT_ServerHelper::RedisGetData_IntRange(str, batchIndexRecord);
	if(batchIndexRecord.size() > 0)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "delete vaild schedule! refresh! serverid:%d, namespace:%s", GetGameServerID(), str);
		UT_ServerHelper::RedisRank_Clear(str);
	}
	else
	{
		return false;
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "load schedule! namespace:%s", str);

	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	GSRTBatchSchedule& kRTBatchSchedule = m_kRTBatchScheduleMap[iKey];
	kRTBatchSchedule.SetSelfAdaptingLevel(iSelfAdaptingLevel);
	if (kBatchSchedule.kConfig.IsHoldForPlayer())
	{
		return false;
	}

	_initBatchScheduleByBatchIndex(iKey, pkConfig, batchIndexRecord);

	return true;
}

void LC_Server_GameStory_Base::_addBatchSchedule(GameStoryScheduleKey iKey, int iSelfAdaptingLevel, GameStoryConfig* pkConfig, bool limitNotice)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Story[%d] Batch Schedule Key[%d] Add, Adapt[%d] ", GetStoryID(), iKey, iSelfAdaptingLevel);
	if (NULL == pkConfig)
	{
		return;
	}
	if (m_kRTBatchScheduleMap.find(iKey) != m_kRTBatchScheduleMap.end())
	{
		if (!GetRepeated())
			return;
	}
	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	GSRTBatchSchedule& kRTBatchSchedule = m_kRTBatchScheduleMap[iKey];
	kRTBatchSchedule.SetSelfAdaptingLevel(iSelfAdaptingLevel);
	if (kBatchSchedule.kConfig.IsHoldForPlayer())
	{
		return;
	}
	if (kBatchSchedule.kConfig.iPrepareTime > 0)
	{
		UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
		kRTBatchSchedule.iInitTimerID = pkTimerMgr->DelayCall((int64_t)kBatchSchedule.kConfig.iPrepareTime * ONE_SECOND_MILI,
										_initBatchCB,
										(void(*)(Utility::timer_id_type, LC_Server_GameStory_Base*, GameStoryScheduleKey, int))NULL,
										this,
										iKey,
										0);
	}
	else
	{
		_initBatchSchedule(iKey, pkConfig, limitNotice);
	}
	return;
}

//--------------------------------------------------------------------------------------------------------------------
static StringType _resolveKillNPCTip(const TipContent* pkTipContent, LC_ActorBase* pkKiller, LC_ActorBase* pkSufferer)
{
	StringType sContent;
	if (NULL == pkTipContent)
	{
		return sContent;
	}
	sContent = pkTipContent->Resolve(pkSufferer, pkKiller);
	StringType::size_type nPos = StringType::npos;
	if (pkSufferer)
	{
		nPos = sContent.find(STC_NPC_NAME_PLACEHOLDER);
		if (nPos != StringType::npos)
		{
			sContent.replace(nPos, strlen(STC_NPC_NAME_PLACEHOLDER), pkSufferer->GetOwnerCharName());
		}
	}
	if (pkKiller)
	{
		nPos = sContent.find(STC_PLAYER_NAME_PLACEHOLDER);
		if (nPos != StringType::npos)
		{
			sContent.replace(nPos, strlen(STC_PLAYER_NAME_PLACEHOLDER), pkKiller->GetOwnerCharName());
		}
	}
	return sContent;
}

void LC_Server_GameStory_Base::ReportMosterStateInfo(GameStoryScheduleKey iKey, Utility::timer_id_type iTimerID)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ReportMosterStateInfo", GetStoryID(), iKey, iTimerID);
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig || !pkConfig->IsKeyReport())
	{
		return;
	}

	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}

	uint64_t curtimestamp = GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
	MG_Report_SS_Alliance_Boss_Info reportMsg;
	MG_Report_SS_Alliance_Boss_Clean clearMsg;
	reportMsg.m_nBossInfo.m_nServerID = clearMsg.m_nServerID = UT_ServerHelper::GetServerId();

	MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	AreaIndices::iterator it = kMapCtrlSetting.m_kKeyAreaIndics.begin();
	for (; it != kMapCtrlSetting.m_kKeyAreaIndics.end(); ++it)
	{
		const AreaData* pkAreaData = kMapCtrlSetting.GetAreaData(*it);
		if (NULL == pkAreaData)
		{
			continue;
		}

		const MonsterConfigData& rkMonConfig = pkAreaData->kMonsterConfig;

		GSRTCommonStats& kRTCmnStat = pkRTBSchedule->kAreaCmnStat[*it];

		for(int lineNum=0; lineNum<kMapCtrlSetting.m_nLineCount; ++lineNum)
		{
			GSResIDSet::const_iterator citMon = rkMonConfig.kKeyIDs.begin();
			for (; citMon != rkMonConfig.kKeyIDs.end(); ++citMon)
			{
				ST_Alliance_Boss_StateInfo kBoss;
				kBoss.m_nState = E_MASTER_GROUP_STATE_None;
				kBoss.m_rLocation.m_iMapID = pkAreaData->lMapResID;
				kBoss.m_rLocation.m_iPosX = pkAreaData->vecLocation.x;
				kBoss.m_rLocation.m_iPosY = pkAreaData->vecLocation.y;
				kBoss.m_nNpcType = *citMon;
				kBoss.m_nLineNum = lineNum;

				GSRTKeyMonster* pkRTKeyMonster = NULL;
				GSRTKeyMonsterMap::iterator itKey = kRTCmnStat.kKeyMonsterMap.begin();
				int npcLogicID = 0;
				for (; itKey != kRTCmnStat.kKeyMonsterMap.end(); ++itKey)
				{
					GSRTKeyMonster& rkRTKeyMonster = itKey->second;
					if (rkRTKeyMonster.iResID != (int32_t)kBoss.m_nNpcType)
					{
						continue;
					}
					if(rkRTKeyMonster.lineNum != kBoss.m_nLineNum)
					{
						continue;
					}
					if (pkRTKeyMonster && rkRTKeyMonster.IsDead() && rkRTKeyMonster.uiTimestamp <= pkRTKeyMonster->uiTimestamp)
					{
						continue;
					}

					pkRTKeyMonster = &rkRTKeyMonster;
					npcLogicID = itKey->first;
					break;
				}
 				if (pkRTKeyMonster)
				{
					BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(pkRTKeyMonster->kBatchIndex.iBatchIdx);
					if(NULL != pkBatchData)
					{
						int isClear = pkBatchData->iIsClear;
						uint32_t preTime = (pkBatchData->iPrepareTime)*100;
					
						kBoss.m_nNpcType = pkRTKeyMonster->iResID;
						kBoss.m_killerID = pkRTKeyMonster->uiCharID;
						TPSTR2STDSTR(pkRTKeyMonster->sCharName, kBoss.m_killName);
						kBoss.m_nState = pkRTKeyMonster->IsDead() ? E_MASTER_GROUP_STATE_DIE : E_MASTER_GROUP_STATE_SURVIVE ;
						kBoss.m_nLineNum = pkRTKeyMonster->lineNum;
						kBoss.m_nPrepareTime = pkBatchData->iPrepareTime;

						if(isClear == 1)
						{
							kBoss.m_nRefreshTime = (kBoss.m_nState == E_MASTER_GROUP_STATE_DIE) ? GetTriTime(pkRTKeyMonster->timeID) : pkRTKeyMonster->uiTimestamp;
						}
						else
						{
							kBoss.m_nRefreshTime = (kBoss.m_nState == E_MASTER_GROUP_STATE_DIE) ? GetBatchNodeTriTime(iKey, pkRTKeyMonster->kBatchIndex.iBatchIdx, iTimerID) : pkRTKeyMonster->uiTimestamp;
						}

						if(pkBatchData->iIsClearBatchNPC && kBoss.m_nState==E_MASTER_GROUP_STATE_SURVIVE)
						{
							int64_t startTime = pkTimerMgr->GetTriTimestamp(iTimerID);
							if(startTime <= 0)
							{
								kBoss.m_nDisAppearTime = kBoss.m_nRefreshTime + pkBatchData->iBatchTime;
							}
							else
							{
								kBoss.m_nDisAppearTime = startTime/1000 + pkBatchData->iBatchTime;
							}
						}
					}
				}
				else
				{
					GSRTID2BatchDetailMap::iterator itBD = pkRTBSchedule->kDetailMap.begin();
					for (; itBD != pkRTBSchedule->kDetailMap.end(); ++itBD)
					{
						GSRTBatchDetail& kRTBDetail = itBD->second;
						BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(kRTBDetail.kBatchIdx.iBatchIdx);
						if (NULL == pkBatchData || pkBatchData->kRelatedReportArea.find(*it) == pkBatchData->kRelatedReportArea.end())
						{
							continue;
						}

						int64_t iTriTime  = 0;
						int64_t iEndTime = 0;
						if (kRTBDetail.iTriTimerID > 0)
						{
							if (kRTBDetail.iTriTimerID != iTimerID)
							{
								iTriTime = pkTimerMgr->GetTriTimestamp(kRTBDetail.iTriTimerID);
							}
							else
							{
								iTriTime = pkTimerMgr->GetNextTriTimestamp(kRTBDetail.iTriTimerID);
							}
						}
						if (kRTBDetail.iEndTimerID > 0)
						{
							if (kRTBDetail.iEndTimerID != iTimerID)
							{
								iEndTime = pkTimerMgr->GetTriTimestamp(kRTBDetail.iEndTimerID);
							}
							else
							{
								iEndTime = pkTimerMgr->GetNextTriTimestamp(kRTBDetail.iEndTimerID);
							}
						}
						kBoss.m_nRefreshTime = uint32_t(iTriTime <= 0 ? iEndTime / 1000 : iTriTime / 1000);
					}
				}
				reportMsg.m_nBossInfo.m_rBossList.push_back(kBoss);

				ST_Alliance_Boss_CleanInfo cleanInfo;
				cleanInfo.m_nNpcType = kBoss.m_nNpcType;
				cleanInfo.m_rLocation.m_iMapID = kBoss.m_rLocation.m_iMapID;
				cleanInfo.m_rLocation.m_iPosX = kBoss.m_rLocation.m_iPosX;
				cleanInfo.m_rLocation.m_iPosY = kBoss.m_rLocation.m_iPosY;
				cleanInfo.m_nLineNum = kBoss.m_nLineNum;
				clearMsg.m_nBossInfo.push_back(cleanInfo);
			}
		}
	}

	if (!reportMsg.m_nBossInfo.m_rBossList.empty())
	{
		int32_t nClearReport = pkConfig->GetKeyClearReport();
		if (nClearReport != GS_K_RP_NO)
		{
			clearMsg.m_nCleanType = nClearReport;
			if(false==pkConfig->IsGlobalNodeUpdateBossInfo())
			{
				SendMsgToSociety(MGPT_SS_FALLIANCE_BOSS_RFINFO_REPCLEAR, &clearMsg);
			}
			else if(true==pkConfig->IsGlobalNodeUpdateBossInfo() && true==GetGameServerApp()->IsNodeType_Global())
			{
				SendMsgToSociety(MGPT_SS_FALLIANCE_BOSS_RFINFO_REPCLEAR_GLOBAL, &clearMsg);
			}
		}

		if(false==pkConfig->IsGlobalNodeUpdateBossInfo())
		{
			SendMsgToSociety(MGPT_SS_FALLIANCE_BOSS_RFINFO_REPORT, &reportMsg);
		}
		else if(true==pkConfig->IsGlobalNodeUpdateBossInfo() && true==GetGameServerApp()->IsNodeType_Global())
		{
			SendMsgToSociety(MGPT_SS_FALLIANCE_BOSS_RFINFO_REPORT_GLOBAL, &reportMsg);
		}

		if (pkConfig->IsKeyReportGroup())
		{
			//全区广播
			SendMsgToChat(MGPT_FALLIANCE_BOSS_INFO_NOTICE, &reportMsg, 0, 0, UT_ServerHelper::GetServerId(), 0, MG_SERVER_ROUTER_FLAG_GROUP);
		}
		else if (pkConfig ->IsKeyReportServer())
		{
			if(true==pkConfig->IsGlobalNodeUpdateBossInfo() && true==GetGameServerApp()->IsNodeType_Global())
			{
				//全服广播
				SendMsgToChat(MGPT_FALLIANCE_BOSS_INFO_NOTICE_NOTIFY, &reportMsg, 0, 0, UT_ServerHelper::GetServerId(), 0, MG_SERVER_ROUTER_FLAG_SINGLE);
			}
			else
			{
				//全服广播
				SendMsgToChat(MGPT_FALLIANCE_BOSS_INFO_NOTICE, &reportMsg, 0, 0, UT_ServerHelper::GetServerId(), 0, MG_SERVER_ROUTER_FLAG_SINGLE);
			}
		}
	}
}

uint32_t LC_Server_GameStory_Base::GetBatchNodeTriTime(GameStoryScheduleKey iKey, int32_t iBatchIdx, Utility::timer_id_type iTimerID)
{
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return 0;
	}
	GSRTBatchDetail* pkDetail = pkRTBSchedule->GetBatchDetailByBatchIdx(iBatchIdx);
	if (NULL == pkDetail)
	{
		return 0;
	}
	return GetBatchNodeTriTime(pkDetail, iTimerID);
}

uint32_t	LC_Server_GameStory_Base::GetTriTime(Utility::timer_id_type iTimerID)
{
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();

	int64_t iTriTime  = 0;
	if (IsTimerIDValid(iTimerID))
	{
		iTriTime = pkTimerMgr->GetTriTimestamp(iTimerID);
	}
	return uint32_t(iTriTime<=0 ? 0 : iTriTime / 1000);
}

uint32_t LC_Server_GameStory_Base::GetNextBatchNodeTriTime(GameStoryScheduleKey iKey, int32_t iBatchIdx, Utility::timer_id_type iTimerID)
{
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return 0;
	}
	GSRTBatchDetail* pkDetail = pkRTBSchedule->GetNextBatchDetailByBatchIdx(iBatchIdx);
	if (NULL == pkDetail)
	{
		return 0;
	}
	return GetBatchNodeTriTime(pkDetail, iTimerID);
}

uint32_t	LC_Server_GameStory_Base::GetBatchNodeTriTime(GSRTBatchDetail* pkDetail, Utility::timer_id_type iTimerID)
{
	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (/*NULL == pkTimerMgr ||*/ NULL == pkDetail)
	{
		return 0;
	}

	int64_t iTriTime  = 0;
	int64_t iEndTime = 0;
	if (IsTimerIDValid(pkDetail->iTriTimerID))
	{
		if (pkDetail->iTriTimerID != iTimerID)
		{
			iTriTime = pkTimerMgr->GetTriTimestamp(pkDetail->iTriTimerID);
		}
		else
		{
			iTriTime = pkTimerMgr->GetNextTriTimestamp(pkDetail->iTriTimerID);
		}
	}
	if (IsTimerIDValid(pkDetail->iEndTimerID))
	{
		if (pkDetail->iTriTimerID != iTimerID)
		{
			iEndTime = pkTimerMgr->GetTriTimestamp(pkDetail->iEndTimerID);
		}
		else
		{
			iEndTime = pkTimerMgr->GetNextTriTimestamp(pkDetail->iEndTimerID);
		}
	}
	return uint32_t(iTriTime <= 0 ? iEndTime / 1000 : iTriTime / 1000);
}

void LC_Server_GameStory_Base::SyncKeyMonsterHP()
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	KeyMonsterLogicIDsMap::iterator it = m_kKeyMonsterLogicIDsMap.begin();
	for (; it != m_kKeyMonsterLogicIDsMap.end(); ++it)
	{
		MG_GAMESTORY_KEY_MONSTER_HP_SYNC syncMsg;
		object_id_set::iterator itObj = it->second.begin();
		for (; itObj != it->second.end(); ++itObj)
		{
			object_id_type iLogicID = *itObj;
			LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(iLogicID);
			if (NULL == pkActor)
			{
				continue;
			}
			GAMESTORY_KEY_MONSTER_HP kMonHP;
			kMonHP.m_iLogicID = iLogicID;
			kMonHP.m_iCharType = pkActor->GetCharType();
			kMonHP.m_iMaxHP = pkActor->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
			kMonHP.m_iCurHP = pkActor->GetHP();
			syncMsg.m_kKeyMonsterHPs.push_back(kMonHP);
		}

		GameStoryScheduleKey iKey = it->first;
		LC_ActorPtrList kActors;
		_getRTScheduleActors(iKey, kActors, pkConfig);
		LC_ActorPtrList::iterator itActor = kActors.begin();
		for (; itActor != kActors.end(); ++itActor)
		{
			((LC_ServerPlayer*)*itActor)->SendMsgToClient(MGPT_GAMESTORY_KEY_MONSTER_HP_SYNC, &syncMsg);
		}
	}
}

void LC_Server_GameStory_Base::AddKeyMonsterLogicID(GameStoryScheduleKey iKey, object_id_type iLogicID)
{
	object_id_set& kIDs = m_kKeyMonsterLogicIDsMap[iKey];
	kIDs.insert(iLogicID);
}

void LC_Server_GameStory_Base::NotifyKillNPCTip(GameStoryScheduleKey iKey, LC_ActorBase* pkKiller, LC_ActorBase* pkSufferer, GameStoryConfig* pkConfig)
{
	if (NULL == pkSufferer || NULL == pkConfig)
	{
		return;
	}

	//广播
	GSRTTipSchedule kRTTipSchedule;
	kRTTipSchedule.uiBenchmark = 1;
	kRTTipSchedule.eScheduleKey = iKey;
	Utility::NCallBackWrapHandler3<StringType, const TipContent*, LC_ActorBase*, LC_ActorBase*> kHandler((void*)_resolveKillNPCTip, NULL, pkKiller, pkSufferer);
	kRTTipSchedule.AddTip(pkConfig->m_kAttackerStatData.kKillTip, pkKiller, pkSufferer->GetCharType(), &kHandler);
	GSRTTipEntry* pkRTTipEntry = kRTTipSchedule.Schedule(1);
	if (pkRTTipEntry && 0==pkSufferer->GetSendKillAnnouncementFlag())
	{
		NotifyTip(*pkRTTipEntry, pkKiller);
	}
	kRTTipSchedule.Reset();
}

void LC_Server_GameStory_Base::_setupActorFilter(GSRTPlayerFilter& kRTActorFilter)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(kRTActorFilter.eScheduleKey);
	if (NULL == pkRTBSchedule)
	{
		kRTActorFilter.eScheduleKey = GS_SCHEDULE_KEY_COMMON;
	}
	const GameLogic::LC_ActorPtrList& kActors = kRTActorFilter.GetCoreActors();
	if (!kRTActorFilter.kMapLgcIDs.empty())
	{
		GSLogicIDSet::iterator it = kRTActorFilter.kMapLgcIDs.begin();
		for (; it != kRTActorFilter.kMapLgcIDs.end(); ++it)
		{
			GameLogic::LC_MapLogicBase* pkMapLogic = pkWorldManager->GetMapLogic(*it);
			if (pkMapLogic)
			{
				kRTActorFilter.AddLogicMap(pkMapLogic);
			}
		}
	}
	else if (!kRTActorFilter.kMapResIDs.empty())
	{
		GSResIDSet::iterator it = kRTActorFilter.kMapResIDs.begin();
		for (; it != kRTActorFilter.kMapResIDs.end(); ++it)
		{
			const LC_ServerMapLogicPlaceVector* pkLogics = pkWorldManager->GetMapLogicByRes(*it);
			if (pkLogics)
			{
				LC_ServerMapLogicPlaceVector::const_iterator itLogic = pkLogics->begin();
				for (; itLogic != pkLogics->end(); ++itLogic)
				{
					LC_ServerMapLogic_Place* pkLogic = (LC_ServerMapLogic_Place*)(*itLogic);
					if (pkLogic/* && pkLogic->GetMapLogicType()!=MT_NEWBEE*/)
					{
						kRTActorFilter.AddLogicMap(pkLogic);
					}
				}
			}
		}
	}
	else
	{
		if (IsScheduleMapLogicKey(kRTActorFilter.eScheduleKey))
		{
			LC_ServerMapLogicBase* pkMapLogic = pkWorldManager->GetMapLogic(kRTActorFilter.eScheduleKey);
			if (NULL == pkMapLogic/* && pkMapLogic->GetMapLogicType()!=MT_NEWBEE*/)
			{
				return;
			}
			kRTActorFilter.AddLogicMap(pkMapLogic);
		}
		else
		{
			if (!kActors.empty())
			{
				GameLogic::LC_ActorPtrList::const_iterator cit = kActors.begin();
				for (; cit != kActors.end(); ++cit)
				{
					LC_ActorBase* pkActor = (*cit);
					if (pkActor)
					{
						LC_ServerMapLogic_Place* pkLogic = (LC_ServerMapLogic_Place*)(pkWorldManager->GetMapLogic(pkActor->GetMapLogicID()));
						if (pkLogic)
						{
							kRTActorFilter.AddLogicMap(pkLogic);
						}
					}
				}
			}
			else
			{
				GSResIDList& kMapResIDs = GetConfig()->m_kMapCtrlSetting.m_kMapResIDList;
				GSResIDList::iterator it = kMapResIDs.begin();
				for (; it != kMapResIDs.end(); ++it)
				{
					const LC_ServerMapLogicPlaceVector* pkLogics = pkWorldManager->GetMapLogicByRes(*it);
					if (pkLogics)
					{
						LC_ServerMapLogicPlaceVector::const_iterator itLogic = pkLogics->begin();
						for (; itLogic != pkLogics->end(); ++itLogic)
						{
							LC_ServerMapLogic_Place* pkLogic = (LC_ServerMapLogic_Place*)(*itLogic);
							if (pkLogic/* && pkLogic->GetMapLogicType()!=MT_NEWBEE*/)
							{
								kRTActorFilter.AddLogicMap(pkLogic);
							}
						}
					}
				}
			}
		}
	}
	{
		GameLogic::LC_ActorPtrList::const_iterator cit = kActors.begin();
		for (; cit != kActors.end(); ++cit)
		{
			LC_ActorBase* pkActor = (*cit);
			if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
			{
				LC_PlayerBase* pkPlayer = ((LC_PlayerBase*)pkActor);
				if (pkPlayer->IsExistGuild())
				{
					kRTActorFilter.kGuildIDs.insert(pkPlayer->GetGuildID());
				}
			}
		}
	}
}

void LC_Server_GameStory_Base::_addTips(GameStoryScheduleKey iKey, uint32_t uiBenchMark, const TipSetting& kTipSetting, GameLogic::LC_ActorBase* pkCoreActor, int32_t iID, GSRTTipSchedule::HandlerType* pkHandler)
{
	GSRTTipSchedule kRTTipSchedule;
	kRTTipSchedule.SetBenchmark(uiBenchMark);
	kRTTipSchedule.eScheduleKey = iKey;
	kRTTipSchedule.AddTip(kTipSetting, pkCoreActor, iID, pkHandler);
	m_kRTTipSchedules.push_back(kRTTipSchedule);
}

void LC_Server_GameStory_Base::_clearTips(GameStoryScheduleKey iKey)
{
	GSRTTipScheduleList::iterator it = m_kRTTipSchedules.begin();
	while (it != m_kRTTipSchedules.end())
	{
		if (it->eScheduleKey == iKey)
		{
			it->Reset();
			m_kRTTipSchedules.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void LC_Server_GameStory_Base::_clearTips(void)
{
	GSRTTipScheduleList::iterator it = m_kRTTipSchedules.begin();

	for (; it != m_kRTTipSchedules.end(); ++it)
	{
		it->Reset();
	}
	m_kRTTipSchedules.clear();
}

void LC_Server_GameStory_Base::SyncFactionScore(GameStoryScheduleKey iKey)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	if (IsScheduleMapLogicKey(iKey))
	{
		MG_GAMESTORY_FACTION_SCORE_SYNC syncMsg;
		for (int32_t i = 0; i < GS_FACTION_SCORE_MAX; ++i)
		{
			syncMsg.m_kFactionScores[i] = GetFactionScore(iKey, i);
		}
		LC_ActorPtrList kActors;
		_getRTScheduleActors(iKey, kActors, pkConfig);

		LC_ActorPtrList::iterator it = kActors.begin();
		for (; it != kActors.end(); ++it)
		{
			((LC_ServerPlayer*)*it)->SendMsgToClient(MGPT_GAMESTORY_FACTION_SCORE_SYNC, &syncMsg);
		}
	}
}

void LC_Server_GameStory_Base::CheckTriConditions(GameStoryScheduleKey iKey, int iParam1, int iParam2, object_id_type iParam3)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	BatchScheduleData& kBatchSchedule = pkConfig->m_kBatchSechedule;
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}
	if (kBatchSchedule.ConditionSize() <= 0)
	{
		return;
	}
	BatchIndices::iterator itIdx = kBatchSchedule.kConditionIndices.begin();
	for (; itIdx != kBatchSchedule.kConditionIndices.end(); ++itIdx)
	{
		BatchData* pkBatch = kBatchSchedule.GetBatchData(*itIdx);
		if (NULL == pkBatch)
		{
			continue;
		}
		bool bCondMatch = false;
		for (size_t i = 0; i < pkBatch->kTriCondOptions.size(); ++i)
		{
			if (_checkConditions(iKey, pkBatch->kTriCondOptions.at(i), iParam1, iParam2))
			{
				//只要有一组条件满足，则满足
				bCondMatch = true;
				break;
			}
		}
		if (!bCondMatch)
		{
			continue;
		}
		GSRTBatchDetail* pkTriDetail = pkRTBSchedule->GetBatchDetailByBatchIdx(pkBatch->iIdx);
		_triggerBatchCB_CF(TIMER_ID_NA, this, iKey, pkTriDetail->iIndex, -1, pkConfig, false, iParam3);
	}
}

const GSRTMonsterIndexCluster* LC_Server_GameStory_Base::GetGSRTMonsterData(int32_t monsterObjectID)
{
	GSRTMonsterRecordMap::iterator findIt = m_kRTMonsterRecord.find(monsterObjectID);
	if(findIt == m_kRTMonsterRecord.end())
	{
		return NULL;
	}
	return &findIt->second;
}

void LC_Server_GameStory_Base::GetNpcAndPlayerInfo(int32_t mapResID, mem::map<int32_t, int32_t>& monsterInfo, LC_PlayerIDPtrMap** pkPlayerIDPtrMap)
{
	if(NULL == pkPlayerIDPtrMap)
	{
		return;
	}

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	const LC_ServerMapLogicPlaceVector* pkLogicMaps = pkWorldManager->GetMapLogicByRes(mapResID);
	if(NULL==pkLogicMaps || pkLogicMaps->size() != 1)
	{
		return;
	}

	LC_ServerMapLogicPlaceVector::const_iterator itLogic = pkLogicMaps->begin();
	if(itLogic == pkLogicMaps->end())
	{
		return;
	}

	LC_ServerMapLogic_Place* pkPlace = *itLogic;
	if(NULL == pkPlace)
	{
		return;
	}

	//没有怪，不需要广播
	GameLogic::LC_ActorPtrList pkNPCList;
	pkPlace->GetAllMonster(pkNPCList);
	if(pkNPCList.empty())
	{
		return;
	}

	for(GameLogic::LC_ActorPtrList::iterator it=pkNPCList.begin(); it!=pkNPCList.end(); ++it)
	{
		LC_ActorBase* pkNpc = *it;
		if(NULL == pkNpc)
		{
			continue;
		}

		int32_t npcCharType = pkNpc->GetCharType();
		mem::map<int32_t, int32_t>::iterator findIt = monsterInfo.find(npcCharType);
		if(findIt != monsterInfo.end())
		{
			findIt->second += 1;
		}
		else
		{
			monsterInfo[npcCharType] = 1;
		}
	}

	*pkPlayerIDPtrMap = pkPlace->GetControllerPlayerMapPtr();
}
bool LC_Server_GameStory_Base::_checkConditions(GameStoryScheduleKey iKey, const Utility::UT_SIMDataList& kConditions, int iParam1, int iParam2)
{
	bool bCondMatch = true;
	Utility::UT_SIMDataList::const_iterator it = kConditions.begin();
	for (; it != kConditions.end(); ++it)
	{
		const Utility::UT_SIMDataInfo& kDataInfo = *it;
		switch (kDataInfo.ID())
		{
			case GST_COND_FACTION_SCORE:
				if (GetFactionScore(iKey, kDataInfo.ParamA()) < kDataInfo.ParamB())
				{
					return false;
					break;
				}
				break;
			case GST_COND_NPC_DEAD:
				{
					if (kDataInfo.ParamA() != iParam1)
					{
						return false;
					}
				}
				break;
			default:
				return false;
				break;
		}
	}
	return true;
}
