

#include "TS_GetItemAffixTran.h"

#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"

#include "MG_GameProtocol.h"
#include "MG_Game2Mail.h"

#include "oss_define.h"

#include "GameServerApp.h"
#include "CF_ShopList.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace Utility;
using namespace GameLogic;
using namespace CSVFile;

emTranOperate AO_Server_ReqGetItemAffix::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_ReqGetItemAffix::Execute_fail, return 1", GetCharID());

		SetResultID(RE_FAIL);

		return TRAN_OP_FAILED;
	}

	LC_BackPackEntry* pkPackEntry = pkPlayer->GetPackAsset()->GetEmptyPackEntry(m_iPackType, m_iPackIndex);
	// 后续需要设计，锁定背包位置功能，这样是前面的业务复杂，后端的简单
	if (NULL == pkPackEntry)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_ReqGetItemAffix::Execute_fails, return 2", GetCharID());
		SetResultID(RE_FAIL);

		return TRAN_OP_FAILED;
	}

	MG_REQ_GetItemAffix* pkReqMsg = static_cast<MG_REQ_GetItemAffix*>(pkMsg);

	{
		pkPackEntry->SetPlaceHolder(uiID);
		
		MG_GM_REQ_TAKE_MAIL_ITEM reqTakeMailItem;
		reqTakeMailItem.m_uiMailID = pkReqMsg->m_iMailID;
		reqTakeMailItem.m_ushIndex = pkReqMsg->m_iIndex;
		reqTakeMailItem.m_uiTransactionID = uiID;
		reqTakeMailItem.m_iPackType = m_iPackType;
		reqTakeMailItem.m_iPackIndex = m_iPackIndex;

		SendMsgToMailDB(MGPT_GM_REQ_TAKE_MAIL_ITEM, &reqTakeMailItem, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		return TRAN_OP_NEXT;
	}

	return TRAN_OP_FAILED;
}
void AO_Server_ReqGetItemAffix::Rollback()
{
	// 写出未正常放入背包物品日志
	GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "player get mail item affix failed!", GetCharID(), GetResultID());
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_ReqGetItemAffix::Rollback_fail, return 1", GetCharID());
		return;
	}

	LC_BackPackEntry* pkPackEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(m_iPackType, m_iPackIndex);
	if (pkPackEntry && pkPackEntry->IsPlaceHolder())
	{
		pkPackEntry->WriteToLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR);
		pkPackEntry->SetPlaceHolder(0);
	}
	else
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_ReqGetItemAffix::Rollback_fail, return 2", GetCharID());
	}
}

void AO_Server_ReqGetItemAffix::Reset()
{
	TS_AtomOperate::Reset();
	m_iPackType = PACK_TYPE_BASIC;
	m_iPackIndex = -1;
}
//////////////////////////////////////////////////////////////////////////
emTranOperate AO_Server_RltGetItemAffix::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{	
	MG_GM_RLT_TAKE_MAIL_ITEM* pkRltMsg = static_cast<MG_GM_RLT_TAKE_MAIL_ITEM*>(pkMsg);

	m_uiMailID = pkRltMsg->m_uiMailID;
	m_uiTranID = pkRltMsg->m_uiTransactionID;
	m_ushIndex = pkRltMsg->m_ushIndex;
	m_kEntry   = pkRltMsg->m_stItem;

	m_iPackType = pkRltMsg->m_iPackType;
	m_iPackIndex = pkRltMsg->m_iPackIndex;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if ( !pkPlayer )
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_RltGetItemAffix::Execute_fail, return 1", GetCharID());
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	LC_BackPackEntry* pkPackEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(m_iPackType, m_iPackIndex);
	if (NULL==pkPackEntry || !pkPackEntry->IsPlaceHolder() || pkPackEntry->GetPlaceHolderTransID()!=uiID)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_RltGetItemAffix::Execute_fail, return 2", GetCharID());
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	pkPackEntry->SetPlaceHolder(0);

	if (pkRltMsg->m_ushResult == RE_SUCCESS)
	{	
		GameLogic::LC_BackPackEntry kEntry;
		kEntry.GetFromProtocol(pkRltMsg->m_stItem);
		
		//放入时  锁定了
		kEntry.SetBackPackValueByType(LPSFET_LOCKED, false);

		int32_t iCusSrcType = SYS_FUNC_TP_MAIL_AFFIX;
		uint64_t iCusSrcID = m_uiMailID;
		int32_t param = 0;

		vector<int> nParams;
		nParams.clear();
		if(pkRltMsg->m_iExtraParam > 0)
			nParams.push_back(pkRltMsg->m_iExtraParam);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MAIL_OPERATE, OPERATE_LOGTYPE1);
		if(pkRltMsg->m_iLogCode > 0)
			nLogCode = pkRltMsg->m_iLogCode;

		int itemID = kEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
		if (IsSpecialItemTypeID(itemID))
		{
			if(itemID == SPEC_ITEM_TID_ACCOUNT_POINT)
			{
				iCusSrcType = kEntry.GetBackPackValueByType(LCSPET_SRC_1);
				param = kEntry.GetBackPackValueByType(LCSPET_SRC_2);
			}
			USE_SPECAIL_ITEM(pkPlayer, itemID, kEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), iCusSrcType, iCusSrcID, param);
			RecordCashOperateLog(pkPlayer,nLogCode, GET_OPERATE, itemID, kEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT),-1,nParams);
			SetResultID(RE_GETMAIL_ITEM_AFFIX_SUCCESS);
			RespondRltMsg();
			oss_role_recv_mail(pkPlayer, "0", "0", "0", 0, 0, kEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), &kEntry);
			return TRAN_OP_NEXT;
		}
		else
		{
			int iPackType = GetWhichPackAutoPut(itemID);
			PackOperateInfoList objOPList;
			objOPList.clear();

			PackOperateInfo objData;
			objData.Reset();

			objData.m_iOPType = OPT_ADD_ITEM;
			objData.m_bOPEntry = true;
			objData.m_bMerger  = true;
			objData.m_iPackType  = iPackType;
			objData.m_kPackEntry.Clone(kEntry);
			objOPList.push_back(objData);

			ResultType iResult = pkPlayer->GetPackAsset()->ExecutePackOP(objOPList, true, NULL, nLogCode, nParams);
			//ResultType iResult = pkPlayer->GetPackAsset()->AddItemToPack(iPackType, kEntry, false);

			if (iResult != RE_SUCCESS)
			{
				SetResultID(RE_GETMAIL_ITEM_FAIL_PACK_FULL);
				return TRAN_OP_FAILED;
			}
			else
			{
				pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

				// 成功，就显示响应消息，并通知mail server
				SetResultID(RE_GETMAIL_ITEM_AFFIX_SUCCESS);
				RespondRltMsg();

				PackOPRecordSet * record_set = pkPlayer->GetPackAsset()->GetRecordSet();
				pkPlayer->HandlePackOPRecord(record_set, iCusSrcType?iCusSrcType:SYS_FUNC_TP_MAIL_AFFIX, iCusSrcID?iCusSrcID:m_uiMailID);
				if(!record_set->m_Add_Items.empty())
				{
					PackEntryOPRecord& rd = *record_set->m_Add_Items.begin();
					LC_BackPackEntry * pack = rd.m_Entry;
					if(pack != NULL)
					{
						oss_role_recv_mail(pkPlayer, "0", "0", "0", 0, 0, rd.m_lNum, pack);
					}
				}
				return TRAN_OP_NEXT;
			}
		}
	}
	else
	{
		// mail servr 返回失败同样是完成流程，
		SetResultID(RE_GETMAIL_ITEM_FAIL_ITEM_NOT_EXIST);

		RespondRltMsg();
		return TRAN_OP_NEXT;
	}
}

void AO_Server_RltGetItemAffix::Rollback()
{
	// 写出未正常放入背包物品日志
	GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "player get mail item affix failed!", GetCharID(), GetResultID());
	LC_BackPackEntry kTempEntry;
	kTempEntry.GetFromProtocol(m_kEntry);
	kTempEntry.WriteToLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR);
}

void AO_Server_RltGetItemAffix::RespondRltMsg()
{
	MG_RLT_GetItemAffix kGetItemAffix;

	kGetItemAffix.m_lPlayerID = GetUserID();
	kGetItemAffix.m_iResultType = GetResultID();
	kGetItemAffix.m_iMailID = m_uiMailID;
	kGetItemAffix.m_iIndex = m_ushIndex;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_GETITEMAFFIX, &kGetItemAffix);

		//通知邮件服务器修改已读标志
		MG_GM_REQ_OPEN_MAIL	kReqMsg;
		kReqMsg.m_uiMailID = m_uiMailID;
		SendMsgToMailDB(MGPT_GM_REQ_OPEN_MAIL, &kReqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
}

void AO_Server_RltGetItemAffix::Reset()
{
	TS_AtomOperate::Reset();
	m_uiMailID = 0;
	m_uiTranID = 0;
	m_ushIndex = 0;
	m_kEntry.clear();
}
//////////////////////////////////////////////////////////////////////////
TS_Server_GetItemAffixFromMail::TS_Server_GetItemAffixFromMail()
{
	m_kStepTranMap[STEP_REQ_GM_GET_ITEM_AFFIX] = T_NEW_D AO_Server_ReqGetItemAffix;
	m_kStepTranMap[STEP_RLT_GM_GET_ITEM_AFFIX] = T_NEW_D AO_Server_RltGetItemAffix;
}

TS_Server_GetItemAffixFromMail::~TS_Server_GetItemAffixFromMail()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}
