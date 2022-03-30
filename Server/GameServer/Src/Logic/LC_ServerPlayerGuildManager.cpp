

#include "LC_ServerPlayerGuildManager.h"

using namespace GameLogic;

//LC_ServerPlayerGuildManager::LC_ServerPlayerGuildManager()
//{	
//	m_kServerPlayerGuildPool.Init(50,10);
//}
//
//LC_ServerPlayerGuildManager::~LC_ServerPlayerGuildManager()
//{
//	PlayerGuildIDPtrMapIter IterBe = m_kPlayerGuildIDPtrMap.begin();
//	PlayerGuildIDPtrMapIter IterEd = m_kPlayerGuildIDPtrMap.end();
//
//	for( ; IterBe != IterEd; ++IterBe)
//	{
//		LC_PlayerGuildBase* pkPlayerGuildBase = (*IterBe).second;
//		_freePlayerGuild(pkPlayerGuildBase);
//	}
//}
//
//LC_ServerPlayerGuild* LC_ServerPlayerGuildManager::_allocPlayerGuild()
//{
//	return m_kServerPlayerGuildPool.Alloc();
//}
//
//void LC_ServerPlayerGuildManager::_freePlayerGuild(LC_PlayerGuildBase* pkPlayerGuild)
//{	
//	m_kServerPlayerGuildPool.Free( static_cast < LC_ServerPlayerGuild* > ( pkPlayerGuild ) );
//}
