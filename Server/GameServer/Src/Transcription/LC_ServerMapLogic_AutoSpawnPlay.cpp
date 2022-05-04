
#include "GameServerApp.h"
#include "LC_ServerWorldManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_AutoSpawnPlay.h"
#include "LC_NPCManagerBase.h"
#include "oss_define.h"
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

#define		SPAWN_POINT_CHECKING_TIMER		300

LC_ServerMapLogic_AutoSpawnPlay::LC_ServerMapLogic_AutoSpawnPlay():
m_Config(),m_State(MapLogic_NormalPlayState_None),m_fLastAutoSpawnTime(0),m_FirstSpawnTimeMin(0),m_FirstSpawnTimeMax(0),m_IntervalSpawnTimeMin(0),m_IntervalSpawnTimeMax(0)
{

}

LC_ServerMapLogic_AutoSpawnPlay::~LC_ServerMapLogic_AutoSpawnPlay()
{
	UnInit();
}

bool LC_ServerMapLogic_AutoSpawnPlay::Init(LC_ServerMapLogic_Place* place,DungeonConfigData::AutoSpawnMonsterDataMap& config)
{
	if (place==NULL)
	{
		return false;
	}
	bool ret=LC_ServerMapLogic_PlayBase::Init(place,PlayType_AutoSpawn);
	if (!ret)
	{
		return false;
	}
	m_Config=config;
	m_FirstSpawnTimeMin=1;
	m_FirstSpawnTimeMax=3;
	m_IntervalSpawnTimeMin=1;
	m_IntervalSpawnTimeMax=3;
	m_State=MapLogic_NormalPlayState_Inited;
	return true;
}

void LC_ServerMapLogic_AutoSpawnPlay::UnInit()
{
	End();
	m_FirstSpawnTimeMin=0;
	m_FirstSpawnTimeMax=0;
	m_IntervalSpawnTimeMin=0;
	m_IntervalSpawnTimeMax=0;
	m_State=MapLogic_NormalPlayState_None;
	m_Config.clear();
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_AutoSpawnPlay::Start()
{
	if (m_State!=MapLogic_NormalPlayState_Inited && m_State!=MapLogic_NormalPlayState_End)
	{
		return false;
	}
	m_State=MapLogic_NormalPlayState_Run;
	int iRandUpdateTime = UT_MathBase::RandInRangeInt(m_FirstSpawnTimeMin,m_FirstSpawnTimeMax);
	m_fLastAutoSpawnTime=GET_PROCESS_TIME+iRandUpdateTime;
	return true;
}

void LC_ServerMapLogic_AutoSpawnPlay::End()
{
	if (!CheckRun())
	{
		return;
	}
	m_State=MapLogic_NormalPlayState_End;
}

bool LC_ServerMapLogic_AutoSpawnPlay::Update(float fCurrentTime, float fDeltaTime)
{
	if(!CheckRun())
	{
		return true;
	}
	float fTimeStep = fCurrentTime;
	fCurrentTime = GET_PROCESS_TIME;
	fTimeStep = fCurrentTime - fTimeStep;
	if (fCurrentTime<m_fLastAutoSpawnTime)
	{
		return true;
	}
	DungeonConfigData::AutoSpawnMonsterDataMapIter IterBe = m_Config.begin();
	DungeonConfigData::AutoSpawnMonsterDataMapIter IterEd = m_Config.end();

	LC_ServerMapLogic_Place* pkMaplogic = GetPlace();
	if (!pkMaplogic)
	{
		return false;
	}
	pkMaplogic->ComputeAutoFitLevel();

	int nSpawnSize = m_Config.size();
	LC_TranscriptionListEntry transEntry;
	for(; IterBe != IterEd; ++IterBe)
	{
		DungeonConfigData::AutoSpawnMonsterData& rkAutoSpawnData = IterBe->second;
		if (rkAutoSpawnData._bWaitActive)
		{
			if(fCurrentTime >= rkAutoSpawnData._lastActiveTime + rkAutoSpawnData._coolDownTime)
			{
				DungeonConfigData::MonsterData* pkData = IterBe->second._spawnData;
				Fill_Spawn_Monster_Data(*pkData,transEntry);
				Utility::UT_SIMDataInfo kEliteInfo;
				if (pkData->m_kEliteProp.Resolve(kEliteInfo) >= 0)
				{
					DungeonConfig::EliteSkillRuleSetType rTmp;
					GameLogic::LC_ActorPtrList pkNPCs = pkMaplogic->SpawnEliteNPC(transEntry, 0, rTmp, -1, FACTION_NONE, (int)kEliteInfo.ID());

					GameLogic::LC_ActorPtrList::iterator it = pkNPCs.begin();
					for (; it!=pkNPCs.end(); ++it)
					{
						LC_ServerNPC* npc = (LC_ServerNPC*)*it;
						npc->SetSpawnPointID(IterBe->first);
						rkAutoSpawnData._objectLogicIDs.insert(npc->GetID());
					}
				}
				else
				{
					LC_ServerNPC* npc=pkMaplogic->SpawnNPC(transEntry);
					if (npc==NULL)
					{
						continue;
					}
					npc->SetSpawnPointID(IterBe->first);
					rkAutoSpawnData._objectLogicIDs.insert(npc->GetID());
				}
				rkAutoSpawnData._bWaitActive = false;
			}
		}
		//else if (fCurrentTime >= rkAutoSpawnData._lastUpdateTime + 300.0f) // check the spawn point ? 
		else if (fCurrentTime >= rkAutoSpawnData._lastUpdateTime + SPAWN_POINT_CHECKING_TIMER) // check the spawn point ? 
		{
			LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
			//int32_t nSpawnId = IterBe->first;
			//int32_t nObjIdSize = rkAutoSpawnData._objectLogicIDs.size();
			object_id_set::iterator it = rkAutoSpawnData._objectLogicIDs.begin(); 
			while (it!=rkAutoSpawnData._objectLogicIDs.end())
			{
				LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkNPCManager->FindNPC(*it);
				if (pkNPC)
				{
					++it;
				}
				else
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Check AutoSpawn [%d] miss a npc [%d]!", IterBe->first, *it);
				}
			}
			rkAutoSpawnData._lastUpdateTime = (int32_t)fCurrentTime;
			if (rkAutoSpawnData._objectLogicIDs.empty())
			{
				rkAutoSpawnData._bWaitActive = true;
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Check AutoSpawn [%d] force active!", IterBe->first);
			}
		}
	}
	int iRandUpdateTime = UT_MathBase::RandInRangeInt(m_IntervalSpawnTimeMin, m_IntervalSpawnTimeMax);
	m_fLastAutoSpawnTime=fCurrentTime+iRandUpdateTime;
	return true;
}

bool LC_ServerMapLogic_AutoSpawnPlay::CheckEnd()
{
	return m_State==MapLogic_NormalPlayState_End;
}

bool LC_ServerMapLogic_AutoSpawnPlay::CheckRun()
{
	return (m_State==MapLogic_NormalPlayState_Run);
}

void LC_ServerMapLogic_AutoSpawnPlay::SetFirstTime(int mintime,int maxtime)
{
	m_FirstSpawnTimeMin=mintime;
	m_FirstSpawnTimeMax=maxtime;
}

void LC_ServerMapLogic_AutoSpawnPlay::SetIntervalTime(int mintime,int maxtime)
{
	m_IntervalSpawnTimeMin=mintime;

}

void LC_ServerMapLogic_AutoSpawnPlay::ActiveSpawnPoint(int lSpawnPointID, object_id_type id)
{
	DungeonConfigData::AutoSpawnMonsterDataMapIter IterFind = m_Config.find(lSpawnPointID);
	if(IterFind == m_Config.end())
	{
		return;
	}
	DungeonConfigData::AutoSpawnMonsterData& rkSpawnData = IterFind->second;

	rkSpawnData._objectLogicIDs.erase(id);

	if (rkSpawnData._objectLogicIDs.empty())
	{
	
		rkSpawnData._lastActiveTime = (int)GET_PROCESS_TIME;
		rkSpawnData._lastUpdateTime = rkSpawnData._lastActiveTime;
		rkSpawnData._objectLogicIDs.clear();;
	}
}

