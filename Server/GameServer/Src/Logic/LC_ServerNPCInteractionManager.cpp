#include "LC_ServerNPCInteractionManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"
#include "MG_PACKET.h"
#include "LC_PackEntryFactory.h"
#include "MG_Common.h"
#include "LC_GameEventManager.h"
#include "LC_SpawnManagerBase.h"
using namespace GameLogic;
using namespace Utility;
using namespace Protocol;

LC_ServerNPCInteractionManager::LC_ServerNPCInteractionManager()
{

}

LC_ServerNPCInteractionManager::~LC_ServerNPCInteractionManager()
{

}

ResultType LC_ServerNPCInteractionManager::NPCInteraction( LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	m_kResultInfo.Reset();

	if (pkNPC == NULL||pkPlayer == NULL)
		return RE_FAIL;

	LC_NPCInteractionInfo* pkInterationInfo = GetNPCInteractionInfo(pkNPC->GetCharType());
	if (pkInterationInfo == NULL)
		return RE_FAIL;

	int iInterationValue = pkNPC->GetInteractionValue();
	if (iInterationValue >= pkInterationInfo->GetMAXInterationValue())
		return RE_FAIL;

 	if ( pkInterationInfo->IsPrivate()
		 && pkNPC->GetInteractionTargetID() > 0
		 && pkNPC->GetInteractionTargetID() != pkPlayer->GetID())
 	{
		return RE_FAIL;
 	}

	int iLastStep = pkInterationInfo->GetValueStep(iInterationValue);
	InteractionOPList* pkOPList = pkInterationInfo->GetValueStepOPList(iLastStep);
	ResultType iResult = _checkOPList(pkOPList,pkNPC,pkPlayer);
	if (iResult != RE_SUCCESS)
		return iResult;

	int iRandNum = UT_MathBase::RandInRangeInt(0,100);
	int iIndex = pkInterationInfo->GetRespondIndex(iRandNum);

	pkOPList = pkInterationInfo->GetRespondOPList(iIndex);
	iResult = _checkOPList(pkOPList,pkNPC,pkPlayer);
	if (iResult != RE_SUCCESS)
		return iResult;

	_handleOPList(pkOPList,pkNPC,pkPlayer);
	int iDeltaValue = pkInterationInfo->GetDeltaValue(iIndex);
	int iNewStep = pkInterationInfo->GetValueStep(iInterationValue+iDeltaValue);
	if ( iLastStep != iNewStep )
	{
		//移除原先某些操作，状态以后根据需求添加
		InteractionOPList* pkOPList = pkInterationInfo->GetValueStepOPList(iLastStep);
		_unhandleOPList(pkOPList,pkNPC,pkPlayer);
		pkOPList = pkInterationInfo->GetValueStepOPList(iNewStep);
		_changeOPList(pkOPList,pkNPC,pkPlayer);
	}
	else
	{
		InteractionOPList* pkOPList = pkInterationInfo->GetValueStepOPList(iLastStep);
		_handleOPList(pkOPList,pkNPC,pkPlayer);
	}

	int iNewValue = iInterationValue+iDeltaValue;
	if (iNewValue < 0)
		iNewValue = 0;

	if (pkInterationInfo->GetMAXInterationValue() <= iNewValue )
	{
		iNewValue = pkInterationInfo->GetMAXInterationValue();
		m_kResultInfo.m_kResultType = RE_SUCCESS;
		m_kResultInfo.m_lParam1		= NIPT_MAX_INTERACTION;
	}
	
	pkNPC->SetInteractionValue(iNewValue);
	pkNPC->SetLastInteractionTime(GET_PROCESS_TIME);
	pkNPC->SetInteractionTargetID(pkPlayer->GetID());

	m_kResultInfo.m_kResultType = RE_SUCCESS;
	return RE_SUCCESS;
}

void LC_ServerNPCInteractionManager::CheckInteractionState( LC_ServerNPC* pkNPC )
{
	if (pkNPC == NULL)
		return;

	LC_NPCInteractionInfo* pkInterationInfo = GetNPCInteractionInfo(pkNPC->GetCharType());
	if (pkInterationInfo == NULL)
		return;

	if (pkNPC->GetInteractionValue() == 0)
		return;

	float fLastInterationTime = pkNPC->GetLastInteractionTime();
	if (GET_PROCESS_TIME - fLastInterationTime > pkInterationInfo->GetResetTime())
	{
		pkNPC->SetLastInteractionTime(GET_PROCESS_TIME);
		pkNPC->SetInteractionValue(0);
		pkNPC->SetInteractionTargetID(0);
	}
}

ResultType LC_ServerNPCInteractionManager::_checkOPList( InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	if (pkOPList == NULL)
		return RE_FAIL;

	InterationOPListIter Iter = pkOPList->begin();
	for (;Iter != pkOPList->end();++Iter)
	{
		NPCInteractionOPData& rOPData = *Iter;
		ResultType iResultType = _checkOPData(rOPData,pkNPC,pkPlayer);
		if (iResultType != RE_SUCCESS)
			return iResultType;
	}

	return RE_SUCCESS;
}

ResultType LC_ServerNPCInteractionManager::_checkOPData( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	switch(rOPData.m_iOPType)
	{
	case IOPT_DELETE_ITEM:
		return _checkDeleteItemOP(rOPData,pkNPC,pkPlayer);
	}

	return RE_SUCCESS;
}

ResultType LC_ServerNPCInteractionManager::_checkDeleteItemOP( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	LC_PackAsset* pkAsset = pkPlayer->GetPackAsset();
	if (pkAsset == NULL)
		return RE_FAIL;

	int iCount = pkAsset->GetCountCanDeleteInPack(rOPData.m_lParam1);
	if (iCount < rOPData.m_lParam2)
		return RE_NPC_INTERACTION_ITEM_NOT_ENOUGH;
	
	return RE_SUCCESS;
}

void LC_ServerNPCInteractionManager::_handleOPList( InteractionOPList* pkOPList, LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer)
{
	if (pkOPList == NULL)
		return;

	InterationOPListIter Iter = pkOPList->begin();
	for (;Iter != pkOPList->end();++Iter)
	{
		NPCInteractionOPData& rOPData = *Iter;
		_handleOPData(rOPData,pkNPC,pkPlayer);
	}
}

void LC_ServerNPCInteractionManager::_unhandleOPList( GameLogic::InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	if (pkOPList == NULL)
		return;

	InterationOPListIter Iter = pkOPList->begin();
	for (;Iter != pkOPList->end();++Iter)
	{
		NPCInteractionOPData& rOPData = *Iter;
		_unhandleOPData(rOPData,pkNPC,pkPlayer);
	}
}

void LC_ServerNPCInteractionManager::_changeOPList( GameLogic::InteractionOPList* pkOPList,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	if (pkOPList == NULL)
		return;

	InterationOPListIter Iter = pkOPList->begin();
	for (;Iter != pkOPList->end();++Iter)
	{
		NPCInteractionOPData& rOPData = *Iter;
		_changeOPData(rOPData,pkNPC,pkPlayer);
	}
}

void LC_ServerNPCInteractionManager::_handleOPData(const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	switch(rOPData.m_iOPType)
	{
	case IOPT_DELETE_ITEM:
		_executeDeleteItemOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_ADD_ITEM:
		_executeAddItemOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_SHOW_MESSAGE:
		_executeShowMessageOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_SPAWN_NPC:
		_executeSpawnNPCOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_ADD_STATE:
		_executeAddStateOP(rOPData,pkNPC,pkPlayer);
		break;
	}
}

void LC_ServerNPCInteractionManager::_unhandleOPData( const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	switch(rOPData.m_iOPType)
	{
	case IOPT_DELETE_ITEM:
		_executeDeleteItemOP(rOPData,pkNPC,pkPlayer);
	}
}

void LC_ServerNPCInteractionManager::_changeOPData( const GameLogic::NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	switch(rOPData.m_iOPType)
	{
	case IOPT_CHANGE_STATE_DELETE_ITEM:
		_executeDeleteItemOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_CHANGE_STATE_ADD_ITEM:
		_executeAddItemOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_CHANGE_STATE_SHOW_MESSAGE:
		_executeShowMessageOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_CHANGE_STATE_SPAWN_NPC:
		_executeSpawnNPCOP(rOPData,pkNPC,pkPlayer);
		break;
	case IOPT_CHANGE_STATE_ADD_STATE:
		_executeAddStateOP(rOPData,pkNPC,pkPlayer);
		break;
	}
}

void LC_ServerNPCInteractionManager::_executeDeleteItemOP( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	PackOperateInfoList objOPList;

	PackOperateInfo objData;
	objData.Reset();

	objData.m_iOPType = OPT_DELETE_ITEM_BY_ID;
	objData.m_bOPEntry = false;
	objData.m_lItemID   = rOPData.m_lParam1;
	objData.m_lCount    = rOPData.m_lParam2;
	objOPList.push_back(objData);

	pkPlayer->GetPackAsset()->ExecutePackOP(objOPList,true);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

	//pkPlayer->GetPackAsset()->DeleteByItemType(rOPData.m_lParam1,rOPData.m_lParam2);
}

void LC_ServerNPCInteractionManager::_executeAddItemOP( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if(NULL != pkPackEntryFactory)
	{
		LC_BackPackEntryList kEntryList;
		kEntryList.clear();
		pkPackEntryFactory->CreateItemEntry(rOPData.m_lParam1, rOPData.m_lParam2, CIET_NONE, kEntryList);

		PackOperateInfoList objOPList;
		objOPList.clear();

		for(int i = 0 ; i < (int)kEntryList.size() ; ++i)
		{
			PackOperateInfo objData;
			objData.Reset();
			objData.m_iOPType = OPT_ADD_ITEM;
			objData.m_bOPEntry = true;
			objData.m_bMerger  = true;
			objData.m_kPackEntry.Clone(kEntryList[i]);
			objOPList.push_back(objData);
		}

		ResultType iResult = pkPlayer->GetPackAsset()->ExecutePackOP(objOPList,true);

		if (iResult == RE_SUCCESS)
		{
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
			for(int i = 0 ; i < (int)kEntryList.size() ; ++i)
			{
				MG_Message rltMsg;
				rltMsg.m_iMessageID		= RE_ITEM_OPERATION_GET;
				rltMsg.m_iParameter1	= kEntryList[i].GetBackPackValueByType(LPSFET_TYPE_ID);
				rltMsg.m_iParameter2	= kEntryList[i].GetBackPackValueByType(LPSFET_OVERLAP_COUNT);

				pkPlayer->SendMsgToClient(MGPT_MESSAGE,&rltMsg);
			}
		}
		else
		{
			MG_Message rltMsg;
			rltMsg.m_iMessageID		= RE_ITEM_PACKAGE_FULL;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE,&rltMsg);
		}

	}
}

void LC_ServerNPCInteractionManager::_executeShowMessageOP( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
		return;

	pkGameEvent->SetEventType(GLET_GAME_STORY_MSG);
	pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,MST_NEAR);
	pkGameEvent->SetEventParam(EP_STORY_MSG_TARGET_TYPE,MST_NEAR);
	pkGameEvent->SetEventParam(EP_STORY_MSG_TARGET_ID,pkNPC->GetID());
	pkGameEvent->SetEventParam(EP_STORY_MSG_ID,rOPData.m_lParam1);
	pkGameEvent->SetEventParam(EP_STORY_MSG_SHOW_TYPE,MST_BUBBLE);
	pkGameEvent->SetEventParam(EP_STORY_MSG_SHOW_PARAM,0);
	pkEventManager->InsertGameEvent(pkGameEvent);
}

void LC_ServerNPCInteractionManager::_executeSpawnNPCOP( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if(NULL == pkManager)
		return;

	LC_MapBase* pkMap = pkPlayer->GetMap();
	if (pkMap == NULL)
		return;

	UT_Vec3Int kTargetLocation = pkPlayer->GetCurrentLocation();
	pkManager->CreateNPCInWorld(pkMap,kTargetLocation,rOPData.m_lParam1,rOPData.m_lParam2);
}

void LC_ServerNPCInteractionManager::_executeAddStateOP( const NPCInteractionOPData& rOPData,LC_ServerNPC* pkNPC,LC_ServerPlayer* pkPlayer )
{
	pkPlayer->ActiveSkillState(rOPData.m_lParam1,rOPData.m_lParam2,pkPlayer->GetID());
}
