

#include "TS_TranDonateGuildMaterials.h"

#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"

#include "MG_CS_Guild.h"
#include "MG_Game2Guild.h"

#include "GameServerApp.h"

using namespace GameLogic;
using namespace Protocol;

TS_Server_ReqDonateGuildMaterials::TS_Server_ReqDonateGuildMaterials()
{
	m_uiTypeID = 0;
	m_uiNumber = 0;
}

emTranOperate TS_Server_ReqDonateGuildMaterials::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	return TRAN_OP_NEXT;
}

void TS_Server_ReqDonateGuildMaterials::RespondRltMsg()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		MG_RLT_DonateGuildMaterials kRltMsg;
		kRltMsg.m_uiCharID = GetCharID();
		kRltMsg.m_uiResult = GetResultID();
		pkPlayer->SendMsgToClient(MGPT_RLT_DONATE_GUILD_MATERIALS, &kRltMsg);		
	}	
}

void TS_Server_ReqDonateGuildMaterials::Rollback()
{
	if ( GetContentFlag() )
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Donated material exception, data UserID:%d, CharID: %llu",  GetUserID(), GetCharID() );
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Cost material:%d, Consumption quantity:%d, Contribution degree:%d", m_uiTypeID, m_uiNumber, m_uiResearch );
	}
}

void TS_Server_ReqDonateGuildMaterials::Reset()
{
	TS_AtomOperate::Reset();
	m_uiTypeID = 0;
	m_uiNumber = 0;
	m_uiResearch = 0;
}

//////////////////////////////////////////////////////////////////////////
emTranOperate TS_Server_RltDonateGuildMaterials::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
//	MG_SS_RLT_ModifySkillResearch* pkModifySkillResearch = static_cast<MG_SS_RLT_ModifySkillResearch*>(pkMsg);
//	if( !pkModifySkillResearch )
//	{
//		SetResultID(RE_FAIL);
//		return TRAN_OP_FAILED;
//	}
//	
////!!!
//
//	if (GUILD_SUCCESS == pkModifySkillResearch->m_usResult)
//	{
//		SetResultID(RE_GUILD_DONATE_MATERIALS_SUCCESS);				
//		RespondRltMsg();
//		SendMsgToSociety( MGPT_GGU_REQ_GUILD_SKILL, NULL, GetUserID(), GetCharID(), 0 );
//
//		return TRAN_OP_NEXT;	
//	}
//	else
//	{		
//		SetResultID(RE_FAIL);
//
//		return TRAN_OP_FAILED;
//	}	
	return TRAN_OP_NEXT;
}

void TS_Server_RltDonateGuildMaterials::RespondRltMsg()
{	
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		MG_RLT_DonateGuildMaterials kRltMsg;
		kRltMsg.m_uiCharID = GetCharID();
		kRltMsg.m_uiResult = GetResultID();

		pkPlayer->SendMsgToClient(MGPT_RLT_DONATE_GUILD_MATERIALS, &kRltMsg);	
	}
}
//////////////////////////////////////////////////////////////////////////
TS_Server_DonateGuildMaterialsTran::TS_Server_DonateGuildMaterialsTran()
{
	m_kStepTranMap[STEP_REQ_GGU_DONATE_GUILDMATERIALS] = T_NEW_D TS_Server_ReqDonateGuildMaterials;
	m_kStepTranMap[STEP_RLT_GGU_DONATE_GUILDMATERIALS] = T_NEW_D TS_Server_RltDonateGuildMaterials;
}

TS_Server_DonateGuildMaterialsTran::~TS_Server_DonateGuildMaterialsTran()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}
