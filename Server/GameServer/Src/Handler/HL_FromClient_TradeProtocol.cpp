#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_ItemProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "oss_define.h"
#include "CF_ShopList.h"

#include "UT_ServerHelper.h"

using namespace CSVFile;
using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_REQUEST_TRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RequestTrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = RE_FAIL;

	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	const unique_id_impl& lTargetCharID = reqMsg.m_lTradeTargetCharID;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTargetCharID);
	if (!pkNetPlayer || !pkNetPlayer->IsInstance())
	{
		return;
	}

	LC_TradeManager* pkTradeManager = pkNetPlayer->GetTradeManager();
	if (!pkTradeManager)
	{
		return;
	}

	ResultType iResult = RE_SUCCESS;
	do 
	{
		if(UT_ServerHelper::GetSrcServerId(pkPlayer) != UT_ServerHelper::GetSrcServerId(pkNetPlayer))
		{
			if (UT_ServerHelper::IsInCross(pkPlayer))
			{
				iResult = RE_CHAT_NOT_SAME_SERVER;
				break;
			}
			if (UT_ServerHelper::IsInCross(pkNetPlayer))
			{
				iResult = RE_CHAT_NOT_SAME_SERVER;
				break;
			}
		}
		if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_NO_TRADE)
		{
			iResult = RE_ITEM_TRADE_HAVE_TRADE;
			break;
		}
		if (pkTradeManager->IsRequestTradeAlready(lTargetCharID))
		{
			iResult = RE_ITEM_TRADE_REQEST_ALREADY;
			break;
		}
		if (pkTradeManager->IsRequestTradeFull())
		{
			iResult = RE_ITEM_TRADE_REQEST_FULL;
			break;
		}
		if (pkNetPlayer->IsForbidTarget(lTargetCharID))
		{
			iResult = RE_ITEM_TRADE_REQUEST_FORBID;
			break;
		}
	} while (false);
	
	if(iResult == RE_SUCCESS)
	{
		pkTradeManager->AddToRequestTradeList(lTargetCharID);

		MG_REQ_RequestTrade reqtradeMsg;
		reqtradeMsg.m_lTradeCharID = pkPlayer->GetInstance();
		reqtradeMsg.m_lTradeTargetCharID = lTargetCharID;
		reqtradeMsg.m_szCharName = reqMsg.m_szCharName;
		pkNetPlayer->SendMsgToClient(MGPT_REQ_ITEM_REQUEST_TRADE, &reqtradeMsg);

		iResult = RE_ITEM_TRADE_REQUEST_SUCCESS ;
	}

	MG_RLT_RequestTrade rltMsg;
	rltMsg.m_lTradeCharID = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID = lTargetCharID;
	rltMsg.m_iReqResult = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_REQUEST_TRADE, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_REPLY_TRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_ReplyTrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if(NULL == pkSafeSystemAsset)
	//	return;

	//ResultType eSafeCheckResult = RE_FAIL;
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();

	//if(eSafeCheckResult != RE_SUCCESS)
	//	return;

	const unique_id_impl& lTargetCharID = reqMsg.m_lTradeTargetCharID;
	pkPlayer->GetTradeManager()->RemoveFormRequestTradeID(lTargetCharID);

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTargetCharID);

	int iResult = RE_SUCCESS;
	if (!pkNetPlayer)
	{
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (reqMsg.m_iReqlyResult != RTS_ACCEPT)
	{
		iResult = reqMsg.m_iReqlyResult;
	}
	else
	{
		if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_NO_TRADE)
		{
			iResult = RE_ITEM_TRADE_HAVE_TRADE;
		}
		else
		{
			LC_TradeManager* pkTradeManager = pkNetPlayer->GetTradeManager();
			LC_ServerPlayer* pkLastPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(pkTradeManager->GetTradeTargetID());
			if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_NO_TRADE)
			{
				pkNetPlayer->InterruptTradeAndClearTradeData();
				if (pkLastPlayer)
				{
					pkLastPlayer->InterruptTradeAndClearTradeData();
				}

				iResult = RE_SUCCESS;

				MG_RLT_InterruptTrade rltMsg;
				rltMsg.m_lTradeCharID        = pkNetPlayer->GetInstance();
				rltMsg.m_lTradeTargetCharID   = pkLastPlayer ? pkLastPlayer->GetInstance() : null_unique_impl;
				rltMsg.m_iResult          = iResult;

				if (pkNetPlayer)
				{
					pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_INTERRUPT, &rltMsg);
				}
				if (pkLastPlayer)
				{
					pkLastPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_INTERRUPT, &rltMsg);
				}
			}
			//判断玩家是否处于交易状态；
			if (pkPlayer->GetTradeManager()->GetTradeState() != PTS_NO_TRADE)
			{
				LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(pkPlayer->GetTradeManager()->GetTradeTargetID());
				pkPlayer->InterruptTradeAndClearTradeData();
				if (pkTargetPlayer)
				{
					pkTargetPlayer->InterruptTradeAndClearTradeData();
				}

				iResult = RE_SUCCESS;

				MG_RLT_InterruptTrade rltMsg;
				rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
				rltMsg.m_lTradeTargetCharID  = pkTargetPlayer ? pkTargetPlayer->GetInstance() : null_unique_impl;
				rltMsg.m_iResult          = iResult;

				if (pkPlayer)
				{
					pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_INTERRUPT, &rltMsg);
				}
				if (pkTargetPlayer)
				{
					pkTargetPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_INTERRUPT, &rltMsg);
				}
			}
			int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pkNetPlayer->GetCurrentLocation());
			if (pkPlayer->GetMapLogicID()!=pkNetPlayer->GetMapLogicID() || iDistance>MAX_TRADE_DISTANCE)
			{
				iResult = RE_ITEM_TRADE_NOT_IN_RANGE;
			}
			else
			{
				pkNetPlayer->GetTradeManager()->UpdateDirtyFlag();
				pkPlayer->GetTradeManager()->UpdateDirtyFlag();
				pkNetPlayer->GetTradeManager()->ClearTradeData();
				pkPlayer->GetTradeManager()->ClearTradeData();
				pkNetPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
				pkPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
				pkNetPlayer->GetTradeManager()->SetTradeTargetID(pkPlayer->GetInstance());
				pkPlayer->GetTradeManager()->SetTradeTargetID(pkNetPlayer->GetInstance());
				pkPlayer->GetTradeManager()->ClearRequestList();

				iResult = RE_ITEM_TRADE_START;
			}
		}
	}

	MG_RLT_ReplyTrade replyMsg;
	replyMsg.m_lRequestCharID = pkPlayer->GetInstance();
	replyMsg.m_lReplyCharID   = pkPlayer->GetInstance();
	replyMsg.m_iResult          = iResult;

	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_REPLY_TRADE, &replyMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_REPLY_TRADE, &replyMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_ADD_ITEM_IN_TRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AddItemToTradePack reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	const unique_id_impl& lTradeTargetCharID		= reqMsg.m_lTradeTargetCharID;
	int  iBackPackIndex		= reqMsg.m_iBackPackIndex;
	int  iItemType			= reqMsg.m_usItemType;
	int  iTradePackIndex	= reqMsg.m_iTradePackIndex;
	int lTradeItemID		= reqMsg.m_lTradeItemID;

	int iResult = RE_SUCCESS;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);
	LC_BackPackEntry* pkEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(iItemType, iBackPackIndex);
	if (!pkNetPlayer)
	{
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE || pkPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_HAVE_NO_TRADE;
	}
	else if (pkPlayer->GetTradeManager()->GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	else if (!pkEntry || !pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		iResult = RE_ITEM_TRADE_CANT_UNVALIDE;
	}
	else if (lTradeItemID != pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID))
	{
		iResult = RE_ITEM_TRADE_ITEM_ERROR;
	}
	else
	{
		iResult = pkPlayer->GetTradeManager()->AddItemToMyTradePack(iBackPackIndex, iItemType, iTradePackIndex, pkEntry);
		if (RE_ITEM_TRADE_ADD_SUCCESS == iResult)
		{
			LC_BackPackEntry* pkMyTradeEntry = pkPlayer->GetTradeManager()->GetItemEntryInMyTradePack(iTradePackIndex);
			iResult = pkNetPlayer->GetTradeManager()->AddItemToTargetTradePack(iTradePackIndex, pkMyTradeEntry);
			pkPlayer->GetPackAsset()->UpdateDirtyFlag(iItemType);
		}
	}

	MG_RLT_AddItemToTradePack rltMsg;
	rltMsg.m_lTradeCharID = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID   = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;

	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_ADD_ITEM_IN_TRADE, &rltMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_ADD_ITEM_IN_TRADE, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_REMOVE_ITEM_IN_TRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RemoveItemFromTradePack reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	const unique_id_impl& lTradeTargetCharID    = reqMsg.m_lTradeTargetCharID;
	int  iTradePackIndex = reqMsg.m_iTradePackIndex;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);

	int iResult = RE_SUCCESS;
	if (!pkNetPlayer)
	{
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE || pkPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_HAVE_NO_TRADE;
	}
	else if (pkPlayer->GetTradeManager()->GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	else
	{
		iResult = pkPlayer->RemoveTradeItemToPackAsset(iTradePackIndex);
		iResult = pkNetPlayer->GetTradeManager()->RemoveItemToTargetTradePack(iTradePackIndex);
	}

	MG_RLT_RemoveItemFromTradePack rltMsg;
	rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID   = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;
	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_REMOVE_ITEM_IN_TRADE, &rltMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_REMOVE_ITEM_IN_TRADE, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_UPDATE_CASH_IN_TRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_UpdateTradeCash reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = RE_FAIL;
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}

	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	const unique_id_impl& lTradeTargetCharID = reqMsg.m_lTradeTargetCharID;
	int32_t iCashType	= reqMsg.m_iCashType;
	MoneyType  llCash    = reqMsg.m_llCash;

	if (CT_UNBIND_CASH==iCashType || !LC_Helper::CheckValidTradeCash(iCashType, llCash))
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);

	int iResult = RE_SUCCESS;
	if (iCashType == CT_UNBIND_YUANBAO && GetGameServerApp()->IsBonusRole(pkPlayer->GetInstanceUniqueID()))
	{
		iResult = RE_BONUS_TRADE_EXPORT_FORBID;
	}
	else if (!pkNetPlayer)
	{
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE || pkPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_HAVE_NO_TRADE;
	}
	else if (pkPlayer->GetTradeManager()->GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	else if (!pkPlayer->IsCashEnough(llCash, iCashType))
	{
		iResult = RE_UNBIND_YUANBAO_NOT_ENOUGH + iCashType - CT_UNBIND_YUANBAO;
	}
	else if (llCash < 0)
	{
		iResult = RE_FAIL;
	}
	else
	{
		iResult = pkPlayer->GetTradeManager()->UpdateCashToMyTradePack(iCashType, llCash);
		if (RE_ITEM_TRADE_UPDATE_CASH_SUCCESS == iResult)
		{
			iResult = pkNetPlayer->GetTradeManager()->UpdateCashToTargetTradePack(iCashType, llCash);
		}
	}

	MG_RLT_UpdateTradeCash rltMsg;
	rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID  = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;
	rltMsg.m_iCashType        = iCashType;
	rltMsg.m_llCash           = llCash;

	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_UPDATE_CASH_IN_TRADE, &rltMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_UPDATE_CASH_IN_TRADE, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_TRADE_LOCK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_LockTrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = RE_FAIL;
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	const unique_id_impl& lTradeTargetCharID = reqMsg.m_lTradeTargetCharID;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);

	int iResult = RE_SUCCESS;

	if (!pkNetPlayer)
	{
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE || pkPlayer->GetTradeManager()->GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_HAVE_NO_TRADE;
	}
	else
	{
		pkPlayer->GetTradeManager()->SetMyTradeLock(true);
		pkNetPlayer->GetTradeManager()->SetTargetTradeLock(true);
		iResult = RE_ITEM_TRADE_LOCK_SUCCESS;
	}

	MG_RLT_LockTrade rltMsg;
	rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID   = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;
	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_LOCK, &rltMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_LOCK, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_TRADE_UNLOCK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_UnLockTrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	const unique_id_impl& lTradeTargetCharID = reqMsg.m_lTradeTargetCharID;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);

	int iResult = RE_SUCCESS;
	if (!pkNetPlayer)
	{
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeState() == PTS_NO_TRADE || pkPlayer->GetTradeManager()->GetTradeState() == PTS_NO_TRADE)
	{
		iResult = RE_ITEM_TRADE_HAVE_NO_TRADE;
	}
	else
	{
		pkPlayer->GetTradeManager()->SetMyTradeLock(false);
		pkPlayer->GetTradeManager()->SetTargetTradeLock(false);
		pkPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
		pkNetPlayer->GetTradeManager()->SetMyTradeLock(false);
		pkNetPlayer->GetTradeManager()->SetTargetTradeLock(false);
		pkNetPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
		iResult = RE_ITEM_TRADE_UNLOCKED_SUCCESS;
	}

	MG_RLT_UnLockTrade rltMsg;
	rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID   = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;
	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_UNLOCK, &rltMsg);
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_UNLOCK, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_TRADE_INTERRUPT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_InterruptTrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	const unique_id_impl& lTradeTargetCharID = reqMsg.m_lTradeTargetCharID;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);

	int iResult = RE_SUCCESS;
	if (!pkNetPlayer)
	{
		pkPlayer->InterruptTradeAndClearTradeData();
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeState() == PTS_NO_TRADE || pkPlayer->GetTradeManager()->GetTradeState() == PTS_NO_TRADE)
	{
		iResult = RE_ITEM_TRADE_HAVE_NO_TRADE;
	}
	else
	{
		pkPlayer->InterruptTradeAndClearTradeData();
		pkNetPlayer->InterruptTradeAndClearTradeData();
		iResult = RE_ITEM_TRADE_INTERRUPT_SUCCESS;
	}

	MG_RLT_InterruptTrade rltMsg;
	rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID   = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;

	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_INTERRUPT, &rltMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_INTERRUPT, &rltMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ITEM_TRADE_FINISH, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_FinishTrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (!pkPlayer->IsInGame() || !pkPlayer->IsInstance())
	{
		return;
	}
	//for the bug, duplicate equip item!
	if (LC_Helper::IsTradeForbid(pkPlayer->GetMapResID()))
	{
		return;
	}

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_TRADE, LAF_TRADE_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = RE_FAIL;
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	const unique_id_impl& lTradeTargetCharID = reqMsg.m_lTradeTargetCharID;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lTradeTargetCharID);

	int iResult = RE_SUCCESS;
	if (!pkNetPlayer || !pkNetPlayer->IsInstance())
	{
		pkPlayer->InterruptTradeAndClearTradeData();
		iResult = RE_ITEM_TRADE_NOT_ONLIE;
	}
	else if (pkNetPlayer->GetTradeManager()->GetTradeTargetID() != pkPlayer->GetInstance() || lTradeTargetCharID != pkPlayer->GetTradeManager()->GetTradeTargetID())
	{
		pkPlayer->InterruptTradeAndClearTradeData();
		pkNetPlayer->InterruptTradeAndClearTradeData();
		iResult = RE_ITEM_TRADE_TARGET_ERROR;
	}
	else
	{
		int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pkNetPlayer->GetCurrentLocation());
		if (pkPlayer->GetMapLogicID()!=pkNetPlayer->GetMapLogicID() || iDistance > MAX_TRADE_DISTANCE)
		{
			pkPlayer->InterruptTradeAndClearTradeData();
			pkNetPlayer->InterruptTradeAndClearTradeData();
			iResult = RE_ITEM_TRADE_NOT_IN_RANGE;
		}
		else if (!pkPlayer->GetTradeManager()->GetMyTradeLock() || !pkNetPlayer->GetTradeManager()->GetMyTradeLock())
		{
			iResult = RE_ITEM_TRADE_UNLOCKED;
		}
		else if (pkNetPlayer->GetTradeManager()->GetTradeState() == PTS_HAVE_TRADE)
		{
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_TRADEPACK);
			pkNetPlayer->UpdateDirtyFlag(DIRTY_FLAG_TRADEPACK);
			pkPlayer->GetTradeManager()->SetTradeState(PTS_FINISH_TRADE);
			iResult = RE_ITEM_TRADE_FINISH_SUCCESS;
		}
		else if (pkNetPlayer->GetTradeManager()->GetTradeState() == PTS_FINISH_TRADE)
		{
			iResult = LC_Helper::CheckTradeData(pkPlayer, pkNetPlayer);
			if (iResult == RE_ITEM_TRADE_CHECK_SUCCESS)
			{
				LC_TradeManager* pkTradeManager = pkPlayer->GetTradeManager();
				LC_TradeManager* pkNetTradeManager = pkNetPlayer->GetTradeManager();
				{
					//MoneyType lmoney1 = pkTradeManager->GetMyTradeCash();
					int itemID_1[MAX_TRADE_PACK_SIZE];
					int itemCount_1[MAX_TRADE_PACK_SIZE];

					for (int i = 0; i < MAX_TRADE_PACK_SIZE; ++i)
					{
						LC_BackPackEntry* pkEntry = pkTradeManager->GetItemEntryInMyTradePack(i);
						if (pkEntry && pkEntry->GetBackPackValueByType(LPSFET_VALID))
						{
							itemID_1[i] = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
							itemCount_1[i] = pkEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
						}
						else
						{
							itemID_1[i] = 0;
							itemCount_1[i] = 0;
						}
					}

					//MoneyType lmoney2 = pkNetTradeManager->GetMyTradeCash();
					int itemID_2[MAX_TRADE_PACK_SIZE];
					int itemCount_2[MAX_TRADE_PACK_SIZE];
					for (int i = 0; i < MAX_TRADE_PACK_SIZE; ++i)
					{
						LC_BackPackEntry* pkEntry = pkNetTradeManager->GetItemEntryInMyTradePack(i);
						if (pkEntry && pkEntry->GetBackPackValueByType(LPSFET_VALID))
						{
							itemID_2[i] = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
							itemCount_2[i] = pkEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
						}
						else
						{
							itemID_2[i] = 0;
							itemCount_2[i] = 0;
						}
					}
				}

				//if (NULL != pkTradeManager && NULL != pkNetTradeManager)
				{
					oss_role_trade(pkPlayer, pkTradeManager->GetMyTradeCash(CT_UNBIND_YUANBAO), pkNetTradeManager->GetTradePackData()->m_akTradeTargetPackage, MAX_TRADE_PACK_SIZE, 
						pkNetPlayer, pkNetTradeManager->GetMyTradeCash(CT_UNBIND_YUANBAO), pkTradeManager->GetTradePackData()->m_akTradeTargetPackage, MAX_TRADE_PACK_SIZE);
				}
				pkPlayer->TradeItemWithTarget();
				pkNetPlayer->TradeItemWithTarget();
				pkPlayer->GetTradeManager()->ClearTradeData();
				pkNetPlayer->GetTradeManager()->ClearTradeData();
				iResult = RE_ITEM_TRADE_SUCCESS;
				//快速同步
				pkPlayer->AsyncUpdateToDBServer();
				pkNetPlayer->AsyncUpdateToDBServer();
			}
			else if (iResult == RE_ITEM_TRADE_PLAYER_NO_SPACE ||
					 iResult == RE_ITEM_TRADE_CASH_NOT_ENOUGH ||
					 iResult == RE_ITEM_TRADE_TARGET_NO_SPACE ||
					 iResult == RE_ITEM_TRADE_LARGER_MAX_CASH)
			{
				pkPlayer->GetTradeManager()->SetMyTradeLock(false);
				pkPlayer->GetTradeManager()->SetTargetTradeLock(false);
				pkPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
				pkNetPlayer->GetTradeManager()->SetMyTradeLock(false);
				pkNetPlayer->GetTradeManager()->SetTargetTradeLock(false);
				pkNetPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
			}
			else
			{
				pkPlayer->InterruptTradeAndClearTradeData();
				pkNetPlayer->InterruptTradeAndClearTradeData();
			}
		}
		else
		{
			pkPlayer->InterruptTradeAndClearTradeData();
			pkNetPlayer->InterruptTradeAndClearTradeData();
			iResult = RE_ITEM_TRADE_FINISH_ERROR;
		}
	}

	MG_RLT_FinishTrade rltMsg;
	rltMsg.m_lTradeCharID        = pkPlayer->GetInstance();
	rltMsg.m_lTradeTargetCharID   = lTradeTargetCharID;
	rltMsg.m_iResult          = iResult;

	if (pkNetPlayer)
	{
		pkNetPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_FINISH, &rltMsg);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_ITEM_TRADE_FINISH, &rltMsg);
}

}
