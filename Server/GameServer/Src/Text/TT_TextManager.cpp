#include "MG_Common.h"
#include "TT_TextManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"

using namespace Utility;
using namespace Protocol;
//-------------------------------------------------------------------
TT_TextManager::TT_TextManager()
{
}
//-------------------------------------------------------------------
TT_TextManager::~TT_TextManager()
{
}
//-------------------------------------------------------------------
void TT_TextManager::Update(float fCurrentTime)
{
	UT_AutoLock kLock(&m_kLock);
	
	LC_ServerWorldManager* pkWorldManager =	SERVER_GET_WORLD_MANAGER();

	StringListIter Iter;
	TRAV(m_kServerAnnoceMsgList,Iter)
	{
		MG_SystemAnnounce kMsg;
		const StringType & kAnnounce = *Iter;
		TPSTR2STDSTR(kAnnounce, kMsg.m_szSystemAnnounce);
		pkWorldManager->BroadCast(MGPT_SYSTEM_ANNOUNCE,&kMsg);
	}	

	m_kServerAnnoceMsgList.clear();
}
//-------------------------------------------------------------------
void TT_TextManager::AddServerAnnoceMsg(const StringType& rkMsg)
{
	//¡Ÿ ±µƒ√¸¡Ó
	UT_AutoLock kLock(&m_kLock);
	m_kServerAnnoceMsgList.push_back(rkMsg);
}