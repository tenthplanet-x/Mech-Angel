#include "TS_CreateGuildTran.h"

#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"
#include "LC_ServerGuildManager.h"

#include "MG_CS_Guild.h"
#include "MG_Game2Guild.h"

#include "GameServerApp.h"

#include "ConfigManager.h"
#include "GlobalSettings.h"

using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

//////////////////////////////////////////////////////////////////////////
emTranOperate AO_Server_ReqCreateGuild::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	MG_REQ_CreateGuild* pkReqCreateGuildMsg = static_cast<MG_REQ_CreateGuild*>(pkMsg);
	if( !pkReqCreateGuildMsg )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	if(pkReqCreateGuildMsg->m_uiCharID != pkPlayer->GetID())
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return TRAN_OP_FAILED;
	}

	CF_GuildCreateLimit::DataEntry* pCreateCfg = pCreateCfgMgr->getLimit();
	if(pCreateCfg == NULL)
	{
		return TRAN_OP_FAILED;
	}

	ResultType iResult = pkPlayer->CanCreateGuild(CodeConvert::Utf8ToWcs(pkReqCreateGuildMsg->m_szName.c_str()), pCreateCfg);
	if (RE_SUCCESS != iResult)
	{
		SetResultID(iResult);
		return TRAN_OP_FAILED;
	}

	// 1. 设置上下文
	{
		pkPlayer->ReduceCash(pCreateCfg->_iCash, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_CREATE, 0);
		m_llCash = pCreateCfg->_iCash;
		m_strGuildName = STDSTR2TPSTR(pkReqCreateGuildMsg->m_szName);

		// 发送消息
		MG_SS_REQ_Create_Guild kSGMsg;
		//STRNCPY( kSGMsg.m_szCharName, pkPlayer->GetOwnerCharName().c_str(), sizeof(kSGMsg.m_szCharName) );
		kSGMsg.m_szGuildName = pkReqCreateGuildMsg->m_szName;
		kSGMsg.m_GuildDeclaration = pkReqCreateGuildMsg->m_szGuildDeclaration;
		SendMsgToSociety( MGPT_GGU_REQ_CREATE_GUILD, &kSGMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance(), uiID );		

		SetContentFlag(true);
	}

	// 2. 组织协议发送给公会服务器

	SetResultID(RE_SUCCESS);
	return TRAN_OP_NEXT;
}

void AO_Server_ReqCreateGuild::RespondRltMsg()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		return;
	}
	else
	{
		MG_RLT_CreateGuild kRltMsg;
		
		kRltMsg.m_uiCharID = GetCharID();
		kRltMsg.m_uiResult = GetResultID();
	
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kRltMsg);		
	}
}

void AO_Server_ReqCreateGuild::Rollback()
{
	if ( GetContentFlag() )
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to create guild: Player",  GetCharID());
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Ready to create guild name:%s, spend money:%lld", m_strGuildName.c_str(), m_llCash);
	}
}

void AO_Server_ReqCreateGuild::Reset()
{
	TS_AtomOperate::Reset();
	m_llCash = 0;
	m_strGuildName = "";
}

//////////////////////////////////////////////////////////////////////////
emTranOperate AO_Server_RltCreateGuild::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{	
	MG_SS_RLT_Create_Guild* pkRltCreateGuildMsg = static_cast<MG_SS_RLT_Create_Guild*>(pkMsg);
	if( !pkRltCreateGuildMsg )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}
	return TRAN_OP_FAILED;
}

void AO_Server_RltCreateGuild::RespondRltMsg()
{
	MG_RLT_CreateGuild kRltMsg;
	kRltMsg.m_uiCharID = GetCharID();
	kRltMsg.m_uiResult = GetResultID();	

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kRltMsg);	
	}
}
//////////////////////////////////////////////////////////////////////////
TS_Server_CreateGuild::TS_Server_CreateGuild()
{
	m_kStepTranMap[STEP_REQ_GG_CRAETE_GUILD] = T_NEW_D AO_Server_ReqCreateGuild;
	m_kStepTranMap[STEP_RLT_GG_CREATE_GUILD] = T_NEW_D AO_Server_RltCreateGuild;
}

TS_Server_CreateGuild::~TS_Server_CreateGuild()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}
	m_kStepTranMap.clear();
}
//////////////////////////////////////////////////////////////////////////