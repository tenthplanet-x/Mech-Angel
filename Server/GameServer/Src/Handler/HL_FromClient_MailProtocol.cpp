#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_Game2Mail.h"
#include "LC_SequenceManager.h"
#include "TS_TranManager.h"
#include "TS_TranType.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "gsnode.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_WRITE_MAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_WriteMail reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!IS_VALID_MAIL_MONEY(reqMsg.m_kWriteMail.m_llMoney))
		return;

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_WRITE_MAIL, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	if (pkTran)
	{
		if (pkTran->GetStep() != TS_Server_WriteMail::STEP_REQ_GM_WRITE_MAIL)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());

			return;
		}

		SS_MG_HEAD kSSMsgHead;
		kSSMsgHead.m_usMsgType = rkMsgHead.m_usMsgType;
		kSSMsgHead.m_uiUserID = rkMsgHead.m_uiUserID;
		kSSMsgHead.m_uiCharID = rkMsgHead.m_uiCharID;

		ResultType iResultType = pkTran->HandlerMsg(kSSMsgHead, &reqMsg);
		if (RE_FAIL == iResultType)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
		}
	}

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_UNTREAD_MAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_UntreadMail reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	MG_GM_REQ_RETURN_MAIL reqReturnMail;
	reqReturnMail.m_uiMailID = reqMsg.m_lMailID;
	SendMsgToMailDB(MGPT_GM_REQ_RETURN_MAIL, &reqReturnMail, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DELETE_MAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_DeleteMail reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	MG_GM_REQ_DELETE_MAIL reqGMDeleteMail;
	reqGMDeleteMail.m_uiMailID = reqMsg.m_lMailID;
	SendMsgToMailDB(MGPT_GM_REQ_DELETE_MAIL, &reqGMDeleteMail, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_MAILASSET, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_MailAsset reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	MG_GM_REQ_LOAD_CHAR_MAIL_INFO msg;
	//msg.m_iServerID = (pkPlayer->GetServerID();
	msg.m_iServerID = GSNodeNetwork::Instance().getEntryIdByMergeId(pkPlayer->GetServerID());
	msg.m_lGlobalMailID = pkPlayer->GetGlobalMailID();

	int64_t iStartTime = pkServerMgr->GetServerStartTime();
	int64_t iConflateTime = pkServerMgr->GetServerConflateTime();
	int64_t iBenchTime = iConflateTime>iStartTime ? iConflateTime : iStartTime;

	msg.iRegTime = (int32_t)UT_MathBase::MaxInt64(pkPlayer->GetRegTime(), iBenchTime);
	// 将请求直接转发给mail server
	SendMsgToMailDB(MGPT_GM_REQ_LOAD_CHAR_MAIL_INFO, &msg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GETITEMAFFIX, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_GetItemAffix reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_GET_ITEM_AFFIX, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	if (pkTran)
	{
		if (pkTran->GetStep() != TS_Server_GetItemAffixFromMail::STEP_REQ_GM_GET_ITEM_AFFIX)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
			return;
		}

		SS_MG_HEAD kSSMsgHead;
		kSSMsgHead.m_usMsgType = rkMsgHead.m_usMsgType;
		kSSMsgHead.m_uiUserID = rkMsgHead.m_uiUserID;
		kSSMsgHead.m_uiCharID = rkMsgHead.m_uiCharID;

		ResultType iResultType = pkTran->HandlerMsg(kSSMsgHead, &reqMsg);
		if (RE_FAIL == iResultType)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
		}
	}
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_GETCASHAFFIX, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_GetCashAffix reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	MG_GM_REQ_TAKE_MAIL_MONEY reqTakeMailMoney;
	reqTakeMailMoney.m_uiMailID = reqMsg.m_iMailID;
	SendMsgToMailDB(MGPT_GM_REQ_TAKE_MAIL_MONEY, &reqTakeMailMoney, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADDITEM_MAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_AddItemToMailAffix reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	MG_RLT_AddItemToMailAffix rltMsg;
	LC_BackPackEntry* pkEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(reqMsg.m_iPackType, reqMsg.m_iItemIndex);
	if (pkEntry != NULL && pkEntry->GetBackPackValueByType(LPSFET_VALID) && !pkEntry->GetBackPackValueByType(LPSFET_LOCKED) && !pkEntry->GetBackPackValueByType(LPSFET_BINDMODE))
	{
		pkEntry->SetBackPackValueByType(LPSFET_LOCKED, true);
		rltMsg.m_iResultType = RE_SUCCESS;
	}
	else
	{
		rltMsg.m_iResultType = RE_FAIL;
	}
	if (reqMsg.m_iTargetPos >= MAIL_ITEM_AFFIX_NUM_PLAYER_SEND)
	{
		rltMsg.m_iResultType = RE_MAIL_ADD_AFFIX_FAIL_IS_FULL;
	}
	rltMsg.m_iPackType = reqMsg.m_iPackType;
	rltMsg.m_iItemIndex = reqMsg.m_iItemIndex;
	rltMsg.m_iTargetPos	= reqMsg.m_iTargetPos;

	pkPlayer->SendMsgToClient(MGPT_RLT_ADDITEM_MAIL, &rltMsg);
}

ON_HANDLE(MGPT_REQ_DELITEM_MAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_DeletItemFromMailAffix reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	MG_RLT_DeletItemFromMailAffix rltMsg;
	LC_BackPackEntry* pkEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(reqMsg.m_iPackType, reqMsg.m_iItemIndex);
	if (pkEntry != NULL && pkEntry->GetBackPackValueByType(LPSFET_VALID) && pkEntry->GetBackPackValueByType(LPSFET_LOCKED) && !pkEntry->GetBackPackValueByType(LPSFET_BINDMODE))
	{
		pkEntry->SetBackPackValueByType(LPSFET_LOCKED, false);
		rltMsg.m_iResultType = RE_SUCCESS;
	}
	else
	{
		rltMsg.m_iResultType = RE_FAIL;
	}

	rltMsg.m_iPackType = reqMsg.m_iPackType;
	rltMsg.m_iItemIndex = reqMsg.m_iItemIndex;
	rltMsg.m_iTargetPos	= reqMsg.m_iTargetPos;

	pkPlayer->SendMsgToClient(MGPT_RLT_DELITEM_MAIL, &rltMsg);
}

ON_HANDLE(MGPT_REQ_PAYFORMAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_PayForMail kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	if (!IS_VALID_CHAR_MONEY(kMsg.m_llPayPrice))
		return;

	if (!pkPlayer->IsCashEnough(kMsg.m_llPayPrice, CT_UNBIND_CASH))
	{
		// 金钱不足
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//	return;

	//ResultType eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();

	//if (eSafeCheckResult != RE_SUCCESS)
	//	return;


	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();

	//if (eSafeCheckResult != RE_SUCCESS)
	//	return;

	// 向邮件服务器查询价格
	MG_GM_REQ_MAIL_PAYPRICE	kReqMsg;
	kReqMsg.m_uiMailID = kMsg.m_uiMailID;
	SendMsgToMailDB(MGPT_GM_REQ_MAIL_PAYPRICE, &kReqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_READMAIL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_ReadMail kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
		return;

	// 通知邮件服务器修改已读标志
	MG_GM_REQ_OPEN_MAIL	kReqMsg;
	kReqMsg.m_uiMailID = kMsg.m_uiMailID;
	SendMsgToMailDB(MGPT_GM_REQ_OPEN_MAIL, &kReqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

}
