#include "GameServerApp.h"
#include "LC_ServerPlayerMonitor.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"

using namespace Utility;
using namespace CSVFile;
using namespace GameLogic;

LC_BehaviorInfo::LC_BehaviorInfo(): m_sType(0), m_fTimestamp(0.0f)
{
}

LC_BehaviorInfo::~LC_BehaviorInfo()
{
}


LC_PlayerBehavior::LC_PlayerBehavior()
{
	m_lPlayerID = 0;
	m_iCount = 0;
}

LC_PlayerBehavior::~LC_PlayerBehavior()
{

}

void LC_PlayerBehavior::Clear( void )
{
	m_lPlayerID = 0;
	m_iCount = 0;
}

void LC_PlayerBehavior::SetID( object_id_type lID )
{
	m_lPlayerID = lID;
}

object_id_type LC_PlayerBehavior::GetID( void )
{
	return m_lPlayerID;
}

bool LC_PlayerBehavior::Insert( const LC_BehaviorInfo& rkInfo )
{
	if (m_iCount < MAX_PLAYER_BEHAVIOR_INFO_COUNT)
	{
		m_akBehaviorInfo[m_iCount] = rkInfo;
		++m_iCount;
		return true;
	}
	return false;
}

float LC_PlayerBehavior::GetBehaviorFirstTime()
{
	if(m_iCount < 1)
	{
		return 0.0f;
	}
	return m_akBehaviorInfo[0].m_fTimestamp;
}

bool LC_PlayerBehavior::Result( float fNow )
{
	bool bCheckRule = false;
	CF_Rule* pkCSVRule = CF_Rule::GetSingletonPtr();
	CF_Rule::DataEntryMapExternIterator kIter = pkCSVRule->GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_Rule::DataEntry* pkData = kIter.PeekNextValuePtr();
		{
			bCheckRule = _checkRule(pkData);
			if(bCheckRule)
			{
				int lUserID = 0;
				int lPlayerLevel = 0;
			
				LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
				LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(m_lPlayerID);
				if(pkPlayer)
				{
					lUserID = pkPlayer->GetUserID();
					lPlayerLevel = pkPlayer->GetLevel();
				}

				//处理玩家
				switch(pkData->_lHandleType)
				{
				case 0:
					break;
				case 1:
					{
						if(pkPlayer)
						{
							LC_ServerPlayerEnterLeaveManager* pkServerPlayerEnterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
							pkServerPlayerEnterLeaveManager->UserLeaveServer(pkPlayer,LMIOT_KICKOFF);
						}
					}
					break;
				default:
					break;
				}
				return true;
			}
		}
		kIter.MoveNext();
	}
	return false;
}

bool LC_PlayerBehavior::_checkRule(CSVFile::CF_Rule::DataEntry* pkRuleData)
{
	if(m_iCount <= 0 || m_iCount > MAX_PLAYER_BEHAVIOR_INFO_COUNT)
	{
		return false;
	}
	float fFirstBehaviorTime = -999.0f;
	float fLastBehaviorTime = 0.0f;
	int	  iBehaviorCount = 0;

	for(int i = 0; i < (int)m_iCount; i++)
	{
		LC_BehaviorInfo& rkBehaviorInfo = m_akBehaviorInfo[i];
		if(rkBehaviorInfo.m_sType == pkRuleData->_lBehaviorType)
		{
			if(rkBehaviorInfo.m_fParameter1 >= pkRuleData->_fBehaviorParameterMinValue && rkBehaviorInfo.m_fParameter1 <= pkRuleData->_fBehaviorParameterMaxValue)
			{
				if(fFirstBehaviorTime < 0.0f)
					fFirstBehaviorTime = rkBehaviorInfo.m_fTimestamp;

				fLastBehaviorTime = rkBehaviorInfo.m_fTimestamp;

				iBehaviorCount++;
			}
		}
	}

	if(iBehaviorCount >= pkRuleData->_lBehaviorCount)
	{
		float fDeltaTime = fLastBehaviorTime - fFirstBehaviorTime;
		if(fDeltaTime > 0.0f && fDeltaTime <= pkRuleData->_fBehaviorDuringTime)
		{
			return true;
		}
	}
	return false;
}

LC_ServerPlayerMonitor::LC_ServerPlayerMonitor()
{
	m_fLastCheckTime = 0.0f;
	m_pkPlayerBehaviorPool = T_NEW_D Utility::UT_MemoryPool<LC_PlayerBehavior>;
	m_pkPlayerBehaviorPool->Init(300,10);
	m_kPlayerBehaviorMap.clear();
}

LC_ServerPlayerMonitor::~LC_ServerPlayerMonitor()
{
	T_SAFE_DELETE(m_pkPlayerBehaviorPool);
	LC_PlayerBehaviorMapIter IterBegin = m_kPlayerBehaviorMap.begin();
	LC_PlayerBehaviorMapIter IterEnd = m_kPlayerBehaviorMap.end();
	for(; IterBegin != IterEnd; ++IterBegin)
	{
		LC_PlayerBehavior* pkPlayer = (*IterBegin).second;
		T_DELETE_D pkPlayer;
	}
	m_kPlayerBehaviorMap.clear();
}

LC_PlayerBehavior* LC_ServerPlayerMonitor::CreatePlayer( object_id_type lID )
{
	LC_PlayerBehavior* pkPlayer = NULL;

	LC_PlayerBehaviorMapIter IterFind = m_kPlayerBehaviorMap.find(lID);
	if (m_kPlayerBehaviorMap.end() == IterFind)
	{
		pkPlayer = m_pkPlayerBehaviorPool->Alloc();
		if(NULL == pkPlayer)
		{
			return NULL;
		}
		pkPlayer->Clear();
		pkPlayer->SetID(lID);
		m_kPlayerBehaviorMap[lID] = pkPlayer;
	}
	else
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"create player with same id! id:%d", lID);
		pkPlayer = NULL;
	}
	return pkPlayer;
}

void LC_ServerPlayerMonitor::DestroyPlayer( LC_PlayerBehavior* pkPlayer)
{
	object_id_type lID = pkPlayer ? pkPlayer->GetID() : INVALID_ACTOR_ID;
	LC_PlayerBehaviorMapIter IterFind = m_kPlayerBehaviorMap.find(lID);
	if (IterFind != m_kPlayerBehaviorMap.end())
	{
		m_kPlayerBehaviorMap.erase(IterFind);
		m_pkPlayerBehaviorPool->Free(pkPlayer);
	}
}

LC_PlayerBehavior* LC_ServerPlayerMonitor::FindPlayer( object_id_type lID )
{
	LC_PlayerBehaviorMapIter IterFind = m_kPlayerBehaviorMap.find(lID);
	return (IterFind!=m_kPlayerBehaviorMap.end()) ? (*IterFind).second : NULL;
}

void LC_ServerPlayerMonitor::Update( float fNow )
{
//#pragma TODO("为机器人搞得，记得去掉")
	if ((fNow - m_fLastCheckTime) < 15.0f)
	{
		return;
	}

	m_fLastCheckTime = fNow;

	LC_PlayerBehaviorMapIter IterBegin = m_kPlayerBehaviorMap.begin();
	LC_PlayerBehaviorMapIter IterEnd = m_kPlayerBehaviorMap.end();

	bool bClear = false;
	for(; IterBegin != IterEnd; )
	{
		LC_PlayerBehavior* pkPlayerBehavior = (*IterBegin).second;
		bClear = pkPlayerBehavior->Result(fNow);
		
		if(!bClear)
		{
			float fBehaviorFirstTime = pkPlayerBehavior->GetBehaviorFirstTime();
			if(fNow - fBehaviorFirstTime > 60.0f)
				bClear = true;
		}

		if(!bClear)
		{
			LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
			LC_PlayerBase* pkPlayer = pkServerPlayerManager->FindPlayer(pkPlayerBehavior->GetID());
			if(NULL == pkPlayer)
				bClear = true;
		}

		if (bClear)
		{
			m_kPlayerBehaviorMap.erase(IterBegin++);
			m_pkPlayerBehaviorPool->Free(pkPlayerBehavior);
		}
		else
		{
			++IterBegin;
		}
	}
}

bool LC_ServerPlayerMonitor::AddOneBehavior( object_id_type lID, const LC_BehaviorInfo& rkInfo )
{
	LC_PlayerBehavior* pkPlayer = FindPlayer(lID);
	if (NULL == pkPlayer)
	{
		pkPlayer = CreatePlayer(lID);
		if (NULL == pkPlayer)
		{
			return false;
		}
	}
	return pkPlayer->Insert(rkInfo);
}


