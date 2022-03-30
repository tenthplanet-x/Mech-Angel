#include "ClientPCH.h"

#include "LC_ClientMailManager.h"

#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"

#include "MG_GameProtocol.h"
#include "NW_Helper.h"

#include "NW_ClientSessionManager.h"

#include "MG_MessageBase.h"
#include "UT_LogicInfoMessage.h"
#include "AS_ClientSoundManager.h"
#include "UT_TimeManager.h"
#include "TT_TextFilter.h"
#include "UI_UIManager.h"
#include "LC_GameEvent.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;

LC_ClientMailManager::LC_ClientMailManager() 
: m_bIsREQMailAsset(false)
, m_bIsLoginRequest(false)
, m_iSelectedIndex(0)
, m_iSelectedMailID(0)
, m_bIsSending(false)
, m_bIsComplete(false)
, m_bIsPay(false)
, m_bMailHint(false)
, m_bMailboxFull(false)
, m_bIsReadingMail(false)
, m_iPreRequestTimeStamp(0)
{
	m_strPendingReceiverName = "";
	m_strPendingSubject = "";
	m_strPendingContent = "";
	m_iPendingGold = 0;
	m_iPendingSilver = 0;
	m_iPendingCopper = 0;
}

LC_ClientMailManager::~LC_ClientMailManager()
{

}

ResultType LC_ClientMailManager::RequestMailAsset()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	int64_t iSystemTime = GET_SYSTEM_TIME;
	if (iSystemTime - m_iPreRequestTimeStamp <= 2)
	{
		return RE_FAIL;
	}
	m_iPreRequestTimeStamp = iSystemTime;
	MG_REQ_MailAsset reqMsg;
	reqMsg.m_lPlayerID = pkGamePlayer->GetID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MAILASSET, &reqMsg);

	if(bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		return RE_SUCCESS;
	}

	return RE_FAIL;
}

void LC_ClientMailManager::PostGetGlobalMail(Utility::timer_id_type timerID)
{
	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
	if (NULL == pkClientMailManager)
	{
		return;
	}
	LC_MailAsset* pkMailAsset = pkClientMailManager->GetMailAsset();
	if (NULL == pkMailAsset)
	{
		return;
	}
	pkMailAsset->AddNotReadMailCount();
	pkClientMailManager->SetMailHintFlag(true);

	pkClientMailManager->RequestMailAsset();
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_NEWMAIL_NOTIFY);
	UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);

}
ResultType LC_ClientMailManager::RequestWriteMailContentCheck(const StringType& strReceiverName, 
							const StringType& strSubject, 
							const StringType& strContent,
							MoneyType iGold, MoneyType iSilver, MoneyType iCopper, bool bIsPay)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	// 无收件人
	if (strReceiverName.length() <= 0)
	{
		SendLogicInfoMessage(RE_WRITEMAIL_FAIL_TARGET_NOT_RIGHT);
		return RE_FAIL;
	}
	// 如果收件人是发件人
	if (pkGamePlayer->GetCharName() == strReceiverName)
	{
		SendLogicInfoMessage(RE_WRITEMAIL_FAIL_TO_SELF);
		return RE_FAIL;
	}
	// 判断好友
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if (NULL == pkSessionManager)
	{
		return RE_FAIL;
	}

	NW_NetCharSimpleInfo* pkInfo = pkSessionManager->FindNetCharSimpleInfo(strReceiverName, true);
	if ( NULL == pkInfo )
	{
		AddPendingName(strReceiverName);

		m_strPendingReceiverName	= strReceiverName;
		m_strPendingSubject			= strSubject;
		m_strPendingContent			= strContent;
		m_iPendingGold				= iGold;
		m_iPendingSilver			= iSilver;
		m_iPendingCopper			= iCopper;
		m_bIsPay					= bIsPay;
		
		return RE_FAIL;
	}
	else
	{
		if (pkInfo->m_iProfileResult != RE_SUCCESS)
		{
			SendLogicInfoMessage(RE_WRITEMAIL_FAIL_TARGET_NOT_RIGHT);
			return RE_FAIL;
		}
	}

	// 无主题
	if (strSubject.length() <= 0)
	{
		SendLogicInfoMessage(RE_WRITEMAIL_FAIL_SUBJECT_NOT_EXIST);
		return RE_FAIL;
	}	

	// 选择付费邮件，但是没有添加物品
	if (bIsPay)
	{
		bool bExistedAffix = false;
		for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
		{
			if (GetClientItemAffixInfo()->IsValid(i))
			{
				bExistedAffix = true;
				break;
			}
		}
		if (!bExistedAffix)
		{
			SendLogicInfoMessage(RE_WRITEMAIL_FAIL_NEED_PUT_AFFIX);
			return RE_FAIL;
		}
	}

	// 发送金钱判定，发送的非绑定钱币
	MoneyType iMoneyAffix = LC_Money::MoneyInfo2MoneyNumber(iGold, iSilver, iCopper);
	if (iMoneyAffix == 0 && bIsPay)
	{
		SendLogicInfoMessage(RE_WRITEMAIL_FAIL_PAY_CASH_IS_ZERO);
		return RE_FAIL;
	}
	if (pkGamePlayer->GetUnBindCash().GetMoney() < iMoneyAffix && !bIsPay)
	{
		SendLogicInfoMessage(RE_WRITEMAIL_FAIL_CASH_NOT_ENOUGH);
		return RE_FAIL;
	}

	return RE_SUCCESS;
}

ResultType LC_ClientMailManager::RequestWriteMailPostageCheck(MoneyType iGold, MoneyType iSilver, MoneyType iCopper)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	//if (NULL == pkLogicManager)
	//{
	//	return RE_FAIL;
	//}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	// 邮资,用绑定钱票
	MoneyType iMailCharge = ComputeMailCharge(iGold, iSilver, iCopper);

	// 钱票不足，钱币足的情况下需要弹出框提示
	if (!pkGamePlayer->IsCashEnough(iMailCharge,CT_BIND_CASH))
	{
		SendLogicInfoMessage(RE_WRITEMAIL_FAIL_CHARGE_NOT_ENOUGH);
		return RE_FAIL;
	}

	//现在不需要判断绑银了。
	//if (pkGamePlayer->IsNeedUnBindCash(iMailCharge))
	//{
	//	return RE_MAIL_POSTAGE_CHECK_BIND_CASH_NOT_ENOUGH;
	//}

	return RE_SUCCESS;
}

void LC_ClientMailManager::RequestWriteMailExcute(const StringType& strReceiverName, 
												  const StringType& strSubject, 
												  const StringType& strContent,
												  MoneyType iGold, MoneyType iSilver, MoneyType iCopper, bool bIsPay)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if (NULL == pkSessionManager)
	{
		return;
	}

	NW_NetCharSimpleInfo* pkInfo = pkSessionManager->FindNetCharSimpleInfo(strReceiverName, true);
	if ( NULL == pkInfo )
	{
		return;
	}

	MG_REQ_WriteMail reqMsg;
	Protocol::PS_WriteMailInfo& kWriteMail = reqMsg.m_kWriteMail;

	if (RE_SUCCESS == pkInfo->m_iProfileResult)
	{
		kWriteMail.lReceiverCharID = pkInfo->m_lCharID;
	}

	kWriteMail.bIsPay = bIsPay;
	kWriteMail.szReceiverCharName = TPSTR2STDSTR(strReceiverName);

	// 进行屏蔽字过滤：主题
	StringType strTemp = strSubject;

	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();	
	if (NULL != pkTextFilter)
	{
		pkTextFilter->SetText(strTemp);
		pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAT);
		strTemp = pkTextFilter->GetText();
	}
	kWriteMail.szSubject = TPSTR2STDSTR(strTemp);

	//  进行屏蔽字过滤：内容
	strTemp = strContent;
	if (NULL != pkTextFilter)
	{
		pkTextFilter->SetText(strTemp);
		pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAT);
		strTemp = pkTextFilter->GetText();
	}
	kWriteMail.szContent = TPSTR2STDSTR(strTemp);

	// 物品附件
	for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
	{
		kWriteMail.aItemAffix[i].bValid = GetClientItemAffixInfo()->IsValid(i);
		kWriteMail.aItemAffix[i].iPackType = GetClientItemAffixInfo()->GetPackType(i);
		kWriteMail.aItemAffix[i].iIndex = GetClientItemAffixInfo()->GetIndex(i);
	}

	// 寄钱
	kWriteMail.m_llMoney = LC_Money::MoneyInfo2MoneyNumber(iGold, iSilver, iCopper);		

	// 发送邮件
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_WRITE_MAIL, &reqMsg);
	if(bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		SetSendingFlag(true);
		return;
	}
}

ResultType LC_ClientMailManager::RequestItemAffix(int iIndex)
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(m_iSelectedMailID);
	if (pkMail == NULL)
	{
		return RE_FAIL;
	}

	//如果未付费不能取附件
	if (!pkMail->GetUntreadFlag() && pkMail->GetPayFlag() == MAIL_NEED_PAY)
	{
		SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_NOT_PAY_CASH);
		return RE_FAIL;
	}
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	
	// 查看背包是否已满
	if (iIndex >= 0 && iIndex < MAIL_ITEM_AFFIX_NUM)
	{
		LC_BackPackEntry& kEntry = pkMail->m_aItemAffix[iIndex];
		int iPackType = GetWhichPackAutoPut( kEntry.GetTypeID() );

		bool bFull = pkGamePlayer->GetPackAsset()->IsPackFull(iPackType);
		if (bFull /*&& !LC_Helper::IsSpecialAffixItemID(kEntry.GetTypeID())*/)
		{
			if (iPackType == PACK_TYPE_BASIC)
			{
				SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_BASIC_PACK_IS_FULL);
			}
			/*
			if (iPackType == PACK_TYPE_MATERIAL)
			{
				SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_MATERIAL_PACK_IS_FULL);
			}
			*/
			return RE_FAIL;
		}
	}
	
	bool bValid = m_kMailAsset.IsValid(pkMail->GetMailID());
	if (bValid)
	{
		MG_REQ_GetItemAffix reqMsg;

		reqMsg.m_lPlayerID = pkGamePlayer->GetID();
		reqMsg.m_iMailID = pkMail->GetMailID();
		reqMsg.m_iIndex = iIndex;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETITEMAFFIX, &reqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}

	return RE_FAIL;
}
ResultType LC_ClientMailManager::RequestAllItemAffix()
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(m_iSelectedMailID);
	if (pkMail == NULL)
	{
		return RE_FAIL;
	}

	//如果未付费不能取附件
	if (!pkMail->GetUntreadFlag() && pkMail->GetPayFlag() == MAIL_NEED_PAY)
	{
		SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_NOT_PAY_CASH);
		return RE_FAIL;
	}
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	
	std::vector<int> kIndexVec;
	kIndexVec.clear();
	int iTrueAffixNumber = 0;
	for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
	{
		LC_BackPackEntry& kEntry = pkMail->m_aItemAffix[i];
		if (kEntry.GetValid())
		{
			kIndexVec.push_back(i);
			//if (!LC_Helper::IsSpecialAffixItemID(kEntry.GetTypeID()))
				iTrueAffixNumber++;
		}
	}
	int iPackEmptyNumber = pkGamePlayer->GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_BASIC);
	if (iTrueAffixNumber > iPackEmptyNumber)
	{
		SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_BASIC_PACK_IS_FULL);
		return RE_FAIL;
	}
	if (kIndexVec.empty())
	{
		return RE_FAIL;
	}
	bool bValid = m_kMailAsset.IsValid(pkMail->GetMailID());
	if (bValid)
	{
		for (std::vector<int>::const_iterator it = kIndexVec.begin(); it != kIndexVec.end(); ++it)
		{
			MG_REQ_GetItemAffix reqMsg;

			reqMsg.m_lPlayerID = pkGamePlayer->GetID();
			reqMsg.m_iMailID = pkMail->GetMailID();
			reqMsg.m_iIndex = *it;

			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETITEMAFFIX, &reqMsg);
			if (bCreateMsg)
			{
				pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
		return RE_SUCCESS;
	}

	return RE_FAIL;
}

int LC_ClientMailManager::GetItemAffixCount(int iIndex)
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMail(iIndex);
	if (pkMail == NULL)
	{
		return 0;
	}

	std::vector<int> kIndexVec;
	kIndexVec.clear();
	pkMail->GetItemAffixMaskIndex(kIndexVec);
	if (kIndexVec.empty())
	{
		return 0;
	}

	bool bValid = m_kMailAsset.IsValid(pkMail->GetMailID());
	if (bValid)
	{
		return kIndexVec.size();
	}

	return 0;
}

ResultType LC_ClientMailManager::RequestAllItemAffixByMailIndex(int iIndex)
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMail(iIndex);
	if (pkMail == NULL)
	{
		return RE_FAIL;
	}

	//如果未付费不能取附件
	if (!pkMail->GetUntreadFlag() && pkMail->GetPayFlag() == MAIL_NEED_PAY)
	{
		//SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_NOT_PAY_CASH);
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT))
	{
		//SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	

	std::vector<int> kIndexVec;
	kIndexVec.clear();
	pkMail->GetItemAffixMaskIndex(kIndexVec);
	int iPackEmptyNumber = pkGamePlayer->GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_BASIC);
	if (kIndexVec.size() > iPackEmptyNumber)
	{
		//SendLogicInfoMessage(RE_GETMAIL_ITEM_FAIL_BASIC_PACK_IS_FULL);
		return RE_GETMAIL_ITEM_FAIL_BASIC_PACK_IS_FULL;
	}
	if (kIndexVec.empty())
	{
		return RE_SUCCESS;
	}
	bool bValid = m_kMailAsset.IsValid(pkMail->GetMailID());
	if (bValid)
	{
		for (std::vector<int>::const_iterator it = kIndexVec.begin(); it != kIndexVec.end(); ++it)
		{
			MG_REQ_GetItemAffix reqMsg;

			reqMsg.m_lPlayerID = pkGamePlayer->GetID();
			reqMsg.m_iMailID = pkMail->GetMailID();
			reqMsg.m_iIndex = *it;

			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETITEMAFFIX, &reqMsg);
			if (bCreateMsg)
			{
				pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
		return RE_SUCCESS;
	}

	return RE_FAIL;
}
ResultType LC_ClientMailManager::RequestCashAffix()
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(m_iSelectedMailID);
	if (NULL == pkMail)
	{
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	
	if (pkMail->GetGold() > 0 || pkMail->GetSilver() > 0 || pkMail->GetCopper() > 0)
	{
		if (pkMail->GetPayFlag() == MAIL_NO_PAY)
		{
			MG_REQ_GetCashAffix reqMsg;

			reqMsg.m_lPlayerID = pkGamePlayer->GetID();
			reqMsg.m_iMailID = pkMail->GetMailID();

			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETCASHAFFIX, &reqMsg);
			if (bCreateMsg)
			{
				pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
				return RE_SUCCESS;
			}
		}
		else if (pkMail->GetPayFlag() == MAIL_NEED_PAY)
		{				
			MoneyType ulMailCash = LC_Money::MoneyInfo2MoneyNumber(pkMail->GetGold(), pkMail->GetSilver(), pkMail->GetCopper());				
			if (pkGamePlayer->GetUnBindCash().GetMoney() < ulMailCash)
			{
				SendLogicInfoMessage(RE_PAYFORMAIL_FAIL_CASH_IS_NOT_ENOUGH);
				return RE_FAIL;
			}

			MG_REQ_PayForMail reqMsg;

			reqMsg.m_uiMailID = pkMail->GetMailID();

			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PAYFORMAIL, &reqMsg);
			if (bCreateMsg)
			{
				pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
				return RE_SUCCESS;
			}
		}
	}

	return RE_FAIL;
}

ResultType LC_ClientMailManager::RequestUntreadMail()
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(m_iSelectedMailID);
	if (NULL == pkMail)
	{
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}
		
	if (pkMail->IsNeedDelete())
	{
		MG_REQ_DeleteMail reqMsg;
		reqMsg.m_lMailID = pkMail->GetMailID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DELETE_MAIL, &reqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}
	else
	{
		MG_REQ_UntreadMail reqMsg;
		reqMsg.m_lMailID = pkMail->GetMailID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UNTREAD_MAIL, &reqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}	

	return RE_FAIL;
}
ResultType LC_ClientMailManager::RequestUntreadMailByMailID(uint64_t MailID)
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(MailID);
	if (NULL == pkMail)
	{
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	if (pkMail->IsNeedDelete())
	{
		MG_REQ_DeleteMail reqMsg;
		reqMsg.m_lMailID = pkMail->GetMailID();

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DELETE_MAIL, &reqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}

	return RE_FAIL;
}
ResultType LC_ClientMailManager::RequestOpenMail()
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(m_iSelectedMailID);
	if (NULL == pkMail)
	{
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}
	if (!pkMail->GetReadFlag())
	{
		m_kMailAsset.MinusNotReadMailCount();
	}
    pkMail->SetReadFlag(true);

    //if(pkMail->GetCompleteMail())
      //  return RE_SUCCESS;

	GetMailAsset()->AddDirtyFlagRevisionID();

    MG_REQ_ReadMail kReqMsg;
    kReqMsg.m_lPlayerID = pkGamePlayer->GetID();
    kReqMsg.m_uiMailID	= pkMail->GetMailID();

    MG_PACKET_ENCODE_HANDLER hMsgHandler;
    bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_READMAIL, &kReqMsg);
    if (bCreateMsg)
    {
        pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
        return RE_SUCCESS;
    }

	return RE_FAIL;
}

ResultType LC_ClientMailManager::RequestCloseMail()
{
	LC_MailInfo* pkMail = GetMailAsset()->GetMailByMailID(m_iSelectedMailID);
	if (NULL == pkMail)
	{
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	if (pkMail->GetPayFlag() == MAIL_NO_PAY)
	{
		if (pkMail->GetGold() > 0 || pkMail->GetSilver() > 0 || pkMail->GetCopper() > 0)
		{
			return RE_FAIL;
		}
	}
	else if (pkMail->GetPayFlag() == MAIL_NEED_PAY)
	{
		if (pkMail->GetGold() > 0 || pkMail->GetSilver() > 0 || pkMail->GetCopper() > 0)
		{
			return RE_FAIL;
		}
	}		

	for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
	{
		if ( pkMail->m_aItemAffix[i].GetValid() )
		{
			return RE_FAIL;
		}
	}

	if ( pkMail->GetType() == MAIL_TYPE_ACTIVITY)
	{
		return RE_FAIL;
	}

	// 否则删除该邮件
	MG_REQ_DeleteMail reqMsg;
	reqMsg.m_lMailID = pkMail->GetMailID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DELETE_MAIL, &reqMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		return RE_SUCCESS;
	}

	return RE_FAIL;
}

ResultType LC_ClientMailManager::RequestAddAffix(int32_t iPos, int32_t iPackType, int32_t iIndex)
{
	RequestDeleletAffix(iPos);

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}
	
	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	
	if (iPos < 0)
	{			
		for (int i = 0; i < MAIL_ITEM_AFFIX_NUM_PLAYER_SEND; ++i)
		{
			if ( !m_kClientItemAffixInfo.IsValid(i) )
			{
				iPos = i;
				break;
			}
		}
	}

	if (iPos < 0)
	{
		SendLogicInfoMessage(RE_MAIL_ADD_AFFIX_FAIL_IS_FULL);
		return RE_MAIL_ADD_AFFIX_FAIL_IS_FULL;
	}

	LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(iPackType, iIndex);
	if (NULL == pkEntry)
	{
		SendLogicInfoMessage(RE_MAIL_ADD_AFFIX_FAIL_ITEM_INVALID);
		return RE_MAIL_ADD_AFFIX_FAIL_ITEM_INVALID;
	}

	if (!pkEntry->GetValid())
	{
		SendLogicInfoMessage(RE_MAIL_ADD_AFFIX_FAIL_ITEM_INVALID);
		return RE_MAIL_ADD_AFFIX_FAIL_ITEM_INVALID;
	}

	if (pkEntry->GetBind())
	{
		SendLogicInfoMessage(RE_MAIL_ADD_AFFIX_FAIL_ITEM_BIND);
		return RE_MAIL_ADD_AFFIX_FAIL_ITEM_BIND;
	}
	
	MG_REQ_AddItemToMailAffix reqMsg;
	reqMsg.m_iTargetPos = iPos;
	reqMsg.m_iPackType = iPackType;
	reqMsg.m_iItemIndex = iIndex;

	m_kClientItemAffixInfo.AddItemAffix(iPos, iPackType, iIndex);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADDITEM_MAIL, &reqMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		return RE_SUCCESS;
	}

	return RE_FAIL;
}

ResultType LC_ClientMailManager::RequestDeleletAffix(int32_t iPos)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//{
	//	return RE_FAIL;
	//}
	if(pkActionForbid->IsActionForbid(LAFT_MAIL, LAF_MAIL_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	
	if ( !GetClientItemAffixInfo()->IsValid(iPos) )
	{
		return RE_FAIL;
	}

	int iPackType = GetClientItemAffixInfo()->GetPackType(iPos);
	int iIndex = GetClientItemAffixInfo()->GetIndex(iPos);

	LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(iPackType, iIndex);
	if (NULL == pkEntry)
	{
		return RE_FAIL;
	}

	if (pkEntry->GetValid())
	{
		// 获取附件
		MG_REQ_DeletItemFromMailAffix reqMsg;
		reqMsg.m_iTargetPos = iPos;
		reqMsg.m_iPackType = iPackType;
		reqMsg.m_iItemIndex = iIndex;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DELITEM_MAIL, &reqMsg);
		if (bCreateMsg)
		{
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			return RE_SUCCESS;
		}
	}

	return RE_FAIL;
}

void LC_ClientMailManager::Reset()
{
	m_bIsREQMailAsset = false;
	m_bIsLoginRequest = false;
	m_iPreRequestTimeStamp = 0;
	m_kMailAsset.Reset();

	m_kClientItemAffixInfo.Reset();

	m_bMailHint = false;

	m_strPendingReceiverName = "";
}

void LC_ClientMailManager::ReceiveMailSimpleInfo(const Protocol::MG_RLT_MailAsset& rkMailInfo)
{
	m_kMailAsset.UpdateMailSampleInfo(rkMailInfo);
}

void LC_ClientMailManager::ReceiveMail(const Protocol::PS_MailInfo& kMail)
{
	m_kMailAsset.UpdateDetailMail(kMail);
}

void LC_ClientMailManager::DeleteMail(uint64_t lMailID)
{
	int iIndex = GetMailAsset()->GetIndexByMailID(lMailID);
	m_kMailAsset.DeleteMail(lMailID);
	SetSelectedMailIndex(iIndex);
}

void LC_ClientMailManager::GetItemAffix(uint64_t iMailID, int iIndex)
{
	m_kMailAsset.DeleteItemAffix(iMailID, iIndex);
}

void LC_ClientMailManager::GetCashAffix(uint64_t iMailID)
{
	m_kMailAsset.DeleteCashAffix(iMailID);
}

LC_MailAsset* LC_ClientMailManager::GetMailAsset()
{
	return &m_kMailAsset;
}

void LC_ClientMailManager::SetSelectedMailIndex(int iIndex)
{
	m_iSelectedIndex = iIndex;
	LC_MailInfo* pkMail = GetMailAsset()->GetMail(iIndex);
	if (pkMail != NULL)
	{
		m_iSelectedMailID = pkMail->GetMailID();
	}	
	else
	{
		m_iSelectedMailID = 0;
	}
}

int LC_ClientMailManager::GetSelectedMailIndex()
{
	int iIndex = GetMailAsset()->GetIndexByMailID(m_iSelectedMailID);
	//m_iSelectedMailID = iIndex;
	return iIndex;
}

void LC_ClientMailManager::CompleteWriteMail()
{
	m_kClientItemAffixInfo.Reset();

	m_strPendingReceiverName = "";
	m_strPendingSubject = "";
	m_strPendingContent = "";
	m_iPendingGold = 0;
	m_iPendingSilver = 0;
	m_iPendingCopper = 0;
}

void LC_ClientMailManager::ClearWriteFlag()
{
	m_bIsSending = false;
	m_bIsComplete = false;
}

bool LC_ClientMailManager::GetSendingFlag()
{
	return m_bIsSending;
}

bool LC_ClientMailManager::GetCompleteFlag()
{
	return m_bIsComplete;
}

void LC_ClientMailManager::SetSendingFlag(bool bFlag)
{
	m_bIsSending = bFlag;
}

void LC_ClientMailManager::SetCompleteFlag(bool bFlag)
{
	m_bIsComplete = bFlag;
}

LC_ClientItemAffixInfo* LC_ClientMailManager::GetClientItemAffixInfo()
{
	return &m_kClientItemAffixInfo;
}

void LC_ClientMailManager::CheckReceiverName(const StringType& strName)
{
	if (strName.size() <= 0)
	{
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if (NULL == pkSessionManager)
	{
		return;
	}

	pkSessionManager->FindNetCharSimpleInfo(strName, true);
}

#include "CF_MailCharge.h"
using namespace CSVFile;

MoneyType LC_ClientMailManager::ComputeMailCharge(MoneyType iGold, MoneyType iSilver, MoneyType iCopper)
{
	bool bHasMoneyAffix = false;
	if (iGold > 0 || iSilver || iCopper)
	{
		bHasMoneyAffix = true;
	}

	int iItemAffixNum = 0;
	for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
	{
		if ( GetClientItemAffixInfo()->IsValid(i) )
		{
			++iItemAffixNum;
		}
	}

	MoneyType iPostage = 0;
	// 邮件无附件
	if(0 == iItemAffixNum)
	{
		// 不寄钱的基本费用(文本邮件)
		if(!bHasMoneyAffix)
		{
			CF_MailCharge::DataEntry* pkDataEntry = CF_MailCharge::GetSingletonPtr()->GetEntryPtr(1);
			if (pkDataEntry)
			{
				iPostage = pkDataEntry->_lCost;
			}
		}
		// 寄钱的邮件费用
		else
		{
			CF_MailCharge::DataEntry* pkDataEntry = CF_MailCharge::GetSingletonPtr()->GetEntryPtr(21);
			if (pkDataEntry)
			{
				iPostage = pkDataEntry->_lCost;
			}
		}		
	}
	// 有附件的邮件，已包含寄钱费用
	else
	{ 
		int iIndex = 10 + iItemAffixNum;
		CF_MailCharge::DataEntry* pkDataEntry = CF_MailCharge::GetSingletonPtr()->GetEntryPtr(iIndex);
		if (pkDataEntry)
		{
			iPostage = pkDataEntry->_lCost;
		}
	}
	return iPostage;
}

StringType LC_ClientMailManager::GetMailCharge(MoneyType iGold, MoneyType iSilver, MoneyType iCopper)
{
	MoneyType iPostage = ComputeMailCharge(iGold, iSilver, iCopper);
	return LC_Money::MoneyNumber2String(iPostage, true);
}

bool LC_ClientMailManager::IsExistPendingName(const StringType& strName)
{
	StringListIter it = std::find(m_kPendingTargetList.begin(),m_kPendingTargetList.end(), strName);
	return ( it != m_kPendingTargetList.end() );	
}

void LC_ClientMailManager::AddPendingName(const StringType& strName)
{
	if ( !IsExistPendingName(strName) )
	{
		m_kPendingTargetList.push_back(strName);
	}	
}

void LC_ClientMailManager::ErasePendingName(const StringType& strName)
{
	StringListIter it = std::find(m_kPendingTargetList.begin(),m_kPendingTargetList.end(), strName);
	if( it != m_kPendingTargetList.end() )
	{
		m_kPendingTargetList.erase(it);
	}
}

ResultType LC_ClientMailManager::SendPendingMail()
{
	ResultType iCheckContentResult = RequestWriteMailContentCheck(m_strPendingReceiverName, m_strPendingSubject, m_strPendingContent, m_iPendingGold, m_iPendingSilver, m_iPendingCopper, m_bIsPay);
	if (iCheckContentResult == RE_SUCCESS)
	{
		ResultType iCheckPostageResult = RequestWriteMailPostageCheck(m_iPendingGold, m_iPendingSilver, m_iPendingCopper);
		if (iCheckPostageResult != RE_FAIL)
		{
			RequestWriteMailExcute(m_strPendingReceiverName, m_strPendingSubject, m_strPendingContent, m_iPendingGold, m_iPendingSilver, m_iPendingCopper, m_bIsPay);
			return RE_SUCCESS;
		}
		else
		{
			return iCheckPostageResult;
		}
	}
	else
	{
		return iCheckContentResult;
	}
}

void LC_ClientMailManager::SetMailHintFlag(bool bHint)
{
	m_bMailHint = bHint;
}

bool LC_ClientMailManager::GetMailHintFlag()
{
	return m_bMailHint;
}

void LC_ClientMailManager::SetMailboxFullFlag(bool bFlag)
{
	m_bMailboxFull = bFlag;
}

bool LC_ClientMailManager::GetMailboxFullFlag()
{
	return m_bMailboxFull;
}

void LC_ClientMailManager::SetIsReadingMailFlag(bool bFlag)
{
	m_bIsReadingMail = bFlag;
}

bool LC_ClientMailManager::GetIsReadingMailFlag()
{
	return m_bIsReadingMail;
}
