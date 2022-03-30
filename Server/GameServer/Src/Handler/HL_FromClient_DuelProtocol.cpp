#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_DuelProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerDuelFlow.h"
#include "LC_ServerDuelFlowMgr.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//------------------------------------------------------------------------------
inline void DuelHL_FillPlayerName(MG_RLT_DuelInvite& rltMsg, LC_ServerPlayer* pkInviter, LC_ServerPlayer* pkBeInviter)
{
	if (pkInviter)
	{
		TPSTR2STDSTR(pkInviter->GetOwnerCharName(), rltMsg.m_ucInviterPlayerName);
	}

	if (pkBeInviter)
	{
		TPSTR2STDSTR(pkBeInviter->GetOwnerCharName(), rltMsg.m_ucBeInviterPlayerName);
	}
}
//------------------------------------------------------------------------------
//处理玩家发起的决斗请求
ON_HANDLE(MGPT_REQ_DUELINVITE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_DuelInvite reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type lInvitePlayerID = reqMsg.m_lInvitePlayerID;
	object_id_type lBeInvitePlayerID = reqMsg.m_lBeInvitePlayerID;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	//检查操作屏蔽
	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_DUEL, LAF_DUEL_MAX_COUNT))
	{
		return;
	}

	LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lBeInvitePlayerID);
	if (!pkBeInvitePlayer)
	{
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	//检查玩家双方是否能够进行决斗
	UINT unResult;
	unResult = LC_PlayerDuelHelper::CheckPlayersCanDuel(lInvitePlayerID, lBeInvitePlayerID);

	//---------检查发起和被邀请的双方是否满足条件 （检查的排序思路，按照开销和能提早过滤非法请求的思路）
	//--检查双方是否在允许决斗的区域内 错误返回  这里不允许决斗
	//--检查是否在合理距离内（水平50M,高度20M） 错误返回 距离太远
	//--检查双方的状态是否允许发起决斗 （发起者本身已经在决斗中，发起者死亡，发起者被控制，被邀请者死亡，被邀请者决斗中，这个通过状态表格来驱动把）
	//---------检查OK

	//Error错误码 添加一个不处理的操作
	if (unResult)
	{
		//发送错误消息，结束
		MG_RLT_DuelInvite rltMsg;
		rltMsg.m_lInvitePlayerID = lInvitePlayerID;
		rltMsg.m_lBeInvitePlayerID = lBeInvitePlayerID;
		rltMsg.m_ucType = DI_ERROR;
		rltMsg.m_lParam = unResult;
		DuelHL_FillPlayerName(rltMsg, pkPlayer, pkBeInvitePlayer);
		pkPlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &rltMsg);
		return;
	}

	//在服务器加入玩家申请列表，保存在被邀请玩家的服务器端
	float fCurTime = GET_PROCESS_TIME;
	LC_ServerDuelFlowMgr* pkDuelFlowMgr = LC_ServerDuelFlowMgr::Instance();
	LC_ServerDuelFlow* pkDuelFlow = pkDuelFlowMgr->NewDuelFlow();

	pkDuelFlow->SetInviter(lInvitePlayerID);
	pkDuelFlow->SetBeInviter(lBeInvitePlayerID);
	pkDuelFlow->ChangeState(LC_ServerDuelFlow::DS_WaitForReq, fCurTime);

	MG_RLT_DuelInvite rltMsg;
	rltMsg.m_lInvitePlayerID = lInvitePlayerID;
	rltMsg.m_lBeInvitePlayerID = lBeInvitePlayerID;
	rltMsg.m_ucType = DW_INVITE;
	rltMsg.m_lParam = pkDuelFlow->GetID();
	rltMsg.m_lParamExtra1 = (uint32_t)pkDuelFlow->GetDuelPos().x;
	rltMsg.m_lParamExtra2 = (uint32_t)pkDuelFlow->GetDuelPos().y;


	DuelHL_FillPlayerName(rltMsg, pkPlayer, pkBeInvitePlayer);

	//消息穿透，发送给被邀请玩家
	//广播一个消息给周围玩家
	pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &rltMsg);
}
//------------------------------------------------------------------------------
//决斗状态
//1。不正常的网络包，不能改变服务器的状态，采用简单丢弃的方法，是否考虑给予回应
ON_HANDLE(MGPT_RLT_DUELINVITE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_DuelInvite rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerDuelFlowMgr* pkDuelFlowMgr = LC_ServerDuelFlowMgr::Instance();
	LC_ServerDuelFlow* pkDuelFlow = pkDuelFlowMgr->GetDuelFlow(rltMsg.m_lParam);
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkInvitePlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rltMsg.m_lInvitePlayerID);
	LC_ServerPlayer* pkBeInvitePlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rltMsg.m_lBeInvitePlayerID);

	//检查操作屏蔽
	if (pkInvitePlayer && pkInvitePlayer->GetActionForbid()->IsActionForbid(LAFT_DUEL, LAF_DUEL_MAX_COUNT))
	{
		return;
	}

	switch (rltMsg.m_ucType)
	{
		case DR_ACCEPT:
			{
				//检查是否合理消息
				if (pkBeInvitePlayer == NULL) //错误网络包，简单丢掉
				{
					return;
				}
				if (pkInvitePlayer == NULL) //邀请玩家不存在
				{
					MG_RLT_DuelInvite SC_RltMsg;
					SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
					SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
					SC_RltMsg.m_ucType = DI_ERROR;
					SC_RltMsg.m_lParam = RE_DUEL_ERROR_BUSY;
					SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

					DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
					pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					if (pkDuelFlow)
					{
						pkDuelFlow->ChangeState(LC_ServerDuelFlow::DS_End, GET_PROCESS_TIME);
					}
					return;
				}
				if (pkInvitePlayer && pkBeInvitePlayer) //检查是不是在同一个决斗的正确回应包
				{
					if (!LC_PlayerDuelHelper::CheckPlayersInSameDuel(pkInvitePlayer, pkBeInvitePlayer, rltMsg.m_lParam)) //不是同一个决斗
					{
						MG_RLT_DuelInvite SC_RltMsg;
						SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
						SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
						SC_RltMsg.m_ucType = DI_ERROR;
						SC_RltMsg.m_lParam = RE_DUEL_ERROR_BUSY;
						SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

						DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
						pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
						return;
					}
				}
				if (pkDuelFlow) //检查PK流程是否正确
				{
					if (pkDuelFlow->GetState() != LC_ServerDuelFlow::DS_WaitForReq)
					{
						MG_RLT_DuelInvite SC_RltMsg;
						SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
						SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
						SC_RltMsg.m_ucType = DI_ERROR;
						SC_RltMsg.m_lParam = RE_DUEL_ERROR_BUSY;
						SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

						DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
						pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
						return; //错误网络包，简单丢掉 状态不同步
					}
				}
				else
				{
					MG_RLT_DuelInvite SC_RltMsg;
					SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
					SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
					SC_RltMsg.m_ucType = DI_ERROR;
					SC_RltMsg.m_lParam = RE_DUEL_ERROR_BUSY;
					SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

					DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
					pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					return;//错误网络包，简单丢弃掉 找不到对应的决斗流程
				}

				//检查玩家是否可以接受，可以接受，就xxx
				UINT bRet = LC_PlayerDuelHelper::CheckPlayersCanDuel((rltMsg.m_lInvitePlayerID), (rltMsg.m_lBeInvitePlayerID) , false);
				if (bRet != 0)
				{
					MG_RLT_DuelInvite SC_RltMsg;
					SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
					SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
					SC_RltMsg.m_ucType = DI_ERROR;
					SC_RltMsg.m_lParam = bRet;
					SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

					DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
					pkInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					pkDuelFlow->ChangeState(LC_ServerDuelFlow::DS_End, GET_PROCESS_TIME);
				}
				else
				{
					MG_RLT_DuelInvite SC_RltMsg;
					SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
					SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
					SC_RltMsg.m_ucType = DR_ACCEPT;
					SC_RltMsg.m_lParam = pkDuelFlow->GetID();
					SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

					DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
					pkInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					pkDuelFlow->ChangeState(LC_ServerDuelFlow::DS_WaitForStart, GET_PROCESS_TIME);
				}
				//生成决斗流程，并保存
			}
			break;

		case DR_REFUSE:
			{
				if (pkBeInvitePlayer == NULL) //错误网络包，简单丢掉
				{
					return;
				}
				if (pkInvitePlayer == NULL) //邀请玩家不存在
				{
					MG_RLT_DuelInvite SC_RltMsg;
					SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
					SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
					SC_RltMsg.m_ucType = DI_ERROR;
					SC_RltMsg.m_lParam = RE_DUEL_ERROR_BUSY;
					SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

					DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
					pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
					if (pkDuelFlow)
					{
						pkDuelFlow->ChangeState(LC_ServerDuelFlow::DS_End, GET_PROCESS_TIME);
					}
					return;
				}
				if (pkInvitePlayer && pkBeInvitePlayer) //检查是不是在同一个决斗的正确回应包
				{
					if (!LC_PlayerDuelHelper::CheckPlayersInSameDuel(pkInvitePlayer, pkBeInvitePlayer, rltMsg.m_lParam)) //不是同一个决斗
					{
						MG_RLT_DuelInvite SC_RltMsg;
						SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
						SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
						SC_RltMsg.m_ucType = DI_ERROR;
						SC_RltMsg.m_lParam = RE_DUEL_ERROR_BUSY;
						SC_RltMsg.m_lParamExtra1 = rltMsg.m_lParam;

						DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);
						pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
						return;
					}
				}
				if (pkDuelFlow) //检查PK流程是否正确
				{
					if (pkDuelFlow->GetState() != LC_ServerDuelFlow::DS_WaitForReq)
					{
						return; //错误网络包，简单丢掉 状态不同步
					}
				}
				else
				{
					return; //错误网络包，简单丢掉 决斗对象不在，简单抛弃掉
				}

				MG_RLT_DuelInvite SC_RltMsg;
				SC_RltMsg.m_lInvitePlayerID  = rltMsg.m_lInvitePlayerID;
				SC_RltMsg.m_lBeInvitePlayerID = rltMsg.m_lBeInvitePlayerID;
				SC_RltMsg.m_ucType = DR_REFUSE;
				SC_RltMsg.m_lParam = rltMsg.m_lParam;
				DuelHL_FillPlayerName(SC_RltMsg, pkInvitePlayer, pkBeInvitePlayer);

				pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
				LC_PlayerDuelHelper::ChangePlayerDuelState(pkBeInvitePlayer, LC_PlayerDuelState::DS_NULL, 0, 0);

				pkInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &SC_RltMsg);
				LC_PlayerDuelHelper::ChangePlayerDuelState(pkInvitePlayer, LC_PlayerDuelState::DS_NULL, 0, 0);
				pkDuelFlow->ChangeState(LC_ServerDuelFlow::DS_End, GET_PROCESS_TIME);
			}
			break;
		default:
			break;
	}

}

}
