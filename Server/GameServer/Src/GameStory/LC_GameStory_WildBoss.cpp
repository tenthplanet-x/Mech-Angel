#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "LC_GameEvent.h"
#include "LC_GameStory_WildBoss.h"
#include "LC_ServerMap.h"

#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerNPC.h"

#include "MG_Game2DB.h"
#include "NW_Helper.h"

#include "GameServerApp.h"
#include "LC_ServerPlayerGroupManager.h"

#include "UT_Timer.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_GameStory_Manager.h"
#include "UT_MathBase.h"

using namespace GameLogic;
using namespace GameStory;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

/******************************* Class Implement *****************************************/

//---------------------------------------------------------------
LC_Server_GameStory_WildBoss::LC_Server_GameStory_WildBoss(int32_t logicID, GameStoryRecord *pkRecord)
:LC_Server_GameStory_Base(logicID, pkRecord)
,m_bTipsRemaindFiveMinutesFlag(false)
,m_bTipsRemaindOneMinutesFlag(false)
{
}

//----------------------------------------------------------------
LC_Server_GameStory_WildBoss::~LC_Server_GameStory_WildBoss()
{
	
}
//-------------------------------------------------------------------
void LC_Server_GameStory_WildBoss::_onActived ()
{
	m_bTipsRemaindFiveMinutesFlag	=	false;
	m_bTipsRemaindOneMinutesFlag	=	false;	
	LC_Server_GameStory_Base::_onActived();
}
//-----------------------------------------------------
void LC_Server_GameStory_WildBoss::_onDeactived ()
{
	m_bTipsRemaindFiveMinutesFlag	=	false;
	m_bTipsRemaindOneMinutesFlag	=	false;
	LC_Server_GameStory_Base::_onDeactived();
}

//---------------------------------------------------------------------
void LC_Server_GameStory_WildBoss::Init()
{
	LC_Server_GameStory_Base::Init();	
}

//---------------------------------------------------------------------
void LC_Server_GameStory_WildBoss::UnInit()
{
	LC_Server_GameStory_Base::UnInit();
}

//----------------------------------------------------------------------------------------------------------------------
void LC_Server_GameStory_WildBoss::ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured)
{
	GameStoryConfig *pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}

	LC_Server_GameStory_Base::ActorBeKilled(pkKiller, pkInjured);
}
//----------------------------------------------------------------------------------------------------------------------
void LC_Server_GameStory_WildBoss::ActorBeDamaged(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, attr_value_type dmg)
{
	LC_Server_GameStory_Base::ActorBeDamaged(pkAttacker, pkInjured, iMapLogicID, dmg);
}
