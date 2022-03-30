#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "UT_CmdLine.h"
#include "LC_CommandParserBase.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_DebugProtocol.h"
#include "LC_WorldManagerBase.h"
#include "GlobalSettings.h"
#include "oss_define.h"

using namespace NetWork;
using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DEBUG_COMMANDLINE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;
	if (GetGlobalSetting.nGMLevel == 0 && 0 >= pkPlayer->GetGMLevel())
		return;
	MG_REQ_DebugCommandLine reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		//处理一条指令
		GfxWriteFmtLog(LOG_GM_FILELINE, LOG_SWITCH_GM, "clientCommandHandlerReq", pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName().c_str(), reqMsg.m_strCommandLine.c_str(), pkPlayer->GetGMPriority(),pkPlayer->GetUserID());
		Utility::UT_CommandLine  cmdLine;
		cmdLine.SplitLine(STDSTR2TPSTR(reqMsg.m_strCommandLine));
		Utility::DebugCommandResult kResult = Utility::LC_CommandParserBase::GetSingletonPtr()->ParseCommand(cmdLine , pkPlayer->GetGMPriority());
		GfxWriteFmtLog(LOG_GM_FILELINE, LOG_SWITCH_GM, "clientCommandHandlerResp", pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName().c_str(), reqMsg.m_strCommandLine.c_str(), kResult.uiRslt, kResult.rsltMsg.c_str());
		oss_role_gm_command(pkPlayer, 0, cmdLine.GetCommandName(), reqMsg.m_strCommandLine.c_str(), kResult.uiRslt);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DEBUG_ACTOR_AI_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DebugActorAIInfo kReq;
	if (kReq.Decode(pMsgBody, usBodySize))
	{
		object_id_type lPlayerID = kReq.m_lPlayerID;
		object_id_type lActorID = kReq.m_lTargetID;
		if (!IS_NPC_ID(lActorID))
		{
			return;
		}
		LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
		LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(lActorID);
		if (NULL == pkActor)
		{
			return;
		}
		LC_ActorHatredList* pkHatredList = pkActor->GetHatredList();

		MG_RLT_DebugActorAIInfo kRlt;
		kRlt.m_lPlayerID = lPlayerID;
		kRlt.m_lTargetID = lActorID;
		LC_ActorHatredEntry* pkHatredEntry = NULL;
		for (int i = 0; i < pkHatredList->GetHatredCount(); ++i )
		{
			pkHatredEntry = pkHatredList->GetSortHatredByIndex(i);
			if (pkHatredEntry)
			{
				kRlt.m_alHatredList[i] = pkHatredEntry->GetTargetID();
			}
		}
		pkPlayer->SendMsgToClient(MGPT_RLT_DEBUG_ACTOR_AI_INFO, &kRlt);
	}
}
}
