#include "HL_PacketHandler.h"
#include "GameServerApp.h"
#include "NW_Helper.h"
#include "MG_Game2Mail.h"
#include "NW_ServerSession.h"

#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"

#include "TS_TranManager.h"
#include "TS_TranType.h"
#include "LC_SequenceManager.h"

#include "oss_define.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;


//----------------------------------------------------------------------------
namespace Protocol
{
#define this_source_type SERVER_TYPE_MAIL_DB

//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_LOAD_CHAR_MAIL_INFO, this_source_type, false)
{
	MG_GM_RLT_LOAD_CHAR_MAIL_INFO msg;

	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "load mail info", pkPlayer->GetID(), pkPlayer->GetInstance(), pkPlayer->GetGlobalMailID(), msg.m_lGlobalMailID, msg.m_ushCount);
	if (msg.m_lGlobalMailID > 0)
	{
		pkPlayer->SetGlobalMailID(msg.m_lGlobalMailID);	
	}

	MG_RLT_MailAsset kMailAssetMsg;
	kMailAssetMsg.m_iResultType = RE_SUCCESS;
	kMailAssetMsg.m_ushCount = msg.m_ushCount;

	if (msg.m_ushCount > MAX_HOLD_MAIL_NUM)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big! msg.m_ushCount:%d" , msg.m_ushCount);
		return;
	}

	for (int i = 0, j = 0; i < kMailAssetMsg.m_ushCount; ++i)
	{
		bool bIsForbidden = pkPlayer->CheckName(msg.m_kMailAssetPart[i].lSenderCharID);
		if (bIsForbidden)
		{
			if (msg.m_kMailAssetPart[i].bMailType == MAIL_TYPE_NORMAL)
			{
				if (msg.m_kMailAssetPart[i].bMailAffixType == MAIL_AFFIX_TEXT)
				{
					// 删掉邮件
					MG_GM_REQ_DELETE_MAIL kReqDeleteMail;
					kReqDeleteMail.m_uiMailID = msg.m_kMailAssetPart[i].lMailID;
					SendMsgToMailDB(MGPT_GM_REQ_DELETE_MAIL, &kReqDeleteMail, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
				}
				else
				{
					// 有物品和金钱则退信
					MG_GM_REQ_RETURN_MAIL kReqReturnMail;
					kReqReturnMail.m_uiMailID = msg.m_kMailAssetPart[i].lMailID;
					SendMsgToMailDB(MGPT_GM_REQ_RETURN_MAIL, &kReqReturnMail, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
				}
			}
		}
		else
		{
			kMailAssetMsg.m_kMailAssetPart[j] = msg.m_kMailAssetPart[i];
			j++;
		}
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_MAILASSET, &kMailAssetMsg);
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_OPEN_MAIL, this_source_type, false)
{
	MG_GM_RLT_OPEN_MAIL msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	if (msg.m_kMailInfo.m_llMoney < 0 || msg.m_kMailInfo.m_llMoney >= MAX_MAIL_MONEY)
		return;

	MG_RLT_ReadMail rltMsg;
	rltMsg.m_kMailInfo	= msg.m_kMailInfo;

	pkPlayer->SendMsgToClient(MGPT_RLT_READMAIL, &rltMsg);
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_POST_MAIL, this_source_type, false)
{
	MG_GM_RLT_POST_MAIL msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "post mail resp", rkMsgHead.m_uiCharID
		, msg.m_uiMailIDOnGameSvr, msg.m_uiMailIDOnMailDB, msg.m_uiTransactionID, msg.m_ushResult);

	TS_TranBase* pkTran = pkTranManager->GetTran(msg.m_uiTransactionID);
	if (!pkTran)
	{
		return;
	}

	if (pkTran->GetStep() != TS_Server_WriteMail::STEP_RLT_GM_WRITE_MAIL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer write mail has a logic error!");
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "return message, trans ID, PlayerID, Result", msg.m_uiTransactionID, rkMsgHead.m_uiCharID, msg.m_ushResult);
	}

	pkTran->HandlerMsg(rkMsgHead, &msg);

	pkTranManager->UnRegisteTran(pkTran->GetID());	// 不管成功失败，均注销事务
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_RETURN_MAIL, this_source_type, false)
{
	MG_GM_RLT_RETURN_MAIL msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_RLT_UntreadMail kUntreadMail;
	kUntreadMail.m_iResultType = msg.m_ushResult;
	kUntreadMail.m_lMailID = msg.m_uiMailID;

	pkPlayer->SendMsgToClient(MGPT_RLT_UNTREAD_MAIL, &kUntreadMail);
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_TAKE_MAIL_MONEY, this_source_type, false)
{
	MG_GM_RLT_TAKE_MAIL_MONEY msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	if (msg.m_ushResult == RE_SUCCESS)
	{
		MoneyType llMoneyAffix = msg.m_llMoney;
		if (!IS_VALID_MAIL_MONEY(llMoneyAffix))
			return;
		vector<int> nParams;
		nParams.clear();
		if(msg.m_iExtraParam > 0)
			nParams.push_back(msg.m_iExtraParam);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MAIL_OPERATE, OPERATE_LOGTYPE1);

		if(msg.m_iLogCode > 0)
			nLogCode = msg.m_iLogCode;

		oss_role_recv_mail(pkPlayer, "0", "0", "0", 0, (int)llMoneyAffix, 0, NULL);
		pkPlayer->AddCash(llMoneyAffix, CT_UNBIND_CASH, SYS_FUNC_TP_MAIL_AFFIX, msg.m_uiMailID,0, true, NULL, nLogCode, nParams);

		MG_RLT_GetCashAffix kGetCashAffix;
		kGetCashAffix.m_iMailID		= msg.m_uiMailID;
		kGetCashAffix.m_iResultType = msg.m_ushResult;

		pkPlayer->SendMsgToClient(MGPT_RLT_GETCASHAFFIX, &kGetCashAffix);
	}
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_TAKE_MAIL_ITEM, this_source_type, false)
{
	MG_GM_RLT_TAKE_MAIL_ITEM msg;

	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (msg.m_ushResult == RE_SUCCESS)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "get item success", rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, msg.m_uiMailID, msg.m_stItem.GetItemBaseData_TypeID());
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->GetTran(msg.m_uiTransactionID);
	if (!pkTran)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer GetTran error!");
		return;
	}

	if (pkTran->GetStep() != TS_Server_GetItemAffixFromMail::STEP_RLT_GM_GET_ITEM_AFFIX)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer GetStep error!");
		return;
	}

	pkTran->HandlerMsg(rkMsgHead, &msg);

	pkTranManager->UnRegisteTran(pkTran->GetID());	// 最后一步，均注销事务
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_DELETE_MAIL, this_source_type, false)
{
	MG_GM_RLT_DELETE_MAIL msg;

	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		GfxWriteLog(LOG_CHECK_FILELINE, LOG_SWITCH_CHECK, "MGPT_GM_RLT_DELETE_MAIL find player error!");
		return;
	}

	if (msg.m_ushResult == RE_SUCCESS)
	{
		MG_RLT_DeleteMail kDleteMail;
		kDleteMail.m_iResultType = msg.m_ushResult;
		kDleteMail.m_lMailID = msg.m_uiMailID;

		pkPlayer->SendMsgToClient(MGPT_RLT_DELETE_MAIL, &kDleteMail);
	}
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_GM_RLT_MAIL_PAYPRICE, this_source_type, false)
{
	MG_GM_RLT_MAIL_PAYPRICE kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (kMsg.m_ushResult != RE_SUCCESS)
	{
		// 发送信息给客户端
		MG_RLT_PayForMail kRltMsg;

		kRltMsg.m_uiMailID  = kMsg.m_uiMailID;
		kRltMsg.m_iResultType = RE_PAYFORMAIL_FAIL_MAIL_NOT_FOUND;

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (pkPlayer)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_PAYFORMAIL, &kRltMsg);
		}

		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_PAYFORMAIL, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);

	if (pkTran)
	{
		if (pkTran->GetStep() != TS_Server_PayForMail::STEP_REQ_GM_MODIFY_MAIL_STATUS)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer GetStep error-2!");
			pkTranManager->UnRegisteTran(pkTran->GetID());

			return;
		}

		ResultType iResultType = pkTran->HandlerMsg(rkMsgHead, &kMsg);
		if (RE_FAIL == iResultType)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
		}
	}
	else
	{
		// 通知繁忙
		MG_RLT_PayForMail kRltMsg;

		kRltMsg.m_uiMailID  = kMsg.m_uiMailID;
		kRltMsg.m_iResultType = RE_PAYFORMAIL_FAIL_MAIL_SERVER_IS_BUSY;

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (pkPlayer)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_PAYFORMAIL, &kRltMsg);
		}
	}
}

ON_HANDLE(MGPT_GM_RLT_PAYMAIL_MODIFY_STATUS, this_source_type, false)
{
	MG_GM_RLT_PAYMAIL_MODIFY_STATUS kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (!pkTran)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GM_RLT_PAYMAIL_MODIFY_STATUS GetTran error!");
		return;
	}

	if (pkTran->GetStep() != TS_Server_PayForMail::STEP_RLT_GM_MODIFY_MAIL_STATUS)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_GM_RLT_PAYMAIL_MODIFY_STATUS GetStep error!");
	}

	pkTran->HandlerMsg(rkMsgHead, &kMsg);

	pkTranManager->UnRegisteTran(pkTran->GetID());	// 最后一步，均注销事务
}

ON_HANDLE(MGPT_GM_RLT_NEW_MAIL, this_source_type, false)
{
	MG_GM_RLT_NEW_MAIL kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	MG_RLT_IsExitNewMail kRltMsg;
	kRltMsg.m_bHint = kMsg.m_uRepeatRemind;
	kRltMsg.m_iCount = kMsg.m_ushCount;
	kRltMsg.m_sendcharID = kMsg.m_sendcharID;
	kRltMsg.m_notreadCount = kMsg.m_notreadCount;
	kRltMsg.m_globalmail = false;

	LC_BlackListInfo* pkBlackListInfo = pkPlayer->GetSocietyAsset()->GetBlackListInfo();
	if (pkBlackListInfo->IsExist(kRltMsg.m_sendcharID))
	{
		return;
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_IS_EXIT_NEWMAIL, &kRltMsg);
}

}
