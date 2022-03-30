#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_VerifyProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_VehicleProtocol.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_UPGRADE_SUBORDINATE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return;

	MG_REQ_UpgraedeSubordinate reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MoneyType preYuanBao = pkPlayer->GetUnBindCash().GetMoney();

	MG_RLT_UpgraedeSubordinate rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_iSubType = reqMsg.m_iSubType;
	rltMsg.m_iPayIndex = reqMsg.m_iPayIndex;

	rltMsg.m_iResult = pkPlayer->Upgrad_Subordinate(reqMsg.m_iSubType, reqMsg.m_iPayIndex, rltMsg.m_ModifyAttrIDVec, rltMsg.m_ModifyAttrLevelVec);
	rltMsg.m_iStage = pkPlayer->GetSubordinateStage(reqMsg.m_iSubType);

	MoneyType nowYuanBao = pkPlayer->GetUnBindCash().GetMoney();
	if (nowYuanBao >= preYuanBao)
	{
		rltMsg.m_iYuanBaoDelta = (int32_t)(nowYuanBao - preYuanBao);
	}
	else
	{
		rltMsg.m_iYuanBaoDelta = (int32_t)(preYuanBao - nowYuanBao);
		rltMsg.m_iYuanBaoDelta = -rltMsg.m_iYuanBaoDelta;
	}
	rltMsg.m_iYuanBaoDelta = (int32_t)(pkPlayer->GetUnBindCash().GetMoney() - preYuanBao);

	if(rltMsg.m_iResult==RE_SUCCESS || rltMsg.m_iResult==RE_UPGRAD_SUB_SUCCESS)
	{
		bool bDirty;
		pkPlayer->SyncSubordinateInfo(bDirty);
		pkPlayer->SetDirtyFlag(DIRTY_FLAG_SUBORDINATE_INFO, false);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_UPGRADE_SUBORDINATE, &rltMsg);
	
	// 名号 萌宠 背饰 幻兽 升阶
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC,false);
	int32_t reasonType = SPART_None;
	switch(reqMsg.m_iSubType)
	{
	case LC_SUB_TP_VEHICLE:
		reasonType = SPART_Sub_Vehicle_1;
		break;
	case LC_SUB_TP_DEVA_EYE:
		reasonType = SPART_Sub_Devaeye_1;
		break;
	case LC_SUB_TP_MUHUN:
		reasonType = SPART_Sub_Wing_1;
		break;
	case LC_SUB_TP_TALISMAN:
		reasonType = SPART_Sub_MingHao_1;
		break;
	}
	pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, reqMsg.m_iSubType);
	return;
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_SUBORDINATE_OPERATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_SubordinateOperation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return;

	MG_RLT_SubordinateOperation rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_iSubType = reqMsg.m_iSubType;
	rltMsg.m_iOPCode = reqMsg.m_iOPCode;
	rltMsg.m_iOPParam = reqMsg.m_iOPParam;

	switch(reqMsg.m_iOPCode)
	{
	case LC_SUB_OP_SET_TRANSFROM:
		{
			rltMsg.m_iOPRslt = pkPlayer->GetSubordinateAsset().ChangeSetTransform(reqMsg.m_iSubType, reqMsg.m_iOPParam);
			if (rltMsg.m_iOPRslt == RE_SUCCESS )
			{
				//更改皮肤的时候，进阶线皮肤id会变化，所以要更新FillRankSubData
				pkPlayer->UpdateRankUserValueByRankType(Rank_Type_Vehicle);
				pkPlayer->SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Subordinate);
			}
		}
		break;
	case LC_SUB_OP_ADD_TRANSFORM:
		{
			rltMsg.m_iOPRslt = pkPlayer->AddSubTransform(reqMsg.m_iSubType, reqMsg.m_iOPParam, false);
		}
		break;
	case LC_SUB_OP_BUY_TALISMAN_EXP:
		{
			rltMsg.m_iOPRslt = pkPlayer->GetSubordinateAsset().BuyTalismanExp();
		}
		break;
	case LC_SUB_OP_ADD_AND_SET_TRANSFORM:
		{
			rltMsg.m_iOPRslt = pkPlayer->AddSubTransform(reqMsg.m_iSubType, reqMsg.m_iOPParam, false);
			if(RE_SUCCESS == rltMsg.m_iOPRslt)
			{
				rltMsg.m_iOPRslt = pkPlayer->GetSubordinateAsset().ChangeSetTransform(reqMsg.m_iSubType, reqMsg.m_iOPParam);
				if (RE_SUCCESS == rltMsg.m_iOPRslt)
				{
					//更改皮肤的时候，进阶线皮肤id会变化，所以要更新FillRankSubData
					pkPlayer->UpdateRankUserValueByRankType(Rank_Type_Vehicle);
					pkPlayer->SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Subordinate);
				}
			}
		}
		break;
	default:
		break;
	}
	
	pkPlayer->SendMsgToClient(MGPT_RLT_SUBORDINATE_OPERATION, &rltMsg);
}

ON_HANDLE(MG_REQ_SETSUBTRANSFORMFLAG, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_SetSubTransformFlag reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SetSubTransformFlag rltMsg;
	rltMsg.m_nSubType = reqMsg.m_nSubType;
	rltMsg.m_nTransformID = reqMsg.m_nTransformID;
	rltMsg.m_nResult = pkPlayer->GetSubordinateAsset().SetSubTransformFlag(reqMsg.m_nSubType, reqMsg.m_nTransformID);
	rltMsg.m_bIsNew = rltMsg.m_nResult ==RE_SUCCESS ? false : true;
	pkPlayer->SendMsgToClient(MG_RLT_SETSUBTRANSFORMFLAG, &rltMsg);
}

}
