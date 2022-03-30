

#include "TS_WriteMailTran.h"

#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"

#include "MG_GameProtocol.h"
#include "MG_Game2Mail.h"

#include "oss_define.h"

#include "UT_ServerHelper.h"

#include "GameServerApp.h"
using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

AO_Server_ReqWriteMail::AO_Server_ReqWriteMail()
{
}

emTranOperate AO_Server_ReqWriteMail::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	MG_REQ_WriteMail* pkReqWriteMailMsg = static_cast<MG_REQ_WriteMail*>(pkMsg);

	MG_GM_REQ_POST_MAIL reqGMPostMail;

	ResultType iResultType = pkPlayer->WriteMail(pkReqWriteMailMsg->m_kWriteMail, reqGMPostMail.m_stMailInfo);

	if (RE_WRITEMAIL_SUCCESS == iResultType)
	{
		// 1. 设置上下文
		{
			SetContentFlag(true);
			m_kMailInfo = reqGMPostMail.m_stMailInfo;
		}
		
		// 2. 组织邮件发给mail server
		reqGMPostMail.m_uiTransactionID = uiID;
		reqGMPostMail.m_stMailInfo.lSenderCharID = pkPlayer->GetInstance();

		SendMsgToMailDB(MGPT_GM_REQ_POST_MAIL, &reqGMPostMail, pkPlayer->GetUserID(), pkPlayer->GetInstance());

		int itemcount = 0;

		for(int i =0; i < MAIL_ITEM_AFFIX_NUM; ++i)
		{
			LC_BackPackEntry kTempEntry;
			kTempEntry.GetFromProtocol(reqGMPostMail.m_stMailInfo.aItemAffix[i]);
			if(kTempEntry.GetBackPackValueByType(LPSFET_TYPE_ID) > 0)
			{
				++itemcount;
			}
			{
				if(kTempEntry.GetBackPackValueByType(LPSFET_VALID))
				{
					m_rlt_opt->hblist.push_back(kTempEntry);
				}
			}
		}

		uint8_t bPayFlag = reqGMPostMail.m_stMailInfo.bPayFlag;
		int64_t llMoney = reqGMPostMail.m_stMailInfo.m_llMoney;

		uint8_t uiMailType = 0;
		// 附件都为空
		if (0 == itemcount)
		{
			// 发送金钱为0，则为 “纯文字”
			if (0 == llMoney)
			{
				uiMailType = 0;
			}
			// 金钱不为0， 则为 “只发送金钱”
			else if (llMoney > 0)
			{
				uiMailType = 2;
			}
		}
		// 有附件
		else
		{
			// 有付费邮件标志，则为 “付费取信”
			if (bPayFlag)
			{
				uiMailType = 4;
			}
			else if (llMoney > 0)
			{
				uiMailType = 3;
			}
			else
			{
				uiMailType = 1;
			}
		}

		m_rlt_opt->charID = pkPlayer->GetInstance();

		m_rlt_opt->toCharID = reqGMPostMail.m_stMailInfo.lReceiverCharID;
		STDSTR2TPSTR(reqGMPostMail.m_stMailInfo.szReceiverCharName, m_rlt_opt->rcname);
		if(uiMailType == 2)
		{
			m_rlt_opt->send_gp = (int)llMoney;
		}
		else if(uiMailType == 3)
		{
			m_rlt_opt->pay_gp = (int)llMoney;
		}
		STDSTR2TPSTR(reqGMPostMail.m_stMailInfo.szSubject, m_rlt_opt->Title);
		STDSTR2TPSTR(reqGMPostMail.m_stMailInfo.szContent, m_rlt_opt->Context);

		SetResultID(RE_SUCCESS);
		return TRAN_OP_NEXT;
	}
	else
	{
		SetResultID(iResultType);
		return TRAN_OP_FAILED;
	}	
}

void AO_Server_ReqWriteMail::RespondRltMsg()
{	
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		MG_RLT_WriteMail rltMsg;
		rltMsg.m_iResultType = GetResultID();

		pkPlayer->SendMsgToClient(MGPT_RLT_WRITE_MAIL, &rltMsg);		
	}
}

void AO_Server_ReqWriteMail::Rollback()
{	
	if ( GetContentFlag() )
	{		
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "send mail error, player:%llu",  GetCharID());

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
		// 写物品附件日志
		
		//邮资
		MoneyType iPostage = 0;
		int iItemAffixNum = 0;
		for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
		{
			if (m_kMailInfo.aItemAffix[i].GetItemBaseData_Valid())
			{
				++iItemAffixNum;
			}
		}
		if (m_kMailInfo.m_llMoney > 0)
		{
			--iItemAffixNum;
		}
		iPostage = 20 + iItemAffixNum * 30;
		
		if (pkPlayer)
		{
			pkPlayer->AddCash(iPostage, CT_UNBIND_CASH, SYS_FUNC_TP_MAIL_SEND, INVALID_MAIL_ID);
			for (int i = 0; i < MAIL_ITEM_AFFIX_NUM; ++i)
			{
				Protocol::PS_ItemAssetEntry& rkEntry = m_kMailInfo.aItemAffix[i];

				LC_BackPackEntry kTempEntry;
				kTempEntry.GetFromProtocol(rkEntry);
				kTempEntry.WriteToLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR);
				int itemID = kTempEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
				if (itemID == SPEC_ITEM_TID_CASH)
				{
					pkPlayer->AddCash(kTempEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), CT_UNBIND_CASH, SYS_FUNC_TP_MAIL_SEND, INVALID_MAIL_ID);
					continue;
				}
				if (itemID == SPEC_ITEM_TID_YUANBAO)
				{
					pkPlayer->AddCash(kTempEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), CT_UNBIND_YUANBAO, SYS_FUNC_TP_MAIL_SEND, INVALID_MAIL_ID);
					continue;
				}
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "mail affix%d item info Begin", i);
				kTempEntry.WriteToLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR);
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "mail affix%d item info End", i);
				int iPackType = GetWhichPackAutoPut(itemID);

				PackOperateInfoList objOPList;
				objOPList.clear();

				PackOperateInfo objData;
				objData.Reset();

				objData.m_iOPType = OPT_ADD_ITEM;
				objData.m_bOPEntry = true;
				objData.m_bMerger  = true;
				objData.m_iPackType  = iPackType;
				objData.m_kPackEntry.Clone(kTempEntry);
				objOPList.push_back(objData);

				ResultType iResult = pkPlayer->GetPackAsset()->ExecutePackOP(objOPList,true);
				if (iResult != RE_SUCCESS)
				{
					SetResultID(RE_GETMAIL_ITEM_FAIL_PACK_FULL);
				}
				else
				{
					// 成功，就显示响应消息，并通知mail server
					/*SetResultID(RE_GETMAIL_ITEM_AFFIX_SUCCESS);

					RespondRltMsg();*/

					pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
					PackOPRecordSet * record_set = pkPlayer->GetPackAsset()->GetRecordSet();
					pkPlayer->HandlePackOPRecord(record_set, SYS_FUNC_TP_MAIL_SEND, INVALID_MAIL_ID);
					if(!record_set->m_Add_Items.empty())
					{
						PackEntryOPRecord& rd = *record_set->m_Add_Items.begin();
						LC_BackPackEntry * pack = rd.m_Entry;
						if(pack != NULL)
						{
							oss_role_recv_mail(pkPlayer,"0","0","0",0,0,rd.m_lNum,pack);
						}
					}
				}
			}
		}
		//// 回滚金钱			
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Attachment money information UnBindCash: %lld", m_kMailInfo.m_llMoney);	
	}
}

void AO_Server_ReqWriteMail::Reset()
{
	TS_AtomOperate::Reset();
	m_kMailInfo.clear();
}
//////////////////////////////////////////////////////////////////////////

emTranOperate AO_Server_RltWriteMail::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID) 
{ 
	MG_GM_RLT_POST_MAIL* msg = static_cast<MG_GM_RLT_POST_MAIL*>(pkMsg);
	
	if (RE_SUCCESS == msg->m_ushResult)
	{
    //oss.mail_id = msg->m_uiMailIDOnMailDB;
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
		if( pkPlayer )
		{
			GameLogic::LC_BackPackEntryPtrList  blist;
			for( GameLogic::LC_BackPackEntryList::iterator i = this->hblist.begin(); i != this->hblist.end(); ++i)
			{
				blist.push_back(&(*i));
			}
			oss_role_send_mail(pkPlayer, "0" , this->toCharID , this->rcname , this->Title , this->Context , 0 , this->send_gp , this->pay_gp, blist);
		}
		SetResultID(RE_WRITEMAIL_SUCCESS);
		RespondRltMsg();
		return TRAN_OP_NEXT;	
	}
	else
	{
		SetResultID(msg->m_ushResult);
		RespondRltMsg();
		return TRAN_OP_FAILED;
	}
}

void AO_Server_RltWriteMail::RespondRltMsg() 
{
	MG_RLT_WriteMail rltMsg;
	rltMsg.m_iResultType = GetResultID();

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_WRITE_MAIL, &rltMsg);	
	}
}

//////////////////////////////////////////////////////////////////////////

TS_Server_WriteMail::TS_Server_WriteMail()
{
  AO_Server_ReqWriteMail * req = T_NEW_D AO_Server_ReqWriteMail;
  AO_Server_RltWriteMail * rlt = T_NEW_D AO_Server_RltWriteMail;
  req->set_rlt_opt(rlt);
	m_kStepTranMap[STEP_REQ_GM_WRITE_MAIL] = req;
	m_kStepTranMap[STEP_RLT_GM_WRITE_MAIL] = rlt;
}

TS_Server_WriteMail::~TS_Server_WriteMail()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}