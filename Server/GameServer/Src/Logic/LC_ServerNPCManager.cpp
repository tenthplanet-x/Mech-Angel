#include "LC_ServerNPCManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerMap.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerNPCInteractionManager.h"
#include "GameServerApp.h"
#include "LC_ServerMapLogic_AutoSpawnPlay.h"
#include "LC_LogicIDManager.h"
#include "UT_GameServerProfile.h"

#ifdef WIN32
#include "XDebugDumper.h"
#endif

using namespace GameLogic;
using namespace Utility;
using namespace Protocol;
using namespace StateMachine;
//--------------------------------------------------------

LC_ServerNPCManager::LC_ServerNPCManager()
 : LC_NPCManagerBase()
 , m_kNPCIDPtrMapCacheForEnable(100)
 , m_kNPCIDPtrMapCacheForDisable(32000)
{
	const Utility::MemPoolConfigEntry & plc = GetGlobalSetting.poolNPC;

	m_pkServerNPCPool = T_NEW_D Utility::UT_MemoryPool<LC_ServerNPC>;
	m_pkServerNPCPool->Init(plc.InitialCount, plc.IncCount, "LC_ServerNPCManager.m_pkServerNPCPool");
	m_pkServerNPCPool->EnableAutoDec(plc.AutoDec, plc.AutoDecInterval, plc.AutoDecCount, plc.AutoDecMinCount);
	m_nNPCKilledCount = 0;

	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_IMM));
}
//--------------------------------------------------------
LC_ServerNPCManager::~LC_ServerNPCManager()
{
	T_SAFE_DELETE(m_pkServerNPCPool);
}

int LC_ServerNPCManager::OnEnableNPCUpdateFlag( LC_ServerNPC* pNPC )
{
	//return true;
	if ( pNPC == NULL )
		return false;
	object_id_type nObjId = pNPC->GetID();

	m_kNPCIDPtrMapCacheForEnable.insert(std::make_pair( nObjId, pNPC ));
	m_kNPCIDPtrMapCacheForDisable.erase(nObjId);


	return true;
}

int LC_ServerNPCManager::OnDisableNPCUpdateFlag( LC_ServerNPC* pNPC )
{
	//return true;
	if ( pNPC == NULL || pNPC->GetSaveDamageDataFlag())
		return false;
	object_id_type nObjId = pNPC->GetID();

	m_kNPCIDPtrMapCacheForDisable.insert(std::make_pair( nObjId, pNPC ));
	m_kNPCIDPtrMapCacheForEnable.erase(nObjId);

	return true;
}


StringType	LC_ServerNPCManager::GetRTInfo(void)
{
	StringStreamType sStream;
	sStream << "----[LC_ServerNPCManager Begin]----\n";
	sStream << m_pkServerNPCPool->GetRTInfo();
	sStream << "m_nNPCKilledCount:[" << m_nNPCKilledCount << "]\n";
	sStream << "----[LC_ServerNPCManager End]----\n";
	return sStream.str();
}
//--------------------------------------------------------
static void UpdateNPC_SEH_Shell(LC_NPCBase * pNPC,float fCurrentTime, float fDeltaTime)
{
//#ifdef WIN32
#if 0
	__try
	{
		pNPC->Update(fCurrentTime,fDeltaTime);
	}
	__except(CrashHandler(GetExceptionInformation()))
	{
	}
#else
	pNPC->Update(fCurrentTime,fDeltaTime);
#endif
}
//--------------------------------------------------------
void LC_ServerNPCManager::Update( float fCurrentTime,float fDeltaTime )
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}

	_refreshEnableDisableObj();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerNPCInteractionManager* pkInterManager = 
		(LC_ServerNPCInteractionManager*)LC_ServerNPCInteractionManager::GetSingletonPtr();
	//更新NPC状态机
	LC_NPCIDSetIter IterBe = m_kNPCIDSet.begin();
	LC_NPCIDSetIter IterEd = m_kNPCIDSet.end();
	int32_t nActivateCount = 0;
	int32_t nDeactivateCount = 0;
	int32_t nDeadCount = 0;
	int32_t nWaitSpawnCount = 0;
	int32_t nInsideHookCount = 0;
	int32_t nTotalSize = m_kNPCIDSet.size();
	bool bUpdateNPC = false;
	for( ; IterBe != IterEd;)
	{
		//LC_NPCBase* pkNPC = IterBe->second;
		object_id_type nCurNPCId = *IterBe;
		LC_NPCBase* pkNPC = FindNPC(nCurNPCId);
		if ( pkNPC == NULL )
		{
			m_kNPCIDSet.erase(IterBe++);
			continue;
		}
		if (pkNPC->GetDead())
		{
			LC_ServerMap* pkMap = (LC_ServerMap*)(pkNPC->GetMap());
			SectorIndex dwSectorIndex = pkNPC->GetSectorIndex();
			//m_kNPCIDPtrMap.erase(IterBe++);
			m_kNPCIDPtrMap.erase(nCurNPCId);
			m_kNPCIDSet.erase(IterBe++);

			int mapLogicID = 0;
			MG_RLT_NPCDead rltMsg;
			rltMsg.m_lNPCID = pkNPC->GetID();
			rltMsg.m_lDeadPlayFlag = pkNPC->GetDeadAnimationPlay();
			if(pkMap)
			{
				mapLogicID = pkMap->GetMapLogicID();
				pkMap->BroadCastToArea(dwSectorIndex, MGPT_RLT_NPC_DEAD, &rltMsg);
			}

			if(pkNPC->GetSpawnPointID() > 0)
			{
				LC_ServerMapLogic_Place* pkMapLogic = pkWorldManager->GetMapLogic(pkNPC->GetMapLogicID());
				if(pkMapLogic!=NULL)
				{
					LC_ServerMapLogic_AutoSpawnPlay* play=(LC_ServerMapLogic_AutoSpawnPlay*)pkMapLogic->GetPlay(PlayType_AutoSpawn);
					if (play!=NULL)
					{
						play->ActiveSpawnPoint(pkNPC->GetSpawnPointID(), pkNPC->GetID());
					}
				}
			}
			pkWorldManager->RemoveObjectFromMap(pkNPC);	

			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(mapLogicID);
			if (NULL != pkPlace)
			{
				UT_ServerHelper::BroadcastNpcInfoToPlayer(pkPlace->GetStoryID());
			}

			int32_t _idx = _NPC_ID_2_INDEX(nCurNPCId);
			if (_idx>=0)
			{
				m_kNPCIndices[_idx] = NULL;
			}
			_freeNPC(pkNPC);
			nDeadCount++;
			continue;
		}

		if (pkNPC->GetSurvivalTime() > 0 && (fCurrentTime - pkNPC->GetSurvivalStartTime() > pkNPC->GetSurvivalTime()) && pkNPC->GetHP()>0)//处理生存周期到的问题
		{
			pkNPC->SetAttackerID(0);
			pkNPC->SetHP(0);
			nWaitSpawnCount++;
			++IterBe;
			continue;
		}
		
		if (pkNPC->CanInteraction())
			pkInterManager->CheckInteractionState((LC_ServerNPC*)pkNPC);
		/*
		bUpdateNPC = false;
		int lMapLogicType = pkNPC->GetMapLogicType();
		if( (pkNPC->GetCurrentStateType() == ST_NPC_IDLE ||
			pkNPC->GetCurrentStateType() == ST_NONE ) && 
			IS_COMMON_MAP(lMapLogicType) &&
			//(MT_NORMAL==lMapLogicType||MT_NEWBEE==lMapLogicType) &&
			pkNPC->GetSurvivalTime() <= 0)
		{
// 			if(pkNPC->GetUpdateFlag()) 
// 				bUpdateNPC = true;
			bUpdateNPC = pkNPC->GetUpdateFlag();
		}
		else if (MT_HOOK_MAP==lMapLogicType)
		{
			bUpdateNPC = pkNPC->GetUpdateFlag();
			//nInsideHookCount++;
			//if ( pkNPC->GetUpdateFlag() )
			//	nHookCount++;
		}
		else
		{
			bUpdateNPC = true;
		}
		/*/
		bUpdateNPC = pkNPC->GetUpdateFlag();
		//*/
		if(bUpdateNPC || pkNPC->CheckForceUpdate())
		{
			//pkNPC->Update(fCurrentTime,fDeltaTime);
			UpdateNPC_SEH_Shell(pkNPC, fCurrentTime, fDeltaTime);
			nActivateCount++;
		}
		else
		{
			pkNPC->SetLastTranslateTime(fCurrentTime);
			nDeactivateCount++;
		}
		++IterBe;
	}


	//< 更新对象池
	m_pkServerNPCPool->Update(fCurrentTime);

	//< 调试代码，汇报Active的NPC数量
	static float s_fLastLogTime = 0;
	if (fCurrentTime - s_fLastLogTime > 10.0f)
	{
		s_fLastLogTime = fCurrentTime;
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "%s, ActiveNpcCount = %d", __FUNCTION__, nTotalSize);
	}
}

void LC_ServerNPCManager::_refreshEnableDisableObj()
{
	LC_NPCIDPtrMap::iterator itEnable = m_kNPCIDPtrMapCacheForEnable.begin();
	for ( ; itEnable != m_kNPCIDPtrMapCacheForEnable.end(); ++itEnable )
	{
		//m_kNPCIDPtrMap.insert( std::make_pair( itEnable->first, itEnable->second ) );
		m_kNPCIDSet.insert( itEnable->first );
	}
	m_kNPCIDPtrMapCacheForEnable.clear();

	LC_NPCIDPtrMap::iterator itDisable = m_kNPCIDPtrMapCacheForDisable.begin();
	for ( ; itDisable != m_kNPCIDPtrMapCacheForDisable.end(); ++itDisable )
	{
		//m_kNPCIDPtrMap.erase( itDisable->first );
		m_kNPCIDSet.erase( itDisable->first );
	}
	m_kNPCIDPtrMapCacheForDisable.clear();
}

//--------------------------------------------------------
LC_NPCBase* LC_ServerNPCManager::_allocNPC()
{
	return m_pkServerNPCPool->Alloc();
}
//--------------------------------------------------------
void LC_ServerNPCManager::_freeNPC( LC_NPCBase* pkNPC )
{
	LC_LogicIDManager::GetSingletonPtr()->ReleaseLogicID(LOT_NPC,pkNPC->GetID());

	pkNPC->ReUse();
	m_pkServerNPCPool->Free((LC_ServerNPC*)pkNPC);
}


//--------------------------------------------------------
LC_NPCBase* LC_ServerNPCManager::CreateNPC(object_id_type lID,int lCharType)
{
	return LC_NPCManagerBase::CreateNPC(lID,lCharType);
}
void LC_ServerNPCManager::DestoryNPC(LC_NPCBase* pkNPC)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if(pkNPC && pkNPC->GetSpawnPointID() > 0)
	{
		LC_ServerMapLogic_Place* pkMapLogic = pkWorldManager->GetMapLogic(pkNPC->GetMapLogicID());
		if(pkMapLogic)
		{
			LC_ServerMapLogic_AutoSpawnPlay* play = (LC_ServerMapLogic_AutoSpawnPlay*)pkMapLogic->GetPlay(PlayType_AutoSpawn);
			if (play)
			{
				play->ActiveSpawnPoint(pkNPC->GetSpawnPointID(), pkNPC->GetID());
			}
		}
	}
	return LC_NPCManagerBase::DestoryNPC(pkNPC);
}