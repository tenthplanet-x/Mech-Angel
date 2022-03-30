#include "LC_GameStory_Manager.h"
#include "LC_ServerMap.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "LC_GameEvent.h"
#include "LC_EventDefine.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerGroup.h"
#include "CF_WorldMapList.h"
#include "CF_GameStory.h"
#include "LC_ServerMap.h"
#include "SC_ServerSkillInterface.h"

#include "MG_Game2DB.h"
#include "MG_Game2Alliance.h"

#include "GameServerApp.h"
#include <time.h>
#include "UT_Timer.h"
#include "UT_ServerHelper.h"

#include "LC_StoryLogic_Manager.h"
#include "LC_GameStory_WildBoss.h"
#include "GlobalSettings.h"
#include "LC_ServerGuildManager.h"
#include "CF_BuddyThemeBossType.h"

using namespace GameLogic;
using namespace GameStory;
using namespace Skill;
using namespace Protocol;
using namespace CSVFile;
using namespace Utility;

//---------------------------------------------
LC_GameStory_Manager::LC_GameStory_Manager()
{
}

//---------------------------------------------
LC_GameStory_Manager::~LC_GameStory_Manager(){}
//---------------------------------------------
void LC_GameStory_Manager::Init(CSVFile::CSVDataLoader* pkLoader, const CSVDataSourceList* pkCSVs)
{
	CF_GameStory* pkGameStory = CF_GameStory::GetSingletonPtr();
	if (!IS_VALID_RELATED_CSV(pkGameStory, pkCSVs))
	{
		return;
	}

	CF_GameStory::DataEntryMapExternIterator listIter = pkGameStory->GetIterator();
	while(listIter.HasMoreElements())
	{
		const CF_GameStory::DataEntry *pkEntry = listIter.PeekNextValuePtr();
		_addStoryRecord(pkEntry);
		listIter.MoveNext();
		if (NULL != pkEntry && pkEntry->_iRelateOther > GS_RELATE_BEGIN)
		{
			_addRelateStory(pkEntry->_iID, pkEntry->_iRelateOther, pkEntry->_iRelateDay, pkEntry->_kCronStr);
		}
	}

	GameStory::GameStoryConfigOperator* pkGSConfigLoader = pkLoader ? dynamic_cast<GameStory::GameStoryConfigOperator*>(pkLoader) : NULL;
	if (NULL == pkGSConfigLoader)
	{
		static GameStory::GameStoryConfigOperator stConfigLoader;
		stConfigLoader.Clear();
		pkGSConfigLoader = &stConfigLoader;
	}

	GameLogic::LC_IDSet kStoryIDs;
	LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
	StoryRecordMapIter itGRcord = m_kStoryRecordMap.begin();
	for(; itGRcord!=m_kStoryRecordMap.end(); ++itGRcord)
	{
		GameStoryRecord& kStoryRecord = itGRcord->second;
		int32_t nStoryId = kStoryRecord.m_iStoryID;
		if (pkGSConfigLoader->IsConfigLoaded(kStoryRecord.m_sScript) || LoadStoryConfig(kStoryRecord.m_sScript, pkLoader ? pkLoader->sVersion : ""))
		{
			(*pkGSConfigLoader)(GetStoryConfig(kStoryRecord.m_sScript));
			pkStoryLogicManager->GetLCStoryImpl(itGRcord->second);
			kStoryRecord.SetDirty(true);
			int32_t nReplevyId = m_kConfigMap[kStoryRecord.m_sScript].m_kCmnStatData.iID;
			if ( nReplevyId > 0 )
			{
				m_ReplevyMap.insert(std::make_pair(nReplevyId,nStoryId));
			}
		}
	}
	//_scheduleGameStory();
}
//----------------------------------------------
void LC_GameStory_Manager::UnInit()
{
	StoryRecordMapIter itGRcord;
	TRAV(m_kStoryRecordMap, itGRcord)
	{
		LC_GameStory_Manager::DeactiveStory(itGRcord->second.m_iStoryCancelId, itGRcord->second.m_iStoryID);
	}

	m_kStoryRecordMap.clear();

	GameStoryCommonManager::Unit();
}
//----------------------------------------------
void LC_GameStory_Manager::LoadRelateTime()
{
	//加载屠龙刀关联时间
	GameServerApp* pkApp = GetGameServerApp();
	if (NULL == pkApp)
	{
		return;
	}
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return;
	}
	int32_t ret = 0;
	std::string key = pkApp->GetRedisGroupPrefix() + CROSS_REALM_WAR_TIME;

	char buff[256] = {0};
	std::vector<std::string> fields;
	sprintf(buff, "pre_time_%d", pkApp->GetServerManager()->GetCurGroupId());
	fields.push_back(std::string(buff));
	sprintf(buff, "next_time_%d", pkApp->GetServerManager()->GetCurGroupId());
	fields.push_back(std::string(buff));

	time_t iPreTime = 0, iNextTime = 0;
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	redispp::MultiBulkEnumerator replies;
	replies = pkRedisConn->hmget(key, fields);
	std::string str;
	if (!str.empty() && replies.next(&str) )
	{
		iPreTime = boost::lexical_cast<time_t>(str);
	}
	str.clear();
	if (!str.empty() && replies.next(&str))
	{
		iNextTime = boost::lexical_cast<time_t>(str);
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
	OnCrossWarTimeChange(iPreTime, iNextTime);
}

void LC_GameStory_Manager::OnCrossWarTimeChange(time_t iPreTime, time_t iNextTime)
{
	GSRelateMap::iterator it = m_kRelateMap.begin();
	for (; it != m_kRelateMap.end(); ++it)
	{
		GameStoryRelate& kRelate = it->second;
		if (kRelate.m_iType != GS_RELATE_CROSS_REALM)
		{
			continue;
		}
		time_t iRealTime = 0;
		if (iPreTime > 0)
		{
			kRelate.m_iFirstTime = LC_Helper::GetDayEndTimestamp((uint32_t)iPreTime, 0) + 
				kRelate.m_iRelateDay * ONE_DAY_SECONDS;
		}
		if (iNextTime > 0)
		{
			kRelate.m_iSecondTime = LC_Helper::GetDayEndTimestamp((uint32_t)iNextTime, 0) +
				kRelate.m_iRelateDay * ONE_DAY_SECONDS;
		}
	}
}

time_t LC_GameStory_Manager::GetRelateTime(int32_t iStoryID) const
{
	GSRelateMap::const_iterator cit = m_kRelateMap.begin();
	if (cit == m_kRelateMap.end())
	{
		return 0;
	}
	const GameStoryRelate& kRelate = cit->second;
	const UT_CronData& kCronData = kRelate.m_kCronData;
	time_t iCurTime = time(NULL);
	time_t iRealTime = 0;
	if (kRelate.m_iFirstTime > iCurTime)
	{
		return kCronData.GetNextTime(kRelate.m_iFirstTime);
	}
	if (iCurTime - kRelate.m_iFirstTime < ONE_DAY_SECONDS)
	{
		iRealTime = kCronData.GetNextTime(iCurTime);
	}
	if (iRealTime >= iCurTime && LC_Helper::CheckIsSameDay(iRealTime, iCurTime))
	{
		return iRealTime;
	}
	if (iCurTime < kRelate.m_iSecondTime)
	{
		return kCronData.GetNextTime(kRelate.m_iSecondTime);
	}
	return kCronData.GetNextTime(iCurTime);
}

void LC_GameStory_Manager::SetRealDay(int32_t iStoryID, int32_t iDay)
{
	GSRelateMap::iterator it = m_kRelateMap.begin();
	if (it == m_kRelateMap.end())
	{
		return;
	}
	uint32_t iCurZeroTime = LC_Helper::GetDayEndTimestamp(GET_CURRENT_TIMESTAMP_IN_SECONDS(), 0);
	GameStoryRelate& kRelate = it->second;
	kRelate.m_iFirstTime = (time_t)iCurZeroTime + iDay * (int32_t)ONE_DAY_SECONDS;
}

void LC_GameStory_Manager::OnGameServerLoaded(void)
{
	LoadRelateTime();
	InitAllRecorderGroupSchedData();
	_scheduleGameStory();
}

void LC_GameStory_Manager::OnSocietyServerConnected()
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "OnSocietyServerConnected ");

	Protocol::MG_Report_SS_Alliance_Boss_Reset kResetMsg;
	kResetMsg.m_nServerID = GetGameServerID();
	SendMsgToSociety(MGPT_SS_FALLIANCE_BOSS_RFINFO_RESET, &kResetMsg);

	MG_SS_Update_Alliance_Boss_Info_To_Login msg;
	LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	StoryRecordMapIter itGRcord = m_kStoryRecordMap.begin();
	for(; itGRcord!=m_kStoryRecordMap.end(); ++itGRcord)
	{
		GameStoryRecord& kStoryRecord = itGRcord->second;
		LC_Server_GameStory_Base* pkStory = pkLogicMgr->GetStoryLogic(kStoryRecord.m_iStoryLogicId);
		if (pkStory && pkStory->IsActive())
		{
			GameStoryConfig* pkConfig = pkStory->GetConfig();
			if(NULL!=pkConfig && pkConfig->IsGlobalNodeUpdateBossInfo() && true!=GetGameServerApp()->IsNodeType_Global())
			{
				msg.m_nStoryID.push_back(kStoryRecord.m_iStoryID);
			}
			else
			{
				pkStory->ReportMosterStateInfo(pkStory->_peekRTBatchScheduleKey(), TIMER_ID_NA);
			}			
		}
	}

	if(msg.m_nStoryID.size() > 0)
	{
		//发给login
		SendMsgToLogin(MGPT_SS_UPDATE_ALLIANCE_BOSS_INFO_NOTICE, &msg);		
	}
}

void LC_GameStory_Manager::_scheduleGameStory(void)
{
	uint32_t uiServerStartTime = (uint32_t)GetGameServerApp()->GetServerManager()->GetServerStartTime();
	uint32_t uiServerConflateTime = (uint32_t)GetGameServerApp()->GetServerManager()->GetServerConflateTime();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_scheduleGameStory all! starttimestamp[%u], conflatetimestamp[%u]", uiServerStartTime, uiServerConflateTime);
	LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (NULL==pkLogicMgr || uiServerStartTime <= 0 /*|| NULL==pkTimerMgr*/)
	{
		return;
	}

	StoryRecordMapIter itGRcord = m_kStoryRecordMap.begin();
	for(; itGRcord!=m_kStoryRecordMap.end(); ++itGRcord)
	{
		GameStoryRecord& kStoryRecord = itGRcord->second;
		LC_Server_GameStory_Base* pkStory = pkLogicMgr->GetStoryLogic(kStoryRecord.m_iStoryLogicId);
		if (pkStory)
		{
			pkStory->PostLoaded();
			if (kStoryRecord.GetDirty())
			{
				if (kStoryRecord.m_bActive)
				{
					DeactiveStory(TIMER_ID_NA, itGRcord->first);
				}
				if(IsTimerIDValid(kStoryRecord.m_iStoryScheduleId))
				{
					pkTimerMgr->CancelTimer(kStoryRecord.m_iStoryScheduleId);
					kStoryRecord.m_iStoryScheduleId = TIMER_ID_NA;
				}

				int storyID = itGRcord->first;
				kStoryRecord.SetUp(uiServerStartTime, uiServerConflateTime);
				kStoryRecord.SetDirty(false);

				GameStoryRecord* pkRecord = pkStory->GetCoreRecord();
				if(NULL!=pkRecord && pkRecord->m_nLoopGroupId > 0)
				{
					continue;
				}
				_scheduleGameStory(itGRcord->first);
			}
		}
	}

	ScheduleLoopGameStory();
}

void LC_GameStory_Manager::_scheduleGameStory(int32_t iStoryID, Utility::timer_id_type timerID, bool bLoop )
{
	GameServerManager* pkAppMgr = GetGameServerApp()->GetServerManager();
	if (!pkAppMgr->isLoaded())
	{
		return;
	}

	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL==pkStory)
	{
		return;
	}

	UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
	if(NULL==pkRecord || pkRecord->m_bActive)
	{
		return;
	}

	GameStoryRecord* pkPreRecord = GetStoryRecord(pkRecord->m_preStoryID);

	if (IsTimerIDValid(pkRecord->m_iStoryScheduleId))
	{
		if (timerID!=pkRecord->m_iStoryScheduleId)
		{
			return;
		}
		pkTimerMgr->CancelTimer(pkRecord->m_iStoryScheduleId);
	}
	LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;

	uint64_t curtimestamp = GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
	GameStoryConfig* pkConfig = pkStory->GetConfig();
	uint64_t uiCoolOffTime = (pkRecord->m_nLoopGroupId > 0) ? pkRecord->m_uiLoopCoolOffTime : pkRecord->m_uiCoolOffTime;
	uint64_t uiValidTime = pkRecord->m_uiValidTime;
	if(curtimestamp>= uiValidTime)
	{
		if(iStoryID==GameStoryIDType_BestGuild)
		{
			uint32_t bestGuildRankClearTime = UT_ServerHelper::GetBestGuildRankClearTime();
			if(0!=bestGuildRankClearTime && curtimestamp/1000<bestGuildRankClearTime)
			{
				if(true==GetGameServerApp()->IsNodeType_Global())
				{
					UT_ServerHelper::LoadWatchTowerFromRedis_finish();
				}
				else
				{
					UT_ServerHelper::UpdateWatchTowerFromRedis();
				}
			}
		}

		//设置了开服有效时间 直接跳过
		return;
	}
	else if (curtimestamp < uiCoolOffTime)
	{
		pkRecord->m_iStoryScheduleId = pkTimerMgr->DelayCall(uiCoolOffTime-curtimestamp, LC_GameStory_Manager::_scheduleGameStoryCB, (void (*)(int, int))NULL, iStoryID);
		pkRecord->ResolveTimeInfo(uiValidTime, pkPreRecord);
	}
	else
	{
		pkRecord->m_iStoryScheduleId = pkTimerMgr->SetCron(pkRecord->m_sCronInfo, pkRecord->TimeLen(), LC_GameStory_Manager::ActiveStory, iStoryID, false, false);
		pkRecord->ResolveTimeInfo(uiValidTime, pkPreRecord);
		if (IsTimerIDValid(pkRecord->m_iStoryScheduleId))
		{
			pkStory->_addTips(GS_SCHEDULE_KEY_NA, (int32_t)(pkRecord->m_iStartTime/1000), pkConfig->m_kBeginTip, NULL, 0);
		}
		else
		{
			 if(iStoryID == GameStoryIDType_BestGuild && true==GetGameServerApp()->IsNodeType_Global())
			 {
				 bool vaild = UT_ServerHelper::CheckWatchTowerInfoValid(pkRecord->m_iTimeLen, pkRecord->m_iEndTime);
				 UT_ServerHelper::LoadWatchTowerFromRedis(vaild);
				 UT_ServerHelper::LoadBossDamageInfoFromRedis(vaild);
				 UT_ServerHelper::UpdateWatchTowerTime(vaild, pkRecord->m_iTimeLen, pkRecord->m_iEndTime);
			 }
			ActiveStory(TIMER_ID_NA, iStoryID, bLoop);
		}
	}
}

void LC_GameStory_Manager::Update_Alliance_Boss_Info(std::vector<int>& storyVec)
{
	GameStory::LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	for(int i=0; i<storyVec.size(); ++i)
	{
		LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(storyVec[i]);
		if(NULL==pkStory || !pkStory->IsActive())
		{
			return;
		}

		GameStory::GameStoryConfig* pkConfig = pkStory->GetConfig();
		if(NULL!=pkConfig && pkConfig->IsGlobalNodeUpdateBossInfo())
		{
			pkStory->ReportMosterStateInfo(pkStory->_peekRTBatchScheduleKey(), TIMER_ID_NA);
		}
	}
}

void LC_GameStory_Manager::ScheduleGameStory(int iStoryID, const StringType& cronStr)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory)
	{
		return;
	}

	GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
	if(NULL == pkRecord || pkRecord->m_sCronInfo == cronStr)
	{
		return;
	}

	UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (pkRecord->m_bActive)
	{
		DeactiveStory(TIMER_ID_NA, iStoryID);
	}
	if(IsTimerIDValid(pkRecord->m_iStoryScheduleId))
	{
		pkTimerMgr->CancelTimer(pkRecord->m_iStoryScheduleId);
		pkRecord->m_iStoryScheduleId = TIMER_ID_NA;
	}
	pkStory->_clearTips();
	pkRecord->m_sCronInfo = cronStr;
	_scheduleGameStory(iStoryID);
}
///////////////////////////////////////////////////////////////////////////////////////
class ResetStoryOP :public GameLogic::LC_ActorOperator
{
public:
	ResetStoryOP(LC_GameStory_Manager *gsMgr, int32_t storyID, int64_t timestamp)
		:pkGSMgr(gsMgr)
		,iStoryID(storyID)
		,curTimeStamp(timestamp)
	{}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkPlayer)
	{
		if (pkPlayer && IS_PLAYER_CLASS_ID(pkPlayer->GetID()))
		{
			switch(iStoryID)
			{
			case GameStoryIDType_Monkey:
				((LC_PlayerBase*)pkPlayer)->ResetYiJiTuXiRewardRecord();
				break;
			case GameStoryIDType_Chest:
				((LC_PlayerBase*)pkPlayer)->ResetYiJiXunBaoRewardRecord();
				break;
			case GameStoryIDType_PetIsland:
				((LC_PlayerBase*)pkPlayer)->ResetPetIslandsRewardRecord();
				break;
			}

			pkGSMgr->_resetStoryData((LC_PlayerBase*)pkPlayer, iStoryID, curTimeStamp);
			return true;
		}
		return false;
	}
private:
	LC_GameStory_Manager *pkGSMgr;
	int32_t iStoryID;
	int64_t curTimeStamp;
};
/////////////////////////////////////////
class SendYiJiStageRewardOP :public GameLogic::LC_ActorOperator
{
public:
	SendYiJiStageRewardOP(int32_t storyID):iStoryID(storyID)	{}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkPlayer)
	{
		if (pkPlayer && IS_PLAYER_CLASS_ID(pkPlayer->GetID()))
		{
			switch(iStoryID)
			{
			case GameStoryIDType_Monkey:
				((LC_PlayerBase*)pkPlayer)->SendYiJiTuXiReward();
				break;
			case GameStoryIDType_Chest:
				((LC_PlayerBase*)pkPlayer)->SendYiJiXunBaoReward();
				break;
			case GameStoryIDType_PetIsland:
				((LC_PlayerBase*)pkPlayer)->SendPetIslandsReward();
				break;
			}
			return true;
		}
		return false;
	}
private:
	int32_t iStoryID;
};
///////////////////////////////////////////////////////////////////////////////////////
void LC_GameStory_Manager::ActiveStory(timer_id_type timerID, int storyID, bool bLoop, bool bRepeat)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ActiveStory storyID:%d, GameServerID:%d, EntryGameServerID:%d\n", storyID, GetGameServerID(), GetGameServerEntryID());
	UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	
	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	StoryRecordMapIter itGRcord = pkGSMgr->m_kStoryRecordMap.find(storyID);
	if(itGRcord == pkGSMgr->m_kStoryRecordMap.end())
	{
		return;
	}

	GameStoryRecord* pkRecord = &itGRcord->second;
	if ( !bLoop && pkRecord->m_nLoopGroupId > 0 && !pkGSMgr->IsGroupBeginMap(pkRecord->m_nLoopGroupId, pkRecord->m_iStoryID) )
		return;

	char* szNameSpace = NULL;
	switch(storyID)
	{
	case GameStoryIDType_YiZhongQinXi_1:
	case GameStoryIDType_YiZhongQinXi_2:
	case GameStoryIDType_YiZhongQinXi_3:
	case GameStoryIDType_YiZhongQinXi_4:
	case GameStoryIDType_YiZhongQinXi_5:
	case GameStoryIDType_HuanJingShiLian:
	case GameStoryIDType_HuanJingShiLian1:
	case GameStoryIDType_HuanJingShiLian2:
		{
			szNameSpace = UT_REDIS_KEY_STORT_END_TIME;
		}
		break;
	case GameStoryIDType_Answer:
		{
			if(GetGameServerID() == GetGameServerEntryID())
			{
				szNameSpace = UT_REDIS_KEY_ANSWER_STORY_END_TIME;
			}
		}		
		break;
	case GameStoryIDType_Monkey:
		{
			if(GetGameServerID() == GetGameServerEntryID())
			{
				szNameSpace = UT_REDIS_KEY_YIJITUXI_STORY_END_TIME;
			}
		}
		break;
	case GameStoryIDType_BestGuild1:
	case GameStoryIDType_BestGuild2:
	case GameStoryIDType_BestGuild3:
	case GameStoryIDType_BestGuild4:
		{
			szNameSpace = UT_REDIS_KEY_BESTGUILD_ACTIVITY_ENDTIME;
		}
		break;
	case GameStoryIDType_YiZhongJiangLin:
	case GameStoryIDType_YiZhongJiangLin1:
	case GameStoryIDType_YiZhongJiangLin2:
	case GameStoryIDType_YiZhongJiangLin3:
	case GameStoryIDType_YiZhongJiangLin4:
		{
			if(GetGameServerApp()->IsNodeType_Global())
			{
				szNameSpace = UT_REDIS_KEY_GLOBAL_STORT_END_TIME;
			}
		}
		break;
	}

	GameStoryRecord* pkPreRecord = pkGSMgr->GetStoryRecord(pkRecord->m_preStoryID);
	if(!pkRecord->m_bActive || bRepeat)
	{
		int64_t curTimeStamp = pkTimerMgr->CurrentTimestampInMiliSeconds();
		uint64_t uiValidTime = pkRecord->m_uiValidTime;
		pkRecord->ResolveTimeInfo(uiValidTime, pkPreRecord);
		int64_t delay = pkRecord->GetCancelDelay(curTimeStamp);			
		if (delay >= 0 && _checkRelateCondition(storyID))
		{
			LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
			pkRecord->m_bActive = TRUE;
			if(pkStoryLogicManager->ActivateStory(*pkRecord, bRepeat))
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ActiveStory success!!! storyID:%d, GameServerID:%d, EntryGameServerID:%d\n", storyID, GetGameServerID(), GetGameServerEntryID());
				if(IsTimerIDValid(pkRecord->m_iStoryCancelId) && !bRepeat)
				{
					pkTimerMgr->CancelTimer(pkRecord->m_iStoryCancelId);
				}
				pkRecord->m_iStoryCancelId = pkTimerMgr->DelayCall(delay, LC_GameStory_Manager::DeactiveStory, (void (*)(int, int))NULL, storyID);
				LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
				if(!bRepeat)
				{
					ResetStoryOP kResetOP(pkGSMgr, pkRecord->m_iStoryID, curTimeStamp);
					pkWorldManager->ForAllPlayer(&kResetOP);
				}

				int32_t currentEndTime = pkRecord->m_iEndTime/1000;
				int32_t endTime = 0;
				if(NULL != szNameSpace)
				{
					mem::vector<uint64_t> input;
					input.push_back(storyID);
					bool flag = UT_ServerHelper::RedisGetData_Int(szNameSpace, input, endTime);
					if(!flag)
					{
						UT_ServerHelper::RedisCreateData(szNameSpace, input);
					}
					UT_ServerHelper::RedisSetData_Int(szNameSpace, input, currentEndTime);

					if(currentEndTime!=endTime && GameStoryIDType_YiZhongJiangLin==storyID)
					{
						//这次活动的结束时间和上次活动的结束时间不一致，说明不是同一次活动，需要补发上次活动的奖励
						UT_ServerHelper::SettleYiZhongJiangLinReward();
					}
				}

				switch(storyID)
				{
				case GameStoryIDType_YiZhongJiangLin1:
				case GameStoryIDType_YiZhongJiangLin2:
				case GameStoryIDType_YiZhongJiangLin3:
				case GameStoryIDType_YiZhongJiangLin4:
					{
						//异种降临，global节点写数据
						if(!GetGameServerApp()->IsNodeType_Global())
						{
							break;
						}

						if(currentEndTime != endTime)
						{
							//不是同一次活动，清榜
							mem::vector<uint64_t> params;
							params.push_back(storyID);

							mem::vector<uint64_t> allPlayers;
							UT_ServerHelper::RedisRank_ReadSimpleDamageRange(UT_REDIS_KEY_DAMAGE_RANK, params, allPlayers);
							for(uint32_t i=0; i<allPlayers.size(); ++i)
							{
								mem::vector<uint64_t> keys;
								keys.push_back(storyID);
								keys.push_back(allPlayers[i]);
								UT_ServerHelper::RedisClearString(UT_REDIS_KEY_COMBAT_INFO, keys);
							}

							UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_DAMAGE_RANK, params);
							UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_GUILD_DAMAGE_INFO, params);
							UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_GUILD_DAMAGE_RANK, params);
							UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_BATTLE_INFO, params);
						}
						
						{
							char* redisNamespace = UT_REDIS_KEY_YIZHONGJIANGLIN_STORY;
							int32_t settleStoryID = 0;
							mem::vector<uint64_t> key;
							key.push_back(GameStoryIDType_YiZhongJiangLin);
							bool flag = UT_ServerHelper::RedisGetData_Int(redisNamespace, key, settleStoryID);
							if(!flag)
							{
								UT_ServerHelper::RedisCreateData(redisNamespace, key);
							}
							UT_ServerHelper::RedisSetData_Int(redisNamespace, key, storyID);
						}						
					}
					break;
				case GameStoryIDType_YiZhongQinXi_1:
				case GameStoryIDType_YiZhongQinXi_2:
				case GameStoryIDType_YiZhongQinXi_3:
				case GameStoryIDType_YiZhongQinXi_4:
				case GameStoryIDType_YiZhongQinXi_5:
					{
						UT_ServerHelper::SetYiZhongQinXiGameEndTime(currentEndTime);
						if(currentEndTime != endTime)
						{
							//不是同一次活动，清榜
							mem::vector<uint64_t> params;
							params.push_back(storyID);

							UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_BATTLE_INFO, params);
							UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_DAMAGE_RANK, params);
						}
					}
					break;
				case GameStoryIDType_HuanJingShiLian:
				case GameStoryIDType_HuanJingShiLian1:
				case GameStoryIDType_HuanJingShiLian2:
					{
						UT_ServerHelper::SetHuanJingEndTime(storyID, currentEndTime);
					}
					break;
				case GameStoryIDType_Answer:
					{
						if(GetGameServerID() != GetGameServerEntryID())
						{
							break;
						}

						int32_t size = 0;
						if(endTime != currentEndTime)
						{
							//活动开始的时候，重新随机题目，存在redis上
							size = UT_ServerHelper::RandomTestDatabase();
							UT_ServerHelper::ClearAnswerRedisRank();
							UT_ServerHelper::SetAnswerCloseState(0);
							GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "answer-3 size:%d\n", size);
						}

						OnSyncTestDatabase info;
						static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
						pkWorldMgr->ForAllPlayer(&info);

						mem::vector<uint32_t> checkTime;
						checkTime.reserve(size);
						for(int index=0; index<size; ++index)
						{
							uint32_t checkAwardTime = UT_ServerHelper::GetCheckAwardTime(index);
							checkTime.push_back(checkAwardTime);
						}

						UT_ServerHelper::SetCheckAwardTimeToRedis(checkTime);
						UT_ServerHelper::SetAnswerActivityEndTime(currentEndTime);
					}
					break;
				case GameStoryIDType_GUILDBONFIRPARTY:
					{
						if(GetGameServerID() != GetGameServerEntryID())
						{
							break;
						}

						uint32_t time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
						int currentEndTime = pkRecord->m_iEndTime/1000;
						mem::vector<uint64_t> input;
						input.push_back(storyID);

						int size = 0;
						int32_t endTime = 0;						
						if(!UT_ServerHelper::RedisGetData_Int(UT_REDIS_BONFIREPARTY_ENDTIME, input, endTime))
						{
							UT_ServerHelper::RedisCreateData(UT_REDIS_BONFIREPARTY_ENDTIME, input);
						}
						if(endTime != currentEndTime)
						{
							UT_ServerHelper::ClearGuildMapStateMutableParameter();
							UT_ServerHelper::RedisSetData_Int(UT_REDIS_BONFIREPARTY_ENDTIME, input, currentEndTime);
						}
					}
					break;
				case GameStoryIDType_BestGuild1:
				case GameStoryIDType_BestGuild2:
				case GameStoryIDType_BestGuild3:
				case GameStoryIDType_BestGuild4:
					{
						//活动开始时，给在线的玩家派发公会战的任务
						//公会战的任务，所有人都可以接取
						UT_ServerHelper::UpdateBestGuildActivityEndTime(storyID, endTime);
						UT_ServerHelper::GameServerAppToSendBestGuildReward2(false);
					}
					break;
				case GameStoryIDType_BJEscort:
					{
						LC_Server_GameStory_Base* pkStory = pkStoryLogicManager->GetStoryLogic(pkRecord->m_iStoryLogicId);
						if(NULL != pkStory)
						{
							AssignEscortTask info(pkStory, GetGameServerID());
							static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
							pkWorldMgr->ForAllPlayer(&info);
						}						
					}
					break;
				}

				GSResIDSet kStoryIDs;
				kStoryIDs.insert(storyID);
				pkGSMgr->_broadcastAllGameStoryContentToClient(&kStoryIDs);
				LC_Server_GameStory_Base* pkStory = pkStoryLogicManager->GetStoryLogic(pkRecord->m_iStoryLogicId);
				if (pkStory)
				{
					GameStoryConfig* pkConfig = pkStory->GetConfig();
					pkStory->_addTips(GS_SCHEDULE_KEY_NA, int32_t(pkRecord->m_iEndTime/1000), pkConfig->m_kEndTip, NULL, 0);
				}
			}
		}			
	}
	else
	{
		if(IsTimerIDValid(timerID))
		{
			pkTimerMgr->CancelTimer(timerID);
		}
	}
	BroadCastQz(storyID);
}

void LC_GameStory_Manager::AyncDeactiveStory(timer_id_type storyID)
{
	UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GameStoryRecord* pkRecord = GetStoryRecord(storyID);
	if(NULL == pkRecord || !pkRecord->m_bActive)
	{
		return;
	}

	if(IsTimerIDValid(pkRecord->m_iStoryCancelId))
	{
		pkTimerMgr->CancelTimer(pkRecord->m_iStoryCancelId);
	}
	pkRecord->m_iStoryCancelId = pkTimerMgr->DelayCall(1, LC_GameStory_Manager::DeactiveStory, (void (*)(int, int))NULL, storyID);
}

void LC_GameStory_Manager::DeactiveStory(Utility::timer_id_type timerID, int storyID)
{
	UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	StoryRecordMapIter itGRcord = pkGSMgr->m_kStoryRecordMap.find(storyID);
	if(itGRcord == pkGSMgr->m_kStoryRecordMap.end() || !itGRcord->second.m_bActive)
	{ 
		return;
	}

	GameStoryRecord* pkRecord = &itGRcord->second;
	GameStoryRecord* pkPreRecord = pkGSMgr->GetStoryRecord(pkRecord->m_preStoryID);

	//活动结束后，要把之前的remainTime设置为活动的整个有效期
	if (pkRecord->m_nLoopGroupId > 0 )
	{
		pkRecord->m_nLoopRemainTime = pkRecord->m_nLoopTimeLen * 60;
	}
	uint64_t uiValidTime = pkRecord->m_uiValidTime;
	pkRecord->ResolveTimeInfo(uiValidTime, pkPreRecord);
	pkRecord->m_bActive = FALSE;

	LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
	pkStoryLogicManager->DeactiveStory(pkRecord->m_iStoryLogicId);

	switch(storyID)
	{
	case GameStoryIDType_Monkey:
	case GameStoryIDType_Chest:
	case GameStoryIDType_PetIsland:
		{
			//遗迹突袭 遗迹寻宝 玩具岛阶段奖励
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			SendYiJiStageRewardOP kResetOP(pkRecord->m_iStoryID);
			pkWorldManager->ForAllPlayer(&kResetOP);
			
			if(GameStoryIDType_Monkey == storyID)
			{
				//结算遗迹突袭
				UT_ServerHelper::SettleYiJiTuXi();
			}
		}
		break;
	case GameStoryIDType_Answer:
		{
			//结算答题活动排名
			UT_ServerHelper::SettleAnswer();
		}
		break;
	case GameStoryIDType_YiZhongQinXi_1:
	case GameStoryIDType_YiZhongQinXi_2:
	case GameStoryIDType_YiZhongQinXi_3:
	case GameStoryIDType_YiZhongQinXi_4:
	case GameStoryIDType_YiZhongQinXi_5:
		{
			UT_ServerHelper::SettleYiZhongQinXi(storyID, false);
		}		
		break;
	case GameStoryIDType_YiZhongJiangLin:
		{
			UT_ServerHelper::SettleYiZhongJiangLinReward();
		}
		break;
	}

	if(IsTimerIDValid(pkRecord->m_iStoryCancelId))
	{
		pkTimerMgr->CancelTimer(pkRecord->m_iStoryCancelId);
		pkRecord->m_iStoryCancelId = TIMER_ID_NA;
	}
	
	GSResIDSet kStoryIDs;
	kStoryIDs.insert(storyID);
	pkGSMgr->_broadcastAllGameStoryContentToClient(&kStoryIDs);
}

GameStoryRecord* LC_GameStory_Manager::GetStoryRecord(int iStoryID)
{
	StoryRecordMap::iterator citGRcord = m_kStoryRecordMap.find(iStoryID);
	if(citGRcord != m_kStoryRecordMap.end())
	{
		return &citGRcord->second;
	}
	return NULL;
}

void LC_GameStory_Manager::CheckReset(GameLogic::LC_PlayerBase *pkPlayer)
{
	int64_t timestamp = GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
	StoryRecordMapIter itRecord = m_kStoryRecordMap.begin();
	for (; itRecord!=m_kStoryRecordMap.end(); ++itRecord)
	{
		GameStoryRecord &kStoryRecord = itRecord->second;
		_resetStoryData(pkPlayer, kStoryRecord.m_iStoryID, timestamp);
	}
}

bool LC_GameStory_Manager::IsFirstStoryIdInGroup( int32_t nGroupId, int32_t nStoryId )
{
	StoryGroup::iterator it = m_LoopStoryGroup.find( nGroupId );
	if ( it ==  m_LoopStoryGroup.end() )
		return false;

	StoryVector& vecData = it->second;
	if ( vecData[0] == nStoryId )
		return true;

	return false;
}

bool LC_GameStory_Manager::StoryIsActive(int32_t storyID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(storyID);
	return pkStory==NULL ? false : pkStory->IsActive();
}

uint32_t LC_GameStory_Manager::GetStoryStartTime(int32_t storyID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(storyID);
	return pkStory==NULL ? 0 : pkStory->GetStartTime();
}

int32_t LC_GameStory_Manager::GetStoryEndTime(int32_t storyID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(storyID);
	return pkStory==NULL ? 0 : pkStory->GetEndTime();
}

int32_t LC_GameStory_Manager::GetKillSpecialMonsterCount(LC_ServerPlayer *pkPlayer, int32_t storyID)
{
	int32_t num = 0;
	GameStoryRecord* pkRecord = GetStoryRecord(storyID);
	if(NULL == pkRecord)
	{
		return num;
	}

	GameStoryConfig* pkConfig = pkRecord->GetConfig();
	GameStoryEntry *pkStoryEntry = pkPlayer->GetGameStoryAsset().GetGameStoryEntry(storyID);
	if(pkConfig && pkStoryEntry)
	{
		if (pkConfig->m_kCmnStatData.IsValid())
		{
			LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();
			num = kCommonStatAsset.GetKillSpecialNpcCount(pkConfig->m_kCmnStatData.iID);
		}
	}

	return num;
}

void LC_GameStory_Manager::ScheduleLoopGameStory()
{
	for(StoryRecordBeginMap::iterator it=m_kStoryGroupBegin.begin(); it!=m_kStoryGroupBegin.end(); ++it)
	{
		int groupID = it->first;
		int startStoryID = it->second;
		int nextStoryID = startStoryID;

		do
		{
			_scheduleGameStory(nextStoryID);
			nextStoryID = GetNextStoryIdInGroup(groupID, nextStoryID);
		} while (nextStoryID != startStoryID);
	}
}

int32_t LC_GameStory_Manager::GetNextStoryIdInGroup( int32_t nGroupId, int32_t nStoryId )
{
	StoryGroup::iterator it = m_LoopStoryGroup.find( nGroupId );
	if ( it ==  m_LoopStoryGroup.end() )
		return -1;

	StoryVector& vecData  = it->second;
	for ( int i = 0; i < vecData.size(); i++ )
	{
		if ( vecData[i] == nStoryId)
		{
			if ( i +1 == vecData.size())
				return vecData[0];
			else
				return vecData[ i+1 ];
		}
	}

	return -1;
}

void LC_GameStory_Manager::TryAddRecordGroup(int32_t nGroupId, int32_t nStoryId)
{
	if ( nGroupId <= 0 )
		return;

	StoryGroup::iterator it = m_LoopStoryGroup.find( nGroupId );
	if ( it == m_LoopStoryGroup.end() )
	{
		StoryVector vec;
		vec.push_back(nStoryId);
		m_LoopStoryGroup.insert( std::make_pair(nGroupId, vec) );
	}
	else
	{
		it->second.push_back( nStoryId );
	}
}

bool LC_GameStory_Manager::IsGroupBeginMap( int32_t nGroupId, int32_t nStoryId )
{
	StoryRecordBeginMap::iterator it = m_kStoryGroupBegin.find(nGroupId);
	if ( it == m_kStoryGroupBegin.end() )
		return false;

	if ( it->second != nStoryId )
		return false;

	return true;
}

void LC_GameStory_Manager::InitAllRecorderGroupSchedData()
{
	StoryGroup::iterator it = m_LoopStoryGroup.begin();
	for ( ; it != m_LoopStoryGroup.end(); ++it )
	{
		StoryVector& data = it->second;
		InitSingleRecorderGroupSchedData( it->first, data );
	}
}

void LC_GameStory_Manager::InitSingleRecorderGroupSchedData(int32_t nGroupId, StoryVector& data)
{
	int nSize = data.size();
	if ( nSize <= 0)
		return;

	int32_t nStarCoolTime = 0;

	std::vector<uint32_t> loopLenInfo;
	loopLenInfo.reserve(data.size());
	for(uint32_t i=0; i<data.size(); ++i)
	{
		const GameStoryRecord* pkRecord = GetStoryRecord(data[i]);
		if(NULL != pkRecord)
		{
			nStarCoolTime = pkRecord->m_nLoopStartCoolTime;
			loopLenInfo.push_back(pkRecord->m_nLoopTimeLen*60);
		}
	}

	GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	uint32_t nRemain = 0;
	uint32_t nStartTime = 0;
	uint32_t nIndex = 0;

	bool tag = pkServerMgr->GetServerStartTimeAbsoluteIntervalLoopInfo(loopLenInfo, nStarCoolTime, nStartTime, nRemain, nIndex);
	if(!tag)
	{
		return;
	}

	GameStoryRecord* pkFindRecord = GetStoryRecord(data[nIndex]);
	if(NULL != pkFindRecord)
	{
		m_kStoryGroupBegin[nGroupId] = data[ nIndex ];
		pkFindRecord->m_nLoopRemainTime = nRemain;
		pkFindRecord->m_uiLoopCoolOffTime = (uint64_t)nStartTime*ONE_MILISECOND_MICRO;
	}
}

bool LC_GameStory_Manager::_checkReset(GameStoryEntry *pkStoryEntry, int64_t timestamp)
{
	if (NULL == pkStoryEntry)
	{
		return false;
	}

	GameStoryRecord* pkRecord = GetStoryRecord(pkStoryEntry->GetStoryID());
	if(NULL == pkRecord)
	{
		return false;
	}

	return pkStoryEntry->CheckReset(timestamp, pkRecord->IsTimestampIn(pkStoryEntry->GetResetTime()));
}

bool LC_GameStory_Manager::_checkRelateCondition(int32_t iStoryID)
{
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	GSRelateMap::const_iterator cit = pkGSMgr->m_kRelateMap.find(iStoryID);
	if (cit == pkGSMgr->m_kRelateMap.end())
	{
		return true;
	}
	if (!LC_Helper::CheckIsSameDay(timestamp, pkGSMgr->GetRelateTime(iStoryID)))
	{
		return false;
	}
	const GameStoryRelate& kRelate = cit->second;
	switch (kRelate.m_iType)
	{
		case GS_RELATE_CROSS_REALM:
		{
			return UT_ServerHelper::GetFactionDefServer() != INVALID_SERVER_ID;
		}
		default:
			return true;
	}
	return true;
}

bool LC_GameStory_Manager::GetStatReward(int iStoryID, unique_id_type charid, GSCommonReward& kReward)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL == pkStory)
	{
		return false;
	}

	pkStory->GetCurrentStatReward(charid, kReward);
	return true;
}

void LC_GameStory_Manager::ClearAreaKeyMonster(int32_t iStoryID, GameStoryScheduleKey iKey, int32_t iAreaIndex)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL == pkStory)
	{
		return;
	}

	pkStory->ClearAreaKeyMonster(iKey, iAreaIndex);
}
ResultType LC_GameStory_Manager::ReqEnterCrossMap(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t iMapResID)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GameServer GameStory ReqEnterCrossMap CharID: iMapResID:", pkPlayer->GetID(), pkPlayer->GetInstance(), iMapResID);
	
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}

	return pkStory->ReqEnterCrossMap(pkPlayer, iMapResID);
}

ResultType LC_GameStory_Manager::ReqTaskWaver(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, bool addItem)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}
	return pkStory->ReqTaskWaver(pkPlayer, addItem);
}

ResultType LC_GameStory_Manager::ReqShopping(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, shop_id_type iShopID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}
	return pkStory->ReqShopping(pkPlayer, iShopID);
}

ResultType LC_GameStory_Manager::ReqTransfer(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t iAreaIdx)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}
	return pkStory->ReqTransfer(pkPlayer, iAreaIdx);
}

ResultType LC_GameStory_Manager::ReqTaskAppoint(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t iTaskID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}
	return pkStory->ReqTaskAppoint(pkPlayer, iTaskID);
}

ResultType LC_GameStory_Manager::ReqBuyTimes(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t buyType, uint32_t times)
{
	if(times < 1)
	{
		return RE_FAIL;
	}

	if(pkPlayer==NULL || !pkPlayer->IsMainController())
	{
		return RE_FAIL;
	}

	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}

	ResultType res = RE_FAIL;
	switch(buyType)
	{
	case Buy_Raid_Type_VIPProfit:
		res = pkStory->BuyVIPProfitTimes(pkPlayer, times);
		break;
	case Buy_Raid_Type_VIPPrivilegeCard:
		res = pkStory->BuyVIPPrivilegeCardTimes(pkPlayer, times);
		break;
	default:
		break;
	}
	
	return res;
}

ResultType LC_GameStory_Manager::ReqBuyMapStepTimes(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory || !pkStory->IsActive())
	{
		return RE_FAIL;
	}
	return pkStory->ReqBuyMapStepTimes(pkPlayer);
}

const GSRTRankEntry* LC_GameStory_Manager::QueryRankSnap(int32_t iStoryID, const RankID& kRankID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if(NULL==pkStory)
	{
		return NULL;
	}
	return pkStory->GetRTRankEntry(kRankID);
}

GSRTPlayerCommonStat* LC_GameStory_Manager::GetPlayerCmnStats(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	return pkStory==NULL ? NULL : pkStory->GetPlayerStats(pkPlayer);
}
void LC_GameStory_Manager::RemovePlayerCmnStats(GameLogic::LC_PlayerBase *pkPlayer)
{
	LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
	for(StoryRecordMap::iterator itRecord=m_kStoryRecordMap.begin(); itRecord!=m_kStoryRecordMap.end(); ++itRecord)
	{
		GameStoryRecord &kStoryRecord = itRecord->second;
		LC_Server_GameStory_Base* pkStory = pkStoryLogicManager->GetStoryLogic(kStoryRecord.m_iStoryLogicId);
		if (pkStory)
		{
			return pkStory->RemovePlayerStateFromCmnStatMap(pkPlayer->GetInstance());
		}
	}
}
//----------------------------------------------
void LC_GameStory_Manager::Update(float fCurrentTime,float fDeltaTime)
{
	
}
//====================================================================================
GameStoryRecord* LC_GameStory_Manager::_addStoryRecord(const CF_GameStory::DataEntry *pkEntry)
{
	if(NULL == pkEntry)
	{
		return NULL;
	}

	StoryRecordMap::iterator it = m_kStoryRecordMap.find(pkEntry->_iID);
	if (it!=m_kStoryRecordMap.end())
	{
		it->second.Init(pkEntry);
		return &it->second;
	}
	m_kStoryRecordMap[pkEntry->_iID] = GameStoryRecord(pkEntry);
	m_kStoryIDs.insert(pkEntry->_iID);
	TryAddRecordGroup(pkEntry->_iLoopGroupId, pkEntry->_iID);
	return &m_kStoryRecordMap[pkEntry->_iID];
}
//===================================================================================
void LC_GameStory_Manager::_addRelateStory(int32_t iStoryID, int32_t iRelateType, int32_t iRelateDay, StringType sCronInfo)
{
	GameStoryRelate& kRelate = m_kRelateMap[iStoryID];
	kRelate.m_iStoryID = iStoryID;
	kRelate.m_iType = iRelateType;
	kRelate.m_iRelateDay = iRelateDay;
	kRelate.m_kCronData.ParseCronStr(sCronInfo);
}
//===================================================================================
void LC_GameStory_Manager::_broadcastAllGameStoryContentToClient(const GSResIDSet* pkStroyIDs/*=NULL*/)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if(pkPlayerManager)
	{
		StringStreamType ssStory;
		ssStory << "[";
		if (pkStroyIDs)
		{
			GSResIDSet::const_iterator citID = pkStroyIDs->begin();
			for (; citID!=pkStroyIDs->end(); ++citID)
			{
				ssStory << *citID << ",";
			}
		}
		ssStory << "]";
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_broadcastAllGameStoryContentToClient, IDS:%s", ssStory.str().c_str());

		const LC_PlayerIDPtrMap& allPlayers = pkPlayerManager->GetPlayerIDMap();
		LC_PlayerIDPtrMap::const_iterator cit = allPlayers.begin();
		for (; cit!=allPlayers.end(); ++cit)
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)cit->second;
			if(pkServerPlayer && pkServerPlayer->IsMainController())
			{
				NotifyPlayerGameStoryContent(pkServerPlayer, pkStroyIDs);
				pkServerPlayer->SetDirtyFlag(DIRTY_FLAG_GAMESTORY_ASSERT, false);
			}
		}
	}
}

class StoryCheckReset : public UT_SimOperator<bool, uint32_t, uint32_t>
{
public:
	StoryCheckReset(LC_Server_GameStory_Base* pkStory)
		:m_pkStory(pkStory)
		,m_iResetType(LC_ST_TP_DAILY)
	{
		if (NULL == m_pkStory)
		{
			return;
		}
		GameStoryConfig* pkConfig = m_pkStory->GetConfig();
		if (NULL == pkConfig)
		{
			return;
		}
		m_iResetType = (pkConfig->m_kCmnStatData.IsValid()) ? pkConfig->m_kCmnStatData.iResetType : LC_ST_TP_DAILY;
	}

	virtual bool operator ()(uint32_t timestamp, uint32_t curtimestamp)
	{
		if (m_pkStory->IsActive()&&!m_pkStory->IsTimestampIn((int64_t)timestamp*1000))
		{
			return true;
		}
		else
		{
			switch (m_iResetType)
			{
			case LC_ST_TP_DAILY:
				return !LC_Helper::CheckIsSameDay(timestamp, curtimestamp);
				break;
			case LC_ST_TP_WEEK:
				return !LC_Helper::CheckIsSameWeek(timestamp, curtimestamp);
				break;
			case LC_ST_TP_MONTH:
				return !LC_Helper::CheckIsSameMonth(timestamp, curtimestamp);
				break;
			default:
				break;
			}
		}
		return false;
	}
protected:
	LC_Server_GameStory_Base* m_pkStory;
	int32_t	m_iResetType;
};

int32_t LC_GameStory_Manager::GetStoryIdByReplevy( int nInput )
{
	int32_t nResult = 0;
	mem::map<int32_t, int32_t>::iterator it = m_ReplevyMap.find(nInput);
	if (it != m_ReplevyMap.end())
		nResult = it->second;
	return nResult;
}

LC_Server_GameStory_Base* LC_GameStory_Manager::GetGameStoryBasePtrByStoryID(int32_t storyID)
{
	if(0 == storyID)
	{
		return NULL;
	}

	GameStoryRecord* pkRecord = GetStoryRecord(storyID);
	if(NULL == pkRecord)
	{
		return NULL;
	}

	LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
	return pkStoryLogicManager->GetStoryLogic(pkRecord->m_iStoryLogicId);
}

void LC_GameStory_Manager::ScheduleGameStory(int32_t iStoryID, Utility::timer_id_type timerID)
{
	_scheduleGameStory(iStoryID, timerID, true);
}

void LC_GameStory_Manager::_resetStoryData(GameLogic::LC_PlayerBase *pkPlayer, int iStoryID, int64_t timestamp)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL == pkStory)
	{
		return;
	}

	StoryCheckReset rTmp(pkStory);
	pkStory->_resetCommonStat(pkPlayer, uint32_t(timestamp/1000), rTmp);

	if(pkStory->IsActive())
	{
		pkStory->_checkTask(pkPlayer);
	}

	LC_GameStoryAsset& kStoryAsset = pkPlayer->GetGameStoryAsset();
	GameStoryEntry* pkStoryEntry = kStoryAsset.GetGameStoryEntry(iStoryID);
	if (pkStoryEntry)
	{
		_checkReset(pkStoryEntry, timestamp);
	}
}
//------------------------------------------------------------------------------------
ResultType LC_GameStory_Manager::IsAllowedEnterTranscription(LC_PlayerBase* pkServerPlayer, int lMapResID, GameStoryEntry* pkStoryEntry)
{
	if(NULL==pkStoryEntry)
	{
		return RE_GAMESTORY_UNVALID;
	}

	GameStoryRecord* pkRecord = GetStoryRecord(pkStoryEntry->GetStoryID());
	if(NULL != pkRecord)
	{
		if (!pkRecord->m_bActive && lMapResID != GetGlobalSetting.BestGuildMapID)
		{
			return RE_GAMESTORY_UNACTIVE;
		}

		GameStoryConfig *pkConfig = pkRecord->GetConfig();
		if (NULL == pkConfig)
		{
			return RE_GAMESTORY_BAD_TRANSCRIPTION;
		}

		MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
		const MapSetting* pkMapSetting = kMapCtrlSetting.GetMapSetting(lMapResID);

		if(NULL ==pkMapSetting)
		{
			return RE_GAMESTORY_BAD_TRANSCRIPTION;
		}

		int iJoinTimes = pkConfig->GetJoinTimesLimit(pkServerPlayer);
		int iFinishTimes = pkConfig->GetFinishTimesLimit(pkServerPlayer);
		LC_StatType eStatType = LC_ST_TP_NA;
		pkStoryEntry->ResolveCheckType(eStatType);
		GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
		if (NULL == pkStat)
		{
			return RE_GAMESTORY_UNVALID;
		}

		if (iFinishTimes>=0 && pkStoryEntry->CountTotalMapFinishedTimes(eStatType)>=iFinishTimes+pkStat->GetAllBuyTimes())
		{
			if(pkConfig->m_iFirstFinishNotRecordRaidFinished == 1)
			{
				LC_RaidAssert*	raidAsset = pkServerPlayer->GetRaidAsset();
				//次数不够的时候，第一次进入副本，可以进
				const RaidInfo* raidInfo = raidAsset->FindRaid(lMapResID);
				if(NULL != raidInfo)
				{
					return RE_GAMESTORY_TIMES_LIMIT;
				}
			}		
			else
			{
				return RE_GAMESTORY_TIMES_LIMIT;
			}
		}

		if(pkMapSetting->IsEnterComsumeJoinTimes() && iJoinTimes>=0 && pkStoryEntry->CountTotalMapEnteredTimes(eStatType)>=iJoinTimes+pkStat->GetAllBuyTimes())
		{
			return RE_GAMESTORY_TIMES_LIMIT;
		}
		else
		{			
			switch (kMapCtrlSetting.m_uiMapCtrlType)
			{
			case  GS_MCTRL_TP_CONTINUOUES:
				{
					if(pkConfig->GetNextTranscriptionResID(pkServerPlayer, pkStoryEntry, pkServerPlayer->GetMapResID())!=lMapResID)
					{
						return RE_GAMESTORY_BAD_TRANSCRIPTION;
					}
				}
				break;
			default:
				break;
			}
		}		
			
		ResultType rslt = pkConfig->PreEnterMapCheck(pkServerPlayer, lMapResID, pkStoryEntry);
		if (RE_SUCCESS != rslt)
		{
			return rslt;
		}

		return RE_SUCCESS;
	}
	return RE_GAMESTORY_UNVALID;
}
//-----------------------------------------------------------------------------
ResultType LC_GameStory_Manager::CheckGameStoryEnterCondition(LC_PlayerBase* pkPlayer, const int iStoryID, uint32_t times)
{
	GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
	if(NULL == pkRecord)
	{
		return RE_FAIL;
	}

	if (!pkRecord->m_bActive)
	{
		return RE_GAMESTORY_UNACTIVE;
	}

	GameStoryConfig *pkConfig = pkRecord->GetConfig();
	if (pkConfig->IsGuildRequired() && !pkPlayer->IsExistGuild())
	{
		return RE_GAMESTORY_GUILD_REQUIRED;
	}

	LC_GameStoryAsset& kStoryAsset = pkPlayer->GetGameStoryAsset();
	GameStoryEntry *pkStoryEntry = kStoryAsset.GetGameStoryEntry(iStoryID);
	if(NULL == pkStoryEntry)
	{
		return RE_FAIL;
	}

	LC_StatType eStatType = LC_ST_TP_NA;
	pkStoryEntry->ResolveCheckType(eStatType);
	GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
	if (NULL == pkStat)
	{
		return RE_FAIL;
	}

	int32_t iJoinTimes = pkConfig->GetJoinTimesLimit(pkPlayer);
	if(iJoinTimes>=0 && pkStat->m_kTimes.m_lEnteredTimes+times>iJoinTimes+pkStat->GetAllBuyTimes())
	{
		return RE_TIMES_OUT;
	}

	int32_t iFinishTimes = pkConfig->GetFinishTimesLimit(pkPlayer);
	if (iFinishTimes>=0 && pkStat->m_kTimes.m_lFinishedTimes+times>iFinishTimes+pkStat->GetAllBuyTimes())
	{
		return RE_TIMES_OUT;
	}
	return RE_SUCCESS;
}

bool LC_GameStory_Manager::CheckIsActiveStory(int32_t iStoryID)
{
	const GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
	return NULL==pkRecord ? false : pkRecord->m_bActive;
}

//-----------------------------------------------------------------------------
void LC_GameStory_Manager::NotifyPlayerGameStoryContent(LC_ServerPlayer* pkServerPlayer, const GSResIDSet* pkStroyIDs)
{
	LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	LC_GameStoryAsset &kStoryAsset = pkServerPlayer->GetGameStoryAsset();
	GSResIDSet kStoryIDs;
	if(NULL == pkStroyIDs)
	{
		kStoryIDs = m_kStoryIDs;
	}
	else
	{
		kStoryIDs = *pkStroyIDs;
		kStoryAsset.GetDirtyStoryIDs(kStoryIDs);
	}

	float fCurrent = GET_PROCESS_TIME;
	GSResIDSet::const_iterator it=kStoryIDs.begin();
	int iLeftSize = (int)kStoryIDs.size();
	while (it!=kStoryIDs.end())
	{
		MG_GAMESTORY_CONTENT_INFO_SYNC kGameStoryContentInfo;
		int iCurSize = UT_MathBase::MinInt(MAX_GAMESTORY_BRODCAST_TO_CLIENT, iLeftSize);
		kGameStoryContentInfo.m_kGameStoryContent.resize(iCurSize);

		while(kGameStoryContentInfo.m_iCount<MAX_GAMESTORY_BRODCAST_TO_CLIENT && it!=kStoryIDs.end())
		{
			int32_t iStoryID = *it;
			GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
			if(NULL == pkRecord)
			{
				continue;
			}

			GAMESTORY_CONTENT_INFO &kStoryInfo = kGameStoryContentInfo.m_kGameStoryContent[kGameStoryContentInfo.m_iCount];
			GameStoryEntry* pkStoryEntry = kStoryAsset.GetGameStoryEntry(iStoryID);
			GameStoryConfig* pkConfig = pkRecord->GetConfig();
			if (NULL==pkConfig || NULL== pkStoryEntry)
			{
				continue;
			}
			PS_GAME_STORY_BASIC_STAT& psBasicStat = kStoryInfo.m_kBasicStat;
			MapCtrlSetting& kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
			BatchScheduleData& kBatchScheduleData = pkConfig->m_kBatchSechedule;

			LC_StatType eStatType = (LC_StatType)pkConfig->m_iStatType;
			if(LC_ST_TP_NA == eStatType)
			{
				pkStoryEntry->ResolveCheckType(eStatType);
			}
			GameStoryCommonStatEntry* pkStat = pkStoryEntry->GetCommonStatEntry(eStatType);
			if (NULL==pkStat)
			{
				continue;
			}

			const GSRaidStatMap& kRaidStat = pkStat->m_kRaidStats;
			kStoryInfo.m_uiStoryID = pkRecord->m_iStoryID;
			kStoryInfo.m_iActive = pkRecord->m_bActive ? 1 : 0;
			TPSTR2STDSTR(pkRecord->m_sCronInfo, kStoryInfo.m_szCronStr);
			kStoryInfo.m_iTimeBegin = pkRecord->m_iStartTime;
			kStoryInfo.m_iTimeEnd = pkRecord->m_iEndTime;
			{
				kStoryInfo.m_iAlreadyEnteredTimes = uint8_t(pkStat->m_kTimes.m_lEnteredTimes);	
				kStoryInfo.m_iAlreadyFinishedTimes = uint8_t(pkStat->m_kTimes.m_lFinishedTimes);
				kStoryInfo.m_iAlreadyBuyTimes = uint8_t(pkStat->GetAllBuyTimes());
				pkStoryEntry->SetDirtyFlag(false);
			}

			LC_Server_GameStory_Base* pkStory = pkLogicMgr->GetStoryLogic(pkRecord->m_iStoryLogicId);

			if (pkStory)
			{
				GSRTPlayerCommonStat* pkPlayerStat = pkStory->GetPlayerStats(pkServerPlayer);
				if (pkPlayerStat)
				{
					GSServerReward reward;
					pkStory->GetCurrentStatReward(pkServerPlayer->GetInstance(), reward);
					for (int32_t is=GSRTPlayerCommonStat::ST_CNT_DAMAGE; is<GSRTPlayerCommonStat::ST_CNT_MAX; ++is)
					{
						uint32_t cnt = pkPlayerStat->GetStatCount(is);
						if (cnt)
						{
							psBasicStat.m_kStatCount[(PS_GAME_STORY_BASIC_STAT::StatCountMap::key_type)is] = (PS_GAME_STORY_BASIC_STAT::StatCountMap::mapped_type)cnt;
						}
					}
					
					psBasicStat.m_uiTotalExp = (uint64_t)reward.dExp;
					psBasicStat.m_uiTotalCash = (uint64_t)reward.dCash;
					psBasicStat.m_uiToalSP = (uint64_t)reward.dSP;
					if (!pkPlayerStat->kStatItems.empty())
					{
						psBasicStat.m_kTotalItems.insert(pkPlayerStat->kStatItems.begin(), pkPlayerStat->kStatItems.end());
					}
					else
					{
						Utility::UT_SIMDataList::iterator itR = reward.kItems.begin();
						for (; itR!=reward.kItems.end(); ++itR)
						{
							psBasicStat.m_kItems[itR->ID()] = (int32_t)itR->ItemCount();
						}
					}
				}
			}

			if (kMapCtrlSetting.m_kMapResIDList.empty())
			{
				GSResIDSet& kMapResIDs = kBatchScheduleData.kMapResIDs;
				GSResIDSet::iterator itMID = kMapResIDs.begin();
				for(; itMID!=kMapResIDs.end(); ++itMID)
				{
					int32_t iMapResID = *itMID;
					GSRaidStatMap::const_iterator cit = kRaidStat.find(iMapResID);
					if(cit!=kRaidStat.end() && !cit->second.IsTrivial())
					{
						PS_GAME_RAID_STAT psRaidStat;
						psRaidStat.m_iType  = iMapResID;
						psRaidStat.m_iEnterTimes = uint16_t(cit->second.EnteredTimes());
						psRaidStat.m_iFinishedTimes = uint16_t(cit->second.FinishedTimes());
						kStoryInfo.m_kRaidStat.push_back(psRaidStat);
					}
				}
			}
			else
			{
				GSResIDList& kMapResIDs = kMapCtrlSetting.m_kMapResIDList;
				GSResIDList::iterator itMID = kMapResIDs.begin();
				for(; itMID!=kMapResIDs.end(); ++itMID)
				{
					int32_t iMapResID = *itMID;
					GSRaidStatMap::const_iterator cit = kRaidStat.find(iMapResID);
					if(cit != kRaidStat.end() && !cit->second.IsTrivial())
					{
						PS_GAME_RAID_STAT psRaidStat;
						psRaidStat.m_iType  = iMapResID;
						psRaidStat.m_iEnterTimes = uint16_t(cit->second.EnteredTimes());
						psRaidStat.m_iFinishedTimes = uint16_t(cit->second.FinishedTimes());
						kStoryInfo.m_kRaidStat.push_back(psRaidStat);
					}
				}
			}

			++kGameStoryContentInfo.m_iCount;
			++it;
			--iLeftSize;
		}
		pkServerPlayer->SendMsgToClient(MGPT_GAMESTORY_CONTENT_INFO_SYNC, &kGameStoryContentInfo);
	}
}

void LC_GameStory_Manager::NotifyActorEncountedStat(LC_ServerPlayer* pkServerPlayer, int32_t iStoryID, object_id_type uiID)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL == pkStory)
	{
		return;
	}

	pkStory->NotifyActorEncountedStat(pkServerPlayer, uiID);
}

void LC_GameStory_Manager::NotifyActorEncountedStatWithoutStoryID(LC_ServerPlayer* pkServerPlayer, object_id_type uiID)
{
	if (NULL == pkServerPlayer)
	{
		return;
	}
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
	notifyMsg.m_iStroyID = 0;
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

void LC_GameStory_Manager::NotifyActorEncountedStatByDamageType(LC_ServerPlayer* pkServerPlayer, int32_t iStoryID, object_id_type uiID, int damageType)
{
	LC_Server_GameStory_Base* pkStory = GetGameStoryBasePtrByStoryID(iStoryID);
	if (NULL==pkStory || !pkStory->IsActive())
	{
		return;
	}
	pkStory->NotifyActorEncountedStatByDamageType(pkServerPlayer, uiID, damageType);
}
//-----------------------------------------------------------------------------------------
int32_t LC_GameStory_Manager::GetTaskExtraProfit(int iStoryID, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, uint32_t timestamp)
{
	GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
	if(NULL == pkRecord)
	{
		return 0;
	}

	GameStoryConfig* pkConfig = pkRecord->GetConfig();
	if (pkConfig)
	{
		pkConfig->GetTaskExtraProfit(pkPlayer, pkTask, timestamp);
	}
	return 0;
}

int32_t LC_GameStory_Manager::GetTaskFailProfit(int iStoryID, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, uint32_t timestamp)
{
	GameStoryRecord* pkRecord = GetStoryRecord(iStoryID);
	if(NULL != pkRecord)
	{
		GameStoryConfig* pkConfig = pkRecord->GetConfig();
		if (pkConfig)
		{
			pkConfig->GetTaskFailProfit(pkPlayer, pkTask, timestamp);
		}
	}
	return 0;
}

int	LC_GameStory_Manager::GetRespawnCostShopID(int lStoryId, int lMapResId)
{
	GameStoryRecord* pkRecord = GetStoryRecord(lStoryId);
	if(NULL != pkRecord)
	{
		GameStoryConfig* pkConfig = pkRecord->GetConfig();
		if (pkConfig)
		{
			return pkConfig->GetRespawnCostShopID(lMapResId);
		}
	}
	return 0;
}

void LC_GameStory_Manager::BroadCastQz(int lStoryId)
{
	CF_GameStory::DataEntry* pStory = SafeGetCSVFileDataEntryPtr<CF_GameStory>(lStoryId);
	if(pStory)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(pStory->_IGameNoticeId);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort);	
	}

}
