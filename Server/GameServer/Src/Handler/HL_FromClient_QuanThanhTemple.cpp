#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "MG_CS_QuanThanhTemple.h"
#include "LC_ServerMapLogicBase.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

ON_HANDLE(MGPT_REQ_ADDHEALTH, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	if (pkPlayer->GetMapLogicID())
	{
		LC_ServerWorldManager* pkServerWorldMgr = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicBase* mapLogic = pkServerWorldMgr->GetMapLogic(pkPlayer->GetMapLogicID());
		if (!mapLogic)
		{
			return;
		}
	}
}

ON_HANDLE(MGPT_REQ_ADDFORCE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	if (pkPlayer->GetMapLogicID())
	{
		LC_ServerWorldManager* pkServerWorldMgr = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicBase* mapLogic = pkServerWorldMgr->GetMapLogic(pkPlayer->GetMapLogicID());
		if (!mapLogic)
		{
			return;
		}
	}

}

ON_HANDLE(MGPT_REQ_ADDTIME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	if (pkPlayer->GetMapLogicID())
	{
		LC_ServerWorldManager* pkServerWorldMgr = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicBase* mapLogic = pkServerWorldMgr->GetMapLogic(pkPlayer->GetMapLogicID());
		if (!mapLogic)
		{
			return;
		}
	}

}

ON_HANDLE(MGPT_REQ_CALLBOSS, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	if (pkPlayer->GetMapLogicID())
	{
		LC_ServerWorldManager* pkServerWorldMgr = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicBase* mapLogic = pkServerWorldMgr->GetMapLogic(pkPlayer->GetMapLogicID());
		if (!mapLogic)
		{
			return;
		}
	}

}

ON_HANDLE(MGPT_REQ_CHANGEELEMENT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	if (pkPlayer->GetMapLogicID())
	{
		LC_ServerWorldManager* pkServerWorldMgr = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicBase* mapLogic = pkServerWorldMgr->GetMapLogic(pkPlayer->GetMapLogicID());
		if (!mapLogic)
		{
			return;
		}
	}
}

}
