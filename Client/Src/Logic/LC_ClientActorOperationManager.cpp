#include "ClientPCH.h"
#include "LC_ClientActorOperationManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "UT_LogicInfoMessage.h"
#include "NW_Helper.h"
#include "AS_ClientSoundManager.h"
//#include "LC_ClientExchangeManager.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;
//------------------------------------------------------------------
LC_ClientActorOperationManager::LC_ClientActorOperationManager()
{

}
//------------------------------------------------------------------
LC_ClientActorOperationManager::~LC_ClientActorOperationManager()
{

}
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setTextSandCompositionOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetTextSandCompositionOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_TEXT_SAND_COMPOSITION;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeTextSandCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_TEXT_SAND_COMPOSITION;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	
	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_resetTextSandCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_TEXT_SAND_COMPOSITION;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setAddTextToEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetAddTextToEquipOperatioParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_ADD_TEXT_TO_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeAddTextToEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_ADD_TEXT_TO_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_resetAddTextToEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_ADD_TEXT_TO_EQUIP;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setRemoveTextFromEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetRemoveTextFromEquipOperatioParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_REMOVE_TEXT_FROM_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeRemoveTextFromEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_REMOVE_TEXT_FROM_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_resetRemoveTextFromEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_REMOVE_TEXT_FROM_EQUIP;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setSandTextInEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetSandTextInEquipOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_SAND_TEXT_IN_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeSandTextInEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_SAND_TEXT_IN_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_resetSandTextInEquipOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_SAND_TEXT_IN_EQUIP;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setAppendAttributeOnEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetAppendAttributeOnEquipOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_APPEND_ATTRIBUTE_ON_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeAppendAttributeOnEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_APPEND_ATTRIBUTE_ON_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetAppendAttributeOnEquipOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_APPEND_ATTRIBUTE_ON_EQUIP;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_setDecompositionEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	ResultType iCheckResult = _checkSetDecompositionEquipOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_DECOMPOSTION_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeDecompositionEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_DECOMPOSTION_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetDecompositionEquipOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_DECOMPOSTION_EQUIP;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setProduceHobbyEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetProduceHobbyEquipOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_PRODUCE_HOBBY_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeProduceHobbyEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_PRODUCE_HOBBY_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetProduceHobbyEquipOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_PRODUCE_HOBBY_EQUIP;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setHobbyRecastEquipOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetHobbyRecastEquipOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_RECAST_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeHobbyRecastEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;	
		reqMsg.m_lOperationType = LAOT_HOBBY_RECAST_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		pkOperation->SetOperationStep(1);
	}

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_resetHobbyRecastEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_REQ_ResetOperation;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_RECAST_EQUIP;
	reqMsg.m_bClearParameters = true;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setHobbyEnhanceEquipOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,long lVal)
{
	ResultType iCheckResult = _checkSetHobbyEnhanceEquipOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;


	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_ENHANCE_EQUIP;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeHobbyEnhanceEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_HOBBY_ENHANCE_EQUIP;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetHobbyEnhanceEquipOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_REQ_ResetOperation;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_ENHANCE_EQUIP;
	reqMsg.m_bClearParameters = true;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_setHobbyEmbedSkillOperationParameter(GameLogic::LC_ActorOperationEntry *pkOperation, GameLogic::LC_PlayerBase *pkPlayer, int iParameterIndex, long lVal)
{
	ResultType iCheckResult = _checkSetHobbyEmbedSkillOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_EMBED_SKILL;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeHobbyEmbedSkillOperation(GameLogic::LC_ActorOperationEntry *pkOperation, GameLogic::LC_PlayerBase *pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_HOBBY_EMBED_SKILL;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetHobbyEmbedSkillOperation(GameLogic::LC_ActorOperationEntry *pkOperation, GameLogic::LC_PlayerBase *pkPlayer, bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_EMBED_SKILL;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//---------------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_checkSummorFriendOperationExecute(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer)
{
	long lCharID = pkOperation->GetOperationParameter(LC_SummorFriendOperation::LOPT_SUMMOR_FRIEND_ID);
	ResultType result = pkPlayer->SummonFriendAvatar(lCharID);
	return (RE_SUMMORFRIEND_SUCCESS == result ? RE_SUCCESS : result);
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_setSummorFriendOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	ResultType iCheckResult = _checkSetSummorFriendOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_SUMMOR_FRIEND;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	pkOperation->SetOperationParameter(iParameterIndex, lVal);

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeSummorFriendOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_SUMMOR_FRIEND;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetSummorFriendOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_SUMMOR_FRIEND;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
/*
ResultType LC_ClientActorOperationManager::_setHobbyUnEmbedSkillOperationParameter(GameLogic::LC_ActorOperationEntry *pkOperation, GameLogic::LC_PlayerBase *pkPlayer, int iParameterIndex, long lVal)
{
	ResultType iCheckResult = _checkSetHobbyUnEmbedSkillOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_UNEMBED_SKILL;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeHobbyUnEmbedSkillOperation(GameLogic::LC_ActorOperationEntry *pkOperation, GameLogic::LC_PlayerBase *pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_HOBBY_UNEMBED_SKILL;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetHobbyUnEmbedSkillOperation(GameLogic::LC_ActorOperationEntry *pkOperation, GameLogic::LC_PlayerBase *pkPlayer, bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_HOBBY_UNEMBED_SKILL;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
*/
//---------------------------------------------------------------------------
// 合成时装
//---------------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeBuildFashionOperation(LC_ActorOperationEntry* pkOperation, LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_BUILD_FASHION;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

ResultType LC_ClientActorOperationManager::_setBuildFashionOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	ResultType iCheckResult = _checkSetBuildFashionOperationParameter(pkOperation, pkPlayer, iParameterIndex, lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType	= LAOT_BUILD_FASHION;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}

ResultType LC_ClientActorOperationManager::_resetBuildFashionOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	pkOperation->SetOperationStep(0);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_BUILD_FASHION;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//---------------------------------------------------------------------------
//挂机
//---------------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeHookOperation(LC_ActorOperationEntry* pkOperation, LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_HOOK;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

ResultType LC_ClientActorOperationManager::_setHookOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	//ResultType iCheckResult = _checkHookOperationParameter(pkOperation, pkPlayer, iParameterIndex, lVal);
	//if(iCheckResult != RE_SUCCESS)
		//return iCheckResult;

	pkOperation->SetOperationParameter(iParameterIndex, lVal);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType	= LAOT_HOOK;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}

ResultType LC_ClientActorOperationManager::_resetHookOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_HOOK;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_setAmuletOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParamIndex,long lValue)
{
	ResultType iCheckResult = _checkSetAmuletOperationParameter(pkOperation, pkPlayer, iParamIndex, lValue);
	if (iCheckResult != RE_SUCCESS)
	{
		return iCheckResult;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_AMULET_OPERATION;
	reqMsg.m_lParameterIndex = iParamIndex;
	reqMsg.m_lParameterVal   = lValue;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);
	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;
	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_executeAmuletOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if (pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_AMULET_OPERATION;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);
		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;
		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
/*ResultType LC_ClientActorOperationManager::_resetAmuletOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_AMULET_OPERATION;
	reqMsg.m_bClearParameters = bClearParameters;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);
	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;
	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}*/
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_setItemCompositionOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	ResultType iCheckResult = _checkSetItemCompositionOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_ITEM_COMPOSITION;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeItemCompositionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_ITEM_COMPOSITION;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetItemCompositionOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_ITEM_COMPOSITION;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_setProductionOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	ResultType iCheckResult = _checkSetProductionOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
		return iCheckResult;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_PRODUCTION;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeProductionOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_PRODUCTION;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);
		int lFormulaID = pkOperation->GetOperationParameter(LC_Production::LOPT_PRODUCTION_FORMULA_ID);
		CF_ProductionFormula* pkCSVFormula = CF_ProductionFormula::GetSingletonPtr();
		CF_ProductionFormula::DataEntry* pkFormulaData = pkCSVFormula->GetEntryPtr(lFormulaID);
		long lMadeItemID = pkFormulaData->_lMadeItemID;
		if (IS_EQUIP_ID(lMadeItemID)) g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Forge_Proc));
		if (IS_GEM_TYPE(lMadeItemID)) g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_DiamondComp_Proc));
		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetProductionOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_PRODUCTION;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_setTokenCompositeOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	ResultType iCheckResult = _checkSetTokenCompositeOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
	{
		return iCheckResult;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_TOKEN_COMPOSITE;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_executeTokenCompositeOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_TOKEN_COMPOSITE;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}
//------------------------------------------------------------------
ResultType LC_ClientActorOperationManager::_resetTokenCompositeOperation(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,bool bClearParameters)
{
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_TOKEN_COMPOSITE;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------

ResultType LC_ClientActorOperationManager::_checkSetExchangeOperationParameter(LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
  return RE_FAIL;
  /*
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	ResultType iCheckResult = RE_FAIL;
	switch( iParameterIndex )
	{
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX0:  
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX1:
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX2:
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX3:
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX4:
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX5:
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX6:
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_ITEM_INDX7:
		{
			if( lVal >= 0 )
			{
				LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
				if( pkPackAsset == NULL )
					return RE_FAIL;
				LC_ClientExchangeManager* pkExchangeMgr = (LC_ClientExchangeManager*)LC_ClientExchangeManager::GetSingletonPtr();
				if( pkExchangeMgr == NULL )
					return RE_FAIL;

				LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry( PACK_TYPE_BASIC, lVal );
				if( pkEntry == NULL || pkEntry->GetValid() == false  )
					return RE_FAIL;
				long lItemID = pkEntry->GetTypeID();
				iCheckResult = pkExchangeMgr->CheckItemNeed( lItemID);
				if( iCheckResult != RE_SUCCESS)
					return iCheckResult;
			}
		}
		break;
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_REWARD_INDX:
		{
			long lExchangeID = pkOperation->GetOperationParameter(LC_ExchangeItemOperation::LOPT_EXCHANGE_EXCHANGEID);
			if( lExchangeID <=0 )
				return RE_FAIL;
			LC_ClientExchangeManager* pkExchangeMgr = (LC_ClientExchangeManager*)LC_ClientExchangeManager::GetSingletonPtr();
			if( pkExchangeMgr == NULL )
				return RE_FAIL;
			ExchangeData* pkData = pkExchangeMgr->GetExchangeData( lExchangeID );
			if( pkData == NULL)
				return RE_FAIL;
			if( pkData->GetRewardType() == RWT_ONEOFTHEM
				&& lVal <0 )
				return RE_ITEM_SELECT_NEEDED;

			return RE_SUCCESS;
		}
		break;
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_EXCHANGEID:
		{
			if( lVal <= 0 )
				return RE_FAIL;
			LC_ClientExchangeManager* pkExchangeMgr = (LC_ClientExchangeManager*)LC_ClientExchangeManager::GetSingletonPtr();
			if( pkExchangeMgr == NULL )
				return RE_FAIL;
			ExchangeData* pkData = pkExchangeMgr->GetExchangeData( lVal );
			if( pkData == NULL)
				return RE_FAIL;

		}
		break;
	case LC_ExchangeItemOperation::LOPT_EXCHANGE_OPT_NUM:
		{
			if( lVal <= 0)
				return RE_FAIL;
		}
		break;
	default:
		return RE_FAIL;
		break;
	}
	return RE_SUCCESS;
  */
}
ResultType		LC_ClientActorOperationManager::_setExchangeOperationParameter(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,int iParameterIndex,int lVal)
{
  return RE_FAIL;
  /*
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	ResultType iCheckResult = _checkSetExchangeOperationParameter(pkOperation,pkPlayer,iParameterIndex,lVal);
	if(iCheckResult != RE_SUCCESS)
	{
		return iCheckResult;
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType = LAOT_EXCHANGE;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
  */
}
ResultType		LC_ClientActorOperationManager::_executeExchangeOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer)
{
  return RE_FAIL;
  /*
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	if(pkOperation->GetOperationStep() == 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ExecuteOperation reqMsg;
		reqMsg.m_lOperationType = LAOT_EXCHANGE;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXECUTE_ACTOR_OPERATION, &reqMsg);

		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

		if(bCreateMsg)
			pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		return RE_SUCCESS;
	}

	return RE_FAIL;
  */
}
ResultType		 LC_ClientActorOperationManager::_resetExchangeOperation(GameLogic::LC_ActorOperationEntry* pkOperation,GameLogic::LC_PlayerBase* pkPlayer,bool bClearParameters)
{
  return RE_FAIL;
  /*
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = LAOT_EXCHANGE;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkPlayer;

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
  */
}
ResultType LC_ClientActorOperationManager::_checkExchangeOperationExecute( LC_ActorOperationEntry* pkOperation,LC_PlayerBase* pkPlayer )
{
  return RE_FAIL;
  /*
	if (NULL == pkOperation || NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	long lExchangeID = pkOperation->GetOperationParameter(LC_ExchangeItemOperation::LOPT_EXCHANGE_EXCHANGEID);
	if( lExchangeID <=0 )
		return RE_CAN_NOT_COMPOSITION;
	LC_ClientExchangeManager* pkExchangeMgr = (LC_ClientExchangeManager*)LC_ClientExchangeManager::GetSingletonPtr();
	if( pkExchangeMgr == NULL )
		return RE_FAIL;

	if(pkExchangeMgr->GetMaxExchangeCount(lExchangeID, pkPlayer)>0  )
		return RE_SUCCESS;
	else
	{
		ExchangeData* pkData = pkExchangeMgr->GetExchangeData(lExchangeID);
		if( pkData ==NULL )
			return RE_FAIL;
		if( pkData->GetExChangeType() ==ET_EXCHAGE )
			return RE_CAN_NOT_COMPOSITION;
		else
			return RE_SKILL_NOT_ENOUGH_ITEM;
	}
  */
}