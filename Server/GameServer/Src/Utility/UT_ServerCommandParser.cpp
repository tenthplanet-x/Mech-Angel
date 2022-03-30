#include "UT_ServerCommandParser.h"
#include "MG_DebugProtocol.h"

#include "TS_TranManager.h"
#include "LC_SequenceManager.h"
#include "TS_GMRequest.h"

using namespace Utility;
//---------------------------------------------------
UT_ServerCommandParser::UT_ServerCommandParser() : LC_CommandParserBase("ServerDebugScriptSample.lua")
{
}
//---------------------------------------------------
void UT_ServerCommandParser::ParseCommand( NetWork::NW_NetSessionBase* pkSession , const UT_CommandLine& rkCommand , uint32_t uiPri, int64_t msg_id, redis_define::gm_msg_req* preq)
{
	LC_CommandParserBase::ParseCommand(rkCommand, uiPri, msg_id, preq);
}
//---------------------------------------------------
TS_Server_GM* UT_ServerCommandParser::GetGMTrans(void)
{
	TS_Server_GM* pkTran = NULL;
	static TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	int64_t msg_idx = 0;
	redis_define::gm_msg_req kreq;
	if (GetChannelReq(msg_idx, kreq))
	{
		pkTran = (TS_Server_GM*)pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_GM, INVALID_USER_ID, null_unique_impl);
		if (pkTran)
		{
			pkTran->SetChannelReq(msg_idx, kreq);
		}
	}
	return pkTran;
}
//---------------------------------------------------
TS_Server_GM* UT_ServerCommandParser::GetGuildGMTrans(void)
{
	TS_Server_GM* pGM = GetGMTrans();
	if(pGM != NULL)
	{
		pGM->AddStep(T_NEW_D AO_Server_QueryGuildRlt());
	}
	return pGM;
}

TS_Server_GM* UT_ServerCommandParser::GetRandomGuildNameGMTrans(void)
{
	TS_Server_GM* pGM = GetGMTrans();
	if(pGM != NULL)
	{
		pGM->AddStep(T_NEW_D AO_Server_RandomGuildNameRlt());
	}
	return pGM;
}

TS_Server_GM* UT_ServerCommandParser::GetCommGMTrans(void)
{
	TS_Server_GM* pGM = GetGMTrans();
	if(pGM != NULL)
	{
		pGM->AddStep(T_NEW_D AO_Server_CommGMRlt());
	}
	return pGM;
}
