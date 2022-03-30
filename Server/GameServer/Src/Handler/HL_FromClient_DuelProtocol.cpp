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
//������ҷ���ľ�������
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

	//����������
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
	//������˫���Ƿ��ܹ����о���
	UINT unResult;
	unResult = LC_PlayerDuelHelper::CheckPlayersCanDuel(lInvitePlayerID, lBeInvitePlayerID);

	//---------��鷢��ͱ������˫���Ƿ��������� ����������˼·�����տ�������������˷Ƿ������˼·��
	//--���˫���Ƿ������������������ ���󷵻�  ���ﲻ�������
	//--����Ƿ��ں�������ڣ�ˮƽ50M,�߶�20M�� ���󷵻� ����̫Զ
	//--���˫����״̬�Ƿ���������� �������߱����Ѿ��ھ����У������������������߱����ƣ����������������������߾����У����ͨ��״̬����������ѣ�
	//---------���OK

	//Error������ ���һ��������Ĳ���
	if (unResult)
	{
		//���ʹ�����Ϣ������
		MG_RLT_DuelInvite rltMsg;
		rltMsg.m_lInvitePlayerID = lInvitePlayerID;
		rltMsg.m_lBeInvitePlayerID = lBeInvitePlayerID;
		rltMsg.m_ucType = DI_ERROR;
		rltMsg.m_lParam = unResult;
		DuelHL_FillPlayerName(rltMsg, pkPlayer, pkBeInvitePlayer);
		pkPlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &rltMsg);
		return;
	}

	//�ڷ�����������������б������ڱ�������ҵķ�������
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

	//��Ϣ��͸�����͸����������
	//�㲥һ����Ϣ����Χ���
	pkBeInvitePlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_DUELINVITE, &rltMsg);
}
//------------------------------------------------------------------------------
//����״̬
//1��������������������ܸı��������״̬�����ü򵥶����ķ������Ƿ��Ǹ����Ӧ
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

	//����������
	if (pkInvitePlayer && pkInvitePlayer->GetActionForbid()->IsActionForbid(LAFT_DUEL, LAF_DUEL_MAX_COUNT))
	{
		return;
	}

	switch (rltMsg.m_ucType)
	{
		case DR_ACCEPT:
			{
				//����Ƿ������Ϣ
				if (pkBeInvitePlayer == NULL) //������������򵥶���
				{
					return;
				}
				if (pkInvitePlayer == NULL) //������Ҳ�����
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
				if (pkInvitePlayer && pkBeInvitePlayer) //����ǲ�����ͬһ����������ȷ��Ӧ��
				{
					if (!LC_PlayerDuelHelper::CheckPlayersInSameDuel(pkInvitePlayer, pkBeInvitePlayer, rltMsg.m_lParam)) //����ͬһ������
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
				if (pkDuelFlow) //���PK�����Ƿ���ȷ
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
						return; //������������򵥶��� ״̬��ͬ��
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
					return;//������������򵥶����� �Ҳ�����Ӧ�ľ�������
				}

				//�������Ƿ���Խ��ܣ����Խ��ܣ���xxx
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
				//���ɾ������̣�������
			}
			break;

		case DR_REFUSE:
			{
				if (pkBeInvitePlayer == NULL) //������������򵥶���
				{
					return;
				}
				if (pkInvitePlayer == NULL) //������Ҳ�����
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
				if (pkInvitePlayer && pkBeInvitePlayer) //����ǲ�����ͬһ����������ȷ��Ӧ��
				{
					if (!LC_PlayerDuelHelper::CheckPlayersInSameDuel(pkInvitePlayer, pkBeInvitePlayer, rltMsg.m_lParam)) //����ͬһ������
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
				if (pkDuelFlow) //���PK�����Ƿ���ȷ
				{
					if (pkDuelFlow->GetState() != LC_ServerDuelFlow::DS_WaitForReq)
					{
						return; //������������򵥶��� ״̬��ͬ��
					}
				}
				else
				{
					return; //������������򵥶��� ���������ڣ���������
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
