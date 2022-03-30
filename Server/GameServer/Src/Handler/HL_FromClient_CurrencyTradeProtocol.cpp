#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "MG_CS_CurrencyTradeProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_Game2Group.h"
#include "MG_Common.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_QUICK_BUY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyTradeQuickBuy kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID || kMsg.m_llRequestCount <= 0 || kMsg.m_llRequestPrice <= 0)
	{
		return;
	}

	MG_GG_REQ_Buy_Yuanbao kReqMsg;
	kReqMsg.m_iType			= EXCHANGE_IMMEDIATE;
	kReqMsg.m_llUnitPrice	= kMsg.m_llRequestPrice;
	kReqMsg.m_llCount		= kMsg.m_llRequestCount;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	SendMsgToTrade(MGPT_SS_REQ_BUY_YUANBAO, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_QUICK_SELL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyTradeQuickSell kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID || kMsg.m_llRequestCount <= 0 || kMsg.m_llRequestPrice <= 0)
	{
		return;
	}

	MG_GG_REQ_Sell_Yuanbao kReqMsg;
	kReqMsg.m_iType			= EXCHANGE_IMMEDIATE;
	kReqMsg.m_llUnitPrice	= kMsg.m_llRequestPrice;
	kReqMsg.m_llCount		= kMsg.m_llRequestCount;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	SendMsgToTrade(MGPT_SS_REQ_SELL_YUANBAO, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_COMMISSION_BUY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyTradeCommissionBuy kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID || kMsg.m_llRequestCount <= 0 || kMsg.m_llRequestPrice <= 0)
	{
		return;
	}

	MG_GG_REQ_Buy_Yuanbao kReqMsg;
	kReqMsg.m_iType			= EXCHANGE_NORMAL;
	kReqMsg.m_llUnitPrice	= kMsg.m_llRequestPrice;
	kReqMsg.m_llCount		= kMsg.m_llRequestCount;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	SendMsgToTrade(MGPT_SS_REQ_BUY_YUANBAO, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_COMMISSION_SELL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyTradeCommissionSell kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID || kMsg.m_llRequestCount <= 0 || kMsg.m_llRequestPrice <= 0)
	{
		return;
	}

	MG_GG_REQ_Sell_Yuanbao kReqMsg;
	kReqMsg.m_iType			= EXCHANGE_NORMAL;
	kReqMsg.m_llUnitPrice	= kMsg.m_llRequestPrice;
	kReqMsg.m_llCount		= kMsg.m_llRequestCount;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	SendMsgToTrade(MGPT_SS_REQ_SELL_YUANBAO, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_QUICK_TRADE_LIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyQuickTradeList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID)
	{
		return;
	}
	SendMsgToTrade(MGPT_SS_REQ_EXCHANGEVIEWLIST, NULL, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_COMMISSION_ITEM_LIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CurrencyCommissionItemList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID)
	{
		return;
	}
	SendMsgToTrade(MGPT_SS_REQ_EXCHANGELIST, NULL, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_TRADE_RECORD_LIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_CurrencyTradeRecordList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID)
	{
		return;
	}
	MG_GG_REQ_ExchangeHistory kReqMsg;
	kReqMsg.m_iPage	= kMsg.m_iPage;
	SendMsgToTrade(MGPT_SS_REQ_EXCHANGEHISTORY, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_REVOKE_COMMISSION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyRevokeCommission kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID)
	{
		return;
	}

	MG_GG_REQ_ExchangeCancel kReqMsg;
	kReqMsg.m_uiExchangeID	= kMsg.m_uiCommissionID;
	kReqMsg.m_uiType		= kMsg.m_uiType;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	SendMsgToTrade(MGPT_SS_REQ_EXCHANGECANCEL, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_YUANBAO_DEPOSIT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyYuanBaoDeposit kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID)
	{
		return;
	}

	if (!IS_VALID_CHAR_MONEY(kMsg.m_llYuanBaoCount))
	{
		return;
	}

	if (!pkPlayer->IsCashEnough(kMsg.m_llYuanBaoCount, CT_UNBIND_YUANBAO))
	{
		return;
	}

	MG_REQ_Draw_Money kReqMsg;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	kReqMsg.m_llDeltaYuanBao = -kMsg.m_llYuanBaoCount;
	kReqMsg.m_iBusinessID = 1;
	SendMsgToLogin(MGPT_SS_REQ_DRAW_MONEY, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CURRENCY_YUANBAO_DRAW, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_CurrencyYuanBaoDraw kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MoneyType llDrawYuanBao = -kMsg.m_llYuanBaoCount;

	if (!IS_VALID_CHAR_MONEY(llDrawYuanBao))
	{
		return;
	}

	if (pkPlayer->GetUserID() != kMsg.m_uiPlayerID)
	{
		return;
	}

	MG_GG_REQ_WithdrawYuanbao kReqMsg;
	kReqMsg.m_llTransYuanBao = kMsg.m_llYuanBaoCount;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	SendMsgToTrade(MGPT_SS_REQ_WITHDRAWYUANBAO, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, iSequence);
}
}
