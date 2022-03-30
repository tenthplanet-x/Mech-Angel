//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltRemoveVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	//MG_RLT_RemoveVehicle rltMsg;
	//if(!rltMsg.Decode(pMsg,wSize))	
	//{
	//	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "�ͻ��˽������������ص�ɾ��������Ϣʧ��");
	//	return;
	//}
	//LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	//if (!pkPlayer || pkPlayer->GetID() != rltMsg.m_lPlayerID)
	//{
	//	return;
	//}

	//if (rltMsg.m_iResult == RE_SUCCESS)
	//{
	//	pkPlayer->GetVehicleAsset()->RemoveVehicle(rltMsg.m_lEntryIndex);
	//}
	//else
	//	SendLogicInfoMessage(rltMsg.m_iResult);

}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltUpgradeCimelia(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	MG_RLT_UpgraedeSubordinate rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "�ͻ��˽������������صĳɳ�ϵͳ[%d]������Ϣʧ��", rltMsg.m_iSubType);
		return;
	}

	LC_ClientGamePlayer *pkPlayer = pkSession->GetPlayer();
	if(NULL != pkPlayer)
	{
		StringStreamType sStream("");
		for(size_t i=0; i<rltMsg.m_ModifyAttrIDVec.size(); ++i)
		{
			sStream << rltMsg.m_ModifyAttrIDVec[i] << ":" << rltMsg.m_ModifyAttrLevelVec[i] << ";";
		}
		pkPlayer->OnRltUpgradeCimelia(rltMsg.m_iResult, rltMsg.m_iSubType, rltMsg.m_iStage, rltMsg.m_iPayIndex, sStream.str());
	}	
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltChangeTalisman(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	MG_RLT_SubordinateOperation rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "�ͻ��˽������������صķ����û�ϵͳ��Ϣʧ��", rltMsg.m_iSubType);
		return;
	}

	LC_ClientGamePlayer *pkPlayer = pkSession->GetPlayer();
	if(NULL != pkPlayer)
	{
		pkPlayer->OnRltChangeTransform(rltMsg.m_iOPRslt, rltMsg.m_iSubType, rltMsg.m_iOPCode, rltMsg.m_iOPParam);
	}
}
//---------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSummonVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDismissVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltActiveVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	//MG_RLT_ActiveVehicle rltMsg;
	//if(!rltMsg.Decode(pMsg,wSize))	
	//{
	//	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_ActiveVehicle����ʧ��");
	//	return;
	//}
	//
	//LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	//if (!pkPlayer || pkPlayer->GetID() != rltMsg.m_lPlayerID)
	//{
	//	return;
	//}

	//if (rltMsg.m_iResult == RE_SUCCESS)
	//{
	//	pkPlayer->GetVehicleAsset()->SetActiveVehicleIdex(rltMsg.m_lEntryIndex);
	//}
	//else
	//	SendLogicInfoMessage(rltMsg.m_iResult);

}