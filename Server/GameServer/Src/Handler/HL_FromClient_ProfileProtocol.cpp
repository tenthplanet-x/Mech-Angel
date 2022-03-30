#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "UT_TimeManager.h"
#include "MG_Stream.h"
#include "MG_Profile.h"
#include "MG_PB.h"
#include "msg.pb.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

ON_HANDLE(MGPT_REQ_CHAR_PROFILE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_Char_Profile kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	// 查询是否过于频繁
	float fTime = GET_PROCESS_TIME;
	if (fTime - pkPlayer->GetLastProfileTime() < 2.0f)
	{
		return;
	}
	else
	{
		pkPlayer->SetLastProfileTime(fTime);
	}
	SendMsgToLogin(MGPT_GP_REQ_ROLE_PROFILE_INFO, &kMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, LC_SequenceManager::Instance().GetSequenceID());
}

}
