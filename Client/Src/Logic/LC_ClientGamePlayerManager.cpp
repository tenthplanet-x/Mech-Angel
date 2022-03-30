#include "ClientPCH.h"

#include "MG_MessageBase.h"

#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientGamePlayer.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
//--------------------------------------------------------
LC_ClientGamePlayerManager::LC_ClientGamePlayerManager()
: LC_PlayerManagerBase()
{
	m_pkClientGamePlayerPool = T_NEW_D Utility::UT_MemoryPool<LC_ClientGamePlayer>;
	m_pkClientGamePlayerPool->Init(1,1);
}
//--------------------------------------------------------
LC_ClientGamePlayerManager::~LC_ClientGamePlayerManager()
{
	T_SAFE_DELETE(m_pkClientGamePlayerPool);
}
//--------------------------------------------------------
void LC_ClientGamePlayerManager::Update( float fCurrentTime,float fDeltaTime)
{
	T_PROFILE_EX("Update::LC_ClientGamePlayerManager", 5.0f );
	LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter IterEd = m_kPlayerIDPtrMap.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		LC_PlayerBase* pkPlayer = (*IterBe).second;
		pkPlayer->Update(fCurrentTime,fDeltaTime);
	}
}
//------------------------------------------------------------------------
void LC_ClientGamePlayerManager::Reset()
{
	LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin();
	LC_PlayerIDPtrMapIter IterEd = m_kPlayerIDPtrMap.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		LC_PlayerBase* pkPlayer = (*IterBe).second;
		pkPlayer->DestoryVisibleData();
		_freePlayer(pkPlayer);
	}

	m_kPlayerIDPtrMap.clear();
	m_kPlayerUIDPtrMap.clear();
}

GameLogic::LC_PlayerBase* LC_ClientGamePlayerManager::CreatePlayer(object_id_type objectID, const unique_id_impl& charID, int32_t lCharType, uint32_t bits/*=PLY_DEF_INSTANCE_BITS*/)
{
	this->Reset();

	return LC_PlayerManagerBase::CreatePlayer(objectID, charID, lCharType, bits);
}

void LC_ClientGamePlayerManager::DestroyPlayer(LC_PlayerBase* pkPlayer)
{
	if (pkPlayer)
	{
		object_id_type iID = pkPlayer->GetSelfID();
		if( pkPlayer->IsInstance() )
		{
			m_kPlayerUIDPtrMap.erase(pkPlayer->GetInstance());
			m_kPlayerIDPtrMap.erase(iID);
		}
		else
		{
			m_kPlayerCloneIDPtrMap.erase(iID);
		}
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mgr destroy player", pkPlayer->GetID(), pkPlayer->GetInstance(), pkPlayer->IsInstance());
		pkPlayer->DestoryVisibleData();
		_freePlayer(pkPlayer);
	}
	else
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "mgr destroy player null");
	}
}
//--------------------------------------------------------
LC_PlayerBase* LC_ClientGamePlayerManager::_allocPlayer()
{
	return m_pkClientGamePlayerPool->Alloc();
}
//--------------------------------------------------------
void LC_ClientGamePlayerManager::_freePlayer( LC_PlayerBase* pkPlayer )
{
	m_pkClientGamePlayerPool->Free((LC_ClientGamePlayer*)pkPlayer);
}


