#include "MG_CompositionProtocol.h"

#include "LC_ClientComposition.h"

using namespace CSVFile;

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltComposition(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_Composition msg;
	if (msg.Decode(pMsg,wSize))
	{
		long lPlayerID = msg.m_lPlayerID;
		ResultType iResult = msg.m_iResult;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"�ϳ�װ��������Ϣ�߼�����");
			return;
		}

		if (iResult == RE_COMPOSITION_SUCCESS)
		{
			//SendLogicInfoMessage(msg.m_iResult);

			for (int i = 0; i < 3; ++i)
			{
				pkGamePlayer->GetClientComposition()->CompositionSlotRemove(i);
			}

			pkGamePlayer->GetComposition()->ReUse();
			
			return;
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltAddItemToCompositionSlot(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_AddItemToCompositionPack msg;
	if (msg.Decode(pMsg,wSize))
	{
		long lPlayerID = msg.m_lPlayerID;
		ResultType iResult = msg.m_iResult;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"���������Ʒ���ϳɱ���������Ϣ�߼�����");
			return;
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltRemoveItemFromCompositionSlot(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RemoveItemFromCompositionPack msg;
	if (msg.Decode(pMsg,wSize))
	{
		long lPlayerID = msg.m_lPlayerID;
		ResultType iResult = msg.m_iResult;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"����Ӻϳɱ����Ƴ���Ʒ������Ϣ�߼�����");
			return;
		}


	}
}
//------------------------------------------------------------------------