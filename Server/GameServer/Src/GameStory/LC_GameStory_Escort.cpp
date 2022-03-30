#include "GameServerApp.h"

#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_GameEvent.h"
#include "LC_GameStory_Escort.h"
#include "LC_ServerMap.h"
#include "LC_ServerNPC.h"
#include "LC_ServerTaskManager.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "oss_define.h"

using namespace GameLogic;
using namespace GameStory;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

/******************************* Class Implement *****************************************/

//---------------------------------------------------------------
LC_GameStory_Escort::LC_GameStory_Escort(int32_t logicID, GameStoryRecord* pkRecord): LC_Server_GameStory_Base(logicID, pkRecord)
{
}

//----------------------------------------------------------------
LC_GameStory_Escort::~LC_GameStory_Escort()
{

}
//---------------------------------------------------------------------
void LC_GameStory_Escort::Init()
{
	m_kEscortSet.clear();
	LC_Server_GameStory_Base::Init();
}

//---------------------------------------------------------------------
void LC_GameStory_Escort::UnInit()
{
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
	if (!pkWorldManager || !GetConfig())
	{
		return;
	}
	LC_Server_GameStory_Base::UnInit();
	object_id_set::iterator it = m_kEscortSet.begin();
	for (; it != m_kEscortSet.end(); ++it)
	{
		pkSkillInterface->RemoveLogicObject(*it);
	}
}
//---------------------------------------------------------------------
void LC_GameStory_Escort::ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured)
{
	LC_Server_GameStory_Base::ActorBeKilled(pkKiller, pkInjured);
	if (pkInjured && IS_NPC_ID(pkInjured->GetID()))
	{
		GSRTActorTriggerReocrd* pkActorTriRecord = _getRTActorTriggerRecord(pkInjured->GetID());
		if (NULL != pkActorTriRecord)
		{
			_clearRTTriActorRecord(pkInjured->GetID());
			m_kRTActorTriggerReocrdMap.erase(pkInjured->GetID());
		}
		//if (m_kEscortSet.count(pkInjured->GetID()))
		{
			m_kEscortSet.erase(pkInjured->GetID());
		}
	}
}

void LC_GameStory_Escort::OnAcceptTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig)
{
	if (pkConfig == NULL)
	{
		pkConfig = GetConfig();
	}
	LC_Server_GameStory_Base::OnAcceptTask(pkPlayer, pkTask, pkConfig);
	if (pkConfig && pkConfig->m_kTaskSetting.GetTaskData(pkTask->GetTaskID()))
	{
		LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
		LC_ProcessingTaskEntry* pkEntry = pkTaskMap->GetProcessingTaskEntryByID(pkTask->GetTaskID());
		if (pkEntry)
		{
			int npcID = pkEntry->GetExParam1();

			if (npcID)
			{
				m_kEscortSet.insert(npcID);
			}
		}
		NotifyPlayerGameStoryBatch(GS_SCHEDULE_KEY_COMMON, (LC_ServerPlayer*)pkPlayer, Utility::TIMER_ID_NA, pkConfig);
	}
}

void LC_GameStory_Escort::CheckUpdateTask(LC_ServerPlayer* pkPlayer)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig || NULL == pkPlayer)
	{
		return;
	}

	{
		uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
		TaskDataMap::iterator it = pkConfig->m_kTaskSetting.kTaskMap.begin();
		for (; it != pkConfig->m_kTaskSetting.kTaskMap.end(); ++it)
		{
			int32_t iTaskID = it->first;
			if (pkTaskMap->IsInFinishedTaskMap(iTaskID))
			{
				continue;
			}

			LC_ProcessingTaskEntry* pkEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskID);
			if (pkEntry)
			{
				int npcID = pkEntry->GetExParam1();
				int serverID = pkEntry->GetExParam2();
				int iCheckTime = pkEntry->GetExParam3();
				bool escortLost = false;
				if (IsActive())
				{
					if (GetGameServerApp()->CheckLocalServer(serverID) == E_LocalServerState_Local)
					{
						LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
						LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(npcID);
						if (NULL == pkActor || pkActor->GetHP() <= 0)
						{
							escortLost = true;
						}
					}
					else
					{
						//跨服数据 无法检测 暂时被动检测  清理玩家数据
						if (iCheckTime > 0)
						{
							if (timestamp - iCheckTime >= 10 * 60)
							{
								escortLost = true;
							}
						}
						else
						{
							pkEntry->SetTaskExParam3(timestamp);
						}
					}
				}
				else
				{
					SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
					pkSkillInterface->RemoveLogicObject(npcID);
					escortLost = true;
				}
				if (escortLost)
				{
					pkTaskMap->SetTaskFail(iTaskID, true);
				}
			}
		}
	}
}
int32_t LC_GameStory_Escort::NotifyPlayerGameStoryBatch(GameStoryScheduleKey iKey, LC_ServerPlayer* pkServerPlayer, Utility::timer_id_type iTimerID, GameStoryConfig* pkConfig)
{
	if (pkConfig == NULL)
	{
		pkConfig = GetConfig();
	}
	if (NULL == pkConfig || NULL == pkServerPlayer)
	{
		return 0;
	}

	{
		LC_TaskMap* pkTaskMap = pkServerPlayer->GetTaskMap();
		TaskDataMap::iterator it = pkConfig->m_kTaskSetting.kTaskMap.begin();
		for (; it != pkConfig->m_kTaskSetting.kTaskMap.end(); ++it)
		{
			int32_t iTaskID = it->first;
			if (pkTaskMap->IsInFinishedTaskMap(iTaskID))
			{
				continue;
			}

			LC_ProcessingTaskEntry* pkEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskID);
			if (pkEntry)
			{
				if (IsActive())
				{
					int npcID = pkEntry->GetExParam1();
					int serverID = pkEntry->GetExParam2();
					GSRTActorTriggerReocrd* pkRTActroTriRecord = _getRTActorTriggerRecord(npcID);
					if (NULL == pkRTActroTriRecord)
					{
						continue;
					}
					MG_GAMESTORY_BATCH_INFO_SYNC syncMsg;
					GAMESTORY_BATCH_INFO& kStoryBatch = syncMsg.m_kGameStoryBatch;
					kStoryBatch.m_uiStoryID = GetStoryID();

					PS_GAMESTORY_BATCH_DETAIL psDetail;
					GSTriggerCntMap::iterator itTriCnt = pkRTActroTriRecord->kTriggerReged.begin();
					for (; itTriCnt != pkRTActroTriRecord->kTriggerReged.end(); ++itTriCnt)
					{
						if (itTriCnt->second)
						{
							++psDetail.m_uiSeqIdx;
						}
					}

					kStoryBatch.m_kDetails.push_back(psDetail);
					if (GetGameServerApp()->CheckLocalServer(serverID) == E_LocalServerState_Local)
					{
						LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
						LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(npcID);
						if (pkActor)
						{
							PS_GAMESTORY_AREA_INFO psArea;
							{
								PS_GAMESTORY_KEY_INFO psKey;
								psKey.m_iResID = pkActor->GetCharType();
								psKey.m_iLCID = npcID;
								psArea.m_kKeyInfos.push_back(psKey);
							}
							kStoryBatch.m_kAreaInfos.push_back(psArea);
						}
					}
					pkServerPlayer->SendMsgToClient(MGPT_GAMESTORY_BATCH_INFO_SYNC, &syncMsg);
				}
			}
		}
	}
	return 1;
}
//---------------------------------------------------------------------
void LC_GameStory_Escort::_onTriggerHit(LC_ServerMap_Trigger* pkTrigger, LC_ActorPtrList& kActors, LC_ServerMapLogic_Place* pkMapLogic)
{
	if (NULL == pkTrigger || NULL == pkMapLogic)
	{
		return;
	}

	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}

	GameStoryScheduleKey iKey = _filterProperRTScheduleKey(pkMapLogic, pkConfig);
	GSRTBatchSchedule* pkRTBSchedule = _getRTBatchSchedule(iKey);
	if (NULL == pkRTBSchedule)
	{
		return;
	}

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	GSRTMapTriggerRecordMap::iterator itRecordMap = m_kRTMapTriggerRecordMap.find(pkMapLogic->GetMapLogicID());
	if (itRecordMap == m_kRTMapTriggerRecordMap.end())
	{
		return;
	}

	GSRTTriggerRecordMap::iterator it = itRecordMap->second.find(pkTrigger->GetId());
	if (it != itRecordMap->second.end())
	{
		GSRTTriggerRecord& kRTTriggerRecord = it->second;
		TrigerData* pkTriggerData = kRTTriggerRecord.pkTriggerData;
		if (pkTriggerData)
		{
			GSRTTriggerSrcInfo kSrcInfo;
			kSrcInfo.kTriIdx = kRTTriggerRecord.kIndex;

			LC_ActorPtrList::iterator itActor = kActors.begin();
			for (; itActor != kActors.end(); ++itActor)
			{
				GameLogic::LC_ActorBase* pkActor = *itActor;
				if (!pkConfig->CheckTriggerForActor(pkActor, pkMapLogic))
				{
					continue;
				}

				int ownerID = pkActor->GetSummonOwnerID();
				if (!IS_PLAYER_ID(ownerID))
				{
					continue;
				}

				kSrcInfo.iSrcActorID = ownerID;
				GSRTActorTriggerReocrd* pkRTActroTriRecord = _getRTActorTriggerRecord(pkActor->GetID());
				if (NULL == pkRTActroTriRecord)
				{
					continue;
				}

				GSTriggerCntMap::iterator itTriCnt = pkRTActroTriRecord->kTriggerReged.find(kRTTriggerRecord.kIndex);
				if (itTriCnt == pkRTActroTriRecord->kTriggerReged.end() || itTriCnt->second)
				{
					continue;
				}

				++itTriCnt->second;
				int iAutoFitLevel = -1;
				LC_ServerPlayer* pkOwner = (LC_ServerPlayer*)pkWorldManager->FindObject(ownerID);
				if (pkOwner)
				{
					NotifyPlayerGameStoryBatch(iKey, pkOwner);
					iAutoFitLevel = pkOwner->GetLevel();
				}

				LC_ActorIDPtrMap kActorMap;
				BatchIndices::iterator itBID = pkTriggerData->kBatchIndices.begin();
				for (; itBID != pkTriggerData->kBatchIndices.end(); ++itBID)
				{
					BatchData* pkBatchData = pkConfig->m_kBatchSechedule.GetBatchData(*itBID);
					if (pkBatchData)
					{
						GSRTBatchDetail* pkRTDetail = pkRTBSchedule->AutoDetail();
						if (pkRTDetail)
						{
							pkRTDetail->kBatchIdx.iBatchIdx = *itBID;
							for (int i = 0; i < pkBatchData->iTimes; ++i)
							{
								_spawnBatchMonster(iKey, pkRTDetail->iIndex, pkConfig, &kActorMap, &kSrcInfo, iAutoFitLevel, pkConfig);
							}
						}
					}
				}

				LC_ActorIDPtrMap::iterator itActorGen = kActorMap.begin();
				for (; itActorGen != kActorMap.end(); ++itActorGen)
				{
					GameLogic::LC_ActorBase* pkActorGen = itActorGen->second;
					if (pkActorGen)
					{
						pkActorGen->SetAdditionTargetID(pkActor->GetID());
						pkActorGen->SetAdditionTargetBeginTime(GET_PROCESS_TIME);
						pkActorGen->SetAdditionTargetDurationTime((float)999999999);
						kRTTriggerRecord.kObj2TriActorMap[pkActorGen->GetID()] = pkActor->GetID();
						pkRTActroTriRecord->kGenActors.insert(pkActorGen->GetID());
					}
				}
			}
		}
	}
}

void LC_GameStory_Escort::_clearRTTriActorRecord(object_id_type iID)
{
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
	GSRTActorTriggerReocrdMap::iterator it = m_kRTActorTriggerReocrdMap.find(iID);
	if (it != m_kRTActorTriggerReocrdMap.end())
	{
		GSRTActorTriggerReocrd& kRTActorTrirecord = it->second;
		object_id_set kIDs = kRTActorTrirecord.kGenActors;
		object_id_set::iterator itID = kIDs.begin();
		for (; itID != kIDs.end(); ++itID)
		{
			_clearSpawnedMonster(*itID, NULL);
			pkSkillInterface->RemoveLogicObject(*itID);
		}

		kRTActorTrirecord.kGenActors.clear();
		LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(iID);
		if (pkActor)
		{
			int32_t iOwnerID = pkActor->GetSummonOwnerID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(iOwnerID);
			if (IS_PLAYER_ID(iOwnerID) && pkPlayer)
			{
				CheckUpdateTask(pkPlayer);
			}
		}
	}
}