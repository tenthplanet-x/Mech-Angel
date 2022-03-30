

#include "TS_PayMailTran.h"

#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"
#include "GameServerApp.h"
#include "MG_Game2Mail.h"
using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

//////////////////////////////////////////////////////////////////////////
AO_Server_ReqPayMail::AO_Server_ReqPayMail()
: m_uiMailID(INVALID_MAIL_ID)
, m_llPayPrice(0)
{

}

emTranOperate AO_Server_ReqPayMail::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if(!pkPlayer)
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	MG_GM_RLT_MAIL_PAYPRICE* pkRltMailPayPriceMsg = static_cast<MG_GM_RLT_MAIL_PAYPRICE*>(pkMsg);
	m_uiMailID   = pkRltMailPayPriceMsg->m_uiMailID;
	
	if(!IS_VALID_MAIL_MONEY(pkRltMailPayPriceMsg->m_llPayPrice))
	{
		SetResultID(RE_PAYFORMAIL_FAIL_CASH_IS_NOT_ENOUGH);
		return TRAN_OP_FAILED;
	}

	if (!pkPlayer->IsCashEnough(pkRltMailPayPriceMsg->m_llPayPrice,CT_UNBIND_CASH))
	{
		SetResultID(RE_PAYFORMAIL_FAIL_CASH_IS_NOT_ENOUGH);
		return TRAN_OP_FAILED;
	}

	// 扣钱操作	
	pkPlayer->ReduceCash(pkRltMailPayPriceMsg->m_llPayPrice, CT_UNBIND_CASH, SYS_FUNC_TP_MAIL_PAY, m_uiMailID);

	// 1. 设置上下文
	{
		SetContentFlag(true);		
		m_llPayPrice = pkRltMailPayPriceMsg->m_llPayPrice;
	}

	// 2. 发修改消息给mailserver
	MG_GM_REQ_PAYMAIL_MODIFY_STATUS kReqMsg;
	kReqMsg.m_uiMailID = m_uiMailID;
	SendMsgToMailDB(MGPT_GM_REQ_PAYMAIL_MODIFY_STATUS, &kReqMsg, GetUserID(), GetCharID(), uiID);
	
	SetResultID(RE_SUCCESS);
	return TRAN_OP_NEXT;
}

void AO_Server_ReqPayMail::RespondRltMsg()
{	
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if (pkPlayer)
	{
		MG_RLT_PayForMail kRltMsg;

		kRltMsg.m_uiMailID  = m_uiMailID;
		kRltMsg.m_iResultType = GetResultID();
		pkPlayer->SendMsgToClient(MGPT_RLT_PAYFORMAIL, &kRltMsg);
	}
}

void AO_Server_ReqPayMail::Rollback()
{	
	if ( GetContentFlag() )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Mail payment failed: Player:%llu ",  GetCharID());
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Payment information MailID:%lld, UnBindCash: %llu", m_uiMailID, m_llPayPrice);	
	}
}

void AO_Server_ReqPayMail::Reset()
{
	TS_AtomOperate::Reset();
	m_uiMailID		= 0;
	m_llPayPrice	= 0;
}
//////////////////////////////////////////////////////////////////////////
emTranOperate AO_Server_RltModifyMailStatus::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	MG_GM_RLT_PAYMAIL_MODIFY_STATUS* pkRltPayMailModifyStatus = static_cast<MG_GM_RLT_PAYMAIL_MODIFY_STATUS*>(pkMsg);

	m_uiMailID = pkRltPayMailModifyStatus->m_uiMailID;
	if (pkRltPayMailModifyStatus->m_ushResult == RE_SUCCESS)
	{				
		SetResultID(RE_SUCCESS);
		RespondRltMsg();
		return TRAN_OP_NEXT;
	}
	else  //(pkRltPayMailModifyStatus->m_ushResult == RE_FAIL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "mailserver Failed to modify paid mail status: MailID:%lld, Player:%llu",  GetCharID());
		SetResultID(RE_PAYFORMAIL_FAIL_MAIL_SERVER_CANT_MODIFYSTATUS);
		return TRAN_OP_FAILED;
	}
}

void AO_Server_RltModifyMailStatus::RespondRltMsg()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if (pkPlayer)
	{
		MG_RLT_PayForMail kRltMsg;

		kRltMsg.m_uiMailID  = m_uiMailID;
		kRltMsg.m_iResultType = GetResultID();
		pkPlayer->SendMsgToClient(MGPT_RLT_PAYFORMAIL, &kRltMsg);
	}
}

void AO_Server_RltModifyMailStatus::Reset()
{
	TS_AtomOperate::Reset();
	m_uiMailID = 0;
}
//////////////////////////////////////////////////////////////////////////
TS_Server_PayForMail::TS_Server_PayForMail()
{
	m_kStepTranMap[STEP_REQ_GM_MODIFY_MAIL_STATUS] = T_NEW_D AO_Server_ReqPayMail;
	m_kStepTranMap[STEP_RLT_GM_MODIFY_MAIL_STATUS] = T_NEW_D AO_Server_RltModifyMailStatus;
}

TS_Server_PayForMail::~TS_Server_PayForMail()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}
//////////////////////////////////////////////////////////////////////////