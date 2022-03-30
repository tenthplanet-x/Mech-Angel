#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_SkillProtocol.h"
#include "CF_SkillLevelUp.h"
#include "CF_SkillExecuteList.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "SK_SkillExecute.h"
#include "SK_Factory.h"
#include "LC_RumorManager.h"
#include "LC_GameEventManager.h"
#include "oss_define.h"
#include "../Buddy/BuddyInstance.h"
#include "SK_ServerSkillEventManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;
using namespace Skill;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SKILL_UPDATE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_UpdateSkillInSkillAsset reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type lPlayerID = reqMsg.m_lPlayerID;
	int32_t nBuddyId = reqMsg.m_iBuddyID;

	if (pkPlayer->GetActionForbid()->IsActionForbid(LAFT_GAME, LAF_GAME_SKILL_LEVEL_UP))
	{
		return;
	}

	BuddyInstance* pBuddyInstance = pkPlayer->GetBuddyInstanceByID(nBuddyId);
	if (pBuddyInstance == NULL)
		return;

	LC_SkillAsset* pBuddySkillAsset = pBuddyInstance->GetSkillAsset();
	if (NULL == pBuddySkillAsset)
		return;

	
	LC_SkillAssetEntry* pkEntry = pBuddySkillAsset->GetSkillEntryInAsset(reqMsg.m_lSkillCategory, reqMsg.m_iEntryIndex);
	if (NULL == pkEntry || !pkEntry->GetValid())
		return;

	int iResultBuddy = pBuddyInstance->UpdateSkillInSkillAsset(reqMsg.m_lSkillCategory, reqMsg.m_iEntryIndex, reqMsg.m_iMax);

	if (iResultBuddy == RE_SKILL_UPDATE_SUCCESS)
	{
		pkEntry = pBuddySkillAsset->GetSkillEntryInAsset(reqMsg.m_lSkillCategory, reqMsg.m_iEntryIndex);
		if (NULL == pkEntry)
		{
			return;
		}
		int lSkillID = pkEntry->GetTypeID();

		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (NULL == pkGameEvent)
			return;

		pkGameEvent->SetEventType(GLET_SKILL_LEVEL_UP);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
		pkGameEvent->SetEventParam(EP_SKILL_LEVEL_UP_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_SKILL_LEVEL_UP_SKILL_ID, lSkillID);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}

	MG_RLT_UpdateSkill kRltMsg;
	kRltMsg.m_iBuddyID = nBuddyId;
	kRltMsg.m_lSkillIDAfterUpdate = pkEntry->GetTypeID();
	kRltMsg.m_lUpdateSkillResult = iResultBuddy;
	pkPlayer->SendMsgToClient(MGPT_RLT_SKILL_UPDATE, &kRltMsg);
}

//学习进阶线技能
ON_HANDLE(MGPT_REQ_LEARNCONTROLLERSKILL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_LearnControllerSkill reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_LearnControllerSkill msg;
	msg.m_nSubType = reqMsg.m_nSubType;
	msg.m_nSkillID = reqMsg.m_nSkillID;
	msg.m_nResult = RE_FAIL;

	int32_t subStage = pkPlayer->GetSubordinateAsset().GetSubStage(reqMsg.m_nSubType);
	if(subStage < 1)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_LEARNCONTROLLERSKILL, &msg);
		return;
	}

	CF_SkillLevelUp::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(reqMsg.m_nSkillID);
	if(NULL == pkData)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_LEARNCONTROLLERSKILL, &msg);
		return;
	}

	
	//道具是否足够
	PackOperateInfoList kConsumeOPs;
	if (!pkData->_kNeedItems.empty())
	{
		LC_Helper::BuildConsumeItemOPs(pkData->_kNeedItems, kConsumeOPs);
		if (RE_SUCCESS != LC_Helper::CheckItemOPs(pkPlayer->GetPackAsset(), kConsumeOPs))
		{
			msg.m_nResult = RE_LEARNSKILL_ERROR_1;
			pkPlayer->SendMsgToClient(MGPT_RLT_LEARNCONTROLLERSKILL, &msg);
			return;
		}
	}

	int32_t iSlotIdx = -1;
	bool tag = pkPlayer->LearnSkillToController(reqMsg.m_nSkillID, iSlotIdx);
	msg.m_nResult = tag ? RE_SUCCESS : RE_FAIL;
	if (tag)
	{
		pkPlayer->AddSubSkill(reqMsg.m_nSubType, reqMsg.m_nSkillID);
		if(!kConsumeOPs.empty())
		{
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, LEARNSKILL_OPERATE, OPERATE_LOGTYPE1);
			vector<int> nParams;
			nParams.clear();
			nParams.push_back(reqMsg.m_nSkillID);
			LC_Helper::DoItemOPs(pkPlayer, kConsumeOPs, SYS_FUNC_TP_LEARNCONTROLLER_SKILL, reqMsg.m_nSkillID, true, true, nLogCode, nParams);
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		}
	}
	
	pkPlayer->SendMsgToClient(MGPT_RLT_LEARNCONTROLLERSKILL, &msg);
}

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_TARGET, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_Req_PlayerExecuteSkillTypeTarget reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type lRequesterID		= reqMsg.m_lRequesterID;
	int lSkillExecuteID	= reqMsg.m_lSkillExecuteID;
	int lSkillItemID		= reqMsg.m_lItemTypeID;
	object_id_type lLockTargetID		= reqMsg.m_lLockTargetID;
	UT_Vec3Int kCurrentLocation(reqMsg.m_lCurrentLocationX, reqMsg.m_lCurrentLocationY, reqMsg.m_lCurrentLocationZ);
	UT_Vec3Int kLockLocation(0, 0, 0);
	UT_Vec3Int kMouseLocation(reqMsg.m_lMouseLocationX, reqMsg.m_lMouseLocationY, 0);
	float fCurrentTime		= GET_PROCESS_TIME;

	pkPlayer->CastSkill( lSkillExecuteID, fCurrentTime, lSkillItemID, lLockTargetID, kCurrentLocation, kLockLocation, kMouseLocation, 0);
	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_DIR, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_Req_PlayerExecuteSkillTypeDir reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type lRequesterID		= reqMsg.m_lRequesterID;
	object_id_type lLockTargetID		= reqMsg.m_lLockedTargetID;
	int  iSkillExecuteID	= reqMsg.m_lSkillExecuteID;
	int lSkillItemID		= reqMsg.m_lItemTypeID;
	UT_Vec3Int				kCurrentLocation(0,0,0);

	UT_Vec2Float			kForwardDir;
	kForwardDir =			UT_MathBase::GetZRotDir(reqMsg.m_iForwardAngle);
	UT_Vec3Int				kLockedLocation(reqMsg.m_lLockedLocationX, reqMsg.m_lLockedLocationY, reqMsg.m_lLockedLocationZ);
	UT_Vec3Int kMouseLocation(reqMsg.m_lMouseLocationX, reqMsg.m_lMouseLocationY, 0);
	float fCurrentTime		= GET_PROCESS_TIME;

	pkPlayer->CastSkill( iSkillExecuteID, fCurrentTime, lSkillItemID, lLockTargetID, kCurrentLocation, kLockedLocation, kMouseLocation, reqMsg.m_iForwardAngle);

	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_LOCATION, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_Req_PlayerExecuteSkillTypeLocation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type lRequesterID		= reqMsg.m_lRequesterID;
	int  iSkillExecuteID	= reqMsg.m_lSkillExecuteID;
	int lSkillItemID		= reqMsg.m_lItemTypeID;
	UT_Vec3Int				kCurrentLocation(reqMsg.m_lCurrentLocationX, reqMsg.m_lCurrentLocationY, reqMsg.m_lCurrentLocationZ);
	UT_Vec3Int				kLockedLocation(reqMsg.m_lLockedLocationX, reqMsg.m_lLockedLocationY, reqMsg.m_lLockedLocationZ);
	UT_Vec3Int kMouseLocation(reqMsg.m_lMouseLocationX, reqMsg.m_lMouseLocationY, 0);
	float fCurrentTime		= GET_PROCESS_TIME;

	pkPlayer->CastSkill( iSkillExecuteID, fCurrentTime, lSkillItemID, 0, kCurrentLocation, kLockedLocation, kMouseLocation, 0);

	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_SYNC_EXECUTE_SKILL, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_Sync_ExecuteSkill syncMsg;
	if (syncMsg.Decode(pMsgBody, usBodySize))
	{
		UT_Vec3Int kCurrentLocation;
		//		object_id_type lRequesterID		= syncMsg.m_lRequesterID;
		int  iSkillExecuteID	= syncMsg.m_lSkillExecuteID;
		kCurrentLocation.x		= syncMsg.m_lCurrentLocationX;
		kCurrentLocation.y		= syncMsg.m_lCurrentLocationY;
		//		float fClientTime		= syncMsg.m_fClientTime;

		if (iSkillExecuteID == pkPlayer->GetCurrentStateType())
		{
			pkPlayer->SetCurrentLocation(kCurrentLocation);
			pkPlayer->Update(GET_PROCESS_TIME, 0.0f);
		}
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_FINISH_EXECUTE_SKILL, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_Finish_ExecuteSkill finishMsg;
	if (!finishMsg.Decode(pMsgBody, usBodySize))
		return;

	pkPlayer->TryFinishSkill(finishMsg.m_lSkillExecuteID, finishMsg.m_fClientTime, finishMsg.m_lCurrentLocationX, finishMsg.m_lCurrentLocationY);

	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_PLAYER_INTERRUPT_SKILL, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_Req_PlayerInterruptSkill reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	object_id_type	lRequesterID		= reqMsg.m_lRequesterID;
	int	lSkillExecuteID		= reqMsg.m_lSkillExecuteID;
	uint8_t usInterruptType		= reqMsg.m_usInterruptType;
	float	fClientTime			= reqMsg.m_fClientTime;

	pkPlayer->TryInterruptSkill(reqMsg.m_lSkillExecuteID, reqMsg.m_fClientTime, reqMsg.m_usInterruptType);
	return;
}

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_DEACTIVE_SKILL_STATE, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_REQ_DeactiveSkillState reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActorStateMap* pkStateMap = pkPlayer->GetStateMap();
	if (!pkStateMap)
	{
		return;
	}

	uint32_t lSkillStateTypeID = reqMsg.m_lSkillStateTypeID;

	ResultType eResult;

	CF_SkillStateList* pkCSVStateList = CF_SkillStateList::GetSingletonPtr();
	CF_SkillStateList::DataEntry* pkStateData = pkCSVStateList->GetEntryPtr(lSkillStateTypeID);
	if (!pkStateData || !pkStateData->_bCanManualDeactive)
	{
		eResult = RE_SKILL_STATE_DEACTIVE_FAIL;
	}
	else
	{
		eResult = pkStateMap->RemoveSkillStateInStateMap(lSkillStateTypeID);
	}
	MG_RLT_DeactiveSkillState rltMsg;
	rltMsg.m_lResult = eResult;
	rltMsg.m_lSkillStateTypeID = lSkillStateTypeID;

	pkPlayer->SendMsgToClient(MGPT_RLT_DEACTIVE_SKILL_STATE, &rltMsg);
}

ON_HANDLE(MGPT_REQ_SKILL_BOOK_UPGRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SkillBookUpgrade reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_SkillBookUpgrade rltMsg;
	rltMsg.m_iID = reqMsg.m_iID;
	rltMsg.m_iPos = reqMsg.m_iPos;
	rltMsg.m_lResult = pkPlayer->SkillBookUpgrade(reqMsg.m_iID, reqMsg.m_iPos);
	pkPlayer->SendMsgToClient(MGPT_RLT_SKILL_BOOK_UPGRADE, &rltMsg);

	if(RE_SKILL_BOOK_UPGRADE_SUCCESS == rltMsg.m_lResult)
	{
		pkPlayer->SyncSkillBookInfo();
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_BULLET_SHOT, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pkPlayer == NULL)
		return;

	MG_REQ_Bullet_Shot reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	pkPlayer->ReqBulletShot(reqMsg);

	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_BULLET_PENETRATE, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_Bullet_Penetrate reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	bool bRet = pkEventManager->ActiveBuddyReqBulletPenetrate(pkPlayer, reqMsg);

	return;
}
ON_HANDLE(MGPT_REQ_SELECT_TARGET, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_Select_Target reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	pkPlayer->SetSelectTargetID(reqMsg.m_lTargetId);
	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_BULLET_EXPLOSIVE, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_Bullet_Explosive reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	bool bRet = pkEventManager->ActiveBuddyReqBulletExplosive(pkPlayer, reqMsg);

	return;
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_BULLET_CHANGE_TARGET_LOCATION, this_source_type, true)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_REQ_Bullet_ChangeTargetLocation reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
		return;

	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	bool bRet = pkEventManager->ActiveBuddyReqBulletChangeTargetLocation(pkPlayer, reqMsg);

	return;
}
}

